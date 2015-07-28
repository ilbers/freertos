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

#ifndef PORTMACRO_H
#define PORTMACRO_H

#include <stdint.h>
#include <FreeRTOSConfig.h>

/* Basic types definition */
#define portCHAR			char
#define portFLOAT			float
#define portDOUBLE			double
#define portLONG			long
#define portSHORT			short
#define portSTACK_TYPE			uint32_t
#define portBASE_TYPE			long

/* Called at the end of an ISR that can cause a context switch. */
#define portEND_SWITCHING_ISR(xSwitchRequired)		\
{							\
	extern uint32_t ulPortYieldRequired;		\
							\
	if( xSwitchRequired != pdFALSE )		\
	{						\
		ulPortYieldRequired = pdTRUE;		\
	}						\
}

#define portYIELD_FROM_ISR( x )		portEND_SWITCHING_ISR( x )
#define portYIELD()			__asm volatile ( "SWI 0" );


#define portTICK_PERIOD_MS		((portTickType )(1000 / configTICK_RATE_HZ))

typedef long				BaseType_t;
typedef unsigned long			UBaseType_t;

typedef uint32_t			TickType_t;
#define portMAX_DELAY			((TickType_t ) 0xffffffffUL)

#define portBYTE_ALIGNMENT		8

/* Stack settings */
typedef portSTACK_TYPE			StackType_t;
#define portSTACK_GROWTH		(-1)

/* Critical section settings */
#define portNO_CRITICAL_NESTING		((unsigned long) 0)
#define portENTER_CRITICAL()		vPortEnterCritical();
#define portEXIT_CRITICAL()		vPortExitCritical();

/* Default task CPSR value */
#define portINITIAL_SPSR		(( portSTACK_TYPE) 0x1f)

/* No usage of FPU */
#define portNO_FLOATING_POINT_CONTEXT	((portSTACK_TYPE) 0)

/* Interrupts settings */
extern void enable_local_irqs();
extern void disable_local_irqs();
#define portCPU_IRQ_ENABLE()		enable_local_irqs()
#define portCPU_IRQ_DISABLE()		disable_local_irqs()

#define portENABLE_INTERRUPTS()		vPortExitCritical()
#define portDISABLE_INTERRUPTS()	vPortEnterCritical()

/* #define portSET_INTERRUPT_MASK_FROM_ISR()				ulPortSetInterruptMask() */
/* #define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)				vPortClearInterruptMask(x) */

/* Setup interrupt priority */
#if configUNIQUE_INTERRUPT_PRIORITIES == 16
  #define portPRIORITY_SHIFT		4
  #define portMAX_BINARY_POINT_VALUE	3
#elif configUNIQUE_INTERRUPT_PRIORITIES == 32
  #define portPRIORITY_SHIFT		3
  #define portMAX_BINARY_POINT_VALUE	2
#elif configUNIQUE_INTERRUPT_PRIORITIES == 64
  #define portPRIORITY_SHIFT		2
  #define portMAX_BINARY_POINT_VALUE	1
#elif configUNIQUE_INTERRUPT_PRIORITIES == 128
  #define portPRIORITY_SHIFT		1
  #define portMAX_BINARY_POINT_VALUE	0
#elif configUNIQUE_INTERRUPT_PRIORITIES == 256
  #define portPRIORITY_SHIFT		0
  #define portMAX_BINARY_POINT_VALUE	0
#else
  #error Invalid configUNIQUE_INTERRUPT_PRIORITIES setting.  configUNIQUE_INTERRUPT_PRIORITIES must be set to the number of unique priorities implemented by the target hardware
#endif

#define portTASK_FUNCTION_PROTO(vFunction, pvParameters)	void vFunction(void *pvParameters)
#define portTASK_FUNCTION(vFunction, pvParameters)		void vFunction(void *pvParameters)

#endif /* PORTMACRO_H */
