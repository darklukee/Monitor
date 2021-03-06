/**
 ******************************************************************************
 * @file    STM32F4-Discovery FreeRTOS demo\hw_config.cpp
 * @brief   Hardware initialization
 ******************************************************************************
 */

/* Library includes. */
#include "hw_config.h"
//#include "glcd_Config.h"
#include "include/glcd_io.h"
//#include "glcd.h"
#include "ExtADC.h"
#include "PGA.h"

/*-----------------------------------------------------------*/
void prvSetupHardware(void)
{
	// Set the Vector Table base address at 0x08000000
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	// Configure LED IOs as output push-pull
	// Initialize LEDs on STM32F4_Discovery board
	prvLED_Config(GPIO);
	// Configure User button pin (PA0) as external interrupt -> modes switching
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);

	// LCD pin init
	//prvLCD_Config();
	// Initialize LCD LED PWM. Timer initialization included
	//prvLCDLED_Config();

	// Keyboard init with interupts
	//prvKeyboard_Config();
	/*
	 //Set pins for communication with external chips
	 prvADC_GPIO_Config(); //i2c
	 prvPGA_GPIO_Config(); //spi

	 //Translator Output Enable conf and set it to 'enable'
	 prvSerialOE_Config();
	 */
	// Configure LIS302 in order to produce data used for TIM4 reconfiguration and LED control
//	prvMEMS_Config();
}

void prvLED_Config(char state)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// GPIOD Periph clock enable
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	// Configure PD12, PD13, PD14 and PD15 in output push-pull mode
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	if (state == GPIO)
	{
		// standard output pin
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
	}
	else
	{
		//-------------------------- GPIO Configuration ----------------------------
		// GPIOD Configuration: Pins 12, 13, 14 and 15 in output push-pull - alternative mode
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_Init(GPIOD, &GPIO_InitStructure);

		// Connect TIM4 pins to AF2
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);
	}
	/* Configuration of Timer4 to control LEDs based on MEMS data */
	prvTIM4_Config();
}

void prvLCDLED_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// GPIOC Periph clock enable
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	// Configure PC6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //Timer b�dzie pracowa� na poziomie kHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //Timer
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// Connect TIM3 pins to AF2
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3);

	// Configuration of Timer3 to control LCD LED backlight
	prvTIM3_Config();
}

void prvLCD_Config(void)
{
	lcdInitConfig();
}

//const uint16_t KEY_PIN[KEYn] =
//{ KEY_UP_PIN, KEY_DOWN_PIN, KEY_LEFT_PIN, KEY_RIGHT_PIN, KEY_OK_PIN, KEY_ESC_PIN };
//GPIO_TypeDef* KEY_PORT[KEYn] =
//{ KEY_UP_GPIO_PORT, KEY_DOWN_GPIO_PORT, KEY_LEFT_GPIO_PORT, KEY_RIGHT_GPIO_PORT, KEY_OK_GPIO_PORT, KEY_ESC_GPIO_PORT };
//const uint32_t KEY_CLK[KEYn] =
//{ KEY_UP_GPIO_CLK, KEY_DOWN_GPIO_CLK, KEY_LEFT_GPIO_CLK, KEY_RIGHT_GPIO_CLK, KEY_OK_GPIO_CLK, KEY_ESC_GPIO_CLK };
//const uint16_t KEY_EXTI_LINE[KEYn] =
//{ KEY_UP_EXTI_LINE, KEY_DOWN_EXTI_LINE, KEY_LEFT_EXTI_LINE, KEY_RIGHT_EXTI_LINE, KEY_OK_EXTI_LINE, KEY_ESC_EXTI_LINE };
//const uint8_t KEY_PORT_SOURCE[KEYn] =
//{ KEY_UP_EXTI_PORT_SOURCE, KEY_DOWN_EXTI_PORT_SOURCE, KEY_LEFT_EXTI_PORT_SOURCE, KEY_RIGHT_EXTI_PORT_SOURCE,
//KEY_OK_EXTI_PORT_SOURCE, KEY_ESC_EXTI_PORT_SOURCE };
//const uint8_t KEY_PIN_SOURCE[KEYn] =
//{ KEY_UP_EXTI_PIN_SOURCE, KEY_DOWN_EXTI_PIN_SOURCE, KEY_LEFT_EXTI_PIN_SOURCE, KEY_RIGHT_EXTI_PIN_SOURCE,
//KEY_OK_EXTI_PIN_SOURCE, KEY_ESC_EXTI_PIN_SOURCE };
//const uint8_t KEY_IRQn[KEYn] =
//{ KEY_UP_EXTI_IRQn, KEY_DOWN_EXTI_IRQn, KEY_LEFT_EXTI_IRQn, KEY_RIGHT_EXTI_IRQn, KEY_OK_EXTI_IRQn, KEY_ESC_EXTI_IRQn };

void prvKeyboard_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//Common settings for all pins

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	// Configure Button pin as input
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	// Configure Button EXTI line
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	// Enable and set Button EXTI Interrupt to the lowest priority
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F; //thats correct, higher val means lower priority
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	//Pin specific:
	for (int i = 0; i < KEYn; i++)
	{
		// Enable the BUTTON Clock
		RCC_AHB1PeriphClockCmd(KEY_CLK[(Key_TypeDef) i], ENABLE);

		GPIO_InitStructure.GPIO_Pin = KEY_PIN[(Key_TypeDef) i];
		GPIO_Init(KEY_PORT[(Key_TypeDef) i], &GPIO_InitStructure);

		// Connect Button EXTI Line to Button GPIO Pin
		SYSCFG_EXTILineConfig(KEY_PORT_SOURCE[(Key_TypeDef) i], KEY_PIN_SOURCE[(Key_TypeDef) i]);

		// Configure Button EXTI line
		EXTI_InitStructure.EXTI_Line = KEY_EXTI_LINE[(Key_TypeDef) i];
		EXTI_Init(&EXTI_InitStructure);

		// Enable and set Button EXTI Interrupt to the lowest priority
		NVIC_InitStructure.NVIC_IRQChannel = KEY_IRQn[(Key_TypeDef) i];
		NVIC_Init(&NVIC_InitStructure);
	}
}

void prvSerialOE_Config(void)
{
	// GPIOE Periph clock enable
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	//Set to high to enable translator outputs
	GPIO_SetBits(GPIOE, GPIO_Pin_6); //TODO: move to tasks, along with ADC and PGA
}

void prvADC_GPIO_Config(void)
{
	ExtADC::GPIO_Config();
}
void prvPGA_GPIO_Config(void)
{
	PGA::GPIO_Config();
}

void prvTIM4_Config(void)
{
	uint16_t PrescalerValue = 0;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	/* --------------------------- System Clocks Configuration -----------------*/
	/* TIM4 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	/* -----------------------------------------------------------------------
	 TIM4 Configuration: Output Compare Timing Mode:

	 In this example TIM4 input clock (TIM4CLK) is set to 2 * APB1 clock (PCLK1),
	 since APB1 prescaler is different from 1 (APB1 Prescaler = 4, see system_stm32f4xx.c file).
	 TIM4CLK = 2 * PCLK1
	 PCLK1 = HCLK / 4
	 => TIM4CLK = 2*(HCLK / 4) = HCLK/2 = SystemCoreClock/2

	 To get TIM4 counter clock at 2 KHz, the prescaler is computed as follows:
	 Prescaler = (TIM4CLK / TIM1 counter clock) - 1
	 Prescaler = (168 MHz/(2 * 2 KHz)) - 1 = 41999

	 To get TIM4 output clock at 1 Hz, the period (ARR)) is computed as follows:
	 ARR = (TIM4 counter clock / TIM4 output clock) - 1
	 = 1999

	 TIM4 Channel1 duty cycle = (TIM4_CCR1/ TIM4_ARR)* 100 = 50%
	 TIM4 Channel2 duty cycle = (TIM4_CCR2/ TIM4_ARR)* 100 = 50%
	 TIM4 Channel3 duty cycle = (TIM4_CCR3/ TIM4_ARR)* 100 = 50%
	 TIM4 Channel4 duty cycle = (TIM4_CCR4/ TIM4_ARR)* 100 = 50%

	 ==> TIM4_CCRx = TIM4_ARR/2 = 1000  (where x = 1, 2, 3 and 4).

	 Note:
	 SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
	 Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
	 function to update SystemCoreClock variable value. Otherwise, any configuration
	 based on this variable will be incorrect.
	 ----------------------------------------------------------------------- */

	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 2000000) - 1; //TIM4 counter clock at 2 MHz

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = TIM_ARR;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	/* Enable TIM4 Preload register on ARR */
	TIM_ARRPreloadConfig(TIM4, ENABLE);

	/* TIM PWM1 Mode configuration: Channel */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = TIM_CCR;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	/* Output Compare PWM1 Mode configuration: Channel1 */
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_CCxCmd(TIM4, TIM_Channel_1, DISABLE);

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

	/* Output Compare PWM1 Mode configuration: Channel2 */
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_CCxCmd(TIM4, TIM_Channel_2, DISABLE);

	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

	/* Output Compare PWM1 Mode configuration: Channel3 */
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_CCxCmd(TIM4, TIM_Channel_3, DISABLE);

	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

	/* Output Compare PWM1 Mode configuration: Channel4 */
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_CCxCmd(TIM4, TIM_Channel_4, DISABLE);

	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
}

void prvTIM3_Config(void)
{
	uint16_t PrescalerValue = 0;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	/* --------------------------- System Clocks Configuration -----------------*/
	/* TIM4 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* -----------------------------------------------------------------------
	 TIM4 Configuration: Output Compare Timing Mode:

	 In this example TIM4 input clock (TIM4CLK) is set to 2 * APB1 clock (PCLK1),
	 since APB1 prescaler is different from 1 (APB1 Prescaler = 4, see system_stm32f4xx.c file).
	 TIM4CLK = 2 * PCLK1
	 PCLK1 = HCLK / 4
	 => TIM4CLK = 2*(HCLK / 4) = HCLK/2 = SystemCoreClock/2

	 To get TIM4 counter clock at 2 KHz, the prescaler is computed as follows:
	 Prescaler = (TIM4CLK / TIM1 counter clock) - 1
	 Prescaler = (168 MHz/(2 * 2 KHz)) - 1 = 41999

	 To get TIM4 output clock at 1 Hz, the period (ARR)) is computed as follows:
	 ARR = (TIM4 counter clock / TIM4 output clock) - 1
	 = 1999

	 TIM4 Channel1 duty cycle = (TIM4_CCR1/ TIM4_ARR)* 100 = 50%
	 TIM4 Channel2 duty cycle = (TIM4_CCR2/ TIM4_ARR)* 100 = 50%
	 TIM4 Channel3 duty cycle = (TIM4_CCR3/ TIM4_ARR)* 100 = 50%
	 TIM4 Channel4 duty cycle = (TIM4_CCR4/ TIM4_ARR)* 100 = 50%

	 ==> TIM4_CCRx = TIM4_ARR/2 = 1000  (where x = 1, 2, 3 and 4).

	 Note:
	 SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
	 Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
	 function to update SystemCoreClock variable value. Otherwise, any configuration
	 based on this variable will be incorrect.
	 ----------------------------------------------------------------------- */

	/* Compute the prescaler value */
	const uint32_t counterFreq = 2000000;
	PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / counterFreq) - 1; //TIM3 counter clock at 2 MHz, PWM clock at 1kHz

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = TIM_ARR;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* TIM PWM1 Mode configuration: Channel */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = TIM_CCR;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	/* Output Compare PWM1 Mode configuration: Channel1 */
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	//TIM_CCxCmd(TIM3, TIM_Channel_1, DISABLE);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

	//TIM_CCxCmd(TIM3, TIM_Channel_1, ENABLE);
	int duty = 20; //%
	TIM_SetCompare1(TIM3, TIM_CCR + (TIM_ARR * duty / 100));

	/* Enable TIM3 Preload register on ARR */
	TIM_ARRPreloadConfig(TIM3, ENABLE);

	TIM_Cmd(TIM3, ENABLE);
}

//void prvMEMS_Config(void)
//{
//	uint8_t ctrl = 0;
//	uint32_t i=0;		//simple index for software delay
//
//	LIS302DL_InitTypeDef  LIS302DL_InitStruct;
//	LIS302DL_InterruptConfigTypeDef LIS302DL_InterruptStruct;
//
//	/* Set configuration of LIS302DL*/
//	LIS302DL_InitStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;
//	LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_400;
//	LIS302DL_InitStruct.Axes_Enable = LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE | LIS302DL_Z_ENABLE;
//	LIS302DL_InitStruct.Full_Scale = LIS302DL_FULLSCALE_2_3;
//	LIS302DL_InitStruct.Self_Test = LIS302DL_SELFTEST_NORMAL;
//	LIS302DL_Init(&LIS302DL_InitStruct);
//
//	/* Set configuration of Internal High Pass Filter of LIS302DL*/
//	LIS302DL_InterruptStruct.Latch_Request = LIS302DL_INTERRUPTREQUEST_LATCHED;
//	LIS302DL_InterruptStruct.SingleClick_Axes = LIS302DL_CLICKINTERRUPT_Z_ENABLE;
//	LIS302DL_InterruptStruct.DoubleClick_Axes = LIS302DL_DOUBLECLICKINTERRUPT_Z_ENABLE;
//	LIS302DL_InterruptConfig(&LIS302DL_InterruptStruct);
//
//	/* Required delay for the MEMS Accelerometer: Turn-on time = 3/Output data Rate
//	                                                            = 3/100 = 30ms */
//	for(i=0;i<0x1FFFF;i++);
//
//	/* Configure Interrupt control register: enable Click interrupt1 */
//	ctrl = 0x07;
//	LIS302DL_Write(&ctrl, LIS302DL_CTRL_REG3_ADDR, 1);
//
//	/* Enable Interrupt generation on click/double click on Z axis */
//	ctrl = 0x70;
//	LIS302DL_Write(&ctrl, LIS302DL_CLICK_CFG_REG_ADDR, 1);
//
//	/* Configure Click Threshold on X/Y axis (10 x 0.5g) */
//	ctrl = 0xAA;
//	LIS302DL_Write(&ctrl, LIS302DL_CLICK_THSY_X_REG_ADDR, 1);
//
//	/* Configure Click Threshold on Z axis (10 x 0.5g) */
//	ctrl = 0x0A;
//	LIS302DL_Write(&ctrl, LIS302DL_CLICK_THSZ_REG_ADDR, 1);
//
//	/* Configure Time Limit */
//	ctrl = 0x03;
//	LIS302DL_Write(&ctrl, LIS302DL_CLICK_TIMELIMIT_REG_ADDR, 1);
//
//	/* Configure Latency */
//	ctrl = 0x7F;
//	LIS302DL_Write(&ctrl, LIS302DL_CLICK_LATENCY_REG_ADDR, 1);
//
//	/* Configure Click Window */
//	ctrl = 0x7F;
//	LIS302DL_Write(&ctrl, LIS302DL_CLICK_WINDOW_REG_ADDR, 1);
//}

/**
 * @brief  MEMS accelerometer management of the timeout situation.
 * @param  None.
 * @retval None.
 */
//uint32_t LIS302DL_TIMEOUT_UserCallback(void)
//{
//  /* MEMS Accelerometer Timeout error has occured */
//  while (1)
//  {
//  }
//  return -1;
//}
