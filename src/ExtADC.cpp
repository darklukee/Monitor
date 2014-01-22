/*
 * ExtADC.cpp
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
#include "FreeRTOS.h"
#include "queue.h"

bool ExtADC::GPIO_Configured = false;
//const uint8_t ExtADC::addr = 0x90;

extern xQueueHandle xQueue_I2CEvent;
extern I2CData i2cData_it[I2C_DATA_SIZE];
extern "C"
{
int getI2cPointer();
}

ExtADC::ExtADC()
{
}

void ExtADC::GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable the CODEC_I2C peripheral clock
	RCC_APB1PeriphClockCmd(ADC_I2C_CLK, ENABLE);
	// Enable I2C GPIO clocks
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE);

	// ADC_I2C SCL and SDA pins common configuration
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //TODO: this may be problematic. External pull-up suggested.

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

void ExtADC::init(void)
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

	NVIC_InitTypeDef NVIC_InitStructure;

	// I2C ER Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = I2C3_ER_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //highest priotiry
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //highest priotiry
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// I2C EV Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = I2C3_EV_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5; //lower priotiry
	NVIC_Init(&NVIC_InitStructure);

	return;
}

/**
 * Configure ExtADC in specific mode
 */
bool ExtADC::config(void)
{
	//TODO: make it possible to change parameters in runtime

	I2CData config;
	//control reg:
	config.dir = DirWrite;
	config.length = 3;
	config.reg = EXTADC_REG_CTRL;
	config.val[0] = EXTADC_BIT_V1_2_DIF; //differential measurement
	config.val[1] = 0; //EXTADC_BIT_V7_8_T | EXTADC_BIT_V7_8_K; //temperature measurement in kelvin
	config.val[2] = EXTADC_BIT_REPEAT; // | EXTADC_BIT_TINT_K; //repeat mode, tint in kelvin
	while(!process(config)); //as long as succeed
//	if (!verifyI2cWrite(config))
//		return false;

	//enable channels:
	config.dir = DirWrite;
	config.length = 1;
	config.reg = EXTADC_REG_CH_EN;
	config.val[0] = EXTADC_CH_V1_2 | EXTADC_CH_V3_4; //| EXTADC_CH_V5_6 | EXTADC_CH_T4; //TODO: Tint and Vcc
	while(!process(config)); //as long as succeed
//	if (!verifyI2cWrite(config)) //bit changes in register
//		return false;

	return true;
}
bool ExtADC::process(I2CData& data)
{
	//TODO: finish implementation

	I2C_ITConfig(ADC_I2C, I2C_IT_EVT | I2C_IT_ERR | I2C_IT_BUF, ENABLE); //enable interrupts
	//TODO: empty queue xQueue_I2CEvent

	if (data.dir == DirWrite)
	{
		//write stuff
		return i2cWrite(data);;
	}
	else
	{
		//read stuff
		return i2cRead(data);
	}
}

bool ExtADC::i2cWrite(I2CData &data)
{
	int it = getI2cPointer();
	int itCheck = -1; //to check if correct request was completed
	copyI2cData(&data,&(i2cData_it[it])); //	i2cData_it[it] = data;
	//Start the config sequence, STOP inside irq handler
	I2C_GenerateSTART(ADC_I2C, ENABLE);
	//peek from queue, compare whether completed or error
	bool ok = false;
	while (!ok)
	{
		if (xQueueReceive(xQueue_I2CEvent, &itCheck, portMAX_DELAY) == pdPASS)
		{
			if (itCheck == it)
				ok = true;
		}
	}
	return ok;
}

bool ExtADC::i2cRead(I2CData &data)
{
	int it = getI2cPointer();
	int itCheck = -1; //to check if correct request was completed
	copyI2cData(&data,&(i2cData_it[it]));
//Start the config sequence, STOP inside irq handler
	I2C_GenerateSTART(ADC_I2C, ENABLE);
//	I2C_AcknowledgeConfig(ADC_I2C, ENABLE);
	I2C_AcknowledgeConfig(ADC_I2C, DISABLE); // make sure it's disabled
//peek from queue, compare whether completed or error
	bool ok = false;
	while (!ok)
	{
		if (xQueueReceive(xQueue_I2CEvent, &itCheck, 1000) == pdPASS)
		{
			if (itCheck == it)
			{
				ok = true;
				//data = i2cData_it[it]; //TODO: manual copy val would be faster
				copyI2cData(&(i2cData_it[it]), &data);
			}
		}
		else
		{
			//I2C deadlock, emergency break
			//TODO: log
			break;
		}
	}
	return ok;
}

void ExtADC::copyI2cData(I2CData* src, I2CData* dest)
{
	dest->dir = src->dir;
	dest->length = src->length;
	dest->reg = src->reg;
	for (int i = 0; i < src->length; i++)
		dest->val[i] = src->val[i];
}

uint8_t ExtADC::getStatus(void)
{
	I2CData status;
	status.dir = DirRead;
	status.reg = EXTADC_REG_STAT;
	status.length = 1;
	process(status);
	return status.val[0];
}

bool ExtADC::getData(I2CData& data)
{
	return process(data);;
}

bool ExtADC::verifyI2cWrite(I2CData& data)
{
	I2CData verify;
	verify.dir = DirRead; //change direction
	verify.reg = data.reg;
	verify.length = data.length;

	process(verify);
	for (int i = 0; i < data.length; i++)
	{
		if (verify.val[i] != data.val[i])
			return false;
	}
	return true;
}
