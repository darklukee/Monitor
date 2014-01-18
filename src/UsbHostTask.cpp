/*
 * UsbHostTask.cpp
 *
 *  Created on: 7 gru 2013
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
#include <UsbHostTask.h>
#include "usbh_usr.h"
#include "stm32f4_discovery.h"

extern __IO uint8_t LED_Toggle;
__IO uint8_t RepeatState = 0;
__IO uint16_t CCR_Val = 16826;
USB_OTG_CORE_HANDLE USB_OTG_Core;
USBH_HOST USB_Host;

extern xSemaphoreHandle xSemaphore_UsbMutex;

UsbHostTask::UsbHostTask() :
	scheduler_task("UsbHostTask", 1024, PRIORITY_MEDIUM, NULL)
{
}

bool UsbHostTask::init()
{
	STM_EVAL_LEDOn(LED4);
	TIM_LED_Config();
	LED_Toggle = 7;
	return true;
}

bool UsbHostTask::taskEntry()
{
	/* Init Host Library */
	USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &USBH_MSC_cb, &USR_Callbacks);
	if(xSemaphoreTake(xSemaphore_UsbMutex, (portTickType ) 1000) != pdPASS)
		return false;
	//semaphore give inside USBH_USR_MSC_Application()

	return true;
}

bool UsbHostTask::run(void *param)
{
	USBH_Process(&USB_OTG_Core, &USB_Host);
	vTaskDelayMs(5);
	return true;
}

void UsbHostTask::TIM_LED_Config(void)
{
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	uint16_t prescalervalue = 0;

	/* TIM4 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the TIM3 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Initialize Leds mounted on STM324F4-EVAL board */
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED6);

	/* Compute the prescaler value */
	prescalervalue = (uint16_t) ((SystemCoreClock) / 550000) - 1;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 65535;
	TIM_TimeBaseStructure.TIM_Prescaler = prescalervalue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	/* Enable TIM4 Preload register on ARR */
	TIM_ARRPreloadConfig(TIM4, ENABLE);

	/* TIM PWM1 Mode configuration: Channel */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	/* Output Compare PWM1 Mode configuration: Channel2 */
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Disable);

	/* TIM Interrupts enable */
	TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);

	/* TIM4 enable counter */
	TIM_Cmd(TIM4, ENABLE);
}
