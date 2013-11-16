/*
 * TestTask.cpp
 *
 *  Created on: 7 lis 2013
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
#include <TestTask.h>
#include "stm32f4_discovery.h"

#define DELAY 250
static const int LEDS[4][2] =
{
	{ LED3, DELAY * 2 }, //orange
	{ LED4, DELAY * 4 }, //green
	{ LED5, DELAY * 1 }, //red
	{ LED6, DELAY * 8 } }; //blue

TestTask::TestTask() :
	scheduler_task("TestTask", 1024, 1, (void *) LEDS[1])
{

}

bool TestTask::init()
{
	hwConfig();

	return true;
}

bool TestTask::run(void *param)
{
	int *LED = (int *) param;
	STM_EVAL_LEDToggle((Led_TypeDef) LED[0]);
	vTaskDelay(OS_MS(LED[1]/2));

	return true;
}

void TestTask::hwConfig()
{
	//already done in hw_config.cpp
}
