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
#include "hw_config.h"

//extern xQueueHandle xQueue_I2CQuery;
//extern xQueueHandle xQueue_I2CRx;
extern xQueueHandle xQueue_AdcData;

ExtADCTask::ExtADCTask() :
	scheduler_task("ExtADCTask", 1024 * 2, PRIORITY_MEDIUM, NULL)
{
	for (int i = 0; i < maxReceiveBuffer; i++)
	{
		receiveBuffer[i].dir = DirRead;
	}
}

bool ExtADCTask::init()
{
	extADC.init();
	prvSerialOE_Config();
	return true;
}

bool ExtADCTask::taskEntry()
{
	return extADC.config(); //TODO: setup LTC2991 for measurement
//	vTaskDelayMs(100);
}

bool ExtADCTask::run(void *param)
{
	uint8_t stat = extADC.getStatus();
	//vTaskDelay(20);
	if ((stat & 0b00001110) == 0b00001110) //(stat != 0) FIXME: 0b01110000
	{
		//TODO: get only values that are available  (based ona stat)
		//TODO: reduce data payload by getting only necessary fields
		AdcData outputData;

		receiveBuffer[0].length = I2C_BUFF_SIZE;
		receiveBuffer[0].reg = EXTADC_REG_V1;
//		receiveBuffer[1].length = I2C_BUFF_SIZE;
//		receiveBuffer[1].reg = EXTADC_REG_V5;
//		receiveBuffer[2].length = 4;
//		receiveBuffer[2].reg = EXTADC_REG_TINT;

		for (int i = 0; i < maxReceiveBuffer; i++)
		{
			extADC.getData(receiveBuffer[i]);
//			vTaskDelay(25);
		}
		outputData.timeStamp = xTaskGetTickCount();

//		outputData.length = receiveBuffer[0].length + receiveBuffer[1].length + receiveBuffer[2].length;
		outputData.stat = stat;
		outputData.length = receiveBuffer[0].length;
//		outputData.stat = 0xff; //TODO: remove of replace
		//TODO: add time stamp

		int j = 0;
		for (int i = 0; i < receiveBuffer[0].length; i++)
		{
			outputData.values[j + i] = receiveBuffer[0].val[i];
		}
//		j += receiveBuffer[0].length;
//		for (int i = 0; i < receiveBuffer[1].length; i++)
//		{
//			outputData.values[j + i] = receiveBuffer[1].val[i];
//		}
//		j += receiveBuffer[1].length;
//		for (int i = 0; i < receiveBuffer[2].length; i++)
//		{
//			outputData.values[j + i] = receiveBuffer[2].val[i];
//		}

		//send output data;
		xQueueSend(xQueue_AdcData, (void * ) &outputData, (portTickType ) 0);
		vTaskDelay(7);
	}
	else
	{
		//int temp = xTaskGetTickCount();
		vTaskDelay(3);
	}

	return true;
}
