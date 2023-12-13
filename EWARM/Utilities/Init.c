
#include "Init.h"

/*******************************************************************************
  *
  *     ������������ ���������� ������� SystemClock
  *     
  *     ��������������� ���������� ������ - 8 ���
  *     �������������� ���������� ������  - 40 ���
  *
  *     HSE       - OFF
  *     LSE       - OFF
  *     HSI       - ON
  *     HSI14     - ON
  *     LSI       - ON
  *     PLL       - ON
  *     PLLSOURCE - HSI
  *     PLL       - 9
  *     PLL       - DIV2
  *
  *     ������� ����        - 36     ��� 
  *     ������� ������, DMA - 36     ���
  *     ������� APB1        - 36     ���
  *     ������� ADC         - 14     ���
  *     ������� RTC         - 40     K��
  *     ������� IWDG        - 40     ��� 
  *
*******************************************************************************/

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef       RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef       RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit     = {0};

  // ��������� ������������ �������
  __HAL_RCC_PWR_CLK_ENABLE();
  
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  
  // ������������ ���������� ���������� LSE
  HAL_PWR_EnableBkUpAccess();
  
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_HIGH);
  
  // ������������ ������ ��� CPU, AHB � APB
  RCC_OscInitStruct.OscillatorType        = RCC_OSCILLATORTYPE_HSI|
                                            RCC_OSCILLATORTYPE_HSI14|
                                            RCC_OSCILLATORTYPE_LSI|
                                            RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState              = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState              = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State            = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue   = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.LSIState              = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState          = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource         = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL            = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL.PREDIV            = RCC_PREDIV_DIV2;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  // ������������� ������ ��� CPU, AHB � APB
  RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK|
                                     RCC_CLOCKTYPE_SYSCLK|
                                     RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
  
  // ������������ ������ ���������
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection    = RCC_RTCCLKSOURCE_LSE;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
}

/*******************************************************************************
  *
  * ������������� ����������� �������
  *
*******************************************************************************/

IWDG_HandleTypeDef hiwdg;

void IWDG_Init(void)
{
  // ����� ���������
  hiwdg.Instance       = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
  hiwdg.Init.Window    = 4095;
  hiwdg.Init.Reload    = 4095;
  HAL_IWDG_Init(&hiwdg);
}

/*******************************************************************************
  *
  * ������������� ����� ��������� �������
  *
  * ������ �����             - 24 ����
  * ���� �� ���������        - 16.04.20
  * ����� �� ���������       - 12.00.00
  * ���� ������ �� ��������� - �����������
  *
*******************************************************************************/

RTC_HandleTypeDef hrtc;
RTC_TimeTypeDef sTimeCurrent = {0};
RTC_DateTypeDef sDateCurrent = {0};

void RTC_Init(void)
{
  // ��������� ������������ ����� ��������� �������
  __HAL_RCC_RTC_ENABLE();
  
  // ����� ���������
  hrtc.Instance            = RTC;
  hrtc.Init.HourFormat     = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv   = 127;
  hrtc.Init.SynchPrediv    = 255;
  hrtc.Init.OutPut         = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
  HAL_RTC_Init(&hrtc);
  
  // ������ ������� � ����
  HAL_RTC_GetTime(&hrtc, &sTimeCurrent, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sDateCurrent, RTC_FORMAT_BIN);
  
    // ���� ����� � ���� �� ���� ����������� ����� 
  // ��� ���� ���������� ������� 
  // �� ������ ����
  if ((sDateCurrent.Date == 1) && 
      (sDateCurrent.Month == 1) && 
      (sDateCurrent.Year == 0))
  {
    // ��������� ������� 
    sTimeCurrent.Hours          = 12;
    sTimeCurrent.Minutes        = 00;
    sTimeCurrent.Seconds        = 00;
    sTimeCurrent.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTimeCurrent.StoreOperation = RTC_STOREOPERATION_RESET;
    HAL_RTC_SetTime(&hrtc, &sTimeCurrent, RTC_FORMAT_BIN);
    
    // ��������� ����
    sDateCurrent.WeekDay        = RTC_WEEKDAY_THURSDAY;
    sDateCurrent.Date           = 10;
    sDateCurrent.Month          = RTC_MONTH_MARCH;
    sDateCurrent.Year           = 22;
    HAL_RTC_SetDate(&hrtc, &sDateCurrent, RTC_FORMAT_BIN);
  }
  
}

/*******************************************************************************
  *
  * ������������� ������� ��������� ���������� (TIM7)
  *
  * ������� 10 ���
  *
*******************************************************************************/

TIM_HandleTypeDef htim7;

void SYS_TIM_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  // ��������� ������������ �������
  __HAL_RCC_TIM7_CLK_ENABLE();
  
  // ����� ���������
  htim7.Instance               = TIM7;
  htim7.Init.Prescaler         = 320;
  htim7.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htim7.Init.Period            = 10;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  HAL_TIM_Base_Init(&htim7);
  
  // ������������ ���������
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig);

  // ���������� ���������� �� �������
  HAL_NVIC_SetPriority(TIM7_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(TIM7_IRQn);
  
  // ���������� ������ ������� � ������ ����������
  __HAL_TIM_ENABLE(&htim7); 
  __HAL_TIM_ENABLE_IT(&htim7, TIM_IT_UPDATE);
}

/*******************************************************************************
  *
  * ������������� SPI � ������ DMA �������
  *
  * SPI2_MOSI - PB15
  * SPI2_MISO - PB14
  * SPI2_SCK  - PB13
  * TFT_DC    - PB6
  *
  * SPI
  * �����������................8 ��� 
  * �������....................18 ����/���
  *
  * DMA1_Channel7 - SPI2_TX
  * �����......................������-���������
  * ��������� ���������........��������
  * ��������� ������...........�������
  * ������ ������ ���������....1 ����
  * ������ ������ ������.......1 ����
  * ����� ������...............����������
  *
*******************************************************************************/

SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi2_tx;

void TFT_SPI_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  // ��������� ������������
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_SPI2_CLK_ENABLE();

  // PA10     ------> TFT_RST  
  // PB12     ------> TFT_CS
  // PB13     ------> TFT_SPI2_SCK
  // PB14     ------> TFT_DC
  // PB15     ------> TFT_SPI2_MOSI
  
  // ������������� ����� SPI
  GPIO_InitStruct.Pin       = TFT_SPI2_SCK_Pin|
                              TFT_SPI2_MOSI_Pin;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_SPI2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  // ��������� ���������� ��������� �����
  HAL_GPIO_WritePin(TFT_DC_GPIO_Port,  TFT_DC_Pin,  GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TFT_CS_GPIO_Port,  TFT_CS_Pin,  GPIO_PIN_SET);
  HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_RESET);
  
  // ������������� ���� TFT_DC
  GPIO_InitStruct.Pin   = TFT_DC_Pin;           
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(TFT_DC_GPIO_Port, &GPIO_InitStruct);
  
  // ������������� ���� TFT_CS
  GPIO_InitStruct.Pin   = TFT_CS_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(TFT_DC_GPIO_Port, &GPIO_InitStruct);
  
  // ������������� ���� TFT_RST
  GPIO_InitStruct.Pin   = TFT_RST_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(TFT_RST_GPIO_Port, &GPIO_InitStruct); 
  
  // ������������ ���������� SPI ��� ����� � ��������
  hspi2.Instance               = SPI2;
  hspi2.Init.Mode              = SPI_MODE_MASTER;
  hspi2.Init.Direction         = SPI_DIRECTION_1LINE;
  hspi2.Init.DataSize          = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity       = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase          = SPI_PHASE_1EDGE;
  hspi2.Init.NSS               = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode            = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation    = SPI_CRCCALCULATION_ENABLE;
  hspi2.Init.CRCPolynomial     = 7;
  hspi2.Init.CRCLength         = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
  HAL_SPI_Init(&hspi2);
  
  // ������������ ������ DMA ��� �������� ������ � �������
  hdma_spi2_tx.Instance                 = DMA1_Channel7;
  hdma_spi2_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_spi2_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_spi2_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_spi2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_spi2_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_spi2_tx.Init.Mode                = DMA_NORMAL;
  hdma_spi2_tx.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
  HAL_DMA_Init(&hdma_spi2_tx);

  // ������ DMA �� SPI_TX
  __HAL_DMA_REMAP_CHANNEL_ENABLE(DMA_REMAP_SPI2_DMA_CH67);
  __HAL_LINKDMA(&hspi2,hdmatx,hdma_spi2_tx);
  
  // ���������� ���������� DMA 
  HAL_NVIC_SetPriority(DMA1_Channel4_5_6_7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);
}

/*******************************************************************************
  *
  * ������������� DMA ���������� ������ �����
  *
  * ����� Memory to Memory
  *
  * DMA1_Channel1
  * �����.....................������-������
  * ��������� ���������.......�������
  * ��������� ������..........��������
  * ������ ������ ���������...1 ����
  * ������ ������ ������......1 ����
  * ����� ������..............����������
  *
*******************************************************************************/

DMA_HandleTypeDef hdma_memtomem_fill;

void TFT_DMA_FILL_Init(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();
  
  // ������������ ������ DMA ��� �������� ����� � �����
  hdma_memtomem_fill.Instance                 = DMA1_Channel1;
  hdma_memtomem_fill.Init.Direction           = DMA_MEMORY_TO_MEMORY;
  hdma_memtomem_fill.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_memtomem_fill.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_memtomem_fill.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_memtomem_fill.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_memtomem_fill.Init.Mode                = DMA_NORMAL;
  hdma_memtomem_fill.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
  HAL_DMA_Init(&hdma_memtomem_fill);
  
  // ���������� ���������� DMA 
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

/*******************************************************************************
  *
  * ������������� ������� ��� ��������� ������� (TIM14)
  *
  * TIM14_CH1 - PB1
  *
  * ������� - 10 ���
  *
*******************************************************************************/

TIM_HandleTypeDef htim14;

void TFT_LED_TIM_Init(void)
{
  GPIO_InitTypeDef          GPIO_InitStruct = {0};
  TIM_OC_InitTypeDef        sConfigOC = {0};

  __HAL_RCC_TIM14_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // PB1     ------> TIM14_CH1
  
  GPIO_InitStruct.Pin       = TFT_LED_TIM_Pin;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_TIM14;
  HAL_GPIO_Init(TFT_LED_TIM_Port, &GPIO_InitStruct);  
  
  // ����� ���������
  htim14.Instance               = TIM14;
  htim14.Init.Prescaler         = 10;
  htim14.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htim14.Init.Period            = 36000; // 1 ���
  htim14.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.RepetitionCounter = 0;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  HAL_TIM_Base_Init(&htim14);
  
  // ������������ ���
  HAL_TIM_PWM_Init(&htim14);
  
  // ������������ ������
  sConfigOC.OCMode       = TIM_OCMODE_PWM1;
  sConfigOC.Pulse        = 0;
  sConfigOC.OCPolarity   = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode   = TIM_OCFAST_DISABLE;
  HAL_TIM_PWM_ConfigChannel(&htim14, &sConfigOC, TIM_CHANNEL_1);
  
  // ��������� ������ ������ 1 ������� TIM14
  TIM_CCxChannelCmd(htim14.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);

  // ��������� ����� ������� � ��� ������
  __HAL_TIM_MOE_ENABLE(&htim14);
  __HAL_TIM_ENABLE(&htim14);
}

/*******************************************************************************
  *
  * ������������� ������� ��� (ADC)
  *
  * ADC_IN1 - PA1 - ������ ����������� ��������������
  * ADC_IN2 - PA2 - ������ ����������� ������
  * ADC_IN3 - PA3 - ������ ����������� ���
  * ADC_IN4 - PA4 - ������ ����������� �����
  * ADC_CHANNEL_VBAT - �������
  *
  * ����������� ��� - 12 ���
  *
*******************************************************************************/

ADC_HandleTypeDef hadc;

void ADC_Init(void)
{
  GPIO_InitTypeDef       GPIO_InitStruct = {0};

  __HAL_RCC_ADC1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  
  // PA1     ------> ADC_IN1 - �� ������������
  // PA2     ------> ADC_IN2 - ����������� �����
  // PA3     ------> ADC_IN3 - ����������� ���
  // PA4     ------> ADC_IN4 - ����������� �����

  GPIO_InitStruct.Pin = T_add_Pin|
                        T_in_Pin|
                        T_out_Pin|
                        T_amb_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  // ����� ���������
  hadc.Instance                   = ADC1;
  hadc.Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution            = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode          = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait      = DISABLE;
  hadc.Init.LowPowerAutoPowerOff  = DISABLE;
  hadc.Init.ContinuousConvMode    = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun               = ADC_OVR_DATA_PRESERVED;
  HAL_ADC_Init(&hadc);
}

/*******************************************************************************
  *
  * ������������� ������ �����/������
  *
  * ������:
  * ALARM      - PC13
  * LED_Fan    - PA11
  * LED_Pump   - PA7
  * KV_CirPump - PA15
  *
  * �����:
  * But_Start  - PB0
  * But_Stop   - PB2
  * But_More   - PA8
  * But_Less   - PA9
  * Stop_EXT   - PF0
  * SW1        - PF1
  * SW2        - PA0
  *
*******************************************************************************/

void GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // ��������� ��������� ��������� 
  HAL_GPIO_WritePin(GPIOC, ALARM_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, LED_Pump_Pin|LED_Fan_Pin|KV_CirPump_Pin, GPIO_PIN_RESET);

  // ������������� ������
  GPIO_InitStruct.Pin   = ALARM_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ALARM_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin   = LED_Pump_Pin|
                          LED_Fan_Pin|
                          KV_CirPump_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin  = But_Start_Pin|
                         But_Stop_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin  = But_More_Pin|
                         But_Less_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin  = Stop_EXT_Pin|
                         SW1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    
  GPIO_InitStruct.Pin  = SW2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin  = Detect_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Detect_GPIO_Port, &GPIO_InitStruct);

  // ��������� ������� ����������
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}

/*******************************************************************************
  *
  * ������������� ���� I2C ������� EEPROM
  *
*******************************************************************************/

I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_tx;
DMA_HandleTypeDef hdma_i2c1_rx;

void EEPROM_I2C1_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  __HAL_RCC_I2C1_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  // PB6     ------> I2C1_SCL
  // PB7     ------> I2C1_SDA
  
  GPIO_InitStruct.Pin       = GPIO_PIN_6|
                              GPIO_PIN_7;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF1_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  // ����� ���������
  hi2c1.Instance              = I2C1;
  hi2c1.Init.Timing           = 0x20303E5D;//0x00808CD2;
  hi2c1.Init.OwnAddress1      = 256;
  hi2c1.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2      = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
  HAL_I2C_Init(&hi2c1);

  // ������������ ����������� �������
  HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE);
  
  // ������������ ��������� �������
  HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0);
  
  // �������� ���������� I2C
  HAL_NVIC_SetPriority(I2C1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(I2C1_IRQn);
  
  // ������������� DMA ��������
  hdma_i2c1_tx.Instance                 = DMA1_Channel2;
  hdma_i2c1_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_i2c1_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_i2c1_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_i2c1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_i2c1_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_i2c1_tx.Init.Mode                = DMA_NORMAL;
  hdma_i2c1_tx.Init.Priority            = DMA_PRIORITY_LOW;
  HAL_DMA_Init(&hdma_i2c1_tx);

  // ������ DMA �� I2C1_TX
  __HAL_LINKDMA(&hi2c1,hdmatx,hdma_i2c1_tx);

  // ������������� DMA ��������
  hdma_i2c1_rx.Instance                 = DMA1_Channel3;
  hdma_i2c1_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdma_i2c1_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_i2c1_rx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_i2c1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_i2c1_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_i2c1_rx.Init.Mode                = DMA_NORMAL;
  hdma_i2c1_rx.Init.Priority            = DMA_PRIORITY_LOW;
  HAL_DMA_Init(&hdma_i2c1_rx);

  // ������ DMA �� I2C1_RX
  __HAL_LINKDMA(&hi2c1,hdmarx,hdma_i2c1_rx);
}

/*******************************************************************************
  *
  * ������������� ���� I2C �������� ������
  *
*******************************************************************************/

I2C_HandleTypeDef hi2c2;

void I2C2_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_I2C2_CLK_ENABLE();
  
  // PB10     ------> I2C2_SCL
  // PB11     ------> I2C2_SDA
  
  GPIO_InitStruct.Pin       = GPIO_PIN_10|
                              GPIO_PIN_11;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF1_I2C2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
  __HAL_RCC_I2C2_CLK_ENABLE();
  
  // ����� ���������
  hi2c2.Instance               = I2C2;
  hi2c2.Init.Timing            = 0x20303E5D;
  hi2c2.Init.OwnAddress1       = 256;
  hi2c2.Init.AddressingMode    = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode   = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2       = 0;
  hi2c2.Init.OwnAddress2Masks  = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode   = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode     = I2C_NOSTRETCH_DISABLE;
  HAL_I2C_Init(&hi2c2);
  
  // ������������ ����������� �������
  HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE);
  
  // ������������ ��������� �������
  HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0);
 
  // �������� ���������� I2C
  HAL_NVIC_SetPriority(I2C2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(I2C2_IRQn);
}

/*******************************************************************************
  *
  * ������������� ������� ������ ��� (����������� ����� TIM2_CH2) - PB3
  *
  * ����� Triac 1 - X4
  *
  * ������� 50 ��
  *
*******************************************************************************/

TIM_HandleTypeDef htim2;

void TIM2_HWSPump_Init(void)
{
  TIM_ClockConfigTypeDef  sClockSourceConfig = {0};
  TIM_OC_InitTypeDef      sConfigOC = {0};
  GPIO_InitTypeDef        GPIO_InitStruct = {0};

  __HAL_RCC_TIM2_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // PB3     ------> TIM2_CH2
  
  GPIO_InitStruct.Pin       = TIM_Triac1_Pin;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM2;
  HAL_GPIO_Init(TIM_Triac1_GPIO_Port, &GPIO_InitStruct);
    
  // ����� ���������
  htim2.Instance               = TIM2;
  htim2.Init.Prescaler         = 29; //32
  htim2.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htim2.Init.Period            = 10000;
  htim2.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_Base_Init(&htim2);
      
  // ������������ ������������
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);
      
  // ������������� ���
  HAL_TIM_PWM_Init(&htim2);
  
  // ������������� ������ OnePulseMode
  HAL_TIM_OnePulse_Init(&htim2, TIM_OPMODE_SINGLE);
      
  // ������������ ������ �������
  sConfigOC.OCMode     = TIM_OCMODE_PWM2;
  sConfigOC.Pulse      = 10002;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2);
  
  TIM_CCxChannelCmd(htim2.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE);
  
  __HAL_TIM_MOE_ENABLE(&htim2);
}

/*******************************************************************************
  *
  * ������������� ������� ����������� (����������� ����� TIM3_CH2) - PB5
  * � ����� (����������� ����� TIM3_CH1) - PB4
  *
  * ����� �2 - Fan
  * ����� X3 - Supply
  *
  * ������� 50 ��
  *
*******************************************************************************/

TIM_HandleTypeDef htim3;

void TIM3_Fan_Supply_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_OC_InitTypeDef     sConfigOC = {0};
  GPIO_InitTypeDef       GPIO_InitStruct = {0};

  __HAL_RCC_TIM3_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  // PB4     ------> TIM3_CH1
  // PB5     ------> TIM3_CH2
  
  GPIO_InitStruct.Pin       = TIM_Supply_Pin|
                              TIM_Fan_Pin;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  // ����� ���������
  htim3.Instance               = TIM3;
  htim3.Init.Prescaler         = 29; // 32
  htim3.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htim3.Init.Period            = 10000;
  htim3.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_Base_Init(&htim3);
      
  // ������������ ������������
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig);
      
  // ������������� ���
  HAL_TIM_PWM_Init(&htim3);
  
  // ������������� ������ OnePulseMode
  HAL_TIM_OnePulse_Init(&htim3, TIM_OPMODE_SINGLE);
      
  // ������������ ������� �������
  sConfigOC.OCMode     = TIM_OCMODE_PWM2;
  sConfigOC.Pulse      = 10002;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);
  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2);
  
  TIM_CCxChannelCmd(htim3.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);
  TIM_CCxChannelCmd(htim3.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE);
  
  __HAL_TIM_MOE_ENABLE(&htim3);
}

/*******************************************************************************
  *
  * ������������� ������� ������� (TIM16_CH1) - PA6
  *
*******************************************************************************/

TIM_HandleTypeDef htim16;

void TIM16_Buzzer_Init(void)
{
  TIM_OC_InitTypeDef sConfigOC = {0};
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  __HAL_RCC_TIM16_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  // PA6     ------> TIM16_CH1

  GPIO_InitStruct.Pin       = TIM_Buzzer_Pin;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_TIM16;
  HAL_GPIO_Init(TIM_Buzzer_GPIO_Port, &GPIO_InitStruct);
    
  // ����� ���������
  htim16.Instance               = TIM16;
  htim16.Init.Prescaler         = 3;
  htim16.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htim16.Init.Period            = 2300;
  htim16.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  HAL_TIM_Base_Init(&htim16);
      
  // ������������� ���
  HAL_TIM_PWM_Init(&htim16);
      
  // ������������ ������ �������
  sConfigOC.OCMode     = TIM_OCMODE_PWM1;
  sConfigOC.Pulse      = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_PWM_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1);
  
  // ���������� ����������
  Buzzer_TIM = (uint32_t) (Buzzer_TIM_Period*0);
  
  HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
}

/*******************************************************************************
  *
  * ������������� ���������� CAN
  *
*******************************************************************************/

CAN_HandleTypeDef hcan;

void CAN_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  __HAL_RCC_CAN1_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  // PB8     ------> CAN_RX
  // PB9     ------> CAN_TX
    
  GPIO_InitStruct.Pin       = GPIO_PIN_8|
                              GPIO_PIN_9;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_CAN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  // ����� ���������
  hcan.Instance                  = CAN;
  hcan.Init.Prescaler            = 16;
  hcan.Init.Mode                 = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth        = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1             = CAN_BS1_1TQ;
  hcan.Init.TimeSeg2             = CAN_BS2_1TQ;
  hcan.Init.TimeTriggeredMode    = DISABLE;
  hcan.Init.AutoBusOff           = DISABLE;
  hcan.Init.AutoWakeUp           = DISABLE;
  hcan.Init.AutoRetransmission   = DISABLE;
  hcan.Init.ReceiveFifoLocked    = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  HAL_CAN_Init(&hcan);
  
  // �������� ���������� CAN
  HAL_NVIC_SetPriority(CEC_CAN_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(CEC_CAN_IRQn);
}