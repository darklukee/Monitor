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
#include "UsbTask.h"
#include "DataStructures.h"

extern xQueueHandle xQueue_Lcd;
extern xQueueHandle xQueue_Lcd_Log;
extern xQueueHandle xQueue_AdcData;
extern xQueueHandle xQueue_Storage;

CalculatorTask::CalculatorTask() :
	scheduler_task("CalculatorTask", 1024 * 2, PRIORITY_LOW, NULL)
{
	voltageTabPt = 0;
	currentTabPt = 0;
	voltageMean = 0;
	currentMean = 0;
	timeStampLast = 0;

	//TODO: initialize all variables

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
			bool dataValid = (data.values[2] & (1 << 7)) != 0;
			if (dataValid)
			{
				bool sign = (data.values[2] & (1 << 6)) != 0; //if 1 then -
				data.values[2] = data.values[2] & (0b00111111); //clear data valid and sign
				uint16_t currentRaw = (data.values[2] << 8) + data.values[3];
				float current;
				if (sign)
				{
					currentRaw = ~(currentRaw | (0xff << 14)) + 1; //bit inversion
					current = -float(currentRaw);
				}
				else
				{
					current = float(currentRaw);
				}
				current *= (CurrentLsb / Resistance);
				addCurrent(current);
			}
		}

		if (data.stat & (1 << 2)) //V3
		{
			bool dataValid = (data.values[4] & (1 << 7)) != 0;
			if (dataValid)
			{
				bool sign = (data.values[4] & (1 << 6)) != 0; //if 1 then -
				data.values[4] = data.values[4] & (0b00111111); //clear data valid and sign
				uint16_t voltageRaw = (data.values[4] << 8) + data.values[5];
				float voltage;
				if (sign)
				{
					voltageRaw = ~(voltageRaw | (0xff << 14)) + 1; //bit inversion
					voltage = -float(voltageRaw);
				}
				else
					voltage = float(voltageRaw);

				voltage *= VoltageLsb;
				if (voltage > -400 && voltage < 4950)
				{
					addVoltage(voltage);
				}
				else //vlotage to high, need to use voltage divider
				{
					dataValid = (data.values[6] & (1 << 7)) != 0;
					if (dataValid)
					{
						sign = (data.values[4] & (1 << 6)) != 0; //if 1 then -
						data.values[6] = data.values[6] & (0b00111111); //clear data valid and sign
						voltageRaw = (data.values[6] << 8) + data.values[7];
						if (sign)
						{
							voltageRaw = ~(voltageRaw | (0xff << 14)) + 1; //bit inversion
							voltage = -float(voltageRaw);
						}
						else
							voltage = float(voltageRaw);
						voltage *= VoltageLsb * VoltageDiv;

						if (voltage > -400 && voltage < 10000)
							addVoltage(voltage);
					}
				}
			}
		}

		addTime(data.timeStamp);
		addEnergy();

		sendToLcd();
		sendToStorage();
	}

	return true;
}

/**
 * Add voltage to tab and calculate mean if possible
 * @param voltage
 * @return
 */
void CalculatorTask::addVoltage(float voltage)
{
	voltageLast = voltage; //for usb storage

	voltageAcc += voltage;
	voltageAccCounter++;
}

float CalculatorTask::getVoltageMean(void)
{
	return voltageMean;
}

float CalculatorTask::getVoltageMean(float voltage)
{
	voltageTabPt++;
	voltageTabPt %= tabSize;
	voltageTab[voltageTabPt] = voltage;

//	if (voltageTabPt == 0) //wyliczamy srednia co tabSize
//	{
	float voltageTemp = 0;
	for (int i = 0; i < tabSize; i++)
	{
		voltageTemp += voltageTab[i];
	}
	voltageMean = voltageTemp / tabSize;
//	}
	return voltageMean;
}

void CalculatorTask::addCurrent(float current)
{
	currentLast = current; //for usb storage

	currentAcc += current;
	currentAccCounter++;
}

float CalculatorTask::getCurrentMean(void)
{
	return currentMean;
}

float CalculatorTask::getCurrentMean(float current)
{

	currentTabPt++;
	currentTabPt %= tabSize;
	currentTab[currentTabPt] = current;

//	if (currentTabPt == 0) //wyliczamy srednia co tabSize
//	{
	float currentTemp = 0;
	for (int i = 0; i < tabSize; i++)
	{
		currentTemp += currentTab[i];
	}
	currentMean = currentTemp / tabSize;
//	}

	return currentMean;
}

void CalculatorTask::addEnergy(void)
{
	if (energyPrevTime == 0)
		energyPrevTime = timeStampLast;

	energyTotal += voltageLast * currentLast * (timeStampLast - energyPrevTime) / 1000000000; //mV*mA*ms/1000000000 = J
	energyPrevTime = timeStampLast;
}

void CalculatorTask::cleanAcc(void)
{
	voltageAcc = 0;
	voltageAccCounter = 0;

	currentAcc = 0;
	currentAccCounter = 0;
}

portTickType CalculatorTask::addTime(portTickType timeStamp)
{
//	timeTabPt++;
//	timeTabPt %= tabSize;
//	timeTab[timeTabPt] = timeStamp;
	timeStampLast = timeStamp;
	return timeStampLast;
}

void CalculatorTask::sendToLcd(void)
{
	static portTickType lastSend = 0;
	if (lastSend + lcdFreq < xTaskGetTickCount())
	{
		LcdData outputData;
		outputData.voltage = voltageAcc / voltageAccCounter;
		outputData.voltageMean = getVoltageMean(outputData.voltage);
		outputData.current = currentAcc / currentAccCounter;
		outputData.currentMean = getCurrentMean(outputData.current);
		outputData.power = outputData.voltage * outputData.current / 1000; //data in mW
		outputData.powerMean = outputData.voltageMean * outputData.currentMean / 1000; //data in mW
		outputData.energy = energyTotal;
		cleanAcc();
		xQueueSendToFront(xQueue_Lcd, (void * ) &outputData, (portTickType ) 0);
		dataToLcd = false;

		lastSend = xTaskGetTickCount();
	}
}

void CalculatorTask::sendToStorage(void)
{
	if (UsbTask::isOk())
	{
		StorageData outputData;
//		outputData.voltage = voltageTab[voltageTabPt];
//		outputData.current = currentTab[currentTabPt];
		outputData.voltage = voltageLast;
		outputData.current = currentLast;
		outputData.timeStamp = timeStampLast;
		xQueueSendToFront(xQueue_Storage, (void * ) &outputData, (portTickType ) 0);
	}
}
