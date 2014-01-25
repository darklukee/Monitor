/*
 * DataStructures.h
 *
 *  Created on: 9 sty 2014
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
#ifndef DATASTRUCTURES_H_
#define DATASTRUCTURES_H_

#include<stdint.h>
#include <portmacro.h> //freeRTOS portTickType

struct LcdData
{
	float voltage;
	float voltageMean;

	float current;
	float currentMean;

	float power;
	float powerMean;
	float energy;
};

struct LcdMinMaxData
{
	float voltageMin;
	float voltageMax;
	float currentMin;
	float currentMax;
};

typedef enum
{
	DirWrite = 0, DirRead = !DirWrite
} Dir;

#define I2C_BUFF_SIZE  8
typedef uint8_t i2cBuff[I2C_BUFF_SIZE]; //FIXME: may be not enough

typedef struct
{
	i2cBuff val;
	Dir dir;
	uint8_t reg;
	uint8_t length;
} I2CData;
#define I2C_DATA_SIZE 4 //TODO: 2 should be sufficient

struct AdcData
{
	uint8_t values[8];
	uint8_t length;
	uint8_t stat;
	portTickType timeStamp;
//	unsigned long timeStamp;
};

struct StorageData
{
	float voltage;
	float current;
	portTickType timeStamp; //TODO: add time and others
//	unsigned long timeStamp;
};
#endif /* DATASTRUCTURES_H_ */
