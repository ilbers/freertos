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
#include <io.h>
#include <mango.h>
#include <sys_print.h>
#include <task.h>

#include <lwip/tcpip.h>

extern void dcTask(void *unused);
extern void networkTask(void *unused);
extern void lwip_app_init(void *args);

void watchdogTask(void *unused)
{
	mango_print_msg("-> Starting watchdog task\r\n");

	mango_watchdog_start();

	for (;;)
	{
		mango_watchdog_ping();

		/* Trigger watchdog every 5 seconds */
		vTaskDelay(configTICK_RATE_HZ * 5);
	}

	vTaskDelete(NULL);
}

void counterTask(void *unused)
{
	uint32_t count = 0;

	mango_print_msg("-> Starting counter task\r\n");

	for (;;)
	{
		mango_print_msg("Run time: %d seconds\r", count++);

		vTaskDelay(configTICK_RATE_HZ);
	}

	vTaskDelete(NULL);
}

int main(void)
{
	portBASE_TYPE ret;

	sys_print_msg("\r\nRunning in partition #%d\r\n", mango_get_partition_id());

	/* Start watchdog */
	ret = xTaskCreate(watchdogTask,
			  (portCHAR *)"watchdogTask",
			  4096,
			  NULL,
			  configTIMER_TASK_PRIORITY - 1,
			  NULL);
	if (ret != pdPASS)
	{
		sys_print_msg("Error creating watchdog task, code (%d)\r\n", ret);
		return 1;
	}

	/* Start demo counter */
	ret = xTaskCreate(counterTask,
			  (portCHAR *) "counterTask",
			  4096,
			  NULL,
			  configTIMER_TASK_PRIORITY - 1,
			  NULL);
	if (ret != pdPASS)
	{
		sys_print_msg("Error creating counter task, code (%d)\r\n", ret);
		return 1;
	}

	/* Start network application */
	ret = xTaskCreate(networkTask,
			  (portCHAR *) "networkTask",
			  4096,
			  NULL,
			  configTIMER_TASK_PRIORITY - 1,
			  NULL);
	if (ret != pdPASS)
	{
		sys_print_msg("Error creating network task, code (%d)\r\n", ret);
		return 1;
	}

	/* Start data-channel application */
	ret = xTaskCreate(dcTask,
			  (portCHAR *) "dcTask",
			  4096,
			  NULL,
			  configTIMER_TASK_PRIORITY - 1,
			  NULL);
	if (ret != pdPASS)
	{
		sys_print_msg("Error creating data-channel task, code (%d)\r\n", ret);
		return 1;
	}

	/* Setup lwIP stack */
	tcpip_init(lwip_app_init, NULL);

	sys_print_msg("Partition start time: %d mS\r\n", mango_partition_run_time());

	sys_print_msg("Starting FreeRTOS scheduler\r\n");
	vTaskStartScheduler();

	sys_print_msg("Scheduler exited\r\n");

	return 0;
}
