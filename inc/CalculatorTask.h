/*
 * CalculatorTask.h
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
#ifndef CALCULATORTASK_H_
#define CALCULATORTASK_H_

#include <cpp_task.hpp>

class CalculatorTask: public scheduler_task
{
public:
	CalculatorTask();
	bool init();
	bool taskEntry();
	bool run(void *param);

private:
	static const uint16_t tabSize = 20;
	static const uint8_t lcdFreq = 250;

	float voltageLast;
	float voltageAcc;
	int voltageAccCounter;
//	float voltageMax;
//	float volatageMin;
	float voltageTab[tabSize];
	float voltageMean;
	uint16_t voltageTabPt;
	void addVoltage(float);
	float getVoltageMean(void);
	float getVoltageMean(float);

	float currentLast;
	float currentAcc;
	int currentAccCounter;
//	float currentMax;
//	float currentMin;
	float currentTab[tabSize];
	float currentMean;
	uint16_t currentTabPt;
	void addCurrent(float);
	float getCurrentMean(void);
	float getCurrentMean(float);

	float energyTotal;
	portTickType energyPrevTime;
	void addEnergy(void);

	void cleanAcc(void);

//	portTickType timeTab[tabSize];
//	uint16_t timeTabPt;
	portTickType timeStampLast;
	portTickType addTime(portTickType);

	bool dataToLcd;
	void sendToLcd(void);
//	bool dataToStorage;
	void sendToStorage(void);

	static const float VoltageLsb = 0.30518; //2500.0 / 8192.0; //0.30518[mV]; 2500mV/2^13; LTC2991 application information
	static const float CurrentLsb = 0.019075; //2500.0 / 131072.0; //0.019075[mV]; 2500mV/2^17; LTC2991 application information
	static const float Resistance = 0.39;
	static const float VoltageDiv = 2;

};

#endif /* CALCULATORTASK_H_ */
