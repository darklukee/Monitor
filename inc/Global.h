/*
 * Global.h
 *
 *  Created on: 10 wrz 2013
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

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "stm32f4xx.h"

#include "FreeRTOS.h"
#include "semphr.h"

enum GlobalSwitch
{
	//keep order with gsTable[]
	eLcdMenu = 0,
	eUsbPresent = 1
};

class Global
{
public:
	static Global& getInstance()
	{
		static Global instance;
		return instance;
	}
	uint8_t getSwitch(GlobalSwitch gSwitch);
	bool setSwitch(GlobalSwitch gSwitch, uint8_t value);

private:
	Global()
	{
		xMutex = xSemaphoreCreateMutex();
		gsLcdMenu = (uint8_t) false;
		gsUsbPresent = (uint8_t) false;
	}
	Global(Global const&); // Don't Implement.
	void operator=(Global const&); // Don't implement

	//Mutex
	xSemaphoreHandle xMutex;

	// Switches
	uint8_t gsLcdMenu;
	uint8_t gsUsbPresent;
};

#endif /* GLOBAL_H_ */
