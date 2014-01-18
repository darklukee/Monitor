/**
 ******************************************************************************
 * @file    STM32F4-Discovery FreeRTOS demo\main.c
 * @author  T.O.M.A.S. Team
 * @version V1.1.0
 * @date    14-October-2011
 * @brief   Main program body
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

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
//cpp_wrapper
#include <cpp_task.hpp>

#include "hw_config.h"

extern "C"
{
#include "usb_core.h"
}

//#include <stdio.h> //printf//#include "math.h"
#include "Global.h"
#include "DataStructures.h"
#include "ExtADC.h"
#include "ExtADCTask.h"
#include "CalculatorTask.h"
#include "Display.h"
#include "DisplayTask.h"
#include "UsbTask.h"
#include "UsbHostTask.h"
#include "UsbReset.h"
#include "KeyboardTask.h"

//#include "TestTask.h"
//#include "LcdPinTestTask.h"

// global queues
//i2c
xQueueHandle xQueue_I2CEvent;
xQueueHandle xQueue_AdcData;
//glcd
xQueueHandle xQueue_Lcd;
xQueueHandle xQueue_Lcd_Log;
//usb
xQueueHandle xQueue_Storage;
xQueueHandle xQueue_UsbReset;
xSemaphoreHandle xSemaphore_UsbMutex;

int main(void)
{
	/*debugger crash workaround TODO: deal with this abomination */
	Global &global = Global::getInstance();
//	glob.setSwitch(eLcdMenu, (uint8_t) true);
	/*end of debugger crash prevention*/

	//create queues
	xQueue_I2CEvent = xQueueCreate(3, sizeof(int));
	xQueue_AdcData = xQueueCreate(10, sizeof(AdcData));
	xQueue_Lcd = xQueueCreate(5, sizeof(LcdData));
	xQueue_Lcd_Log = xQueueCreate(3, sizeof(char[22]));
	xQueue_Storage = xQueueCreate(20, sizeof(StorageData));
	xQueue_UsbReset = xQueueCreate(2, sizeof(USB_OTG_CORE_HANDLE*));
	//create semaphores
	xSemaphore_UsbMutex = xSemaphoreCreateMutex();
	//register queue names
	vQueueAddToRegistry(xQueue_I2CEvent, (signed char*)"I2cEvent");
	vQueueAddToRegistry(xQueue_AdcData, (signed char*)"AdcData");
	vQueueAddToRegistry(xQueue_Lcd, (signed char*)"Lcd");
	vQueueAddToRegistry(xQueue_Lcd_Log, (signed char*)"Lcd_Log");
	vQueueAddToRegistry(xQueue_Storage, (signed char*)"Storage");
	vQueueAddToRegistry(xQueue_UsbReset, (signed char*)"UsbReset");
	//vQueueAddToRegistry(xSemaphore_UsbMutex, (signed char*)"UsbMutex"); //useless

	/* initialize hardware... */
	prvSetupHardware();

	/* Start the tasks  */
//	scheduler_add_task(new TestTask());
//	scheduler_add_task(new LcdPinTestTask());
	scheduler_add_task(new UsbReset());
	scheduler_add_task(new DisplayTask());
	scheduler_add_task(new ExtADCTask());
	scheduler_add_task(new UsbHostTask());
	scheduler_add_task(new UsbTask());
	scheduler_add_task(new KeyboardTask());
	scheduler_add_task(new CalculatorTask());

	/* Start the scheduler. */
	//vTaskStartScheduler();
	scheduler_start(false);

	//no point in deleting Tasks
	/* Will only get here if there was not enough heap space to create the idle task. */
	return 0;
}
//-----------------------------------------------------------------------------

/*-----------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif
