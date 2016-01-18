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
#include <mango.h>
#include <sys_print.h>

#define MAX_MESSAGE_SIZE	128

#define MSG_TYPE_INFO		0x01

typedef struct __attribute__((__packed__)) {
	uint8_t type;
	uint32_t size;
	uint8_t data[MAX_MESSAGE_SIZE];
} app_msg_t;

void app_process_msg(uint8_t *data)
{
	app_msg_t *msg = (void *)data;

	switch (msg->type)
	{
	case MSG_TYPE_INFO:
		mango_print_msg("\r\ninfo: %s\r\n", msg->data);
		break;
	default:
		mango_print_msg("error: invalid message type (%d)\r\n", msg->type);
		break;
	}
}
