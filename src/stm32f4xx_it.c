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

#include "usb_hcd_int.h"
#include "usb_bsp.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
portBASE_TYPE xHigherPriorityTaskWoken;
extern xQueueHandle xQueue_I2CEvent;

//USB
uint16_t capture = 0;
extern __IO uint16_t CCR_Val;
__IO uint8_t LED_Toggle = 0;
extern USB_OTG_CORE_HANDLE USB_OTG_Core;
//extern USBH_HOST USB_Host;

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
