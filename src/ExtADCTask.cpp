/*
 * ExtADCTask.cpp
 *
 *  Created on: 29 sie 2013
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

#include "ExtADCTask.h"

extern xQueueHandle xQueue_I2CQuery;
extern xQueueHandle xQueue_I2CRx;

bool ExtADCTask::init()
{
	extADC.init();
	return true;
}

bool ExtADCTask::taskEntry()
{
	extADC.config(); //TODO: setup LTC2991 for measurement
	return true;
}

bool ExtADCTask::run(void *param)
{
	if (xQueueReceive(xQueue_I2CQuery,&xBuffer_receive,0) == pdPASS)
	{
		uint8_t receive;
		if (extADC.process(xBuffer_receive, &receive)) //TODO: this may fail
		{
			//receiver mode. send answer back
			xQueueSend( xQueue_I2CRx, ( void * ) &receive, 0 );
		}
	}
	taskYIELD(); //task is going to ready state to allow next one to run
	return true;
}
