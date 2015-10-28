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

#ifndef __SYS_PRINT_H__
#define __SYS_PRINT_H__

#include <stdint.h>

#define PRINT_BUFF_SIZE		1024

typedef struct
{
	void    (*init)(void);					/* driver initialization callback */
	int32_t (*send)(const char *buff, uint32_t len);	/* TX callback */
	int32_t (*recv)(char *buff, uint32_t len);		/* RX callback */
} uart_driver_t;

/* UART routine */
void uart_init(void);
int32_t uart_tx(const char *buff, uint32_t len);
int32_t uart_rx(char *buff, uint32_t len);
void register_uart_driver(uart_driver_t *drv);

/* Main output routine */
uint32_t sys_print_msg(const char *fmt, ...);
uint32_t mango_print_msg(const char *fmt, ...);

#endif /* __SYS_PRINT_H__ */
