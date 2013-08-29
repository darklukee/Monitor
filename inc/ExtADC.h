/*
 * ExtADC.h
 *
 *  Created on: 28 sie 2013
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

#ifndef EXTADC_H_
#define EXTADC_H_

#include <stdint.h>


enum Dir
{
	DirWrite = 0,
	DirRead = !DirWrite
};

struct I2CData
{
	uint8_t reg;
	uint8_t val;
	Dir dir;
};

class ExtADC
{
public:
	ExtADC();
	~ExtADC();
	static void GPIO_Config(void);
	void init(void);
	void config(void);
	bool process(I2CData data, uint8_t* retVal);

private:
	void i2cWrite(uint8_t reg, uint8_t value);
	uint8_t i2cRead(uint8_t reg);


	static bool GPIO_Configured;
	static const uint8_t addr = 0x90; //LTC2991 hardware address
};

#endif /* EXTADC_H_ */
