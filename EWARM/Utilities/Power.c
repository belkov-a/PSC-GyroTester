
#include "main.h"
#include "Menu.h"
#include "ILI9341.h"
#include "Init.h"
#include "24C_EEPROM.h"

/*******************************************************************************
  *
  * Отрисовка даты и времени
  * 
  * Выбирается тип данных для вывода
  *  - TIME    - текущее время
  *  - DATE    - текущая дата
  *  - WEEKDAY - день недели
  *
*******************************************************************************/

const char WeekDay[2][14] = {"ПнВтСрЧтПтСбВс","MoTuWeThFrSaSu"};
uint8_t Old_Second = 0;
uint8_t DoublePoint_Count = 0;

void RTC_Write(uint8_t DataType,    // тип данных
               uint8_t x,           // координата по горизонтали
               uint8_t y,           // координата по вертикали
               uint8_t align,       // выравнивание по ширине
               uint16_t color,      // цвет шрифта
               uint32_t background, // цвет фона
               sFont font)          // ссылка на шрифт
{ 
  char NumBuffer[9] = {0};
  switch (DataType)
  {
    case TIME: 
       HAL_RTC_GetTime(&hrtc, &sTimeCurrent, RTC_FORMAT_BIN);
       HAL_RTC_GetDate(&hrtc, &sDateCurrent, RTC_FORMAT_BIN);
       NumBuffer[0] = sTimeCurrent.Hours/10 + '0';
       NumBuffer[1] = sTimeCurrent.Hours%10 + '0';
       if (sTimeCurrent.Seconds == Old_Second) NumBuffer[2] = ':';
       else 
       {
         DoublePoint_Count++;
         NumBuffer[2] = '^'; 
         if (DoublePoint_Count == 5) {Old_Second = sTimeCurrent.Seconds; DoublePoint_Count = 0;}
       }
       NumBuffer[3] = sTimeCurrent.Minutes/10 + '0';
       NumBuffer[4] = sTimeCurrent.Minutes%10 + '0';
       break;
    case DATE:
       HAL_RTC_GetDate(&hrtc, &sDateCurrent, RTC_FORMAT_BIN);
       NumBuffer[0] = sDateCurrent.Date/10 + '0';
       NumBuffer[1] = sDateCurrent.Date%10 + '0';
       NumBuffer[2] = '.';
       NumBuffer[3] = sDateCurrent.Month/10 + '0';
       NumBuffer[4] = sDateCurrent.Month%10 + '0';
       NumBuffer[5] = '.';
       NumBuffer[6] = sDateCurrent.Year/10 + '0';
       NumBuffer[7] = sDateCurrent.Year%10 + '0';
       break;
    case WEEKDAY:
       HAL_RTC_GetDate(&hrtc, &sDateCurrent, RTC_FORMAT_BIN);
       NumBuffer[0] = WeekDay[Language][(sDateCurrent.WeekDay-1)*2];
       NumBuffer[1] = WeekDay[Language][(sDateCurrent.WeekDay-1)*2 + 1];
       break;
  }

  TFT_WriteString(x,y,align,color,background,NumBuffer,font);
}

/*******************************************************************************
  *
  * Вычисление системных счетчиков
  *
*******************************************************************************/

uint8_t  SPI_DMA_BUSY_Counter  = 0;  // Счетчик занятости DMA SPI
uint16_t BLINK_Counter         = 0;  // Счетчик мигания параметром
uint16_t BLINK_BATTERY_Counter = 0;  // Счетчик мигания батареей
uint16_t BLINK_DISPLAY_Counter = 0;  // Сетчик мигания дисплеем
uint16_t Event_Counter   = 0;        // Счетчик событий
uint16_t Hour_Counter    = 0;        // Счетчик часа
uint8_t  LED_Counter     = 0;        // Счетчик включения подсветки
uint16_t MinAntiStop_Counter = 0;    // Счетчик минут для режима Анти-СТОП

uint8_t  ModeChange_Counter = 2;     // Счетчик переключения между режимами 2 сек
uint8_t  FeedTime_Counter = 0;       // Счетчик подачи топлива
uint8_t  FeedTime_Num = 0;           // Повторение подачи
uint8_t  BlowingTime_Counter = 0;    // Счетчик продува
uint16_t Buzzer_Counter = 0;         // Счетчик пищалки
uint16_t Buzzer_Period_Counter = 30; // Счетчик периода пищания
uint16_t Extinguishing_Counter = 0;  // Счетчик тушения
uint16_t CirPump_Counter = 0;        // Счетчик насоса отопления
uint16_t CirPumpStart_Counter = 0;   // Счетчик первого включения насоса отопления
uint16_t Overflow_Counter = 0;       // Счетчик пересыпки
uint16_t EnergySaving_Counter = 0;   // Счетчик энергосбережения
uint16_t ScreenSaver_Counter = 0;    // Счетчик хранителя экрана
uint8_t  ScreenSaverChange_Counter = 0; // Счетчик изменения положения температуры в хранителе
uint16_t AntiSTOP_Counter = 0;
uint8_t  Screw_Sensor_Counter[2] = {0};   // Счетчик датчика шнека
uint8_t  HWS_Sensor_Counter[2] = {0};     // Счетчик датчика ГВС
uint8_t  AntiSTOPHour_Counter[2] = {0};   // Счетчик Анти-СТОП
uint8_t  ADC_Read_Counter = 0;       // Счетчик чтения АЦП
uint32_t FreqCount = 0;              // Счетчик частоты
uint16_t  FreqPoint = 0;              // Счетчик усреднений частоты

uint32_t Period = 0;
uint8_t Freq_old = 0;
uint8_t Freq_new = 0;

uint32_t FAN_TIM_CAL = 0;

void PWR_ProduceSysCounters(void)
{
  // Счетчик частоты
  if (FreqCount > 5100) 
    {
      FreqCount = 0; 
      sFrequency.Value = FreqPoint;
      FreqPoint = 0; 
      if ((sFrequency.Value >= 35) || (sFrequency.Value <= 100))
      {
        Period = 500000/sFrequency.Value;
      }
    }
  else
  FreqCount++;
  
  // Мигание параметром
  BLINK_Counter++;
  if (BLINK_Counter == 3000) BLINK_SET;
  else if (BLINK_Counter == 6000) {BLINK_RESET; BLINK_Counter = 0;}
  
  // Мигание батареей
  BLINK_BATTERY_Counter++;
  if (BLINK_BATTERY_Counter == 5000) BLINK_BATTERY_SET;
  else if (BLINK_BATTERY_Counter == 10000) {BLINK_BATTERY_RESET; BLINK_BATTERY_Counter = 0;}
  
  // Событие секунда и 1/4 секунды
  Event_Counter++;
  if ((Event_Counter == 2500) ||
      (Event_Counter == 5000) ||
      (Event_Counter == 7500)) EVENT_4HZ_SET;
  if (Event_Counter == 10000) {EVENT_1HZ_SET; Event_Counter = 0;}
  
  // 4 Гц
  if (IS_EVENT_4HZ)
  {
    // Пищалка
    if (Buzzer_Counter > 0) Buzzer_Counter--;
    
    EVENT_4HZ_RESET;
  }
  
  // 1 Гц
  if (IS_EVENT_1HZ)
  {
    // Считаем 2 секунды перед стартом подсветки
    if (LED_Counter < 3) 
    {
      LED_Counter++;
      // Включение подсветки
      if (LED_Counter == 2) TFT_LED_Set_Brightness(100);
    }
    
    // Раз в час прибавляем счетчик предупреждений датчиков шнека и гвс
    if ((IS_SCREW_SENSOR) || (IS_HWS_SENSOR))
    {
      if (Hour_Counter == 3600)
      {
        Hour_Counter = 0;
        // Если датчик шнека поврежден
        if (IS_SCREW_SENSOR) 
        {
          // Если прошло 7 дней
          if (Screw_Sensor_Counter[0] < 168) Screw_Sensor_Counter[0]++;
          // Пишем в память текущий час
          EEPROM_WriteByte ((uint8_t*) Screw_Sensor_Counter,SCREW_SENS_ADDR);
        }
        // Если датчик ГВС поврежден
        if (IS_HWS_SENSOR)
        {
         // Если не прошло 7 дней
          if (HWS_Sensor_Counter[0] < 168) HWS_Sensor_Counter[0]++;
          // Пишем в память текущий час
          EEPROM_WriteByte ((uint8_t*) HWS_Sensor_Counter,HWS_SENS_ADDR);
        } 
      }
      else Hour_Counter++;
      
      if (HWS_Sensor_Counter[0] == 168) {HWS_SENSOR_DEAD_SET;}
      if (Screw_Sensor_Counter[0] == 168) {SCREW_SENSOR_DEAD_SET;}
    }
    else Hour_Counter = 0;
    
    // Антистоп, работает только в режиме Выключен
    if (MODE_FLAG == SWITCHOFF_MODE)
    {
      // Считаем минуты
      if (MinAntiStop_Counter == 60)
      {
        MinAntiStop_Counter = 0;
        // Считаем неделю в минутах
        if (AntiSTOP_Counter < 10080) AntiSTOP_Counter++;
        if (AntiSTOP_Counter == 10080) AntiSTOP_Counter = 0;
        // Раз в час пишем текущую минуту
        if ((AntiSTOP_Counter % 60) == 0)
        {
          if (AntiSTOP_Counter != 0) AntiSTOPHour_Counter[0]++;
          else AntiSTOPHour_Counter[0] = 0;
          EEPROM_WriteByte ((uint8_t*) AntiSTOPHour_Counter,ANTI_STOP_ADDR);
        }
            
      }
      else MinAntiStop_Counter++;
    }
    else 
    {
      MinAntiStop_Counter = 0; 
      AntiSTOP_Counter = 0;
      // Сбрасываем в памяти счетчик, если не равен 0
      if (AntiSTOPHour_Counter[0] != 0) 
      {
        EEPROM_WriteByte ((uint8_t*) AntiSTOPHour_Counter,ANTI_STOP_ADDR);
        AntiSTOPHour_Counter[0] = 0;
      }
    }
    
    
    // Чтение АЦП
    ADC_Read_Counter++;
    if (ADC_Read_Counter == 2)
    {
      ADC_READ_FLAG_SET;
      ADC_Read_Counter = 0;
    }
    
    // Выводим АЦП
     ADC_FLAG_SET;
    
    // Переключаем режим через 2 сек после условия
    if (ModeChange_Counter < 2) ModeChange_Counter++;   
    
    // Время подачи
    if ((MODE_FLAG == IGNITION_MODE) || 
        (MODE_FLAG == REGULATION_MODE)||
        (MODE_FLAG == EXTINGUISHING_MODE))
    {
      FeedTime_Counter++;
      if (FeedTime_Counter == sSupplyPeriod.Value + sSupplyTime.Value)
      {
        FeedTime_Counter = 0;
      }
    }
    else
    if (MODE_FLAG == CONTROL_MODE)
    {
      FeedTime_Counter++;
      if (FeedTime_Counter == (sBlowingPeriod.Value*60 + sBlowingTime.Value))
      {
        FeedTime_Counter = 0;
        
        // Повторение подач
        FeedTime_Num++;
        if ((FeedTime_Num == sSupplyRepetition.Value) ||
            (FeedTime_Num == sSupplyRepetition.Value + 2))
          FeedTime_Num = 0;
      }
    }
    
    // Время пересыпки
    if (IS_OVERFLOW)
    {
      Overflow_Counter++;
      if (Overflow_Counter == sTimeTransfer.Value * 60) 
      {
        Overflow_Counter = 0;
        OVERFLOW_OFF;
      }
    }
    else
    {
      Overflow_Counter = 0;
    }
    
    // Вентилятор
    if (MODE_FLAG == CONTROL_MODE)
    {
      BlowingTime_Counter++;
      if (BlowingTime_Counter == (sBlowingPeriod.Value*60 + sBlowingTime.Value)) BlowingTime_Counter = 0;
    }
    
    // Тушение
    if (MODE_FLAG == EXTINGUISHING_MODE)
    {
      if (Extinguishing_Counter > 0) Extinguishing_Counter--;
    }
    else Extinguishing_Counter = 0;
    
    // Насос отопления
    if ((IS_THERMO_ON) && (IS_THERMO_CLOSE))
    {
      if (CirPumpStart_Counter < 25) 
      {
        CirPumpStart_Counter++;
        CirPump_Counter = sTimePumpOn.Value;
      }
      else
      {
        CirPump_Counter++;
        if (CirPump_Counter == sTimePumpOn.Value + sTimePumpOff.Value*60) CirPump_Counter = 0; 
      }
    }
    else {CirPump_Counter = 0; CirPumpStart_Counter = 0;}
    
    // Счетчик периода между писками
    if (Buzzer_Period_Counter > 0) Buzzer_Period_Counter--;
    
    // Энергосбережение
    if (ISNT_OFF(sEnergySaving) && (Menu_SelectedItem != (sMenu*) &CheckWindow))
    {
      if (EnergySaving_Counter == 60 * sEnergySaving.Value)
      {
        // Установка яркости на 30%
        if (ISNT_ENERGY) TFT_LED_Set_Brightness(2);
        ENERGY_SET;
      }
      else EnergySaving_Counter++;
    }
    else EnergySaving_Counter = 0;
    
    // Хранитель экрана
    if (ISNT_OFF(sScreenSaver) && (WARNING_FLAGS == 0) && 
        (Menu_SelectedItem != (sMenu*) &CheckWindow))
    {
      // Раз в 5 сек сек меняем положение хранителя, полный цикл 60 сек
      if (IS_SCREENSAVER)
      {
        if ((ScreenSaverChange_Counter == 60))
        {
          ScreenSaverChange_Counter = 0;
        }
        else ScreenSaverChange_Counter++;
      }
      
      // Переход в хранитель экрана
      if (ScreenSaver_Counter == 60 * sScreenSaver.Value)
      {
        // Включаем хранитель экрана
        SCREENSAVER_SET;
      }
      else ScreenSaver_Counter++;
    }
    else {ScreenSaver_Counter = 0; ScreenSaverChange_Counter = 0;}
    
    EVENT_1HZ_RESET;
  }
  
  
  // Сброс счетчика сторожевого таймера
  IWDG->KR = 0xAAAA;
}