/*
 * UsbTask.cpp
 *
 *  Created on: 19 lis 2013
 *      Author: lukee
 *
 * Copyright (C) 2013  darklukee
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see {http://www.gnu.org/licenses/}.
 */
#include <UsbTask.h>
#include <stdio.h>
#include "usbh_usr.h"

UsbTask::UsbTask() :
	scheduler_task("UsbTask", 1024 * 10, PRIORITY_LOW, NULL)
{
	// TODO Auto-generated constructor stub
	iter = 0;
}

bool UsbTask::init()
{
	return true;
}

bool UsbTask::taskEntry()
{
	return true;
}

bool UsbTask::run(void *param)
{
	char folderName[] = "lukee";
	char buf[20] = "";
	sprintf(buf, "%s_%03d", folderName, iter++);
	f_mkdir(buf);
	vTaskDelay(OS_MS(5000));
	return true;
}
