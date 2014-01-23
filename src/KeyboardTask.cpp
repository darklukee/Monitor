/*
 * KeyboardTask.cpp
 *
 *  Created on: 13 gru 2013
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
#include <KeyboardTask.h>

#include "stm32f4xx_exti.h"
#include "stm32f4xx_gpio.h"

#include "UsbTask.h"
#include "hw_config.h"

#include "Lcd_Log_Messages.h"
extern xQueueHandle xQueue_Lcd_Log;

#include <stdio.h>

xQueueHandle xQueue_Keyboard;
uint8_t keyPressed; //for queue

const int debouncePeriod = 5; //ms
const int numOfKeys = KEYn; //without one on discovery board
const int timesChecked = 15; //how many times button is checked;
bool debounceEnabled[numOfKeys];
bool debounceTable[numOfKeys][timesChecked];

const int maxKeyDobounceCount = timesChecked + 5;
int keyDebounceCount[numOfKeys];

KeyboardTask::KeyboardTask() :
	scheduler_task("KeyboardTask", 1024, PRIORITY_LOW, NULL)
{
	debounceIter = 0;
}

bool KeyboardTask::init()
{
	prvKeyboard_Config();
	keyClearAll();
	xQueue_Keyboard = xQueueCreate(2, sizeof(uint8_t*)); //TODO: set size
	return true;
}

bool KeyboardTask::taskEntry()
{
	//this->setFrequency(debouncePeriod);
	return true;
}

bool KeyboardTask::run(void* param)
{
	//get from queue
	uint8_t *key = NULL;

	if (!enabledLeft()) //no enabled keys left, block task
	{
		keyClearAll();
		debounceIter = 0;
		xQueuePeek(xQueue_Keyboard, &key, portMAX_DELAY); //only to block task, receiving elsewhere
	}
	xQueueReceive(xQueue_Keyboard, &key, 0);

	if (key) //something received
	{
		for (uint8_t i = 0; i < numOfKeys && ((*key) > 0); i++)
		{
			if ((*key) & (1 << i))
			{
				debounceEnabled[i] = true;
				(*key) &= (0xff - (1 << i));
			}
		}
	}

	debounce();
	vTaskDelayMs(debouncePeriod);
	return true;
}

void KeyboardTask::keyAction(int key)
{
	LcdLogEnum log;
	switch (key)
	{
	case KEY_UP:
		xQueueSend(xQueue_Lcd_Log, (void * ) &(log = LOG_kUp), (portTickType ) 0);
		break;
	case KEY_DOWN:
		xQueueSend(xQueue_Lcd_Log, (void * ) &(log = LOG_kDown), (portTickType ) 0);
		break;
	case KEY_LEFT:
		xQueueSend(xQueue_Lcd_Log, (void * ) &(log = LOG_kLeft), (portTickType ) 0);
		break;
	case KEY_RIGHT:
		xQueueSend(xQueue_Lcd_Log, (void * ) &(log = LOG_kRight), (portTickType ) 0);
		break;
	case KEY_OK:
		xQueueSend(xQueue_Lcd_Log, (void * ) &(log = LOG_kOk), (portTickType ) 0);
		UsbTask::toggleEnabled(); //TODO: move?
		break;
	case KEY_ESC:
		xQueueSend(xQueue_Lcd_Log, (void * ) &(log = LOG_kEsc), (portTickType ) 0);
		break;
	default:
		break;
	}
}

void KeyboardTask::debounce(void)
{
	for (int i = 0; i < numOfKeys; i++)
	{
		if (debounceEnabled[i])
		{
			debounceTable[i][debounceIter] = (bool) GPIO_ReadInputDataBit(KEY_PORT[i], KEY_PIN[i]);
			if (keyCheck(i))
			{
				keyAction(i);
				keyClear(i);
			}
		}

	}

	debounceIter++;
	debounceIter %= timesChecked;
}

/**
 * Check if key is in steady state
 * @param key
 * @return
 */
bool KeyboardTask::keyCheck(int key)
{
	keyDebounceCount[key]++;
	if (keyDebounceCount[key] > maxKeyDobounceCount)
	{
		keyClear(key);
		return false;
	}

	for (int i = 0; i < timesChecked; i++)
	{
		if (debounceTable[key][i]) //true - not pressed
			return false;
	}
	//keyClear(key);
	return true;
}

/**
 * Clears dobounce info about key
 * @param key to clear
 */
void KeyboardTask::keyClear(int key)
{
	keyDebounceCount[key] = 0;
	for (int i = 0; i < timesChecked; i++)
	{
		debounceTable[key][i] = true;
	}
	debounceEnabled[key] = false;
}

/**
 * Clear all key info
 */
void KeyboardTask::keyClearAll(void)
{
	for (int i = 0; i < numOfKeys; i++)
	{
		keyClear(i);
	}
}

/**
 * Check if any key is still enabled
 * @return
 */
bool KeyboardTask::enabledLeft()
{
	for (int i = 0; i < numOfKeys; i++)
	{
		if (debounceEnabled[i])
			return true;
	}
	return false;
}

//C wrapper for interrupts
extern "C"
{
void KeyboardTaskResume(void)
{
//	KeyboardTask::taskResume();
	xTaskResumeFromISR(scheduler_task::getTaskPtrByName("KeyboardTask")->getTaskHandle());
}
}
