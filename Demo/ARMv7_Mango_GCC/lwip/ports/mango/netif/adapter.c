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

/* This function is called by the TCP/IP stack when an IP packet
 * should be sent. It calls the function called low_level_output()
 * to do the actual transmission of the packet.
 */

#include <sys_print.h>
#include <netif/etharp.h>
#include <netif/pqueue.h>
#include <lwip/mem.h>
#include "lwip/sys.h"
#include <FreeRTOSConfig.h>
#include <portmacro.h>
#include <mm.h>

#include <interrupt.h>
#include <mango.h>

#define MANGO_LWIP_DC_CHANNEL	1				/* Channel ID to be used for networking */
#define MANGO_LWIP_DC_IRQ_NR	(130 + MANGO_LWIP_DC_CHANNEL)	/* Data channel IRQ */
#define MANGO_DEST_PARTITION	0				/* Partition ID to connect to */

#define MANGO_MAX_FRAME_SIZE	512

static struct netif *this_netif = 0;
static pq_queue_t *rx_queue = 0;

static void low_level_input(struct pbuf *p)
{
	struct eth_hdr *ethhdr;

	/* points to packet payload, which starts with an Ethernet header */
	ethhdr = p->payload;

	switch (htons(ethhdr->type))
	{
	/* IP or ARP packet? */
	case ETHTYPE_IP:
	case ETHTYPE_ARP:
		/* mango_print_msg(" -> Got IP/ARP packet\r\n"); */

		/* full packet send to tcpip_thread to process */
		if (this_netif->input(p, this_netif) != ERR_OK)
		{
			mango_print_msg("error: IP input error\r\n");
			pbuf_free(p);
			p = NULL;
		}

		break;
	default:
		mango_print_msg(" -> Got unknown packet\r\n");

		pbuf_free(p);
		p = NULL;

		break;
	}
}

static void mango_dc_irq(void)
{
	struct pbuf *p;
	int n, size;

	if (!this_netif)
	{
		mango_print_msg("error: RX processing failed, LwIP stack is down\r\n");
		return;
	}

	/* get frame size */
	n = mango_dc_read(MANGO_LWIP_DC_CHANNEL, (void *)&size, 4);
	if (n != 4)
	{
		mango_print_msg("error: dc sync failed, requested (%d) - got (%d)\r\n",
				4, n);
		return;
	}

	/* allocate new pbuffer */
	p = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);

	/* move received packet into a new pbuf */
	n = mango_dc_read(MANGO_LWIP_DC_CHANNEL, p->payload, size);
	if (n != size)
	{
		mango_print_msg("error: dc sync failed, requested (%d) - got (%d)\r\n",
				size, n);
		pbuf_free(p);
		return;
	}

	/* mango_print_msg("LwIP: RX, got %d bytes\r\n", n); */

	if (pq_enqueue(rx_queue, (void *)p) < 0)
	{
		mango_print_msg("error: failed to queue RX packet\r\n");
		pbuf_free(p);
	}
}

void mango_net_input_thread(void *netif)
{
	int ret;
	struct pbuf *p;
	SYS_ARCH_DECL_PROTECT(lev);

	mango_print_msg("LwIP: starting RX thread\r\n");

	/* Register IRQ for incomming data channel data */
	register_irq_handler(MANGO_LWIP_DC_IRQ_NR, mango_dc_irq);

	/* Open Mango data channel */
	ret = mango_dc_open(MANGO_LWIP_DC_CHANNEL, MANGO_DEST_PARTITION);
	if (ret)
	{
		mango_print_msg("error: failed to open data channel #%d\r\n",
				MANGO_LWIP_DC_CHANNEL);
		return;
	}

	gic_set_irq_priority(MANGO_LWIP_DC_IRQ_NR, configTICK_PRIORITY << portPRIORITY_SHIFT);
	enable_one_irq(MANGO_LWIP_DC_IRQ_NR);

	for (;;)
	{
		SYS_ARCH_PROTECT(lev);
		/* See if there is data to process */
		if (pq_qlength(rx_queue) == 0)
		{
			p = 0;
		}
		else
		{
			/* Return one packet from receive q */
			p = (struct pbuf *)pq_dequeue(rx_queue);
		}
		SYS_ARCH_UNPROTECT(lev);

		if (p)
		{
			mango_print_msg("LwIP: got a packet, process it\r\n");
			low_level_input(p);
		}
	}
}

static err_t low_level_init(struct netif *netif)
{

	netif->mtu = 256;
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

	/* Store network interface pointer for RX routine */
	this_netif = netif;

	rx_queue = pq_create_queue();
	if (!rx_queue)
	{
		return ERR_MEM;
	}

	return ERR_OK;
}

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
	unsigned char buf[MANGO_MAX_FRAME_SIZE];

	mango_print_msg("LwIP: TX (%d) bytes\r\n", p->len);

	if (mando_dc_tx_free_space(MANGO_LWIP_DC_CHANNEL) < p->len)
	{
		mango_print_msg("error: TX failed, no space left in data channel\r\n");
		return ERR_MEM;
	}

	/* set frame size in header */
	memcpy(buf, (void *)&p->len, 4);
	/* fill data */
	memcpy(buf + 4, p->payload, p->len);

	/* send frame */
	mango_dc_write(MANGO_LWIP_DC_CHANNEL, buf, p->len + 4);

	return ERR_OK;
}

static err_t mango_if_output(struct netif *netif,
			     struct pbuf *p,
			     struct ip_addr *ipaddr)
{
	/* resolve hardware address, then send (or queue) packet */
	return etharp_output(netif, p, ipaddr);
}


err_t mango_net_init(struct netif *netif)
{
	mango_print_msg("adapter init\r\n");

	netif->name[0] = 'i';
	netif->name[1] = '0';
	netif->output = mango_if_output;
	netif->linkoutput = low_level_output;
	low_level_init(netif);

	return ERR_OK;
}