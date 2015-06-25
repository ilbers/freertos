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
#include <io.h>
#include <sys_print.h>
#include <task.h>

void firstTask(void *unused)
{
	sys_print_msg("In first task\r\n");

	for (;;)
	{
		vTaskDelay(1);
		sys_print_msg("1");
	}

	vTaskDelete(NULL);
}

void secondTask(void *unused)
{
	sys_print_msg("In second task\r\n");

	for (;;)
	{
		vTaskDelay(30);
		sys_print_msg("2");
	}

	vTaskDelete(NULL);
}

int main(void)
{
	portBASE_TYPE ret;

	sys_print_msg("Creating first task\r\n");
	ret = xTaskCreate(firstTask,
			  (portCHAR *)"firstTask",
			  4096,
			  NULL,
			  configTIMER_TASK_PRIORITY - 1,
			  NULL);
	if (ret != pdPASS)
	{
		sys_print_msg("Error creating task, status was %d\r\n", ret);
		return 1;
	}

	sys_print_msg("Creating second task\r\n");
	ret = xTaskCreate(secondTask,
			  (portCHAR *) "secondTask",
			  4096,
			  NULL,
			  configTIMER_TASK_PRIORITY - 1,
			  NULL);
	if (ret != pdPASS)
	{
		sys_print_msg("Error creating task, status was %d\r\n", ret);
		return 1;
	}

	sys_print_msg("Starting scheduler\r\n");
	vTaskStartScheduler();

	sys_print_msg("Scheduler exited\r\n");

	return 0;
}
