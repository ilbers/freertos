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

#include <FreeRTOS.h>
#include <mm.h>
#include <sys_print.h>

#include <lwip/tcpip.h>
#include <lwip/sockets.h>

extern err_t mango_net_init(struct netif *net_if);
extern void mango_net_input_thread(void *netif);

void status_callback(struct netif *net_if)
{
        if (netif_is_up(net_if) != 0)
	{
		mango_print_msg("lwIP: network is up\r\n");
		mango_print_msg("lwIP: addr %d.%d.%d.%d\r\n",
				net_if->ip_addr.addr & 0xff, (net_if->ip_addr.addr >> 8) & 0xff,
				(net_if->ip_addr.addr >> 16) & 0xff, (net_if->ip_addr.addr >> 24) & 0xff);
	}
	else
	{
		mango_print_msg("lwIP: network is down\r\n");
	}

}

void lwip_app_init(void *args)
{
	ip_addr_t ip_addr, net_mask, gateway;
	static struct netif net_if;

	mango_print_msg("lwIP: setup networking stack\r\n");

	/* Set up the network interface */
	ip_addr_set_zero(&gateway);
	ip_addr_set_zero(&ip_addr);
	ip_addr_set_zero(&net_mask);

	IP4_ADDR((&ip_addr), 192, 167, 20, 2);
	IP4_ADDR((&gateway), 192, 167, 20, 1);
	IP4_ADDR((&net_mask), 255, 255, 255, 0);

	/* Set MAC address */
	net_if.hwaddr_len = 6;
	net_if.hwaddr[ 0 ] = 0xaa;
	net_if.hwaddr[ 1 ] = 0xbb;
	net_if.hwaddr[ 2 ] = 0xcc;
	net_if.hwaddr[ 3 ] = 0xdd;
	net_if.hwaddr[ 4 ] = 0xee;
	net_if.hwaddr[ 5 ] = 0x00;

	netif_set_default(
			  netif_add(
				    &net_if,
				    &ip_addr,
				    &net_mask,
				    &gateway,
				    NULL,
				    mango_net_init,
				    tcpip_input
				   )
			  );
	netif_set_status_callback(&net_if, status_callback);
	netif_set_up(&net_if);

	sys_thread_new("lwIP_In",
		       mango_net_input_thread,
		       &net_if,
		       4096,
		       configTIMER_TASK_PRIORITY - 1);
}

static void vProcessConnection(struct netconn *pxNetCon)
{
	mango_print_msg("LwIP: connection to port 80 established.\r\n");
}

void networkTask(void *unused)
{
	struct netconn *pxHTTPListener, *pxNewConnection;

	mango_print_msg("-> Starting network task\r\n");

	/* Create a new tcp connection handle */
	pxHTTPListener = netconn_new(NETCONN_TCP);
	netconn_bind(pxHTTPListener, NULL, 80);
	netconn_listen(pxHTTPListener);

	for (;;)
	{
		/* Wait for connection. */
		netconn_accept(pxHTTPListener, &pxNewConnection);

		if(pxNewConnection != NULL)
		{
			/* Service connection. */
			vProcessConnection(pxNewConnection);
			while (netconn_delete(pxNewConnection) != ERR_OK)
			{
				vTaskDelay(10);
			}
		}
	}
}
