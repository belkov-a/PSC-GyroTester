
#include "main.h"
#include "Init.h"
#include "ILI9341.h"
#include "Menu.h"
#include "24C_EEPROM.h"
#include "adc.h"
#include <stdlib.h>

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

int16_t x, y, z;
float x_f,y_f,z_f;
int x_out, y_out, z_out;

float temp = 0;
  
int main(void)
{ 
  // Рестарт периферии, инициализация флеш-интерфейса и системного таймера
  HAL_Init();

  // Конфигурация системного таймера
  SystemClock_Config();
  
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
  
  HAL_Delay(500);
  
  TFT_LED_Set_Brightness(100);

  // Инициализация I2C внешнего устройства
  I2C2_Init();
  
  // Инициализация портов ввода/вывода
  GPIO_Init();
  
  FLASH->KEYR = 0x45670123;
  FLASH->KEYR = 0xCDEF89AB;
  
  NEED_UPDATE_SET;
  NEED_UPDATE_HEAD_SET;
  NEED_UPDATE_SCROLL_SET;
  ADC_READ_FLAG_SET;
  
  WriteReg(CTRL_REG4,0x00);
  WriteReg(CTRL_REG1,0x0F);

  while (1)
  {    
    rx_byte[0] = 0;
    temp = 0;
    if(ReadReg(WHO_AM_I) == 0xD3)
    {
      TFT_WriteString(0, 20,C_ALIGN,GREEN,BACK_COLOR,"Init - OK",Calibri);
      
      WriteReg(CTRL_REG1,0xCF);
      WriteReg(CTRL_REG2,0x01);
      WriteReg(CTRL_REG3,0x00);
      WriteReg(CTRL_REG4,0x00);
      WriteReg(CTRL_REG5,0x02);
      
      temp = (51 - (ReadReg(OUT_TEMP) * 1.33))*10;
      
      TFT_WriteString(100, 60,NO_ALIGN,GREEN,BACK_COLOR,"Temp",Calibri);
      TFT_WriteFloatValue(170, 60,NO_ALIGN,(int) temp,2,1,NON_SIGNED,"C",GREEN,BACK_COLOR,Calibri);
      
      while((ReadReg(STATUS_REG) & 0x08) == 0) {}
    
      if ((ReadReg(STATUS_REG) & 0x80) == 0x80)
      {
        xyz[0] = ReadReg(OUT_X_L);
        xyz[1] = ReadReg(OUT_X_H);
        x = ((xyz[1] << 0x08) + xyz[0]);
        x_f = x * 0.00875 * 100;
        if (abs((int)x_f) > 300) x_out = (int) x_f;
        else x_out = 0;
          
        TFT_WriteString(100, 100,NO_ALIGN,GREEN,BACK_COLOR,"x",Calibri);
        TFT_WriteFloatValue(140, 100,NO_ALIGN,(int) x_out,2,2,SIGNED,"*/с",GREEN,BACK_COLOR,Calibri);
      
        xyz[2] = ReadReg(OUT_Y_L);
        xyz[3] = ReadReg(OUT_Y_H);
        y = ((xyz[3] << 0x08) + xyz[2]);
        y_f = y * 0.00875 * 100;
        if (abs((int)y_f) > 300) y_out = (int) y_f;
        else y_out = 0;
        
        TFT_WriteString(100, 140,NO_ALIGN,GREEN,BACK_COLOR,"y",Calibri);
        TFT_WriteFloatValue(140, 140,NO_ALIGN,(int) y_out,2,2,SIGNED,"*/с",GREEN,BACK_COLOR,Calibri);
          
          
        xyz[4] = ReadReg(OUT_Z_L);
        xyz[5] = ReadReg(OUT_Z_H);
        z = ((xyz[5] << 0x08) + xyz[4]);
        z_f = z * 0.00875 * 100;
        if (abs((int)z_f) > 300) z_out = (int) z_f;
        else z_out = 0;
          
        TFT_WriteString(100, 180,NO_ALIGN,GREEN,BACK_COLOR,"z",Calibri);
        TFT_WriteFloatValue(140, 180,NO_ALIGN,(int) z_out,2,2,SIGNED,"*/с",GREEN,BACK_COLOR,Calibri);
      }
    }
    else
    {
      TFT_WriteString(0, 20,C_ALIGN,ORANGE,BACK_COLOR,"Init - N/A",Calibri);
      
      TFT_WriteString(100, 60,NO_ALIGN,ORANGE,BACK_COLOR,"Temp",Calibri);
      TFT_WriteString(170, 60,NO_ALIGN,ORANGE,BACK_COLOR,"N/A       ",Calibri);
    
      TFT_WriteString(100, 100,NO_ALIGN,ORANGE,BACK_COLOR,"x",Calibri);
      TFT_WriteString(140, 100,NO_ALIGN,ORANGE,BACK_COLOR,"      N/A            ",Calibri);  
    
      TFT_WriteString(100, 140,NO_ALIGN,ORANGE,BACK_COLOR,"y",Calibri);
      TFT_WriteString(140, 140,NO_ALIGN,ORANGE,BACK_COLOR,"      N/A            ",Calibri);  
      
      TFT_WriteString(100, 180,NO_ALIGN,ORANGE,BACK_COLOR,"z",Calibri);
      TFT_WriteString(140, 180,NO_ALIGN,ORANGE,BACK_COLOR,"      N/A            ",Calibri);  
    }
    
    HAL_Delay(300);
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
