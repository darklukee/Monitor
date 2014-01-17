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
#include <string.h>
#include "usbh_usr.h"
#include "DataStructures.h"

extern xQueueHandle xQueue_Storage;
extern xSemaphoreHandle xSemaphore_UsbMutex;

bool UsbTask::connected = false;
bool UsbTask::enabled = true;

UsbTask::UsbTask() :
	scheduler_task("UsbTask", 1024 * 10, PRIORITY_LOW, NULL)
{
	// TODO Auto-generated constructor stub
	lastSave = 0;
	//freq = 20;
	state = UsbTaskInit;
	strcpy(fileName, "test_file.txt");
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
	FRESULT fr;
	static portTickType initTime;
	if ( xSemaphoreTake( xSemaphore_UsbMutex, ( portTickType ) 100 ) == pdTRUE)
	{
		switch (state)
		{
		case UsbTaskInit:
			if (isOk())
			{
				//vTaskDelay(OS_MS(100));
				//TODO: dynamic filename, csv header
				fr = f_open(&file, fileName, FA_CREATE_ALWAYS | FA_WRITE); //TODO: change to FA_OPEN_ALWAYS and f_lseek
				if (fr == FR_OK)
				{
					state = UsbTaskRun;
					initTime = xTaskGetTickCount();
					xQueueReset(xQueue_Storage);
				}
			}
			else
			{
				xSemaphoreGive( xSemaphore_UsbMutex );
				vTaskDelay(OS_MS(500));
			}
			break;
		case UsbTaskRun:
			if (isOk())
			{
				StorageData data;
				while (xQueueReceive(xQueue_Storage, &data, 50) && isOk()) //wait for 10 ticks
				{
					const unsigned int size = 64;
					char buf[size];
//				static int iter = 0;
					unsigned int len = sprintf(buf, "%lu,%f,%f,%lu\r\n", data.timeStamp - initTime, data.voltage, data.current,
						uxQueueMessagesWaiting(xQueue_Storage));
					if (len <= size)
					{
						UINT bw;
						fr = f_write(&file, buf, len, &bw); //TODO: dynamic size?
						if (fr != FR_OK || bw < len)
						{
							state = UsbTaskDeInit;
							setEnabled(false);
						}
						if (lastSave + 5000 < xTaskGetTickCount()) //save file every 5s
						{
							f_sync(&file);
							lastSave = xTaskGetTickCount();
						}
					}
				}
			}
			else
			{
				state = UsbTaskDeInit;
			}
			break;
		case UsbTaskDeInit:
			//vTaskDelay(OS_MS(200));
			f_close(&file);
			state = UsbTaskInit;
			break;
		default:
			state = UsbTaskInit;
			break;
		}
		xSemaphoreGive( xSemaphore_UsbMutex );
	}
	return true;
}

//bool UsbTask::isConnected()
//{
//	return connected;
//}
//
//void UsbTask::setConnected(bool _connected)
//{
//	connected = _connected;
//}
//
//bool UsbTask::isEnabled()
//{
//	return enabled;
//}
//
//void UsbTask::setEnabled(bool _enabled)
//{
//	enabled = _enabled;
//}
//
//void UsbTask::toggleEnabled()
//{
//	enabled ^= true;
//}

//C extern functions wrappers
extern "C"
{
void UsbTaskToggleEnabled()
{
	UsbTask::toggleEnabled();
}
void UsbTaskSetConnected(bool _connected)
{
	UsbTask::setConnected(_connected);
}
}
