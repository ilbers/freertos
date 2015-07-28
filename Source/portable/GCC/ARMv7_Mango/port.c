/*
 * Copyright (C) 2015 ilbers GmbH
 *
 * Alexander Smirnov <asmirnov@ilbers.de>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License** as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <FreeRTOS.h>
#include <portmacro.h>
#include <interrupt.h>
#include <sys_print.h>
#include <task.h>

volatile unsigned long ulCriticalNesting = 9999UL;

/* Counts the interrupt nesting depth.  A context switch is
 * only performed if the nesting depth is 0.
 */
uint32_t ulPortInterruptNesting = 0UL;

/* Saved as part of the task context. If ulPortTaskHasFPUContext is
 * non-zero then a floating point context must be saved and restored
 * for the task.
 */
uint32_t ulPortTaskHasFPUContext = pdFALSE;

__attribute__(( used )) const uint32_t ulMaxAPIPriorityMask = (configMAX_API_CALL_INTERRUPT_PRIORITY << portPRIORITY_SHIFT);

/* Set to 1 to pend a context switch from an ISR. */
uint32_t ulPortYieldRequired = pdFALSE;

extern void vPortRestoreTaskContext(void);

void vPortEnterCritical( void )
{
	if (ulCriticalNesting == 0)
	{
		/* Disable local IRQs if we are not in critical
		 * section already.
		 */
		portCPU_IRQ_DISABLE();
	}

	ulCriticalNesting++;
}

void vPortExitCritical( void )
{
	if( ulCriticalNesting > portNO_CRITICAL_NESTING )
	{
		/* Decrement the nesting count as the critical
		 * section is being exited.
		 */
		ulCriticalNesting--;

		/* If the nesting level has reached zero then all
		 * interrupt priorities must be re-enabled.
		 */
		if( ulCriticalNesting == portNO_CRITICAL_NESTING )
		{
			/* Critical nesting has reached zero so
			 * all interrupts should be unmasked.
			 */
			 portCPU_IRQ_ENABLE();
		}
	}
}

portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
	*pxTopOfStack = 0;
	pxTopOfStack--;
	*pxTopOfStack = 0;
	pxTopOfStack--;
	*pxTopOfStack = 0;
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) portINITIAL_SPSR;

	pxTopOfStack--;

	*pxTopOfStack = ( portSTACK_TYPE ) pxCode;
	pxTopOfStack--;

	*pxTopOfStack = ( portSTACK_TYPE ) 0x00000000;		/* R14 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x12121212;		/* R12 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x11111111;		/* R11 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x10101010;		/* R10 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x09090909;		/* R9 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x08080808;		/* R8 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x07070707;		/* R7 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x06060606;		/* R6 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x05050505;		/* R5 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x04040404;		/* R4 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x03030303;		/* R3 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x02020202;		/* R2 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x01010101;		/* R1 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) pvParameters;	/* R0 */
	pxTopOfStack--;

	*pxTopOfStack = portNO_CRITICAL_NESTING;
	pxTopOfStack--;

	*pxTopOfStack = portNO_FLOATING_POINT_CONTEXT;

	return pxTopOfStack;
}

void FreeRTOS_Tick_Handler(void)
{
	/* Set interrupt mask before altering scheduler structures.
	 * The tick handler runs at the lowest priority, so interrupts
	 * cannot already be masked, so there is no need to save and
	 * restore the current mask value.  It is necessary to turn
	 * off interrupts in the CPU itself while the ICCPMR is being
	 * updated.
	 */

	/* Increment the RTOS tick. */
	if (xTaskIncrementTick() != pdFALSE)
	{
		ulPortYieldRequired = pdTRUE;
	}
}

BaseType_t xPortStartScheduler( void )
{
	portCPU_IRQ_DISABLE();

	/* Start the timer that generates the tick ISR. */
	configSETUP_TICK_INTERRUPT();

	/* Start the first task executing. */
	vPortRestoreTaskContext();

	return 0;
}

void vPortEndScheduler( void )
{
	/* No need to stop scheduler */
}
