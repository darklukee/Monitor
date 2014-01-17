/*
 * UsbTask.h
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
#ifndef USBTASK_H_
#define USBTASK_H_

#include <cpp_task.hpp>
extern "C"
{
#include "ff.h"
}
extern xQueueHandle xQueue_Lcd_Log;

enum UsbTaskState
{
	UsbTaskInit, UsbTaskRun, UsbTaskDeInit
};

class UsbTask: public scheduler_task
{
public:
	UsbTask();
	bool init();
	bool taskEntry();
	bool run(void *param);

	static bool isConnected(void)
	{
		return connected;
	}
	static void setConnected(bool _connected)
	{
		connected = _connected;
		if (connected)
			xQueueSend(xQueue_Lcd_Log, (void * ) &("usb connected"), (portTickType ) 0);
		else
			xQueueSend(xQueue_Lcd_Log, (void * ) &("usb disconnected"), (portTickType ) 0);
	}
	static bool isEnabled(void)
	{
		return enabled;
	}
	static void setEnabled(bool _enabled)
	{
		xQueueSend(xQueue_Lcd_Log, (void * ) &("usb setEnabled"), (portTickType ) 0);
		enabled = _enabled;
	}
	static void toggleEnabled(void)
	{
		enabled ^= true;
		if (enabled)
			xQueueSend(xQueue_Lcd_Log, (void * ) &("usb Enabled"), (portTickType ) 0);
		else
			xQueueSend(xQueue_Lcd_Log, (void * ) &("usb Disabled"), (portTickType ) 0);
	}
	static bool isOk(void)
	{
		return enabled && connected;
	}

private:
	unsigned long lastSave;
	int freq;
	static bool connected;
	static bool enabled;
	FIL file; //FatFs file object
	char fileName[32];
	UsbTaskState state;
};

#endif /* USBTASK_H_ */
