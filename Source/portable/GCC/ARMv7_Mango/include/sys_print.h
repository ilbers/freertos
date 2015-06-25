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

#ifndef __SYS_PRINT_H__
#define __SYS_PRINT_H__

#include <stdint.h>

#define PRINT_BUFF_SIZE		1024

#ifdef CONFIG_EARLY_PRINT
    int early_uart_print(char *msg);
    #define early_print_msg(a)	early_uart_print(a)
#else
    #define early_print_msg(a)
#endif

/* UART routine */
void uart_init();
int32_t uart_tx(const char *buff, uint32_t len);
int32_t uart_rx(char *buff, uint32_t len);

/* Main output routine */
uint32_t sys_print_msg(const char *fmt, ...);

#endif /* __SYS_PRINT_H__ */
