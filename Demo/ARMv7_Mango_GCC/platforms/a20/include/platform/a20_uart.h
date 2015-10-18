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

#ifndef __PLATFORM_A20_UART_H__
#define __PLATFORM_A20_UART_H__

#define A20_UART0_BASE		0x01c28000

#define A20_UART_THR		0x00		/* Transmit Holding Register */
#define A20_UART_RBR		0x00		/* Receive Buffer Register */
#define A20_UART_LSR		0x14		/* Line Status Register */

#define A20_UART_LSR_THRE	(1 << 5)	/* TX Holding Register Empty */

#define EARLY_UART_BASE		A20_UART0_BASE

#endif /* __PLATFORM_A20_UART_H__ */
