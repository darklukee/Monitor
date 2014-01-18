/*
 * UsbHostTask.h
 *
 *  Created on: 7 gru 2013
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
#ifndef USBHOSTTASK_H_
#define USBHOSTTASK_H_

#include <cpp_task.hpp>

class UsbHostTask: public scheduler_task
{
public:
	UsbHostTask();
	bool init();
	bool taskEntry();
	bool run(void *param);

	//usb specific
	static void TIM_LED_Config(void);
};

#endif /* USBHOSTTASK_H_ */
