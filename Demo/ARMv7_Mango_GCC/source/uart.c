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

#include <asm/early_print.h>
#include <io.h>

static uint32_t uart_base;

void uart_init()
{
	uart_base = EARLY_UART_BASE;
}

static uint8_t uart_tx_busy()
{
	uint32_t data = mem_read32(uart_base + A20_UART_LSR);
	return (data & A20_UART_LSR_THRE);
}

int32_t uart_tx(const char *buff, uint32_t len)
{
	int32_t i;

	for (i = 0; i < len; i++)
	{
		while (!uart_tx_busy());

		mem_write8(uart_base + A20_UART_THR, buff[i]);
	}

	return len;
}

int32_t uart_rx(char *buff, uint32_t len)
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

uint32_t strlen(const char *buf)
{
	uint32_t i = 0;

	while (*buf++ != 0)
	{
		i++;
	}

	return i;
}
