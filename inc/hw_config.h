/* Library includes. */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_lis302dl.h"

#ifndef HW_CONFIG_H
#define HW_CONFIG_H

extern GPIO_InitTypeDef GPIO_InitStructure;
extern NVIC_InitTypeDef NVIC_InitStructure;

/* TIM4 and TIM3 Autoreload and Capture Compare register values */
#define TIM_ARR                          ((uint16_t)1999) //1 kHz na wyjsciu przy preskaler 200 khz#define TIM_CCR                          ((uint16_t)0) //0%/* definitions used by prvLED_Config() to reconfigure LED pins GPIO<->TIM4 */
#define GPIO	0
#define TIMER	1

void prvSetupHardware(void);
void prvTIM4_Config(void);
void prvTIM3_Config(void);
void prvLED_Config(char state);
void prvLCD_Config(void);
void prvLCDLED_Config(void);
void prvKeyboard_Config(void);
void prvSerialOE_Config(void);
void prvADC_GPIO_Config(void);
void prvPGA_GPIO_Config(void);

//Keyboard key macros
//Keep in sync with stm32f4xx_it.c !!!
#define KEYn 6

#define KEY_UP_PIN GPIO_Pin_4
#define KEY_UP_GPIO_PORT GPIOE
#define KEY_UP_GPIO_CLK RCC_AHB1Periph_GPIOE
#define KEY_UP_EXTI_LINE EXTI_Line4
#define KEY_UP_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
#define KEY_UP_EXTI_PIN_SOURCE EXTI_PinSource4
#define KEY_UP_EXTI_IRQn EXTI4_IRQn

#define KEY_DOWN_PIN GPIO_Pin_7
#define KEY_DOWN_GPIO_PORT GPIOB
#define KEY_DOWN_GPIO_CLK RCC_AHB1Periph_GPIOB
#define KEY_DOWN_EXTI_LINE EXTI_Line7
#define KEY_DOWN_EXTI_PORT_SOURCE EXTI_PortSourceGPIOB
#define KEY_DOWN_EXTI_PIN_SOURCE EXTI_PinSource7
#define KEY_DOWN_EXTI_IRQn EXTI9_5_IRQn

#define KEY_LEFT_PIN GPIO_Pin_5
#define KEY_LEFT_GPIO_PORT GPIOE
#define KEY_LEFT_GPIO_CLK RCC_AHB1Periph_GPIOE
#define KEY_LEFT_EXTI_LINE EXTI_Line5
#define KEY_LEFT_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
#define KEY_LEFT_EXTI_PIN_SOURCE EXTI_PinSource5
#define KEY_LEFT_EXTI_IRQn EXTI9_5_IRQn

#define KEY_RIGHT_PIN GPIO_Pin_8
#define KEY_RIGHT_GPIO_PORT GPIOB
#define KEY_RIGHT_GPIO_CLK RCC_AHB1Periph_GPIOB
#define KEY_RIGHT_EXTI_LINE EXTI_Line8
#define KEY_RIGHT_EXTI_PORT_SOURCE EXTI_PortSourceGPIOB
#define KEY_RIGHT_EXTI_PIN_SOURCE EXTI_PinSource8
#define KEY_RIGHT_EXTI_IRQn EXTI9_5_IRQn

#define KEY_OK_PIN GPIO_Pin_3
#define KEY_OK_GPIO_PORT GPIOE
#define KEY_OK_GPIO_CLK RCC_AHB1Periph_GPIOE
#define KEY_OK_EXTI_LINE EXTI_Line3
#define KEY_OK_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
#define KEY_OK_EXTI_PIN_SOURCE EXTI_PinSource3
#define KEY_OK_EXTI_IRQn EXTI3_IRQn

#define KEY_ESC_PIN GPIO_Pin_6
#define KEY_ESC_GPIO_PORT GPIOD
#define KEY_ESC_GPIO_CLK RCC_AHB1Periph_GPIOD
#define KEY_ESC_EXTI_LINE EXTI_Line6
#define KEY_ESC_EXTI_PORT_SOURCE EXTI_PortSourceGPIOD
#define KEY_ESC_EXTI_PIN_SOURCE EXTI_PinSource6
#define KEY_ESC_EXTI_IRQn EXTI9_5_IRQn

static const uint16_t KEY_PIN[KEYn] =
{ KEY_UP_PIN, KEY_DOWN_PIN, KEY_LEFT_PIN, KEY_RIGHT_PIN, KEY_OK_PIN, KEY_ESC_PIN };
static GPIO_TypeDef* KEY_PORT[KEYn] =
{ KEY_UP_GPIO_PORT, KEY_DOWN_GPIO_PORT, KEY_LEFT_GPIO_PORT, KEY_RIGHT_GPIO_PORT, KEY_OK_GPIO_PORT, KEY_ESC_GPIO_PORT };
static const uint32_t KEY_CLK[KEYn] =
{ KEY_UP_GPIO_CLK, KEY_DOWN_GPIO_CLK, KEY_LEFT_GPIO_CLK, KEY_RIGHT_GPIO_CLK, KEY_OK_GPIO_CLK, KEY_ESC_GPIO_CLK };
static const uint16_t KEY_EXTI_LINE[KEYn] =
{ KEY_UP_EXTI_LINE, KEY_DOWN_EXTI_LINE, KEY_LEFT_EXTI_LINE, KEY_RIGHT_EXTI_LINE, KEY_OK_EXTI_LINE, KEY_ESC_EXTI_LINE };
static const uint8_t KEY_PORT_SOURCE[KEYn] =
{ KEY_UP_EXTI_PORT_SOURCE, KEY_DOWN_EXTI_PORT_SOURCE, KEY_LEFT_EXTI_PORT_SOURCE, KEY_RIGHT_EXTI_PORT_SOURCE,
KEY_OK_EXTI_PORT_SOURCE, KEY_ESC_EXTI_PORT_SOURCE };
static const uint8_t KEY_PIN_SOURCE[KEYn] =
{ KEY_UP_EXTI_PIN_SOURCE, KEY_DOWN_EXTI_PIN_SOURCE, KEY_LEFT_EXTI_PIN_SOURCE, KEY_RIGHT_EXTI_PIN_SOURCE,
KEY_OK_EXTI_PIN_SOURCE, KEY_ESC_EXTI_PIN_SOURCE };
static const uint8_t KEY_IRQn[KEYn] =
{ KEY_UP_EXTI_IRQn, KEY_DOWN_EXTI_IRQn, KEY_LEFT_EXTI_IRQn, KEY_RIGHT_EXTI_IRQn, KEY_OK_EXTI_IRQn, KEY_ESC_EXTI_IRQn };

typedef enum
{
	//order sensitive. DO NOT TOUCH
	KEY_UP = 0,
	KEY_DOWN = 1,
	KEY_LEFT = 2,
	KEY_RIGHT = 3,
	KEY_OK = 4,
	KEY_ESC = 5,
} Key_TypeDef;

// I2C peripheral configuration defines
#define ADC_I2C I2C3
#define ADC_I2C_CLK RCC_APB1Periph_I2C3
#define ADC_I2C_SCL_GPIO_CLOCK RCC_AHB1Periph_GPIOA
#define ADC_I2C_SDA_GPIO_CLOCK RCC_AHB1Periph_GPIOC
#define ADC_I2C_GPIO_AF GPIO_AF_I2C3
#define ADC_I2C_SCL_GPIO GPIOA
#define ADC_I2C_SDA_GPIO GPIOC
#define ADC_I2C_SCL_PIN GPIO_Pin_8
#define ADC_I2C_SDA_PIN GPIO_Pin_9
#define ADC_I2C_SCL_PINSRC GPIO_PinSource8
#define ADC_I2C_SDA_PINSRC GPIO_PinSource9

static const uint8_t ExtADCAddr = 0x90; //LTC2991 hardware address

//void prvMEMS_Config(void);
//uint32_t LIS302DL_TIMEOUT_UserCallback(void);

#endif /*HW_CONFIG_H*/
