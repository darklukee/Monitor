/*
 * LcdPinTestTask.cpp
 *
 *  Created on: 13 lis 2013
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
#include <LcdPinTestTask.h>
#include "glcd.h"
#include "include/glcd_io.h"
#include "include/ARMio.h"
#include "stm32f4_discovery.h"

#define DELAY 250
static const int LEDS[4][2] =
{
{ LED3, DELAY * 2 }, //orange
		{ LED4, DELAY * 4 }, //green
		{ LED5, DELAY * 1 }, //red
		{ LED6, DELAY * 8 } }; //blue

LcdPinTestTask::LcdPinTestTask() :
	scheduler_task("DisplayTask", 1024, 1, NULL)
{
	// TODO Auto-generated constructor stub
	toggle = false;
}

bool LcdPinTestTask::init()
{
//	lcd.Init(); //cant use here
	return true;
}

bool LcdPinTestTask::taskEntry()
{
	GLCD.Init();
//	GLCD.ClearScreen();
//	GLCD.CursorTo(0,0);
//	GLCD.SelectFont(SystemFont5x7);
	return true;
}

bool LcdPinTestTask::run(void *param)
{
	const int _time = 200;
	lcdPinMode(glcdData2Pin, GPIO_Mode_IN);
	lcdPinMode(glcdData0Pin, GPIO_Mode_OUT);
	for (int i = 0; i < 3; i++)
	{
		lcdfastWrite(glcdData0Pin, 1);
		uint8_t val = ARMio_ReadBit(glcdData2Pin);
		if (val > 0)
			STM_EVAL_LEDOn(LED6);
		else
			STM_EVAL_LEDOff(LED6);
		vTaskDelay(OS_MS(_time));
		lcdfastWrite(glcdData0Pin, 0);
		val = ARMio_ReadBit(glcdData2Pin);
		if (val > 0)
			STM_EVAL_LEDOn(LED6);
		else
			STM_EVAL_LEDOff(LED6);
		vTaskDelay(OS_MS(_time));
	}

	lcdPinMode(glcdData0Pin, GPIO_Mode_IN);
	lcdPinMode(glcdData2Pin, GPIO_Mode_OUT);
	for (int i = 0; i < 3; i++)
	{
		lcdfastWrite(glcdData2Pin, 1);
		uint8_t val = ARMio_ReadBit(glcdData0Pin);
		if (val > 0)
			STM_EVAL_LEDOn(LED3);
		else
			STM_EVAL_LEDOff(LED3);
		vTaskDelay(OS_MS(_time));
		lcdfastWrite(glcdData2Pin, 0);
		val = ARMio_ReadBit(glcdData0Pin);
		if (val > 0)
			STM_EVAL_LEDOn(LED3);
		else
			STM_EVAL_LEDOff(LED3);
		vTaskDelay(OS_MS(_time));
	}

	return true;
}

