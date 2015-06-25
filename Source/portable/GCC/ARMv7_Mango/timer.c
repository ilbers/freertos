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

#include <asm/sys_ctrl.h>
#include <interrupt.h>
#include <io.h>
#include <portmacro.h>
#include <sys_print.h>

#define TIMER_IRQ_NR		27

#define TICK_HZ			configTIMER_HZ		/* Desired tick resolution */
#define TIMER_HZ		24000000		/* Physical counter resolution */
#define TIMER_PERIOD_MS		(TIMER_HZ / TICK_HZ)	/* Scheduling period */

/* FreeRTOS tick processing */
extern void FreeRTOS_Tick_Handler(void);

static inline void timer_sched_tick()
{
	uint64_t cval;

	/* Re-schedule next interrupt */
	cval = READ_CP_REG64(CNTV_CVAL(R0_inline, R1_inline));
	cval += TIMER_PERIOD_MS;
	WRITE_CP_REG64(cval, CNTV_CVAL(R0_inline, R1_inline));

	/* Enable timer */
	WRITE_CP_REG32(1, CNTV_CTL(R0_inline));

	ISB(sy);
}

static void timer_irq_handler()
{
	/* Disable timer */
	WRITE_CP_REG32(0, CNTV_CTL(R0_inline));

	FreeRTOS_Tick_Handler();

	/* Schedule next tick */
	timer_sched_tick();
}

void timer_init()
{
	uint64_t cval;

	register_irq_handler(TIMER_IRQ_NR, &timer_irq_handler);

	cval = READ_CP_REG64(CNTVCT(R0_inline, R1_inline));
	WRITE_CP_REG64(cval, CNTV_CVAL(R0_inline, R1_inline));

	/* Enable timer */
	WRITE_CP_REG32(1, CNTV_CTL(R0_inline));

	sys_print_msg("[TIMER] start counter\r\n");

	gic_set_irq_priority(TIMER_IRQ_NR, configTICK_PRIORITY << portPRIORITY_SHIFT);
	enable_one_irq(TIMER_IRQ_NR);
}
