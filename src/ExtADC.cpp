/*
 * LTC2991.cpp
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

#include "ExtADC.h"
#include "hw_config.h"
#include "stm32f4xx_i2c.h"

ExtADC::ExtADC()
{
	// TODO Auto-generated constructor stub

	GPIO_Configured = false;
}

ExtADC::~ExtADC()
{
	// TODO Auto-generated destructor stub
}

void ExtADC::GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable the CODEC_I2C peripheral clock
	RCC_APB1PeriphClockCmd(ADC_I2C_CLK, ENABLE);
	// Enable I2C GPIO clocks
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE);

	// CODEC_I2C SCL and SDA pins common configuration
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	//specific
	GPIO_InitStructure.GPIO_Pin = ADC_I2C_SCL_PIN;
	GPIO_Init(ADC_I2C_SCL_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ADC_I2C_SDA_PIN;
	GPIO_Init(ADC_I2C_SDA_GPIO, &GPIO_InitStructure);

	// Connect pins to I2C peripheral
	GPIO_PinAFConfig(ADC_I2C_SCL_GPIO, ADC_I2C_SCL_PINSRC, ADC_I2C_GPIO_AF);
	GPIO_PinAFConfig(ADC_I2C_SDA_GPIO, ADC_I2C_SDA_PINSRC, ADC_I2C_GPIO_AF);

	GPIO_Configured = true;
	return;
}

void ExtADC::Init(void)
{
	if (!GPIO_Configured)
		GPIO_Config();

	I2C_InitTypeDef I2C_InitStructure;

	/* CODEC_I2C peripheral configuration */
	I2C_DeInit(ADC_I2C);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 100000;
	/* Enable the I2C peripheral */
	I2C_Cmd(ADC_I2C, ENABLE);
	I2C_Init(ADC_I2C, &I2C_InitStructure);

	return;


}
