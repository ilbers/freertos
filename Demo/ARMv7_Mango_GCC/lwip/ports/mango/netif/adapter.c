/*
 * Copyright (C) 2015-2016 ilbers GmbH
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

#define MANGO_LWIP_NET_IRQ_NR	140		/* Network interface IRQ */
#define MANGO_DEST_PARTITION	0		/* Partition ID to connect to */
#define MANGO_NET_IFACE		0		/* Use Mango interface #0 */

#define MANGO_MAX_FRAME_SIZE	1500

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
		/* full packet send to tcpip_thread to process */
		if (this_netif->input(p, this_netif) != ERR_OK)
		{
			mango_print_msg("error: IP input error\r\n");
			pbuf_free(p);
			p = NULL;
		}

		break;
	default:
		/* Unknown packet type */
		pbuf_free(p);
		p = NULL;

		break;
	}
}

static void mango_net_irq(void)
{
	struct pbuf *p;
	int ret, size;

	if (!this_netif)
	{
		mango_print_msg("error: RX processing failed, lwIP stack is down\r\n");
		return;
	}

	for (;;)
	{
		/* get frame size */
		size = mango_net_get_rx_size(MANGO_NET_IFACE);
		if (size == 0)
		{
			/* Buffer is empty */
			return;
		}

		/* allocate new pbuffer */
		p = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);

		/* move received packet into a new pbuf */
		ret = mango_net_rx(MANGO_NET_IFACE, p->payload, size);
		if (ret)
		{
			mango_print_msg("error: net RX failed, code (%d)\r\n", ret);
			pbuf_free(p);

			return;
		}

		/* mango_print_msg("LwIP: RX (%d) bytes\r\n", n); */

		if (pq_enqueue(rx_queue, (void *)p) < 0)
		{
			mango_print_msg("error: failed to queue RX packet\r\n");
			pbuf_free(p);
		}
	}
}

void mango_net_input_thread(void *netif)
{
	int ret;
	struct pbuf *p;
	SYS_ARCH_DECL_PROTECT(lev);

	mango_print_msg("lwIP: starting RX thread\r\n");

	/* Register IRQ for incomming data channel data */
	register_irq_handler(MANGO_LWIP_NET_IRQ_NR, mango_net_irq);

	/* Open Mango data channel */
	ret = mango_net_open(MANGO_NET_IFACE);
	if (ret)
	{
		mango_print_msg("error: failed to open network interface%d\r\n");
		return;
	}

	gic_set_irq_priority(MANGO_LWIP_NET_IRQ_NR, configTICK_PRIORITY << portPRIORITY_SHIFT);
	enable_one_irq(MANGO_LWIP_NET_IRQ_NR);

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
			low_level_input(p);
		}
	}
}

static err_t low_level_init(struct netif *netif)
{

	netif->mtu = MANGO_MAX_FRAME_SIZE;
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
	unsigned char buf[MANGO_MAX_FRAME_SIZE + SIZEOF_ETH_HDR + 4];
	uint32_t len = p->tot_len, offset;
	struct pbuf* q;

	offset = 0;

	/* compose a packet from buffers */
	for (q = p; q != NULL; q = q->next)
	{
		/* fill data */
		memcpy(buf + offset, q->payload, q->len);
		offset += q->len;
	}

	/* send frame */
	mango_net_tx(MANGO_NET_IFACE, MANGO_DEST_PARTITION, buf, len);

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
	netif->name[0] = 'i';
	netif->name[1] = '0';
	netif->output = mango_if_output;
	netif->linkoutput = low_level_output;
	low_level_init(netif);

	return ERR_OK;
}
