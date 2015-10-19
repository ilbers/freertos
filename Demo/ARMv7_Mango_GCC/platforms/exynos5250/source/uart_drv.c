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
#include <platform/exynos5250_uart.h>

/* UART port base address */
static uint32_t uart_base;

static void exynos5250_uart_init(void)
{
	uart_base = EXYNOS5250_UART0_BASE;
}

static uint8_t exynos5250_uart_tx_busy(void)
{
	uint32_t data = mem_read32(uart_base + UART_UTRSTAT);
	return (data & UART_UTRSTAT_TXBE);
}

static int32_t exynos5250_uart_tx(const char *buff, uint32_t len)
{
	int32_t i;

	for (i = 0; i < len; i++)
	{
		while (!exynos5250_uart_tx_busy());

		mem_write8(uart_base + UART_UTXH, buff[i]);
	}

	return len;
}

int32_t exynos5250_uart_rx(char *buff, uint32_t len)
{
	int32_t i = 0;

	do
	{
		uint8_t byte;

		byte = mem_read8(uart_base + UART_RXH);

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
	drv->init = exynos5250_uart_init;
	drv->send = exynos5250_uart_tx;
	drv->recv = exynos5250_uart_rx;
}
