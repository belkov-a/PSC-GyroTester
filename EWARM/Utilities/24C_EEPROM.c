
#include "main.h"
#include "Init.h"
#include "24C_EEPROM.h"
#include "Menu.h"
#include "adc.h"

s24C32xx sEEPROM = {32, 128, 4, 0};

  uint8_t byte = 0xA;
  uint8_t byte_rx;
  uint8_t *byte_tx1 = &byte;
  uint8_t *byte_rx1 = &byte_rx;
  uint8_t buf[32];
  uint8_t buf_rx[32];

uint8_t EEPROM_BUF[32];
uint8_t EEPROM_BUF_RX[32];
uint8_t  MODE_Buf[2] = {0};


/*******************************************************************************
  *
  * Запись байта в память
  *
*******************************************************************************/

void EEPROM_WriteByte (uint8_t *pByte,      // байт для записи
                       uint16_t ByteAddr)  // адрес байта
{
  // Если память занята - ждём
  while (sEEPROM.Lock == 1) {}
  while (HAL_I2C_IsDeviceReady(&hi2c1, EEPROM_ADDRESS, 1, HAL_MAX_DELAY) != HAL_OK) {}
  
  sEEPROM.Lock = 1;
  
  // Передаем байт  
  HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, ByteAddr, I2C_MEMADD_SIZE_16BIT, pByte, 1);
}

/*******************************************************************************
  *
  * Запись страницы в память
  *
*******************************************************************************/

void EEPROM_WritePage (uint8_t *pBuffer,       // указатель на буфер
                       uint8_t PageNumber,     // номер страницы
                       uint8_t NumByteToWrite) // количество байтов для записи (размер вплоть до страницы)
{
  // Если память занята - ждём
  while (sEEPROM.Lock == 1) {}
  while (HAL_I2C_IsDeviceReady(&hi2c1, EEPROM_ADDRESS, 1, HAL_MAX_DELAY) != HAL_OK) {}
  
  sEEPROM.Lock = 1;
  
  // Определяем адрес страницы
  uint16_t PageAddr = PageNumber*sEEPROM.PageSize;
  
  // Передаем страницу  
  HAL_I2C_Mem_Write_IT(&hi2c1, EEPROM_ADDRESS, PageAddr, I2C_MEMADD_SIZE_16BIT, (uint8_t*) pBuffer, NumByteToWrite);
}                   

/*******************************************************************************
  *
  * Чтение байта из памяти
  *
*******************************************************************************/

void EEPROM_ReadByte (uint8_t *pByte,     // байт для записи
                      uint16_t ByteAddr) // адрес байта
{
  // Если память занята - ждём
  while (sEEPROM.Lock == 1) {}
  while (HAL_I2C_IsDeviceReady(&hi2c1, EEPROM_ADDRESS, 1, HAL_MAX_DELAY) != HAL_OK) {}
  
  sEEPROM.Lock = 1;
  
  // Принимаем байт  
  HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS, ByteAddr, I2C_MEMADD_SIZE_16BIT, pByte, 1);
}

/*******************************************************************************
  *
  * Чтение страницы из памяти
  *
*******************************************************************************/

void EEPROM_ReadPage (uint8_t *pBuffer,       // указатель на буфер
                      uint8_t PageNumber,     // номер страницы
                      uint8_t NumByteToWrite) // количество байтов для записи (размер вплоть до страницы)
{                      
  // Если память занята - ждём
  while (sEEPROM.Lock == 1) {}
  while (HAL_I2C_IsDeviceReady(&hi2c1, EEPROM_ADDRESS, 1, HAL_MAX_DELAY) != HAL_OK) {}
  
  sEEPROM.Lock = 1;
  
  // Определяем адрес страницы
  uint16_t PageAddr = PageNumber*sEEPROM.PageSize;
  
  // Принимаем страницу
  HAL_I2C_Mem_Read_IT(&hi2c1, EEPROM_ADDRESS, PageAddr, I2C_MEMADD_SIZE_16BIT, (uint8_t*) pBuffer, NumByteToWrite);
}

/*******************************************************************************
  *
  * Очистка страницы
  *
*******************************************************************************/

const uint8_t EEPROM_EraseBuf[32] = 
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void EEPROM_ErasePage (uint8_t PageNumber) // Номер страницы
{
  EEPROM_WritePage ((uint8_t*) EEPROM_EraseBuf,PageNumber,sEEPROM.PageSize);
}

/*******************************************************************************
  *
  * Очистить память
  *
  * Не чистим первую страницу. В ней хранится шифр устройства!
  *
*******************************************************************************/

void EEPROM_EraseFull (void)
{
  for (uint8_t i = 1; i < sEEPROM.PageCount; i++)
    EEPROM_WritePage ((uint8_t*) EEPROM_EraseBuf,i,sEEPROM.PageSize);
}

/*******************************************************************************
  *
  * Очистка буфера памяти
  *
*******************************************************************************/

void EEPROM_EraseBuffer (void)
{
    // Если память занята - ждём
  while (sEEPROM.Lock == 1) {}
  while (HAL_I2C_IsDeviceReady(&hi2c1, EEPROM_ADDRESS, 1, HAL_MAX_DELAY) != HAL_OK) {}
  
  for (uint8_t i = 0; i < 32; i++) EEPROM_BUF[i] = 0;
}

/*******************************************************************************
  *
  * Проверка лицензии 
  *
*******************************************************************************/

char* DeviceNamePoint;
const char DeviceNameSTART[2][13] = {"ТЕРМОКУБ С10","TERMOKUB C10"};

void EEPROM_CheckLicense (void)
{
  // Считываем страницу лицензии
  EEPROM_ReadPage (EEPROM_BUF, LICENSE_PAGE, sEEPROM.PageSize);
  
  // Если память занята - ждём
  while (sEEPROM.Lock == 1) {}
  while (HAL_I2C_IsDeviceReady(&hi2c1, EEPROM_ADDRESS, 1, HAL_MAX_DELAY) != HAL_OK) {}
  
  // Читаем дату производства
  for (int i = 0; i < 9; i++) 
  {
    // Если дата производства не записана, то пишем "?"
    if ((EEPROM_BUF[11] == 0x00) || (EEPROM_BUF[11] == 0xFF)) 
    {
      ManufactDate[0] = '?';
      break;
    }
    ManufactDate[i] = EEPROM_BUF[11+i];
  }
  
  // Читаем серийный номер
  for (int i = 0; i < 10; i++) 
  {
    // Если серийный номер не записан, то пишем "?"
    if ((EEPROM_BUF[25] == 0x00) || (EEPROM_BUF[25] == 0xFF))
    {
      SerialNum[0] = '?';
      break;
    }
    SerialNum[i] = EEPROM_BUF[25+i];
  }
  
  // Если код лицензии совпадает, то переходим на стартовую страницуы
  // иначе на страницу "Не лицензионное ПО"
  if ((EEPROM_BUF[5] == 'S') && (EEPROM_BUF[6] == 'T') && (EEPROM_BUF[7] == 'A') &&
      (EEPROM_BUF[8] == 'R') && (EEPROM_BUF[9] == 'T'))
  {
    if (But_STOP_State == GPIO_PIN_SET)
    {
      Menu_SelectedItem = (sMenu*)&CheckWindow;
    }
    else
      Menu_SelectedItem = (sMenu*)&StartWindow;  
  }
  else
  {
    Menu_SelectedItem = (sMenu*)&LicenseWindow; 
  }
}

/*******************************************************************************
  *
  * Запись кодового слова в память
  * 
*******************************************************************************/

void EEPROM_WriteLicence (void)
{
  EEPROM_BUF[5] = 'S';
  EEPROM_BUF[6] = 'T';
  EEPROM_BUF[7] = 'A';
  EEPROM_BUF[8] = 'R';
  EEPROM_BUF[9] = 'T';
  
  EEPROM_PageFormation(LICENSE_PAGE);
  EEPROM_WritePage (EEPROM_BUF,LICENSE_PAGE,sEEPROM.PageSize);
}

/*******************************************************************************
  *
  * Запись настроек в память
  *
  * sFloat - 0xPVVV VVVV, P - бит OffValue, V - значение Value
  * sEnum  - 0xVVVV, V - значение Point
  *
*******************************************************************************/

void EEPROM_WriteSettings(void)
{
  EEPROM_PageFormation(FLOAT_PAGE_1);
  EEPROM_WritePage ((uint8_t*) EEPROM_BUF,FLOAT_PAGE_1,sEEPROM.PageSize);
  EEPROM_EraseBuffer();
  EEPROM_PageFormation(FLOAT_PAGE_2);
  EEPROM_WritePage ((uint8_t*) EEPROM_BUF,FLOAT_PAGE_2,sEEPROM.PageSize);
  EEPROM_EraseBuffer();
  EEPROM_PageFormation(ENUM_PAGE_1);
  EEPROM_WritePage ((uint8_t*) EEPROM_BUF,ENUM_PAGE_1,sEEPROM.PageSize);
  EEPROM_EraseBuffer();
}

/*******************************************************************************
  *
  * Запись параметра в буфер памяти
  *
*******************************************************************************/

void EEPROM_ParamToBuf (sFloat *Dig,       // указатель на числовой параметр
                        sEnum *Enum,       // указатель на параметр перечисления
                        uint8_t NumOnPage) // номер параметра на странице 
                                           // от 0 до 15 - для sFloat
                                           // от 0 до 31 - для Enum
{
  if (Enum != 0)
  {
    EEPROM_BUF[NumOnPage] = (uint8_t) ((*Enum).Point);
  }
  else
  if (Dig != 0)
  {
    if ((*Dig).Min < 0)
    {
      EEPROM_BUF[2*NumOnPage] = (uint8_t) (((*Dig).Value >> 8) & 0xFF);
      EEPROM_BUF[2*NumOnPage + 1] =(uint8_t) ((*Dig).Value & 0xFF);
    }
    else
    {
      EEPROM_BUF[2*NumOnPage] = (uint8_t) ((((*Dig).Value >> 8) & 0x7F) +
                                           (((*Dig).Off & OFF_STATE) << 7));
      EEPROM_BUF[2*NumOnPage + 1] =(uint8_t) ((*Dig).Value & 0xFF);
    }
  }
}

/*******************************************************************************
  *
  * Формирование буфера для записи в страницу
  *
*******************************************************************************/

void EEPROM_PageFormation(uint8_t PageNum)
{
  // Если память занята - ждём
  while (sEEPROM.Lock == 1) {}
  while (HAL_I2C_IsDeviceReady(&hi2c1, EEPROM_ADDRESS, 1, HAL_MAX_DELAY) != HAL_OK) {}
  
  switch (PageNum)
  {
    // 1 страница числовых параметров
    case FLOAT_PAGE_1:
      EEPROM_ParamToBuf(&sTemperatureSetting,0,0);
      EEPROM_ParamToBuf(&sSupplyTime,0,1);
      EEPROM_ParamToBuf(&sSupplyPeriod,0,2);
      EEPROM_ParamToBuf(&sFanPower,0,3);
      EEPROM_ParamToBuf(&sSupplyRepetition,0,4);
      EEPROM_ParamToBuf(&sBlowingTime,0,5);
      EEPROM_ParamToBuf(&sBlowingPeriod,0,6);
      EEPROM_ParamToBuf(&sFanPowerBlowing,0,7);
      EEPROM_ParamToBuf(&sFanPowerFirewood,0,8);
      EEPROM_ParamToBuf(&sTemperatureHWS,0,9);
      EEPROM_ParamToBuf(&sNightCorrection,0,10);
      EEPROM_ParamToBuf(&sTemperatureSettingMin,0,11);
      EEPROM_ParamToBuf(&sTemperatureSettingMax,0,12);
      EEPROM_ParamToBuf(&sHysteresisBoilerSensor,0,13);
      EEPROM_ParamToBuf(&sHysteresisHWSSensor,0,14);
      EEPROM_ParamToBuf(&sFanPowerMin,0,15);
      break;
    // 2 страницы числовых параметров
    case FLOAT_PAGE_2:
      EEPROM_ParamToBuf(&sFanPowerMax,0,0);
      EEPROM_ParamToBuf(&sFuelLackTime,0,1);
      EEPROM_ParamToBuf(&sTemperatureHeatingPumpStart,0,2);
      EEPROM_ParamToBuf(&sTimePumpOff,0,3);
      EEPROM_ParamToBuf(&sTimePumpOn,0,4);
      EEPROM_ParamToBuf(&sTemperatureOff,0,5);
      EEPROM_ParamToBuf(&sTemperatureScrewAlarm,0,6);
      EEPROM_ParamToBuf(&sTimeTransfer,0,7);
      EEPROM_ParamToBuf(&sNightPeriodStartTime,0,8);
      EEPROM_ParamToBuf(&sDayPeriodStartTime,0,9);
      EEPROM_ParamToBuf(&sFanPowerFirewood,0,10);
      EEPROM_ParamToBuf(&sEnergySaving,0,11);
      EEPROM_ParamToBuf(&sScreenSaver,0,12);
      break;
    // 1 страница параметров перечисления
    case ENUM_PAGE_1:
      EEPROM_ParamToBuf(0,&sLanguage,0);
      EEPROM_ParamToBuf(0,&sMode,1);
      EEPROM_ParamToBuf(0,&sDailyMode,2);
      EEPROM_ParamToBuf(0,&sBoilerPriority,3);
      EEPROM_ParamToBuf(0,&sThermostat,4);
      break;
  }
}

/*******************************************************************************
  *
  * Чтение настроек из памяти
  *
  * sFloat - 0xPVVV VVVV, P - бит OffValue, V - значение Value
  * sEnum  - 0xVVVV, V - значение Point
  *
*******************************************************************************/

void EEPROM_ReadSettings(void)
{
  EEPROM_EraseBuffer();
  EEPROM_ReadPage ((uint8_t*) EEPROM_BUF,FLOAT_PAGE_1,sEEPROM.PageSize);
  EEPROM_PageExtraction(FLOAT_PAGE_1);
  EEPROM_EraseBuffer();
  EEPROM_ReadPage ((uint8_t*) EEPROM_BUF,FLOAT_PAGE_2,sEEPROM.PageSize);
  EEPROM_PageExtraction(FLOAT_PAGE_2);
  EEPROM_EraseBuffer();
  EEPROM_ReadPage ((uint8_t*) EEPROM_BUF,ENUM_PAGE_1,sEEPROM.PageSize);
  EEPROM_PageExtraction(ENUM_PAGE_1);
  
  // Если настроек нет (смотрим по языку), то сбрасываем настройки
  if (sLanguage.Point == 0xFF)
  {
    Menu_ResetSettings();
    sLanguage.Point = 0;
  } 
  
  // Обработка изменения параметров
  Menu_ProduceParameters();
  
  if (Language != sLanguage.Point)
    Language = sLanguage.Point;
  
  // Определение наименования устройства
  DeviceNamePoint = (char*) DeviceNameSTART[Language];
  
  if (sTemperatureSetting.Value != sTemperatureSet.Value)
    sTemperatureSet.Value = sTemperatureSetting.Value;
  
  // Считываем счетчики тревоги датчиков шнека и ГВС
  EEPROM_ReadByte ((uint8_t*) Screw_Sensor_Counter,SCREW_SENS_ADDR);
  EEPROM_ReadByte ((uint8_t*) HWS_Sensor_Counter,HWS_SENS_ADDR);
  EEPROM_ReadByte ((uint8_t*) AntiSTOPHour_Counter,ANTI_STOP_ADDR);
  
  HAL_Delay(20);
  
  if (Screw_Sensor_Counter[0] == 0xFF) Screw_Sensor_Counter[0] = 0;
  if (HWS_Sensor_Counter[0] == 0xFF) HWS_Sensor_Counter[0] = 0;
  if (AntiSTOPHour_Counter[0] == 0xFF) AntiSTOPHour_Counter[0] = 0;
  else AntiSTOP_Counter = AntiSTOPHour_Counter[0] * 60;
  
  // Если счетчики равны 7 дням, то взводим флаги
  if ((Screw_Sensor_Counter[0] == 168) && ISNT_OFF(sTemperatureScrewAlarm))
  {
    SCREW_SENSOR_DEAD_SET;
  }
  if ((HWS_Sensor_Counter[0] == 168) && ISNT_OFF(sTemperatureHWS))
  {
    HWS_SENSOR_DEAD_SET;
  }
  
  EEPROM_ReadByte ((uint8_t*) MODE_Buf,MODE_FLAG_ADDR);
  HAL_Delay(20);
  ADC_ReadValues();
  // Если записанный режим не ВЫКЛЮЧЕН
  if ((MODE_Buf[0] != 0xFF) && (MODE_Buf[0] != SWITCHOFF_MODE)) 
  {
    Menu_SelectedItem = (sMenu*) &MainWindow;
    
    // Если температура выше температуры тушения котла, то переходим в режим 
    // в соответствии с текущей температурой, иначе переходим в выключен
    if (sTempIn.Value > sTemperatureOff.Value*10) MODE_FLAG = MODE_Buf[0];
    else 
    {
      MODE_FLAG = SWITCHOFF_MODE;
      MODE_Buf[0] = MODE_FLAG;
      EEPROM_WriteByte ((uint8_t*) MODE_Buf,MODE_FLAG_ADDR);
    }
  }
}

/*******************************************************************************
  *
  * Чтение параметра из буфера
  *
*******************************************************************************/

void EEPROM_ParamFromBuf (sFloat *Dig,       // указатель на числовой параметр
                          sEnum *Enum,       // указатель на параметр перечисления
                          uint8_t NumOnPage) // номер параметра на странице 
                                             // от 0 до 15 - для sFloat
                                             // от 0 до 31 - для Enum
{
  if (Enum != 0)
  {
    (*Enum).Point = (uint8_t) EEPROM_BUF[NumOnPage];
  }
  else
  if (Dig != 0)
  {
    if ((*Dig).Min < 0)
    {
      (*Dig).Value = (int16_t) ((EEPROM_BUF[2*NumOnPage] << 8) +
                                (EEPROM_BUF[2*NumOnPage + 1]));      
    }
    else
    {
      (*Dig).Off = (uint8_t) ((*Dig).Off & OFF_SUPPORT) + (((EEPROM_BUF[2*NumOnPage] & 0x80) >> 7) & OFF_STATE);
      (*Dig).Value = (uint16_t) ((EEPROM_BUF[2*NumOnPage] & 0x7F) +
                                (EEPROM_BUF[2*NumOnPage + 1]));
    }
  }  
}

/*******************************************************************************
  *
  * Извлечение параметров из буфера
  *
*******************************************************************************/

void EEPROM_PageExtraction(uint8_t PageNum)
{
  // Если память занята - ждём
  while (sEEPROM.Lock == 1) {}
  while (HAL_I2C_IsDeviceReady(&hi2c1, EEPROM_ADDRESS, 1, HAL_MAX_DELAY) != HAL_OK) {}
  
  switch (PageNum)
  {
    // 1 страница числовых параметров
    case FLOAT_PAGE_1:
      EEPROM_ParamFromBuf(&sTemperatureSetting,0,0);
      EEPROM_ParamFromBuf(&sSupplyTime,0,1);
      EEPROM_ParamFromBuf(&sSupplyPeriod,0,2);
      EEPROM_ParamFromBuf(&sFanPower,0,3);
      EEPROM_ParamFromBuf(&sSupplyRepetition,0,4);
      EEPROM_ParamFromBuf(&sBlowingTime,0,5);
      EEPROM_ParamFromBuf(&sBlowingPeriod,0,6);
      EEPROM_ParamFromBuf(&sFanPowerBlowing,0,7);
      EEPROM_ParamFromBuf(&sFanPowerFirewood,0,8);
      EEPROM_ParamFromBuf(&sTemperatureHWS,0,9);
      EEPROM_ParamFromBuf(&sNightCorrection,0,10);
      EEPROM_ParamFromBuf(&sTemperatureSettingMin,0,11);
      EEPROM_ParamFromBuf(&sTemperatureSettingMax,0,12);
      EEPROM_ParamFromBuf(&sHysteresisBoilerSensor,0,13);
      EEPROM_ParamFromBuf(&sHysteresisHWSSensor,0,14);
      EEPROM_ParamFromBuf(&sFanPowerMin,0,15);
      break;
    // 2 страницы числовых параметров
    case FLOAT_PAGE_2:
      EEPROM_ParamFromBuf(&sFanPowerMax,0,0);
      EEPROM_ParamFromBuf(&sFuelLackTime,0,1);
      EEPROM_ParamFromBuf(&sTemperatureHeatingPumpStart,0,2);
      EEPROM_ParamFromBuf(&sTimePumpOff,0,3);
      EEPROM_ParamFromBuf(&sTimePumpOn,0,4);
      EEPROM_ParamFromBuf(&sTemperatureOff,0,5);
      EEPROM_ParamFromBuf(&sTemperatureScrewAlarm,0,6);
      EEPROM_ParamFromBuf(&sTimeTransfer,0,7);
      EEPROM_ParamFromBuf(&sNightPeriodStartTime,0,8);
      EEPROM_ParamFromBuf(&sDayPeriodStartTime,0,9);
      EEPROM_ParamFromBuf(&sFanPowerFirewood,0,10);
      EEPROM_ParamFromBuf(&sEnergySaving,0,11);
      EEPROM_ParamFromBuf(&sScreenSaver,0,12);
      break;
    // 1 страница параметров перечисления
    case ENUM_PAGE_1:
      EEPROM_ParamFromBuf(0,&sLanguage,0);
      EEPROM_ParamFromBuf(0,&sMode,1);
      EEPROM_ParamFromBuf(0,&sDailyMode,2);
      EEPROM_ParamFromBuf(0,&sBoilerPriority,3);
      EEPROM_ParamFromBuf(0,&sThermostat,4);
      break;
  }
}

/*******************************************************************************
  *
  * Функция, вызываемая после передачи данных в память
  *
*******************************************************************************/

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  sEEPROM.Lock = 0;
}

/*******************************************************************************
  *
  * Функция, вызываемая после приема данных из памяти
  *
*******************************************************************************/

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  sEEPROM.Lock = 0;
}