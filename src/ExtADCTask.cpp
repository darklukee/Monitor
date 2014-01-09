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

//extern xQueueHandle xQueue_I2CQuery;
//extern xQueueHandle xQueue_I2CRx;
extern xQueueHandle xQueue_AdcData;

ExtADCTask::ExtADCTask() :
	scheduler_task("ExtADCTask", 1024, PRIORITY_MEDIUM, NULL)
{
	for (int i = 0; i < maxReceiveBuffer; i++)
	{
		receiveBuffer[i].dir = DirRead;
	}
}

bool ExtADCTask::init()
{
	extADC.init();
	return true;
}

bool ExtADCTask::taskEntry()
{
	return extADC.config(); //TODO: setup LTC2991 for measurement
}

bool ExtADCTask::run(void *param)
{
	uint8_t stat = extADC.getStatus();
	if (stat != 0)
	{
		//TODO: get only values that are available  (based ona stat)
		//TODO: reduce data payload by getting only necessary fields

		receiveBuffer[0].length = maxI2cBuff;
		receiveBuffer[0].reg = EXTADC_REG_V1;
		receiveBuffer[1].length = maxI2cBuff;
		receiveBuffer[1].reg = EXTADC_REG_V5;
		receiveBuffer[2].length = 4;
		receiveBuffer[2].reg = EXTADC_REG_TINT;

		for (int i = 0; i < maxReceiveBuffer; i++)
		{
			extADC.getData(receiveBuffer[i]);
		}

		AdcData outputData;
		outputData.length = receiveBuffer[0].length + receiveBuffer[1].length + receiveBuffer[2].length;
		outputData.stat = stat;
		//TODO: add time stamp

		int j = 0;
		for (int i = 0; i < receiveBuffer[0].length; i++)
		{
			outputData.values[j + i] = receiveBuffer[0].val[i];
		}
		j += receiveBuffer[0].length;
		for (int i = 0; i < receiveBuffer[1].length; i++)
		{
			outputData.values[j + i] = receiveBuffer[1].val[i];
		}
		j += receiveBuffer[1].length;
		for (int i = 0; i < receiveBuffer[2].length; i++)
		{
			outputData.values[j + i] = receiveBuffer[2].val[i];
		}

		//send output data;
		xQueueSend(xQueue_AdcData, (void * ) &outputData, (portTickType ) 0);
	}

	taskYIELD(); //task is going to ready state to allow next one to run
	return true;
}
