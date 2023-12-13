
/* Includes ------------------------------------------------------------------*/
#include "main.h"

void SystemClock_Config(void);

void RTC_Init(void);
void SYS_TIM_Init(void);

extern TIM_HandleTypeDef htim7;
extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef sTimeCurrent;
extern RTC_DateTypeDef sDateCurrent;

/*********************************** Дисплей **********************************/

void TFT_SPI_Init(void);
void TFT_DMA_FILL_Init(void);
void TFT_LED_TIM_Init(void);

extern SPI_HandleTypeDef hspi2;
extern DMA_HandleTypeDef hdma_spi2_tx;
extern DMA_HandleTypeDef hdma_memtomem_fill;;
extern TIM_HandleTypeDef htim14;

// TFT_SCK
#define TFT_SPI2_SCK_Pin GPIO_PIN_13
#define TFT_SPI2_SCK_GPIO_Port GPIOB
// TFT MOSI
#define TFT_SPI2_MOSI_Pin GPIO_PIN_15
#define TFT_SPI2_MOSI_GPIO_Port GPIOB
// TFT_DC
#define TFT_DC_Pin GPIO_PIN_14
#define TFT_DC_GPIO_Port GPIOB
#define TFT_DC_DATA()      HAL_GPIO_WritePin(TFT_DC_GPIO_Port,TFT_DC_Pin,GPIO_PIN_SET)
#define TFT_DC_COMMAND()   HAL_GPIO_WritePin(TFT_DC_GPIO_Port,TFT_DC_Pin,GPIO_PIN_RESET)
// TFT_CS
#define TFT_CS_Pin GPIO_PIN_12
#define TFT_CS_GPIO_Port GPIOB
#define TFT_CS_ACTIVE()    HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_RESET)
#define TFT_CS_IDLE()      HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_SET)
// TFT_RESET
#define TFT_RST_Pin GPIO_PIN_10
#define TFT_RST_GPIO_Port GPIOA
#define TFT_RST_ACTIVE()   HAL_GPIO_WritePin(TFT_RST_GPIO_Port,TFT_RST_Pin,GPIO_PIN_RESET)
#define TFT_RST_IDLE()     HAL_GPIO_WritePin(TFT_RST_GPIO_Port,TFT_RST_Pin,GPIO_PIN_SET)
// TFT_LED_TIM
#define TFT_LED_TIM_Pin  GPIO_PIN_1
#define TFT_LED_TIM_Port GPIOB

/************************************* АЦП ************************************/

extern ADC_HandleTypeDef hadc;

void ADC_Init(void);

// Датчик температуры дополнительный
#define T_add_Pin GPIO_PIN_1
#define T_add_GPIO_Port GPIOA
// Датчик температуры подачи
#define T_in_Pin GPIO_PIN_2
#define T_in_GPIO_Port GPIOA
// Датчик температуры обратки
#define T_out_Pin GPIO_PIN_3
#define T_out_GPIO_Port GPIOA
// Датчик температуры воздуха
#define T_amb_Pin GPIO_PIN_4
#define T_amb_GPIO_Port GPIOA

/***************************** Порты ввода/вывода *****************************/

void GPIO_Init(void);

#define ALARM_Pin       GPIO_PIN_13
#define ALARM_GPIO_Port GPIOC
#define ALARM_PIN_ON    HAL_GPIO_WritePin(ALARM_GPIO_Port,ALARM_Pin,GPIO_PIN_SET)
#define ALARM_PIN_OFF   HAL_GPIO_WritePin(ALARM_GPIO_Port,ALARM_Pin,GPIO_PIN_RESET)

// Внешние прерывания

extern uint8_t SW2_P;

#define Stop_EXT_Pin GPIO_PIN_0
#define Stop_EXT_GPIO_Port GPIOF
#define SW1_Pin GPIO_PIN_1
#define SW1_GPIO_Port GPIOF
#define SW2_Pin GPIO_PIN_0
#define SW2_GPIO_Port GPIOA
#define Stop_EXT_State HAL_GPIO_ReadPin(Stop_EXT_GPIO_Port,Stop_EXT_Pin)
#define SW1_State HAL_GPIO_ReadPin(SW1_GPIO_Port,SW1_Pin)
#define SW2_State HAL_GPIO_ReadPin(SW2_GPIO_Port,SW2_Pin) //SW2_P
#define Detect_Pin GPIO_PIN_5
#define Detect_GPIO_Port GPIOA
#define Detect_EXTI_IRQn EXTI4_15_IRQn

// Светодиоды
#define LED_Fan_Pin         GPIO_PIN_11
#define LED_Fan_GPIO_Port   GPIOA
#define LED_Fan_ON          HAL_GPIO_WritePin(LED_Fan_GPIO_Port,LED_Fan_Pin,GPIO_PIN_SET)
#define LED_Fan_OFF         HAL_GPIO_WritePin(LED_Fan_GPIO_Port,LED_Fan_Pin,GPIO_PIN_RESET)
#define LED_Pump_Pin        GPIO_PIN_7
#define LED_Pump_GPIO_Port  GPIOA
#define LED_Pump_ON         HAL_GPIO_WritePin(LED_Pump_GPIO_Port,LED_Pump_Pin,GPIO_PIN_SET)
#define LED_Pump_OFF        HAL_GPIO_WritePin(LED_Pump_GPIO_Port,LED_Pump_Pin,GPIO_PIN_RESET)

// Кнопки
#define But_Start_Pin       GPIO_PIN_0
#define But_Start_GPIO_Port GPIOB
#define But_START_State     HAL_GPIO_ReadPin(But_Start_GPIO_Port,But_Start_Pin)
#define But_Stop_Pin        GPIO_PIN_2
#define But_Stop_GPIO_Port  GPIOB
#define But_STOP_State      HAL_GPIO_ReadPin(But_Stop_GPIO_Port,But_Stop_Pin)
#define But_More_Pin        GPIO_PIN_8
#define But_More_GPIO_Port  GPIOA
#define But_MORE_State      HAL_GPIO_ReadPin(But_More_GPIO_Port,But_More_Pin)
#define But_Less_Pin        GPIO_PIN_9
#define But_Less_GPIO_Port  GPIOA
#define But_LESS_State      HAL_GPIO_ReadPin(But_Less_GPIO_Port,But_Less_Pin)

// Реле
#define KV_CirPump_Pin       GPIO_PIN_15
#define KV_CirPump_GPIO_Port GPIOA
#define KV_CirPump_ON        HAL_GPIO_WritePin(KV_CirPump_GPIO_Port,KV_CirPump_Pin,GPIO_PIN_SET)
#define KV_CirPump_OFF       HAL_GPIO_WritePin(KV_CirPump_GPIO_Port,KV_CirPump_Pin,GPIO_PIN_RESET)

/********************************* I2C ****************************************/

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern DMA_HandleTypeDef hdma_i2c1_tx;
extern DMA_HandleTypeDef hdma_i2c1_rx;

void EEPROM_I2C1_Init(void);
void I2C2_Init(void);

/********************************* CAN ****************************************/

extern CAN_HandleTypeDef hcan;

void CAN_Init(void);

/******************************** IWDG ****************************************/

extern IWDG_HandleTypeDef hiwdg;

void IWDG_Init(void);

/************************** Таймеры симисторов ********************************/

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

void TIM2_HWSPump_Init(void);
void TIM3_Fan_Supply_Init(void);

#define TIM_Triac1_Pin       GPIO_PIN_3
#define TIM_Triac1_GPIO_Port GPIOB
#define TIM_Supply_Pin       GPIO_PIN_4
#define TIM_Supply_GPIO_Port GPIOB
#define TIM_Fan_Pin          GPIO_PIN_5
#define TIM_Fan_GPIO_Port    GPIOB

#define HWSPump_TIM        TIM2 -> CCR2
#define HWSPump_TIM_Period htim2.Init.Period  // Период таймера насоса ГВС
#define Supply_TIM         TIM3 -> CCR1
#define Supply_TIM_Period  htim3.Init.Period // Период таймера шнека
#define Fan_TIM            TIM3 -> CCR2
#define Fan_TIM_Period     htim3.Init.Period  // Период таймера вентилятора

/**************************** Таймера пищалки *********************************/

extern TIM_HandleTypeDef htim16;

void TIM16_Buzzer_Init(void);

#define TIM_Buzzer_Pin       GPIO_PIN_6
#define TIM_Buzzer_GPIO_Port GPIOA

#define Buzzer_TIM         TIM16 -> CCR1
#define Buzzer_TIM_Period  htim16.Init.Period/100 // Период таймера пищалки в %