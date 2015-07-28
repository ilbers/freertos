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

#include <interrupt.h>
#include <io.h>
#include <mm.h>
#include <sys_print.h>

#define RTOS_VECTOR_BASE	0x40400000
#define ARM_VECTOR_BASE		0xffff0000

extern void *vector;

void traps_init()
{
	/* Map FreeRTOS vector */
	map_memory_region(RTOS_VECTOR_BASE,
			  RTOS_VECTOR_BASE,
			  MEM_BLOCK_4KB,
			  MEM_NORMAL);

	/* Map ARM vector */
	map_memory_region(ARM_VECTOR_BASE,
			  RTOS_VECTOR_BASE,
			  MEM_BLOCK_4KB,
			  MEM_NORMAL);

	/* Setup correct exception vector */
	copy_memory_region((void *)ARM_VECTOR_BASE,
			   (void *)((uint32_t)&vector),
			   MEM_BLOCK_4KB);

	sys_print_msg("[TRAP] vector is mapped\r\n");
}

static void system_stop()
{
	sys_print_msg("Entered state FATAL.\r\n");
	sys_print_msg("System stopped.\r\n");

	for (;;);
}

void do_data_abort()
{
	mango_print_msg("trap: data abort\r\n");
	system_stop();
}

void do_prefetch_abort()
{
	sys_print_msg("trap: prefetch abort\r\n");
	system_stop();
}

void do_irq()
{
	gic_do_interrupt();
}
