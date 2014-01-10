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
const uint8_t ExtADC::addr = 0x90;

extern xQueueHandle xQueue_I2CEvent;

ExtADC::ExtADC()
{
	// TODO Auto-generated constructor stub
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15; //highest priotiry
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
	config.val[1] = EXTADC_BIT_V7_8_T | EXTADC_BIT_V7_8_K; //temperature measurement in kelvin
	config.val[2] = EXTADC_BIT_REPEAT | EXTADC_BIT_TINT_K; //repeat mode, tint in kelvin
	process(config);
	if(!verifyI2cWrite(config))
		return false;

	//enable channels:
	config.dir = DirWrite;
	config.length = 1;
	config.reg = EXTADC_REG_CH_EN;
	config.val[0] = EXTADC_CH_V1_2 | EXTADC_CH_V3_4 | EXTADC_CH_V5_6 | EXTADC_CH_T4; //TODO: Tint and Vcc
	process(config);
	if(!verifyI2cWrite(config))
		return false;

	return true;
}
bool ExtADC::process(I2CData& data)
{
	//TODO: finish implementation

	I2C_ITConfig(ADC_I2C, I2C_IT_EVT | I2C_IT_ERR | I2C_IT_BUF, ENABLE); //enable interrupts
	//TODO: empty queue xQueue_I2CEvent

	//Start the config sequence, STOP inside i2cWrite and i2cRead
	I2C_GenerateSTART(ADC_I2C, ENABLE);

	if (data.dir == DirWrite)
	{
		//write stuff
		i2cWrite(data);
		return false;
	}
	else
	{
		//read stuff
		i2cRead(data);
		return true;
	}
}

void ExtADC::i2cWrite(I2CData &data)
{
	uint32_t event;
	uint8_t index = 0;
	bool finished = false;
	while ((xQueueReceive(xQueue_I2CEvent, &event, portMAX_DELAY) == pdPASS) && (!finished)) //TODO: delay time
	{
		switch (event)
		{
		// EV5
		case I2C_EVENT_MASTER_MODE_SELECT:
			//Wyslanie adresu w trybie zapisujacego urzadzenia nadrzednego
			I2C_Send7bitAddress(ADC_I2C, addr, I2C_Direction_Transmitter);
			break;

			// EV6
		case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
			// First byte (register)
			I2C_SendData(ADC_I2C, data.reg);
			break;

			// EV8
		case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
			if (index < data.length)
			{
				//send next bytes
				I2C_SendData(ADC_I2C, data.val[index++]);
			}
//			else //FIXME: unnecessary ?
//			{
//				I2C_ITConfig(ADC_I2C, I2C_IT_BUF, DISABLE);
//			}
			break;

			// EV8_2
		case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
			if (index < data.length) //protection for situation where next byte wasn't quick enough
			{
				//send next bytes
				I2C_SendData(ADC_I2C, data.val[index++]);
				//TODO: add some logging here
			}
			else //everything ok
			{
				// STOP
				I2C_GenerateSTOP(ADC_I2C, ENABLE);
				I2C_ITConfig(ADC_I2C, I2C_IT_EVT | I2C_IT_ERR | I2C_IT_BUF, DISABLE); //FIXME: Disable all or only EVT?
				finished = true;
			}
			break;

		default:
			//TODO: show info
			break;
		}
	}
	return;
}

void ExtADC::i2cRead(I2CData &data)
{

	uint32_t event;
	uint8_t index = 0;
	bool regSent = false;
	bool finished = false;
	I2C_AcknowledgeConfig(ADC_I2C, ENABLE);
	while ((xQueueReceive(xQueue_I2CEvent, &event, portMAX_DELAY) == pdPASS) && (!finished)) //TODO: delay time
	{
		switch (event)
		{
		// EV5
		case I2C_EVENT_MASTER_MODE_SELECT:
			if (!regSent)
			{
				//Wyslanie adresu w trybie zapisujacego urzadzenia nadrzednego
				I2C_Send7bitAddress(ADC_I2C, addr, I2C_Direction_Transmitter);
				regSent = true;
			}
			else
			{
				//Send addr in read mode
				I2C_Send7bitAddress(ADC_I2C, addr, I2C_Direction_Receiver);
			}
			break;

			//EV6 in transmitter mode
		case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
			// Send register
			I2C_SendData(ADC_I2C, data.reg);
			break;

			// EV6 in receiver mode
		case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
			//cool story bro :)
			break;

			// EV8_2
		case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
			// Reg sent, change to receiver
			I2C_GenerateSTART(ADC_I2C, ENABLE);
			break;

			// EV7
		case I2C_EVENT_MASTER_BYTE_RECEIVED:
			//FIXME: possible change in order; optimization needed
			//first check whether to send ACK (EV7_1)
			if (index >= (data.length - 1)) // one before last packet
			{
				/* Disable I2C acknowledgment */
				I2C_AcknowledgeConfig(ADC_I2C, DISABLE);
				/* Send I2C STOP Condition */
				I2C_GenerateSTOP(ADC_I2C, ENABLE);
			}

			data.val[index++] = I2C_ReceiveData(ADC_I2C);

			if (index >= data.length) //all data received, can ignore rest
			{
				I2C_ITConfig(ADC_I2C, I2C_IT_EVT | I2C_IT_ERR | I2C_IT_BUF, DISABLE);
				finished = true;
			}
			break;

		default:
			//TODO: show info
			break;
		}
	}

	return;
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

}

bool ExtADC::verifyI2cWrite(I2CData& data)
{
	I2CData verify;
	verify.dir = DirRead; //change direction
	verify.reg = data.reg;
	verify.length = data.length;

	process(verify);
	for(int i =0; i<data.length; i++)
		{
			if(verify.val[i] != data.val[i])
				return false;
		}
	return true;
}
