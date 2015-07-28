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

#ifndef FREERTOSCONFIG_H
#define FREERTOSCONFIG_H

/* Use old-style FreeRTOS definition.
 * See 'Source/include/deprecated_definitions.h' for more information
 */
#define __ARMv7_MANGO__				1

extern void vPortEnterCritical(void);
extern void vPortExitCritical(void);

#define configMINIMAL_STACK_SIZE		((unsigned short) 1024)
#define configMAX_PRIORITIES			(5)
#define configUSE_PREEMPTION			1
#define configUSE_IDLE_HOOK			0
#define configUSE_TICK_HOOK			0
#define configUSE_16_BIT_TICKS			0
#define configTOTAL_HEAP_SIZE			((size_t) (1048576 ))

#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete			1
#define INCLUDE_vTaskCleanUpResources		1
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay			1

#define configMAX_API_CALL_INTERRUPT_PRIORITY	25
#define configTICK_PRIORITY			30
#define configLWIP_TASK_PRIORITY		30

#define configUNIQUE_INTERRUPT_PRIORITIES	32

extern void timer_init();
#define configSETUP_TICK_INTERRUPT()		timer_init()

#define configTIMER_TASK_PRIORITY		(configMAX_PRIORITIES - 1)

#define configUSE_PORT_OPTIMISED_TASK_SELECTION	0

#define configUSE_MUTEXES			1
#define configUSE_COUNTING_SEMAPHORES		1

#define configTIMER_HZ				1000
#define configTICK_RATE_HZ			configTIMER_HZ

/* Build configuration for A20 SoC */
#define CONFIG_A20				1
#define CONFIG_EARLY_PRINT			1

/* A20 settings */
#define GIC_DIST_BASE				0x01c81000
#define GIC_CPU_BASE				0x01c82000
#define NR_CPU_SPIS				128		/* Shared peripheral interrupts */

#endif /* FREERTOSCONFIG_H */
