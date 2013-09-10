/*
 * Global.cpp
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

#include "Global.h"

uint8_t Global::getSwitch(GlobalSwitch gSwitch)
{
	if ( xSemaphoreTake( xMutex, ( portTickType ) 10 ) == pdTRUE)
	{
		uint8_t retVal;
		switch (gSwitch)
		{
		case eLcdMenu:
			retVal = gsLcdMenu;
			break;
		case eUsbPresent:
			retVal = gsUsbPresent;
			break;
		default:
			retVal = 0xff;
			break;
		}
		xSemaphoreGive(xMutex);
		return retVal;
	}
	else
	{
		//something went terribly wrong
		//throw?
		return 0xff;
	}
}
bool Global::setSwitch(GlobalSwitch gSwitch, uint8_t value)
{
	if ( xSemaphoreTake( xMutex, ( portTickType ) 10 ) == pdTRUE)
	{
		switch (gSwitch)
		{
		case eLcdMenu:
			gsLcdMenu = value;
			break;
		case eUsbPresent:
			gsUsbPresent = value;
			break;
		default:
			break;
		}
		xSemaphoreGive(xMutex);
		return true;
	}
	else
	{
		//something went terribly wrong
		//throw?
		return false;
	}
}

