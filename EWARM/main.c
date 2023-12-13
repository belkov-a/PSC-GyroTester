
#include "main.h"
#include "Init.h"
#include "ILI9341.h"
#include "Menu.h"
#include "24C_EEPROM.h"
#include "adc.h"

// Наименования блока управления
// PSC_PELLETOR
// PSC_START
const char SoftVersion[4]  = "3.5";
const char SoftDate[9]     = "24.06.23";
char ManufactDate[9];
char SerialNum[4];

// Глобальные флаги
uint32_t   GLOBAL_FLAGS = 0x00000000;

// Флаги кнопок
uint32_t   BUTTONS = 0x00000000;

// Флаги предупреждений 
uint32_t   WARNING_FLAGS = 0x00000000;

// Графические флаги - мигание
uint32_t   GRAPHIC_FLAGS = 0x00000000;

// Язык интерфейса
uint8_t    Language = RUS;

uint8_t rx_byte[4] = {0,0,0,0};
uint8_t tx_byte[5] = {0,0,0,0,0};
uint8_t reg_bytes[32];
uint8_t xyz[6];
  
int main(void)
{ 
  // Рестарт периферии, инициализация флеш-интерфейса и системного таймера
  HAL_Init();

  // Конфигурация системного таймера
  SystemClock_Config();
  
  HAL_Delay(500);

  // Инициализация аналоговой части
  //RTC_Init();
  
  // Инициализация системного таймера 
  //SYS_TIM_Init();
  
  // Инициализация SPI дисплея
  TFT_SPI_Init();
  // Инициализация дисплея
  TFT_ILI9341_Init();
  // Инициализация DMA заполнения буфера дисплея фоном
  TFT_DMA_FILL_Init();
  // Иницилизация таймера подсветки дисплея
  TFT_LED_TIM_Init();
  // Заливка дисплея фоном
  TFT_FillScreen(BACK_COLOR);
  
  // Инициализация каналов АЦП
  //ADC_Init();
  
  // Инициализация I2C внешней EEPROM
  //EEPROM_I2C1_Init();

  // Инициализация I2C внешнего устройства
  I2C2_Init();

  // Инициализация CAN
  //CAN_Init();
  
  // Инициализация таймеров симисторов 
  // - Насоса ГВС (TIM2_CH2)  - PB3
  // - Шнек       (TIM3_CH1)  - PB4
  // - Вентилятор (TIM3_CH2)  - PB5
  //TIM2_HWSPump_Init();
  //TIM3_Fan_Supply_Init();

  // Инициализация таймера пищалки (TIM16_CH1) - PA6
  //TIM16_Buzzer_Init();
  
  // Инициализация портов ввода/вывода
  GPIO_Init();

  // Инициализация сторожевого таймера
  //IWDG_Init();
  
  FLASH->KEYR = 0x45670123;
  FLASH->KEYR = 0xCDEF89AB;
  
  //FLASH_BLOCK();
 
  //EEPROM_CheckLicense();
  
  NEED_UPDATE_SET;
  NEED_UPDATE_HEAD_SET;
  NEED_UPDATE_SCROLL_SET;
  ADC_READ_FLAG_SET;
  
  //EEPROM_ReadSettings();
  reg_bytes[0] = ReadReg(WHO_AM_I);
  
  WriteReg(CTRL_REG4,0x00);
  WriteReg(CTRL_REG1,0x0F);
  
  //reg_bytes[1] = ReadReg(CTRL_REG1);
  
  /*
  
  tx_byte[0] = FIFO_CTRL_REG;
  HAL_I2C_Master_Transmit(&hi2c2,L3G4200D_ADDR,tx_byte,1,1000);
  
  HAL_I2C_Master_Receive(&hi2c2,L3G4200D_ADDR,rx_byte,1,1000);
  reg_bytes[6] = rx_byte[0];
  
  tx_byte[0] = INT1_CFG;
  HAL_I2C_Master_Transmit(&hi2c2,L3G4200D_ADDR,tx_byte,1,1000);
  
  HAL_I2C_Master_Receive(&hi2c2,L3G4200D_ADDR,rx_byte,1,1000);
  reg_bytes[7] = rx_byte[0];
  
  tx_byte[0] = INT1_TSH_XH;
  HAL_I2C_Master_Transmit(&hi2c2,L3G4200D_ADDR,tx_byte,1,1000);
  
  HAL_I2C_Master_Receive(&hi2c2,L3G4200D_ADDR,rx_byte,1,1000);
  reg_bytes[8] = rx_byte[0];
  
  tx_byte[0] = INT1_TSH_XL;
  HAL_I2C_Master_Transmit(&hi2c2,L3G4200D_ADDR,tx_byte,1,1000);
  
  HAL_I2C_Master_Receive(&hi2c2,L3G4200D_ADDR,rx_byte,1,1000);
  reg_bytes[9] = rx_byte[0];
  
  tx_byte[0] = INT1_TSH_YH;
  HAL_I2C_Master_Transmit(&hi2c2,L3G4200D_ADDR,tx_byte,1,1000);
  
  HAL_I2C_Master_Receive(&hi2c2,L3G4200D_ADDR,rx_byte,1,1000);
  reg_bytes[10] = rx_byte[0];
  
  tx_byte[0] = INT1_TSH_YL;
  HAL_I2C_Master_Transmit(&hi2c2,L3G4200D_ADDR,tx_byte,1,1000);
  
  HAL_I2C_Master_Receive(&hi2c2,L3G4200D_ADDR,rx_byte,1,1000);
  reg_bytes[11] = rx_byte[0];
  
  tx_byte[0] = INT1_TSH_ZH;
  HAL_I2C_Master_Transmit(&hi2c2,L3G4200D_ADDR,tx_byte,1,1000);
  
  HAL_I2C_Master_Receive(&hi2c2,L3G4200D_ADDR,rx_byte,1,1000);
  reg_bytes[12] = rx_byte[0];
  
  tx_byte[0] = INT1_TSH_ZL;
  HAL_I2C_Master_Transmit(&hi2c2,L3G4200D_ADDR,tx_byte,1,1000);
  
  HAL_I2C_Master_Receive(&hi2c2,L3G4200D_ADDR,rx_byte,1,1000);
  reg_bytes[13] = rx_byte[0];
  
  tx_byte[0] = INT1_DURATION;
  HAL_I2C_Master_Transmit(&hi2c2,L3G4200D_ADDR,tx_byte,1,1000);
  
  HAL_I2C_Master_Receive(&hi2c2,L3G4200D_ADDR,rx_byte,1,1000);
  reg_bytes[14] = rx_byte[0];
  
    tx_byte[0] = CTRL_REG1;
  tx_byte[1] = 0x0F;
  HAL_I2C_Master_Transmit(&hi2c2,L3G4200D_ADDR,tx_byte,2,1000);
  HAL_Delay(5);
*/
  while (1)
  {
  //reg_bytes[1] = ReadReg(CTRL_REG1);
  
  HAL_Delay(10);
  
  xyz[0] = ReadReg(OUT_X_L);
  xyz[1] = ReadReg(OUT_X_H);
  xyz[2] = ReadReg(OUT_Y_L);
  xyz[3] = ReadReg(OUT_Y_H);
  xyz[4] = ReadReg(OUT_Z_L);
  xyz[5] = ReadReg(OUT_Z_H);
  
  reg_bytes[1] = ReadReg(CTRL_REG1);
  reg_bytes[2] = ReadReg(CTRL_REG2);
  reg_bytes[3] = ReadReg(CTRL_REG3);
  reg_bytes[4] = ReadReg(CTRL_REG4);
  reg_bytes[5] = ReadReg(CTRL_REG5);
  reg_bytes[6] = ReadReg(FIFO_CTRL_REG);
  reg_bytes[7] = ReadReg(INT1_CFG);
  reg_bytes[8] = ReadReg(INT1_TSH_XH);
  reg_bytes[9] = ReadReg(INT1_TSH_XL);
  reg_bytes[10] = ReadReg(INT1_TSH_YH);
  reg_bytes[11] = ReadReg(INT1_TSH_YL);
  reg_bytes[12] = ReadReg(INT1_TSH_ZH);
  reg_bytes[13] = ReadReg(INT1_TSH_ZL);
  reg_bytes[14] = ReadReg(INT1_DURATION);
  
  
  
  
    
    // Чтение данных АЦП
    //ADC_ReadValues();

    // Отрисовка верхней панели
    //Menu_DrawHeaderPanel();
    
    // Обработчик режимов работы
    //Work_ModeHandler();
    
    // Отрисовка окон меню
    //Menu_DrawWindows(Menu_SelectedItem);
    
    // Отрисовка хранителя экрана
    //Menu_ScreenSaver();
  }
}

void WriteReg (uint8_t reg, uint8_t value)
{
  tx_byte[0] = reg;
  tx_byte[1] = value;
  HAL_I2C_Master_Transmit(&hi2c2,L3G4200D_ADDR,tx_byte,2,1000);
  HAL_Delay(1);
}

uint8_t ReadReg (uint8_t reg)
{
  tx_byte[0] = reg;
  HAL_I2C_Master_Transmit(&hi2c2,L3G4200D_ADDR,tx_byte,1,1000);
  HAL_Delay(1);
  HAL_I2C_Master_Receive(&hi2c2,L3G4200D_ADDR,rx_byte,1,1000); 
  HAL_Delay(1);
  
  return rx_byte[0];
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  for(int i = 0; i < 1; i++) {}
}

void Error_Handler(void) { __disable_irq(); while (1) {} }

//////////////////////////// Защита памяти от чтения ///////////////////////////     

void FLASH_BLOCK (void)
{
  if (FLASH_OB_GetRDP() == OB_RDP_LEVEL_0)        //checking protection status
  {
      HAL_FLASH_Unlock();       //unblock the FLASH (!!)
      HAL_FLASH_OB_Unlock();    //unblock the Option Byte
      if (FLASH_OB_RDP_LevelConfig(OB_RDP_LEVEL_1) == HAL_OK) HAL_FLASH_OB_Launch();
      HAL_FLASH_OB_Lock();
      HAL_FLASH_Lock();
  }
}
