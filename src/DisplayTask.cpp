/*
 * DisplayTask.cpp
 *
 *  Created on: 10 lis 2013
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
#include <DisplayTask.h>
#include "stdio.h"

#include "glcd.h"
#include "fonts/SystemFont5x7.h"

#include "DataStructures.h"
#include "hw_config.h"

extern xQueueHandle xQueue_Lcd;

DisplayTask::DisplayTask() :
	scheduler_task("DisplayTask", 1024 * 20, PRIORITY_LOW, NULL)
{
	// TODO Auto-generated constructor stub
	toggle = false;
	initiated = false;
}

bool DisplayTask::init()
{
	// LCD pin init
	prvLCD_Config();
	// Initialize LCD LED PWM. Timer initialization included
	prvLCDLED_Config();
	return true;
}

bool DisplayTask::taskEntry()
{
	this->setFrequency(500); //500ms
//	GLCD.Init(INVERTED);
	GLCD.Init();
	GLCD.ClearScreen();
	GLCD.SelectFont(SystemFont5x7);
//	GLCD.DefineArea(0,0,127,63);

	initiated = true;
	return true;
}

bool DisplayTask::run(void *param)
{
	LcdData values;
	if (xQueueReceive(xQueue_Lcd, &values, 0)) //TODO: take only latest data and discard rest
	{
		GLCD.CursorTo(0, 0);
		printf("Voltage: %6f mV\nCurrent: %6f mA\n", values.voltage, values.current);
//		fflush(stdout);
	}

	static int i = 0;
	GLCD.CursorTo(0, 7);
	printf("status: %d", i++);
	fflush(stdout);
	//Demo();

//	vTaskSuspend(this->getTaskHandle());
	return true;
}

void DisplayTask::putChar(char ch)
{
	//TODO: add mutex on glcd
	GLCD.PutChar(ch);
}

extern "C"
{
void DisplayTaskPutChar(char ch)
{
	static DisplayTask* display;
	if (!display) //until successful
		display = dynamic_cast<DisplayTask*>(scheduler_task::getTaskPtrByName("DisplayTask"));

	if (display)
	{
		if (display->isInitiated())
		{
			display->putChar(ch);
		}
	}
}
}

bool DisplayTask::isInitiated(void)
{
	return initiated;
}

void DisplayTask::Demo()
{
	char hello[] = "Hello World";
	static unsigned int i;
	i++;
	if (i >= 8)
	{
		GLCD.ClearScreen();
	}
	i %= 8;
	//GLCD.DrawCircle(10,10,5);
	if (toggle)
	{
		GLCD.SetFontColor(BLACK);
		GLCD.CursorTo(i, i);
		GLCD.Puts(hello);
//		GLCD.PutChar('c');
//		GLCD.DrawCircle(100,20,10,BLACK);
//		GLCD.SetDot(55,55,BLACK);
//		GLCD.DrawString(hello, 5, 5);

	}
	else
	{
//		GLCD.SetFontColor(WHITE);
//		GLCD.CursorTo(i,i);
//		GLCD.PutChar('d');
//		GLCD.DrawCircle(100,20,10,WHITE);
//		GLCD.SetDot(55,55,WHITE);
//		GLCD.DrawString(hello, 5, 5);
		float pi = 3.141592;
		char pi_char[15];
		sprintf(pi_char, " float %.3f, int %d", pi, i);
		GLCD.Puts(pi_char);
	}
	toggle ^= true;
}
