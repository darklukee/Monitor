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

#include "hw_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
portBASE_TYPE xHigherPriorityTaskWoken;
extern xQueueHandle xQueue_I2CEvent;

/* Private function prototypes -----------------------------------------------*/

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
	if(EXTI_GetITStatus(KEY_OK_EXTI_LINE) != RESET)
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
	if(EXTI_GetITStatus(KEY_ESC_EXTI_LINE) != RESET)
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
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)
  {
		//DO NOT use, something is wrong here
		EXTI_ClearITPendingBit(EXTI_Line5);
  }
	if(EXTI_GetITStatus(KEY_UP_EXTI_LINE) != RESET)
  {
		EXTI_ClearITPendingBit(KEY_UP_EXTI_LINE);
  }
	if(EXTI_GetITStatus(KEY_DOWN_EXTI_LINE) != RESET)
  {
		EXTI_ClearITPendingBit(KEY_DOWN_EXTI_LINE);
  }
	if(EXTI_GetITStatus(KEY_LEFT_EXTI_LINE) != RESET)
  {
		EXTI_ClearITPendingBit(KEY_LEFT_EXTI_LINE);
  }
	if(EXTI_GetITStatus(KEY_RIGHT_EXTI_LINE) != RESET)
  {
		EXTI_ClearITPendingBit(KEY_RIGHT_EXTI_LINE);
  }
}

void I2C3_ER_IRQHandler(void)
{
	//TODO: stub

}

void I2C3_EV_IRQHandler(void)
{
	xHigherPriorityTaskWoken = pdFALSE;
	//I2C_ClearITPendingBit ?? chyba getlastevent to załatwia
	//
	//if (EXTI_GetITStatus(EXTI_Line0) != RESET) //FIXME: wrong EXTI_Line
	//{
		//xSemaphoreGiveFromISR(xSemaphoreSW,&xHigherPriorityTaskWoken);
		xQueueSendFromISR(xQueue_I2CEvent, I2C_GetLastEvent(I2C3), &xHigherPriorityTaskWoken); //TODO poiter, references of sth

		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	//}
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
