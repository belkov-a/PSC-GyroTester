
#include "main.h"
#include "stm32f0xx_it.h"
#include "Init.h"
#include "ILI9341.h"
#include "Menu.h"

extern DMA_HandleTypeDef hdma_i2c1_tx;
extern DMA_HandleTypeDef hdma_i2c1_rx;
extern I2C_HandleTypeDef hi2c1;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern SPI_HandleTypeDef hspi1;

/*******************************************************************************
  *
  * ���������� ���������� ������� TIM7 (�������� ������ 10 ���)
  *
  * ����������� ������������� ���� ������ ������ ����������
  *
********************************************************************************/

uint32_t FrameCount = 0;
uint8_t clear = 0;

void TIM7_IRQHandler(void)
{
  if(__HAL_TIM_GET_IT_SOURCE(&htim7, TIM_IT_UPDATE) != RESET)
  {
    __HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);
    
    // ��������� ������ ������
    if (FrameCount == 0) Menu_ProduceButtonFlags();
    
    // ��������� ��������� ���������
    //PWR_ProduceSysCounters();
    
    if (ISNT_SPI_DMA_BUSY) {clear = 0;}
    // ��������� �������� ������ 12,5 ��
    FrameCount++;
    if (FrameCount == 600) 
    {
      FrameCount = 0;
      if (clear == 1) 
      {
        SPI_DMA_BUSY_RESET; 
        clear = 0;
      }
      if (IS_SPI_DMA_BUSY) clear = 1; 
    }
  }
}


/*******************************************************************************
  *
  * ���������� ��������� ����
  *
*******************************************************************************/

void EXTI4_15_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(Detect_Pin);
  
  FreqPoint++;
  
  if (((sFrequency.Value >= 35) || (sFrequency.Value <= 100)) &&
      (LED_Counter == 3))
  {
      TIM3->ARR = Period;
      TIM2->ARR = Period;
      TIM2->CNT = 0;
      TIM3->CNT = 0;
            
      // �������� ��������
      // ������ ������� �� Period c ��������� Power_Fan_mdf*Period/100
      // ��� ���� Power_Fan_mdf, ��� ������ ������ �������� �������
        
      if (IS_FAN) Fan_TIM = (uint32_t) (Period + 10 - (Power_Fan_mdf*Period)/100);
      else Fan_TIM = (uint32_t) (Period + 2000);
      
      if (IS_SCREW) Supply_TIM = (uint32_t) (Period - Power_Screw*Period/100);
      else Supply_TIM = (uint32_t) (Period + 2000);
      
      if (IS_HWSPUMP) HWSPump_TIM = (uint32_t) (Period - Power_HWS*Period/100);
      else HWSPump_TIM = (uint32_t) (Period + 2000);
    
    // ��������� ������� ������� ����������� � �����
    __HAL_TIM_ENABLE(&htim3);
    // �������� ������� ������ ���
    __HAL_TIM_ENABLE(&htim2);
  }
}

/*******************************************************************************
  *
  * ���������� DMA ������� �������
  * 
  * ���� ������� ������ ��������, �� ��������� ������ �������
  *
*******************************************************************************/

void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_memtomem_fill);
  
  if (IS_CLEAR_TFT_BUF)    
  {
    CLEAR_TFT_BUF_RESET;
    // �������� ���������
    SPI_DMA_BUSY_RESET;
  }
}

/*******************************************************************************
  *
  * ���������� DMA I2C1 ������� EEPROM
  *
*******************************************************************************/

void DMA1_Channel2_3_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_i2c1_tx);
  HAL_DMA_IRQHandler(&hdma_i2c1_rx);
}

/*******************************************************************************
  *
  * ���������� DMA SPI2 �������
  *
*******************************************************************************/

void DMA1_Channel4_5_6_7_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi2_tx);
}

/*******************************************************************************
  *
  * ���������� I2C1 ������� EEPROM
  *
*******************************************************************************/

void I2C1_IRQHandler(void)
{
  if (hi2c1.Instance->ISR & (I2C_FLAG_BERR | I2C_FLAG_ARLO | I2C_FLAG_OVR)) {
    HAL_I2C_ER_IRQHandler(&hi2c1);
  } else {
    HAL_I2C_EV_IRQHandler(&hi2c1);
  }
}

/*******************************************************************************
  *
  * ���������� I2C2 �������� ������
  *
*******************************************************************************/

uint8_t aTxBuffer[4];
uint8_t aRxBuffer[4];
uint8_t Got = 0;

void I2C2_IRQHandler(void)
{
  if (hi2c2.Instance->ISR & (I2C_FLAG_BERR | I2C_FLAG_ARLO | I2C_FLAG_OVR)) {
    HAL_I2C_ER_IRQHandler(&hi2c2);
  } else {
    HAL_I2C_EV_IRQHandler(&hi2c2);
  }
  
  if (Got)
  {
    Got=0;
    if(HAL_I2C_Slave_Transmit_IT(&hi2c2, (uint8_t*)aTxBuffer, 1)!= HAL_OK)
    {
      /* Transfer error in transmission process */
      Error_Handler();    
    }
  }
}

/******************************************************************************/
/*           Cortex-M0 Processor Interruption and Exception Handlers          */
/******************************************************************************/

void NMI_Handler(void) { while (1) {}}
void HardFault_Handler(void) { while (1){}}
void SVC_Handler(void) {}
void PendSV_Handler(void) {}
void SysTick_Handler(void) { HAL_IncTick(); }
