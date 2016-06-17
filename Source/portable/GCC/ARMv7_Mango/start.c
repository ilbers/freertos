/*
 * Copyright (C) 2015 ilbers GmbH
 *
 * Alexander Smirnov <asmirnov@ilbers.de>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License (version 2) as published
 * by the Free Software Foundation.
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

#include <mm.h>
#include <interrupt.h>
#include <io.h>
#include <asm/sys_ctrl.h>
#include <sys_print.h>

extern int main();
extern void traps_init();

static void print_banner()
{
	sys_print_msg("\r\n");
	sys_print_msg(" *******************************************\r\n");
	sys_print_msg(" **       Welcome to LotOS framework      **\r\n");
	sys_print_msg(" **  Copyright (C) 2015-2016 ilbers GmbH  **\r\n");
	sys_print_msg(" **                                       **\r\n");
	sys_print_msg(" **         starting FreeRTOS             **\r\n");
	sys_print_msg(" *******************************************\r\n");
	sys_print_msg("\r\n");
}

void start_kernel()
{
	/* Setup kernel components */
	uart_init();
	mm_init();
	traps_init();
	irq_init();

	print_banner();

	sys_print_msg("starting application...\r\n");

	/* Start main application */
	main();

	for(;;);
}
