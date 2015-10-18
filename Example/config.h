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

#ifndef __CONFIG_H__
#define __CONFIG_H__

/* Application network interface parameters */
#define HOST_IP_ADDR		192,167,20,2
#define HOST_NET_MASK		255,255,255,0
#define GW_IP_ADDR		192,167,20,1

/* Macro to correctly expand addr macros */
#define _IP4_ADDR(p, addr)	IP4_ADDR(p, addr)

/* Set MAC address to (aa:bb:cc:dd:ee:00) */
#define HW_ADDR_0		0xaa
#define HW_ADDR_1		0xbb
#define HW_ADDR_2		0xcc
#define HW_ADDR_3		0xdd
#define HW_ADDR_4		0xee
#define HW_ADDR_5		0x00

/* IP address to bind the socket */
#define SOCK_IP_ADDR		"192.167.20.2"
#define SOCK_PORT		3310

/* Size of the buffer to store incoming messages */
#define MSG_BUF_SIZE		256

#endif /* __CONFIG_H__ */
