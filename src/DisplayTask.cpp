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
#include "glcd.h"
#include "fonts/SystemFont5x7.h"

DisplayTask::DisplayTask() : scheduler_task("DisplayTask", 1024*10, 1, NULL)
{
	// TODO Auto-generated constructor stub
	toggle = false;
}

bool DisplayTask::init()
{
//	lcd.Init(); //cant use here
	return true;
}

bool DisplayTask::taskEntry()
{
//	lcd.Init();
//	lcd.ClearScreen();
//	lcd.CursorTo(0,0);
//	lcd.SelectFont(SystemFont5x7);

//	GLCD.Init(INVERTED);
	GLCD.Init();
	GLCD.ClearScreen();
	GLCD.CursorTo(0,0);
	GLCD.SelectFont(SystemFont5x7);
	return true;
}

bool DisplayTask::run(void *param)
{
//	char hello[] = "Hello World";
	GLCD.DrawCircle(10,10,5);
	if (toggle)
	{
//		GLCD.SetFontColor(BLACK);
		GLCD.DrawCircle(64,64,10,BLACK);
		GLCD.SetDot(55,55,BLACK);
	}
	else
	{
//		GLCD.SetFontColor(WHITE);
		GLCD.DrawCircle(64,64,10,WHITE);
		GLCD.SetDot(55,55,WHITE);
	}
	toggle ^= true;
	//GLCD.Puts(hello);
	vTaskDelay(OS_MS(500));

//	vTaskSuspend(this->getTaskHandle());
	return true;
}
