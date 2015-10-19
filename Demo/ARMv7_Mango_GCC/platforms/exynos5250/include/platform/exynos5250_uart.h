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

#ifndef __PLATFORM_EXYNOS5250_UART_H__
#define __PLATFORM_EXYNOS5250_UART_H__

#define EXYNOS5250_UART0_BASE	0x12c20000

#define UART_UTRSTAT		0x10				/* TX/RX Status */
#define UART_UTXH		0x20				/* Transmit Buffer */
#define UART_RXH		0x24				/* Receive Buffer */

#define UART_UTRSTAT_TXBE	(1 << 1)			/* Transmit Buffer Empty */

#endif /* __PLATFORM_EXYNOS5250_UART_H__ */
