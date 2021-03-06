/*
 * Copyright (C) 2015-2016 ilbers GmbH
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

#include <stdint.h>

/* Mango hypercall identifiers, should be the same as in Mango core */
#define MANGO_HVC_AUTH			0x01

#define MANGO_HVC_DC_OPEN		0x10
#define MANGO_HVC_DC_WRITE		0x11
#define MANGO_HVC_DC_READ		0x12
#define MANGO_HVC_DC_CLOSE		0x13
#define MANGO_HVC_DC_TX_FREE_SPACE	0x14
#define MANGO_HVC_DC_RESET		0x15

#define MANGO_HVC_PARTITION_ID		0x20
#define MANGO_HVC_PARTITION_RESET	0x21
#define MANGO_HVC_PARTITION_RUN_TIME	0x22

#define MANGO_HVC_WD_START		0x30
#define MANGO_HVC_WD_STOP		0x31
#define MANGO_HVC_WD_PING		0x32
#define MANGO_HVC_WD_SET_TIMEOUT	0x33

#define MANGO_HVC_CONSOLE_WRITE		0x40

#define MANGO_HVC_DEBUG			0x50

#define MANGO_HVC_NET_OPEN		0x61
#define MANGO_HVC_NET_SET_MODE		0x62
#define MANGO_HVC_NET_TX		0x63
#define MANGO_HVC_NET_RX		0x64
#define MANGO_HVC_NET_CLOSE		0x65
#define MANGO_HVC_NET_RX_SIZE		0x66
#define MANGO_HVC_NET_RESET		0x67

#define mango_hypervisor_call_0(nr)					\
({									\
	register uint32_t _ret;						\
	asm volatile ("hvc	%1\r\n"					\
		      "mov	%0, r0\r\n"				\
		      : "=r"(_ret)					\
		      : "I" (nr)					\
		      : "cc");						\
	_ret;								\
})

#define mango_hypervisor_call_1(nr, arg1)				\
({									\
	register uint32_t _ret;						\
	asm volatile ("mov	r0, %2\r\n"				\
		      "hvc	%1\r\n"					\
		      "mov	%0, r0\r\n"				\
		      : "=r"(_ret)					\
		      : "I" (nr), "r" (arg1)				\
		      : "cc");						\
	_ret;								\
})

#define mango_hypervisor_call_2(nr, arg1, arg2)				\
({									\
	register uint32_t _ret;						\
	asm volatile ("mov	r0, %2\r\n"				\
		      "mov	r1, %3\r\n"				\
		      "hvc	%1\r\n"					\
		      "mov	%0, r0\r\n"				\
		      : "=r"(_ret)					\
		      : "I" (nr), "r" (arg1), "r" (arg2)		\
		      : "cc");						\
	_ret;								\
})

#define mango_hypervisor_call_3(nr, arg1, arg2, arg3)			\
({									\
	register uint32_t _ret;						\
	asm volatile ("mov	r0, %2\r\n"				\
		      "mov	r1, %3\r\n"				\
		      "mov	r2, %4\r\n"				\
		      "hvc	%1\r\n"					\
		      "mov	%0, r0\r\n"				\
		      : "=r"(_ret)					\
		      : "I" (nr), "r" (arg1), "r" (arg2), "r" (arg3)	\
		      : "cc");						\
	_ret;								\
})

#define mango_hypervisor_call_4(nr, arg1, arg2, arg3, arg4)		\
({									\
	register uint32_t _ret;						\
	asm volatile ("mov	r0, %2\r\n"				\
		      "mov	r1, %3\r\n"				\
		      "mov	r2, %4\r\n"				\
		      "mov	r3, %5\r\n"				\
		      "hvc	%1\r\n"					\
		      "mov	%0, r0\r\n"				\
		      : "=r"(_ret)					\
		      : "I" (nr), "r" (arg1), "r" (arg2), "r" (arg3),	\
			"r" (arg4)					\
		      : "cc");						\
	_ret;								\
})

/***********************************/
/*         Mango Core API          */
/***********************************/
uint32_t mango_unlock(unsigned char *token)
{
	uint32_t ret;

	ret = mango_hypervisor_call_1(MANGO_HVC_AUTH, token);

	return ret;
}

/***********************************/
/*     Mango Data Channel API      */
/***********************************/
uint32_t mango_dc_open(uint32_t ch, uint32_t dest)
{
	uint32_t ret;

	ret = mango_hypervisor_call_2(MANGO_HVC_DC_OPEN, ch, dest);

	return ret;
}

uint32_t mango_dc_write(uint32_t ch, const uint8_t *p, uint32_t len)
{
	uint32_t ret;

	ret =  mango_hypervisor_call_3(MANGO_HVC_DC_WRITE,
				       ch,
				       (uint32_t)p,
				       len);

	return ret;
}

uint32_t mango_dc_read(uint32_t ch, uint8_t *p, uint32_t len)
{
	uint32_t ret;

	ret =  mango_hypervisor_call_3(MANGO_HVC_DC_READ,
				       ch,
				       (uint32_t)p,
				       len);

	return ret;
}

uint32_t mango_dc_close(uint32_t ch)
{
	uint32_t ret;

	ret = mango_hypervisor_call_1(MANGO_HVC_DC_CLOSE, ch);

	return ret;
}

uint32_t mango_dc_tx_free_space(uint32_t ch)
{
	uint32_t ret;

	ret = mango_hypervisor_call_1(MANGO_HVC_DC_TX_FREE_SPACE, ch);

	return ret;
}

uint32_t mango_dc_reset(uint32_t ch)
{
	uint32_t ret;

	ret = mango_hypervisor_call_1(MANGO_HVC_DC_RESET, ch);

	return ret;
}

/*******************************************/
/*     Mango Partition Management API      */
/*******************************************/
uint32_t mango_get_partition_id(void)
{
	uint32_t ret;

	ret = mango_hypervisor_call_0(MANGO_HVC_PARTITION_ID);

	return ret;
}

uint32_t mango_partition_reset(void)
{
	uint32_t ret;

	ret = mango_hypervisor_call_0(MANGO_HVC_PARTITION_RESET);

	return ret;
}

uint32_t mango_partition_run_time(void)
{
	uint32_t ret;

	ret = mango_hypervisor_call_0(MANGO_HVC_PARTITION_RUN_TIME);

	return ret;
}

/*******************************/
/*     Mango Watchdog API      */
/*******************************/
uint32_t mango_watchdog_start(void)
{
	uint32_t ret;

	ret = mango_hypervisor_call_0(MANGO_HVC_WD_START);

	return ret;
}

uint32_t mango_watchdog_ping(void)
{
	uint32_t ret;

	ret = mango_hypervisor_call_0(MANGO_HVC_WD_PING);

	return ret;
}

uint32_t mango_watchdog_set_timeout(uint32_t timeout)
{
	uint32_t ret;

	ret = mango_hypervisor_call_1(MANGO_HVC_WD_SET_TIMEOUT, timeout);

	return ret;
}

/*******************************/
/*     Mango console API       */
/*******************************/
uint32_t mango_console_write(uint8_t *buff, uint32_t size)
{
	uint32_t ret;

	ret = mango_hypervisor_call_2(MANGO_HVC_CONSOLE_WRITE, buff, size);

	return ret;
}

/*******************************/
/*      Mango debug API        */
/*******************************/
uint32_t mango_debug_call(uint32_t code)
{
	uint32_t ret;

	ret = mango_hypervisor_call_1(MANGO_HVC_DEBUG, code);

	return ret;
}

/*********************************/
/*     Mango Networking API      */
/*********************************/
uint32_t mango_net_open(uint32_t iface)
{
	uint32_t ret;

	ret = mango_hypervisor_call_1(MANGO_HVC_NET_OPEN, iface);

	return ret;
}

uint32_t mango_net_tx(uint32_t iface, uint32_t dest, const uint8_t *p, uint32_t len)
{
	uint32_t ret;

	ret =  mango_hypervisor_call_4(MANGO_HVC_NET_TX,
				       iface,
				       dest,
				       (uint32_t)p,
				       len);

	return ret;
}

uint32_t mango_net_rx(uint32_t iface, uint8_t *p, uint32_t len)
{
	uint32_t ret;

	ret =  mango_hypervisor_call_3(MANGO_HVC_NET_RX,
				       iface,
				       (uint32_t)p,
				       len);

	return ret;
}

uint32_t mango_net_close(uint32_t iface)
{
	uint32_t ret;

	ret = mango_hypervisor_call_1(MANGO_HVC_NET_CLOSE, iface);

	return ret;
}

uint32_t mango_net_set_mode(uint32_t iface, uint32_t mode)
{
	uint32_t ret;

	ret = mango_hypervisor_call_2(MANGO_HVC_NET_SET_MODE, iface, mode);

	return ret;
}

uint32_t mango_net_get_rx_size(uint32_t iface)
{
	uint32_t ret;

	ret = mango_hypervisor_call_1(MANGO_HVC_NET_RX_SIZE, iface);

	return ret;
}

uint32_t mango_net_reset(uint32_t iface)
{
	uint32_t ret;

	ret = mango_hypervisor_call_1(MANGO_HVC_NET_RESET, iface);

	return ret;
}
