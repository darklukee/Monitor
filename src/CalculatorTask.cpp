/*
 * CalculatorTask.cpp
 *
 *  Created on: 8 sty 2014
 *      Author: lukee
 *
 * Copyright (C) 2014  darklukee
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
#include <CalculatorTask.h>
#include "ExtADC.h"
#include "DataStructures.h"

extern xQueueHandle xQueue_Lcd;
extern xQueueHandle xQueue_AdcData;

const float VoltageLsb = 2500 / 8192; //0.30518[mV]; 2500mV/2^13; LTC2991 application information
const float CurrentLsb = 2500 / 131072; //0.019075[mV]; 2500mV/2^17; LTC2991 application information
const float Resistance = 0.39;

CalculatorTask::CalculatorTask() :
	scheduler_task("CalculatorTask", 1024, PRIORITY_LOW, NULL)
{
	voltageTabPt = 0;
	currentTabPt = 0;
	voltageMean = 0;
	currentMean = 0;

	dataToLcd = false;
//	dataToStorage = false;
}

bool CalculatorTask::init()
{
	return true;
}

bool CalculatorTask::taskEntry()
{
	return true;
}

bool CalculatorTask::run(void *param)
{
	AdcData data;
	if (xQueueReceive(xQueue_AdcData, &(data), portMAX_DELAY))
	{
		if (data.stat & (1 << 1)) //V1-V2
		{
			bool dataValid = (data.values[2] & (1 << 7)) != 0; //TODO: do something about this
			bool sign = (data.values[2] & (1 << 6)) != 0; //if 1 then -
			data.values[2] = data.values[2] & (0b00111111); //clear data valid and sign
			uint16_t currentRaw = (data.values[2] << 8) + data.values[3];
			float current = float(sign ? -currentRaw : currentRaw) * CurrentLsb / Resistance;
			addCurrent(current);
		}
		if (data.stat & (1 << 2)) //V3
		{
			bool dataValid = (data.values[4] & (1 << 7)) != 0; //TODO: do something about this
			bool sign = (data.values[4] & (1 << 6)) != 0; //if 1 then -
			data.values[4] = data.values[4] & (0b00111111); //clear data valid and sign
			uint16_t voltageRaw = (data.values[4] << 8) + data.values[5];
			float voltage = float(sign ? -voltageRaw : voltageRaw) * VoltageLsb;
			addVoltage(voltage);
		}
		sendToLcd();
		sendToStorage();
		//allDataSent();
	}

	return true;
}

/**
 * Add voltage to tab and calculate mean if possible
 * @param voltage
 * @return
 */
float CalculatorTask::addVoltage(float voltage)
{
	voltageTabPt++;
	voltageTabPt %= tabSize;
	voltageTab[voltageTabPt] = voltage;

	if(voltageTabPt == 0) //wyliczamy srednia co tabSize
	{
		dataToLcd = true;
		float voltageTemp=0;
		for(int i = 0; i<tabSize; i++)
		{
			voltageTemp += voltageTab[i];
		}
		voltageMean = voltageTemp/tabSize;
	}

	return voltageMean;
}

float CalculatorTask::getVoltageMean(void)
{
	return voltageMean;
}

float CalculatorTask::addCurrent(float current)
{
	currentTabPt++;
	currentTabPt %= tabSize;
	currentTab[currentTabPt] = current;

	if(currentTabPt == 0) //wyliczamy srednia co tabSize
	{
		dataToLcd = true;
		float currentTemp=0;
		for(int i = 0; i<tabSize; i++)
		{
			currentTemp += currentTab[i];
		}
		currentMean = currentTemp/tabSize;
	}

	return currentMean;
}

float CalculatorTask::getCurrentMean(void)
{
	return currentMean;
}

void CalculatorTask::sendToLcd(void)
{
	if(dataToLcd)
	{
		LcdData outputData;
		outputData.voltage = voltageMean;
		outputData.current = currentMean;
		xQueueSend(xQueue_Lcd, (void * ) &outputData, (portTickType ) 0);
	}
}

void CalculatorTask::sendToStorage(void)
{
}
