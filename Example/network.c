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

	mango_print_msg("#### lwIP stack init ####\r\n");

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

void networkTask(void *unused)
{
	long socket;
	struct sockaddr_in addr;

	mango_print_msg("-> Starting network task\r\n");

	socket = lwip_socket(AF_INET, SOCK_STREAM, 0);

	if (socket >= 0)
	{
		memset(&addr, 0, sizeof(addr));
		addr.sin_len = sizeof(addr);
		addr.sin_family = AF_INET;
		addr.sin_port = 1234;
		addr.sin_addr.s_addr = inet_addr("192.167.20.1");

		lwip_connect(socket, (struct sockaddr *)&addr, sizeof(addr));
	}

	for (;;)
	{
		vTaskDelay(configTICK_RATE_HZ * 5);
	}
}
