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

#include "hw_config.h"

#include <stdio.h> //printf//#include "math.h"

//cpp_wrapper
#include <cpp_task.hpp>

//debugger crash test TODO: check if necessary
#include "Global.h"
#include "ExtADC.h"
#include "ExtADCTask.h"
#include "Display.h"

#include "TestTask.h"
#include "DisplayTask.h"

// global queues
//i2c
xQueueHandle xQueue_I2CEvent;
xQueueHandle xQueue_I2CQuery;
xQueueHandle xQueue_I2CRx;
//glcd
xQueueHandle xQueue_Lcd;



int main(void)
{
	/*debugger crash workaround TODO: deal with this abomination */
	Global &glob = Global::getInstance();
//	glob.setSwitch(eLcdMenu, (uint8_t) true);
	ExtADC ext;
	Display dis;
	ExtADCTask ext2;

	printf("%f", 3.141592);

	/*end of debugger crash prevention*/

	//create queues
	xQueue_I2CEvent = xQueueCreate(10, sizeof(uint8_t)); //TODO: set size
	xQueue_I2CQuery = xQueueCreate(10, sizeof(uint8_t)); //TODO: set size
	xQueue_I2CRx = xQueueCreate(10, sizeof(uint8_t)); //TODO: set size
	xQueue_Lcd = xQueueCreate(10, sizeof(uint8_t)); //TODO: set size

	/* initialize hardware... */
	prvSetupHardware();

	/* Start the tasks  */
	scheduler_add_task(new TestTask());
	scheduler_add_task(new DisplayTask());

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
