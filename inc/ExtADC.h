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

#include "stm32f4xx.h"

enum Dir
{
	DirWrite = 0, DirRead = !DirWrite
};

typedef uint8_t i2cBuff[4];

struct I2CData
{
	uint8_t reg;
	i2cBuff val;
	uint8_t length;
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
	bool process(I2CData &data);

private:
	void i2cWrite(I2CData &data);
	void i2cRead(I2CData &data);

	static bool GPIO_Configured;
	static const uint8_t addr; //LTC2991 hardware address
};

//generic registers; use with length > 1
#define EXTADC_REG_STAT	((uint8_t)0x00) // length = 2#define EXTADC_REG_CTRL	((uint8_t)0x06) //length = 4#define EXTADC_REG_V1		((uint8_t)0x0A) // length = 2#define EXTADC_REG_V2		((uint8_t)0x0C) // length = 2#define EXTADC_REG_V3		((uint8_t)0x0E) // length = 2#define EXTADC_REG_V4		((uint8_t)0x10) // length = 2#define EXTADC_REG_V5		((uint8_t)0x12) // length = 2#define EXTADC_REG_V6		((uint8_t)0x14) // length = 2#define EXTADC_REG_V7		((uint8_t)0x16) // length = 2#define EXTADC_REG_V8		((uint8_t)0x18) // length = 2#define EXTADC_REG_TINT		((uint8_t)0x0A) // length = 2#define EXTADC_REG_VCC		((uint8_t)0x0A) // length = 2
//specific registers
#define EXTADC_REG_STAT_LOW		((uint8_t)0x00)
#define EXTADC_REG_STAT_HI		((uint8_t)0x01)
#define EXTADC_REG_CH_EN		EXTADC_REG_STAT_HI
#define EXTADC_REG_CTRL_V1_4	((uint8_t)0x06)
#define EXTADC_REG_CTRL_V5_8	((uint8_t)0x07)
#define EXTADC_REG_CTRL_MODE	((uint8_t)0x08)
#define EXTADC_REG_CTRL_PWM		((uint8_t)0x09)

// channel enable
#define EXTADC_CH_V1_2		((uint8_t)0x10)
#define EXTADC_CH_V3_4		((uint8_t)0x20)
#define EXTADC_CH_V5_6		((uint8_t)0x40)
#define EXTADC_CH_V7_8		((uint8_t)0x80)
#define EXTADC_CH_TINT_VCC		((uint8_t)0x08)
//channel enable synonyms
#define EXTADC_CH_T1 EXTADC_CH_V1_2
#define EXTADC_CH_T2 EXTADC_CH_V3_4
#define EXTADC_CH_T3 EXTADC_CH_V5_6
#define EXTADC_CH_T4 EXTADC_CH_V7_8

//control registers bits
#define EXTADC_BIT_V1_2_DIF		((uint8_t)0x01)
#define EXTADC_BIT_V1_2_T			((uint8_t)0x02)
#define EXTADC_BIT_V1_2_K			((uint8_t)0x04)
#define EXTADC_BIT_V1_2_FILT	((uint8_t)0x08)
#define EXTADC_BIT_V3_4_DIF		((uint8_t)0x10)
#define EXTADC_BIT_V3_4_T			((uint8_t)0x20)
#define EXTADC_BIT_V3_4_K			((uint8_t)0x40)
#define EXTADC_BIT_V3_4_FILT	((uint8_t)0x80)
#define EXTADC_BIT_V5_6_DIF		((uint8_t)0x01)
#define EXTADC_BIT_V5_6_T			((uint8_t)0x02)
#define EXTADC_BIT_V5_6_K			((uint8_t)0x04)
#define EXTADC_BIT_V5_6_FILT	((uint8_t)0x08)
#define EXTADC_BIT_V7_8_DIF		((uint8_t)0x10)
#define EXTADC_BIT_V7_8_T			((uint8_t)0x20)
#define EXTADC_BIT_V7_8_K			((uint8_t)0x40)
#define EXTADC_BIT_V7_8_FILT	((uint8_t)0x80)

#define EXTADC_BIT_REPEAT			((uint8_t)0x10)
#define EXTADC_BIT_TINT_K			((uint8_t)0x04)
#define EXTADC_BIT_TINT_FILT	((uint8_t)0x08)

#endif /* EXTADC_H_ */
