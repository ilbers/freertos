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

#include <FreeRTOS.h>
#include <interrupt.h>
#include <mango.h>
#include <sys_print.h>

#define CHANNEL_ID	0	/* Channel Id to use */
#define DEST_PART_ID	0	/* Partition to connect to (Linux) */
#define DC_IRQ_NR	130	/* Data channel interrupt nr */

#define BUFF_SIZE	64	/* Size of communication buffer */

static void dc_irq_handler(void)
{
	uint32_t len;
	char buf[BUFF_SIZE];

	/* Get message */
	len = mango_dc_read(CHANNEL_ID, (void *)buf, BUFF_SIZE);

	/* Send echo reply */
	mango_dc_write(CHANNEL_ID, (void *)buf, len);
}

void dcTask(void *unused)
{
	uint32_t ret;

	mango_print_msg("-> Starting data-channel task\r\n");

	ret = mango_dc_open(CHANNEL_ID, DEST_PART_ID);
	if (ret)
	{
		mango_print_msg("error: failed to open channel #%d\r\n", CHANNEL_ID);
		return;
	}

	register_irq_handler(DC_IRQ_NR, dc_irq_handler);

	/* Start echo server */
	enable_one_irq(DC_IRQ_NR);

	for (;;);
}
