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

const uint16_t tabSize = 50;

class CalculatorTask: public scheduler_task
{
public:
	CalculatorTask();
	bool init();
	bool taskEntry();
	bool run(void *param);

private:
	float voltageTab[tabSize];
	float voltageMean;
	uint16_t voltageTabPt;

	float currentTab[tabSize];
	float currentMean;
	uint16_t currentTabPt;

	float addVoltage(float);
	float getVoltageMean(void);
	float addCurrent(float);
	float getCurrentMean(void);

	bool dataToLcd;
	void sendToLcd(void);
//	bool dataToStorage;
	void sendToStorage(void);

};

#endif /* CALCULATORTASK_H_ */
