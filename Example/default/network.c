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

#include <FreeRTOS.h>
#include <mm.h>
#include <sys_print.h>

#include <lwip/tcpip.h>
#include <lwip/sockets.h>

#include "config.h"

extern err_t mango_net_init(struct netif *net_if);
extern void mango_net_input_thread(void *netif);
extern void app_process_msg(uint8_t *data);

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

	_IP4_ADDR((&ip_addr), HOST_IP_ADDR);
	_IP4_ADDR((&gateway), GW_IP_ADDR);
	_IP4_ADDR((&net_mask), HOST_NET_MASK);

	/* Set MAC address */
	net_if.hwaddr_len = 6;
	net_if.hwaddr[0] = HW_ADDR_0;
	net_if.hwaddr[1] = HW_ADDR_1;
	net_if.hwaddr[2] = HW_ADDR_2;
	net_if.hwaddr[3] = HW_ADDR_3;
	net_if.hwaddr[4] = HW_ADDR_4;
	net_if.hwaddr[5] = HW_ADDR_5;

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
	long socket, conn_fd;
	socklen_t addr_size;
	struct sockaddr_in c_addr, s_addr;
 
        mango_print_msg("-> Starting network task\r\n");
 
	socket = lwip_socket(AF_INET, SOCK_STREAM, 0);

	if (socket < 0)
	{
		mango_print_msg("error: failed to create socket\r\n");
		return;
	}

	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_len = sizeof(s_addr);
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(SOCK_PORT);
	s_addr.sin_addr.s_addr = inet_addr(SOCK_IP_ADDR);

	if (lwip_bind(socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1)
	{
		mango_print_msg("error: failed to bind socket\r\n");
		return;
	}

	if (lwip_listen(socket, 5) != 0)
	{
		mango_print_msg("error: failed to listen socket\r\n");
		return;
	}

	addr_size = sizeof(c_addr);

	/* Start TCP server */
	for (;;)
	{
		int32_t bytes;

		/* Flush client address */
		memset(&c_addr, 0, sizeof(c_addr));

		conn_fd = lwip_accept(socket, (struct sockaddr *)&c_addr, &addr_size);

		if (conn_fd > 0)
		{
			mango_print_msg("LwIP: get connection from %d.%d.%d.%d\r\n",
					c_addr.sin_addr.s_addr & 0xff, (c_addr.sin_addr.s_addr >> 8) & 0xff,
					(c_addr.sin_addr.s_addr >> 16) & 0xff, (c_addr.sin_addr.s_addr >> 24) & 0xff);

			/* Main event loop */
			do
			{
				uint8_t buf[MSG_BUF_SIZE];

				bytes = lwip_recv(conn_fd, buf, MSG_BUF_SIZE, 0);
				if (bytes > 0)
				{
					app_process_msg(buf);
				}
			}
			while (bytes > 0);

			mango_print_msg("LwIP: connection closed\r\n");

			lwip_close(conn_fd);
		}
	}
}
