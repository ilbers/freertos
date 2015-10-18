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
#include <platform/a20_uart.h>

/* UART port base address */
static uint32_t uart_base;

static void a20_uart_init(void)
{
	uart_base = EARLY_UART_BASE;
}

static uint8_t a20_uart_tx_busy(void)
{
	uint32_t data = mem_read32(uart_base + A20_UART_LSR);
	return (data & A20_UART_LSR_THRE);
}

static int32_t a20_uart_tx(const char *buff, uint32_t len)
{
	int32_t i;

	for (i = 0; i < len; i++)
	{
		while (!a20_uart_tx_busy());

		mem_write8(uart_base + A20_UART_THR, buff[i]);
	}

	return len;
}

int32_t a20_uart_rx(char *buff, uint32_t len)
{
	int32_t i = 0;

	do
	{
		uint8_t byte;

		byte = mem_read8(uart_base + A20_UART_RBR);

		if (byte != 0)
		{
			buff[i] = byte;
			i++;
		}
	}
	while (i < len);

	return i;
}

void register_uart_driver(uart_driver_t *drv)
{
	/* Register callbacks for UART driver */
	drv->init = a20_uart_init;
	drv->send = a20_uart_tx;
	drv->recv = a20_uart_rx;
}
