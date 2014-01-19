/**
 ******************************************************************************
 * @file    stm32f4xx_it.c
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    19-September-2011
 * @brief   Main Interrupt Service Routines.
 *          This file provides all exceptions handler and peripherals interrupt
 *          service routine.
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#include "usb_hcd_int.h"
#include "usb_bsp.h"

#include "hw_config.h"
#include "DataStructures.h"

#include <stdbool.h>

portBASE_TYPE xHigherPriorityTaskWoken;

//USB
uint16_t capture = 0;
extern __IO uint16_t CCR_Val;
__IO uint8_t LED_Toggle = 0;
extern USB_OTG_CORE_HANDLE USB_OTG_Core;
//extern USBH_HOST USB_Host;

//Keyboard
extern uint8_t keyPressed;
extern void KeyboardTaskResume(void);
extern xQueueHandle xQueue_Keyboard;

//ADC i2c
extern xQueueHandle xQueue_I2CEvent;
int i2cPointer = 0;
I2CData i2cData_it[I2C_DATA_SIZE];
int getI2cPointer(void)
{
	i2cPointer++;
	i2cPointer %= I2C_DATA_SIZE;
	//	(++i2cPointer) %=I2C_DATA_SIZE; //works only in c++
	return i2cPointer;
}

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief   This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1)
	{
	}
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void)
{
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1)
	{
	}
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void)
{
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1)
	{
	}
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void)
{
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1)
	{
	}
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None

 void SVC_Handler(void)
 {
 }
 */

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None

 void PendSV_Handler(void)
 {
 }
 */

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None

 void SysTick_Handler(void)
 {
 }
 */

/******************************************************************************/
/*                 STM32Fxxx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32fxxx.s).                                               */
/******************************************************************************/

/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
/*void PPP_IRQHandler(void)
 {
 }*/
/* User button handler */
void EXTI0_IRQHandler(void)
{
	//TODO: stub
	if (EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

/**
 * @brief  Key OK hander
 * @param  None
 * @retval None
 */
void EXTI3_IRQHandler(void)
{
	//TODO: stub
	if (EXTI_GetITStatus(KEY_OK_EXTI_LINE) != RESET)
	{
		EXTI_ClearITPendingBit(KEY_OK_EXTI_LINE);
	}
}

/**
 * @brief  Key ESC hander
 * @param  None
 * @retval None
 */
void EXTI4_IRQHandler(void)
{
	//TODO: stub
	if (EXTI_GetITStatus(KEY_ESC_EXTI_LINE) != RESET)
	{
		EXTI_ClearITPendingBit(KEY_ESC_EXTI_LINE);
	}
}

/**
 * @brief  Arrow keys hander. Multiple EXTI lines to handle!
 * @param  None
 * @retval None
 */
void EXTI9_5_IRQHandler(void)
{
	//TODO: stub
	if (EXTI_GetITStatus(EXTI_Line5) != RESET)
	{
		//DO NOT use, something is wrong here
		EXTI_ClearITPendingBit(EXTI_Line5);
	}
	if (EXTI_GetITStatus(KEY_UP_EXTI_LINE) != RESET)
	{
		EXTI_ClearITPendingBit(KEY_UP_EXTI_LINE);
	}
	if (EXTI_GetITStatus(KEY_DOWN_EXTI_LINE) != RESET)
	{
		EXTI_ClearITPendingBit(KEY_DOWN_EXTI_LINE);
	}
	if (EXTI_GetITStatus(KEY_LEFT_EXTI_LINE) != RESET)
	{
		EXTI_ClearITPendingBit(KEY_LEFT_EXTI_LINE);
	}
	if (EXTI_GetITStatus(KEY_RIGHT_EXTI_LINE) != RESET)
	{
		//		UsbTaskToggleEnabled(); //TODO: remove
		xHigherPriorityTaskWoken = pdFALSE;
		keyPressed |= 1 << KEY_RIGHT;
		uint8_t *key = &keyPressed;
		xQueueSendFromISR(xQueue_Keyboard, (void* ) &key, &xHigherPriorityTaskWoken); //TODO poiter, references of sth
		KeyboardTaskResume();
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
		EXTI_ClearITPendingBit(KEY_RIGHT_EXTI_LINE);
	}
}

void I2C3_ER_IRQHandler(void)
{
	//TODO: stub
	volatile uint32_t SR1Register, SR2Register;
	/* Read the I2C1 status register */
	SR1Register = ADC_I2C->SR1;
	if (SR1Register & 0x0F00)
	{ //an error
		//error = true;
		// I2C1error.error = ((SR1Register & 0x0F00) >> 8);        //save error
		// I2C1error.job = job;    //the task
	}
	/* If AF, BERR or ARLO, abandon the current job and commence new if there are jobs */
	if (SR1Register & 0x0700)
	{
		SR2Register = ADC_I2C->SR2; //read second status register to clear ADDR if it is set (note that BTF will not be set after a NACK)
		I2C_ITConfig(ADC_I2C, I2C_IT_BUF, DISABLE); //disable the RXNE/TXE interrupt - prevent the ISR tailchaining onto the ER (hopefully)
		if (!(SR1Register & 0x0200) && !(ADC_I2C->CR1 & 0x0200))
		{ //if we dont have an ARLO error, ensure sending of a stop
			if (ADC_I2C->CR1 & 0x0100)
			{ //We are currently trying to send a start, this is very bad as start,stop will hang the peripheral
				while (ADC_I2C->CR1 & 0x0100)
					; //wait for any start to finish sending
				I2C_GenerateSTOP(ADC_I2C, ENABLE); //send stop to finalise bus transaction
				while (ADC_I2C->CR1 & 0x0200)
					; //wait for stop to finish sending
//				i2cInit(ADC_I2C); //reset and configure the hardware//TODO: add
			}
			else
			{
				I2C_GenerateSTOP(ADC_I2C, ENABLE); //stop to free up the bus
				I2C_ITConfig(ADC_I2C, I2C_IT_EVT | I2C_IT_ERR, DISABLE); //Disable EVT and ERR interrupts while bus inactive
			}
		}
	}
	ADC_I2C->SR1 &= ~0x0F00; //reset all the error bits to clear the interrupt
}

void I2C3_EV_IRQHandler(void)
{
	xHigherPriorityTaskWoken = pdFALSE;

	static int index = 0;
	static bool regSent = false;
	uint32_t event = I2C_GetLastEvent(I2C3);
	if ((i2cData_it[i2cPointer].dir) == DirWrite)
	{
		switch (event)
		{
		// EV5
		case I2C_EVENT_MASTER_MODE_SELECT:
			//Wyslanie adresu w trybie zapisujacego urzadzenia nadrzednego
			I2C_Send7bitAddress(ADC_I2C, ExtADCAddr, I2C_Direction_Transmitter);
			break;

			// EV6
		case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
			// First byte (register)
			I2C_SendData(ADC_I2C, i2cData_it[i2cPointer].reg);
			break;

			// EV8
		case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
			if (index < i2cData_it[i2cPointer].length)
			{
				//send next bytes
				I2C_SendData(ADC_I2C, i2cData_it[i2cPointer].val[index++]);
			}
			else //FIXME: unnecessary ?
			{
				I2C_ITConfig(ADC_I2C, I2C_IT_BUF, DISABLE);
			}
			break;

			// EV8_2
		case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
			if (index < i2cData_it[i2cPointer].length) //protection for situation where next byte wasn't quick enough
			{
				//send next bytes
				I2C_SendData(ADC_I2C, i2cData_it[i2cPointer].val[index++]);
				//TODO: add some logging here
			}
			else //everything ok
			{
				// STOP
				index = 0;
				I2C_GenerateSTOP(ADC_I2C, ENABLE);
				I2C_ITConfig(ADC_I2C, I2C_IT_EVT | I2C_IT_ERR | I2C_IT_BUF, DISABLE); //FIXME: Disable all or only EVT?
				xQueueSendFromISR(xQueue_I2CEvent, (void* ) &i2cPointer, &xHigherPriorityTaskWoken);
			}
			break;

		default:
			//TODO: show info
			break;
		}
	}
	else
	{
		switch (event)
		{
		// EV5
		case I2C_EVENT_MASTER_MODE_SELECT:
			if (!regSent)
			{
				//Wyslanie adresu w trybie zapisujacego urzadzenia nadrzednego
				I2C_Send7bitAddress(ADC_I2C, ExtADCAddr, I2C_Direction_Transmitter);
				regSent = true;
			}
			else
			{
				//Send addr in read mode
				I2C_Send7bitAddress(ADC_I2C, ExtADCAddr, I2C_Direction_Receiver);
			}
			break;

			//EV6 in transmitter mode
		case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
			// Send register
			I2C_SendData(ADC_I2C, i2cData_it[i2cPointer].reg);
			break;

			// EV6 in receiver mode
		case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
			//cool story bro :)
			break;

			// EV8
		case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
			//sending register. ignore
			break;

			// EV8_2
		case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
			// Reg sent, change to receiver
			I2C_GenerateSTART(ADC_I2C, ENABLE);
			if (i2cData_it[i2cPointer].length > 1)
				I2C_AcknowledgeConfig(ADC_I2C, ENABLE);
			break;

			// EV7
		case I2C_EVENT_MASTER_BYTE_RECEIVED:
			//first check whether to send ACK (EV7_1)
			if (index >= (i2cData_it[i2cPointer].length - 2)) // two before last packet
			{
				//Disable I2C acknowledgment
				I2C_AcknowledgeConfig(ADC_I2C, DISABLE);
			}
			if (index >= (i2cData_it[i2cPointer].length - 1)) // one before last packet
			{
				//Send I2C STOP Condition
				I2C_GenerateSTOP(ADC_I2C, ENABLE);
			}

			i2cData_it[i2cPointer].val[index++] = I2C_ReceiveData(ADC_I2C);

			if (index >= i2cData_it[i2cPointer].length) //all data received, can ignore rest
			{
				index = 0;
				regSent = false;
				I2C_ReceiveData(ADC_I2C); //TODO: check! clear data register. flag I2C_SR1_RXNE
				I2C_ITConfig(ADC_I2C, I2C_IT_EVT | I2C_IT_ERR | I2C_IT_BUF, DISABLE);
				xQueueSendFromISR(xQueue_I2CEvent, (void* ) &i2cPointer, &xHigherPriorityTaskWoken);
			}
			break;
		default:
			//TODO: show info
			break;
		}
	}

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);

}

/**
 * @brief  This function handles TIM2 global interrupt request.
 * @param  None
 * @retval None
 */
void TIM2_IRQHandler(void)
{
	USB_OTG_BSP_TimerIRQ();
}

/**
 * @brief  This function handles USB-On-The-Go FS global interrupt request.
 * @param  None
 * @retval None
 */
void OTG_FS_IRQHandler(void)
{
	USBH_OTG_ISR_Handler(&USB_OTG_Core);
}

/**
 * @brief  This function handles TIM4 global interrupt request.
 * @param  None
 * @retval None
 */
void TIM4_IRQHandler(void)
{

	/* Checks whether the TIM interrupt has occurred */
	if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
		if (LED_Toggle == 3)
		{
			/* LED3 Orange toggling */
			STM_EVAL_LEDToggle(LED3);
			STM_EVAL_LEDOff(LED6);
			STM_EVAL_LEDOff(LED4);
		}
		else if (LED_Toggle == 4)
		{
			/* LED4 Green toggling */
			STM_EVAL_LEDToggle(LED4);
			STM_EVAL_LEDOff(LED6);
			STM_EVAL_LEDOff(LED3);
		}
		else if (LED_Toggle == 6)
		{
			/* LED6 Blue toggling */
			STM_EVAL_LEDOff(LED3);
			STM_EVAL_LEDOff(LED4);
			STM_EVAL_LEDToggle(LED6);
		}
		else if (LED_Toggle == 0)
		{
			/* LED6 Blue On to signal Pause */
			STM_EVAL_LEDOn(LED6);
		}
		else if (LED_Toggle == 7)
		{
			/* LED4 toggling with frequency = 439.4 Hz */
			STM_EVAL_LEDOff(LED3);
			STM_EVAL_LEDOff(LED4);
			STM_EVAL_LEDOff(LED5);
			STM_EVAL_LEDOff(LED6);
		}
		capture = TIM_GetCapture1(TIM4);
		TIM_SetCompare1(TIM4, capture + CCR_Val);
	}
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
