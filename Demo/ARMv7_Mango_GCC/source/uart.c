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

#include <io.h>
#include <sys_print.h>

static uart_driver_t uart_drv;

void uart_init()
{
	uart_drv.init = 0;
	uart_drv.send = 0;
	uart_drv.recv = 0;

	register_uart_driver(&uart_drv);

	if (uart_drv.init)
	{
		uart_drv.init();
	}
}

int32_t uart_tx(const char *buff, uint32_t len)
{
	int32_t ret = 0;

	if (uart_drv.send)
	{
		ret = uart_drv.send(buff, len);
	}

	return ret;
}

int32_t uart_rx(char *buff, uint32_t len)
{
	int32_t ret = 0;

	if (uart_drv.recv)
	{
		ret = uart_drv.recv(buff, len);
	}

	return ret;
}

uint32_t strlen(const char *buf)
{
	uint32_t i = 0;

	while (*buf++ != 0)
	{
		i++;
	}

	return i;
}
