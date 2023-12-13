#include "main.h"
#include "Init.h"
#include "Menu.h"
#include "adc.h"
#include "24C_EEPROM.h"

/*******************************************************************************
  *
  * Обработчик режимов работы
  *
*******************************************************************************/

uint8_t MODE_FLAG = SWITCHOFF_MODE;

void Work_ModeHandler(void)
{
  if (Menu_SelectedItem != (sMenu*) &CheckWindow)
  {
    switch(MODE_FLAG)
    {
      // Режим выключен
      case SWITCHOFF_MODE:
        // Сброс счетчиков 
        FeedTime_Counter = 0;
        FeedTime_Num = 0;
        BlowingTime_Counter = 0;

        break;
              
      // Режим разжигания
      case IGNITION_MODE:
        // Проверка температуры на достижение заданной
        Work_TempSetAchieve();
        // Проверка температуры на переход в регуляцию
        Work_TempRegulationAchieve();
        break;
      
      // Режим контроль
      case CONTROL_MODE:
        // Проверка температуры на упадок гистерезиса
        Work_TempFallHyster();
        // Проверка на падение температуры до температуры отключения
        Work_TempOffAchieve();
        break;
        
      // Режим регуляции
      case REGULATION_MODE:
        // Проверка на достижение заданной температуры
        Work_TempSetAchieve();
        // Проверка на падение температуры до температуры отключения
        Work_TempOffAchieve();
        break;
        
      // Режим тушения
      case EXTINGUISHING_MODE:      
        // Проверка на выход температуры выше температуры отключения
        Work_TempOnAchieve();          
        // Проверка на переход в ожидание через время тушения
        Work_ExtinguishingEnd();
        break;
        
      // Режим ожидания
      case WAITING_MODE:
    
        break;
    }
    // Цикл вентилятора
    Work_FanCycle();
    
    // Цикл подачи
    Work_SupplyCycle();
    
    // Цикл термостата
    Work_ThermostatCycle();
    
    // Цикл день/ночь
    Work_DayNightCycle();
    
    // Цикл насоса ГВС
    Work_HWSPumpCycle();
    
    // Цикл циркуляционного насоса отопления
    Work_CirPumpCycle();
    
    // Цикл обработки предупреждений
    Work_TempAlarmCycle();
    
    // Короткий звук пищалки
    Work_BuzzerShort(); 
    
    // Внешний стоп
    Work_StopEXT();
  }
}
  
/*******************************************************************************
  *
  * Устанавливаем скорость вентилятора
  *
*******************************************************************************/

float Power_Fan_mdf = 0;

void Work_FanPowerSet (uint16_t Power)
{
  // Расчет значения мощности реальной
  // Power меняется в диапазоне от 10 до 100, который приводится к диапазону 
  // между максимальным и минимальным значением:
  // Power = 100% -> Fan = Wmax
  // Power = 10% - > Fan = Wmin + 10% 
  
  if (Power > 0)
  {
    float k = sFanPowerMax.Value-sFanPowerMin.Value;
    k /= sFanPower.Max;
    uint16_t b = sFanPowerMin.Value;
    
    Power_Fan_mdf = Power * k + b;
  }
  else Power_Fan_mdf = 0;
  
  if (Power == 0) {FAN_OFF;}
  else            {FAN_ON;}

  sFanCurrent.Value = Power;
}

/*******************************************************************************
  *
  * Цикл работы вентилятора
  *
  * В меню ручного управления не контролируется
  *
*******************************************************************************/

void Work_FanCycle (void)
{
  // Если не в ручном режиме и не критическая температура
  if (ISNT_MANUAL_MENU && ISNT_CRIT_TEMP)
  {
    // Если выключена пересыпка
    if (ISNT_OVERFLOW)
    {
      switch (MODE_FLAG)
      {
        // В режиме ВЫКЛЮЧЕН И ОЖИДАНИЕ отключаем вентилятор
        case SWITCHOFF_MODE:
        case WAITING_MODE:
          Work_FanPowerSet(0);
          break;
        // В режиме РАЗЖИГАНИЕ, РЕГУЛЯЦИЯ или ТУШЕНИЕ
        case IGNITION_MODE:
        case REGULATION_MODE:
        case EXTINGUISHING_MODE:
          // Включаем вентилятор на мощность "Мощность на дровах"
          if (IS_FIREWOOD)        Work_FanPowerSet(sFanPowerFirewood.Value);
          // Включаем вентилятор на мощность "Мощность вентилятора"
          else if (ISNT_FIREWOOD) Work_FanPowerSet(sFanPower.Value);
          break;  
        // В режиме КОНТРОЛЬ
        case CONTROL_MODE:
          // Включаем вентилятора на время продува и "Мощность в продувах"
          if (BlowingTime_Counter < sBlowingTime.Value)
          {
            Work_FanPowerSet(sFanPowerBlowing.Value);
          }
          // Между продувами
          else
          {
            Work_FanPowerSet(0);
          }
          break;
      }
    }
    else
    {
      // Отключаем вентилятор при пересыпке
      Work_FanPowerSet(0);
    }
  }
}

/*******************************************************************************
  *
  * Устанавливаем скорость шнека
  *
*******************************************************************************/

uint8_t Power_Screw = 0;

void Work_SupplyPowerSet (uint16_t Power)
{   
  if (Power == 0) 
  {
    SCREW_OFF;
  }
  else            
  {
    SCREW_ON;
    // Таймер считает до Period c задержкой Power*Perc
    // Чем выше Power, тем раньше таймер начинает считать
    Power_Screw = Power;
  }
}

/*******************************************************************************
  *
  * Цикл работы шнека
  *
  * В ручном режиме не контролируется
  *
*******************************************************************************/

void Work_SupplyCycle (void)
{
  // Если датчик не поврежден 7 дней
  if (ISNT_SCREW_SENSOR_DEAD)
  {
    // Если выключена пересыпка
    if (ISNT_OVERFLOW)
    {
      // Если не в ручном режиме и не критическая температура
      if (ISNT_MANUAL_MENU && ISNT_CRIT_TEMP)
      {
        switch (MODE_FLAG)
        {
          // В режиме ВЫКЛЮЧЕН и ОЖИДАНИЕ отключаем шнек
          case SWITCHOFF_MODE:
          case WAITING_MODE:
            Work_SupplyPowerSet(0);
            break;
          // В режиме РАЗЖИГАНИЕ, РЕГУЛЯЦИЯ или ТУШЕНИЕ
          case IGNITION_MODE:
          case REGULATION_MODE:
          case EXTINGUISHING_MODE:
            // Если идет подача и не на дровах, включаем шнек
            if ((FeedTime_Counter < sSupplyTime.Value) && ISNT_FIREWOOD)
            {
              sSupplyCurrent.Value = sSupplyTime.Value - FeedTime_Counter;
              Work_SupplyPowerSet(100);
            }
            else
            // Между подачами или дрова
            {
              Work_SupplyPowerSet(0);
            }
            break;
          
          // В режиме контроль
          case CONTROL_MODE:
            // Включаем подачу через повторение подач, если не на дровах
            if ((FeedTime_Counter < sSupplyTime.Value) &&
                (FeedTime_Num == 0) &&
                ISNT_FIREWOOD)
            {
              sSupplyCurrent.Value = sSupplyTime.Value - FeedTime_Counter;
              Work_SupplyPowerSet(100);
            }
            else
            // Между подачами или дрова
            {
              Work_SupplyPowerSet(0);
            } 
            break;
        }
      }
    }
    else
    // Если включена пересыпка
    {
      Work_SupplyPowerSet(100);
      sSupplyCurrent.Value = sTimeTransfer.Value*60 - Overflow_Counter;
    }
  }
}

/*******************************************************************************
  *
  * Короткий сигнал пищалки
  *
*******************************************************************************/

void Work_BuzzerShort (void)
{
    // Включаем на время кратно 250 мс
  if (Buzzer_Counter > 0) {BUZZER_ON;}
  else                    {BUZZER_OFF;}
}

/*******************************************************************************
  *
  * Цикл работы термостата
  *
*******************************************************************************/

uint8_t SW2_P = 0; // Термостат для проверки

void Work_ThermostatCycle (void)
{
  // Проверяем замыкание термостата, если включен
  if (IS_THERMO)
  {
    if (SW2_State) THERMO_CLOSE;
    else           THERMO_OPEN;
  }
}

/*******************************************************************************
  *
  * Проверка перехода в ОЖИДАНИЕ через время тушения
  *
*******************************************************************************/

void Work_ExtinguishingEnd (void)
{
  // Если время тушения закончилось
  if (IS_EXTINGUISHING_MODE && (Extinguishing_Counter == 0))
  {
    // Переход в ожидание
    MODE_FLAG = WAITING_MODE;
    
    ADC_FLAG_SET;
    // Сброс счетчиков 
    FeedTime_Counter = 0;
    FeedTime_Num = 0;
    BlowingTime_Counter = 0;
  }
}

/*******************************************************************************
  *
  * Цикл работы режима день/ночь
  *
*******************************************************************************/

void Work_DayNightCycle(void)
{
  // Если режим день/ночь
  if (IS_DAYNIGHT)
  {
    // Если ночное время
    if ((sTimeCurrent.Hours >= sNightPeriodStartTime.Value) ||
        (sTimeCurrent.Hours < sDayPeriodStartTime.Value))
      NIGHT_SET;
    else
    // Если дневное время
    if ((sTimeCurrent.Hours >= sDayPeriodStartTime.Value) &&
        (sTimeCurrent.Hours < sNightPeriodStartTime.Value))
      DAY_SET;
  }
}

/*******************************************************************************
  *
  * Устанавливаем скорость насоса ГВС
  *
*******************************************************************************/

uint8_t Power_HWS = 0;

void Work_HWSPumpPowerSet (uint16_t Power)
{ 
  if (Power == 0) 
  {
    HWSPUMP_OFF;
  }
  else            
  {
    HWSPUMP_ON;
    // Таймер считает до Period c задержкой Power*Period/100
    // Чем выше Power, тем раньше таймер начинает считать
    Power_HWS = Power;
  }
}

/*******************************************************************************
  *
  * Цикл работы насоса ГВС
  *
*******************************************************************************/

void Work_HWSPumpCycle (void)
{
  // Если сработал аварийный термостат, то включаем насос
  if (IS_ALARM_THERMO_TRIG)
  {
    Work_HWSPumpPowerSet(100);
  }
  else
  // Насос ГВС включается при температуре котла выше 40 и не отключен
  if ((sTempIn.Value >= 400) && ISNT_OFF(sTemperatureHWS) && ISNT_HWS_SENSOR_DEAD)
  {
    // Если перегрев, то включаем насос ГВС
    if (sTempIn.Value >= 800)
    {
      Work_HWSPumpPowerSet(100);
    }
    else
    {
      // Если приоритет бойлера отключен
      if (ISNT_BOILER_PRIORITY)
      {
        // Не включаем насос ГВС, если температура ГВС выше температуры котла
        if (sTempHWS.Value > sTempIn.Value)
        {
          Work_HWSPumpPowerSet(0);
          HWS_MORE_SET; 
        }
        else
        // Если температура ГВС падает ниже котла минус гистерезис или растет до
        // равной, то идем по алгоритму
        if (((sTempHWS.Value < sTempIn.Value - 20) && IS_HWS_MORE_SET) ||
            ((sTempHWS.Value <= sTempIn.Value) && ISNT_HWS_MORE_SET))
        {
          HWS_LESS_SET;
          // Если температура ГВС упала ниже заданной ГВС минус гистерезис,
          // то включаем насос ГВС
          if (sTempHWS.Value < ((sTemperatureHWS.Value - sHysteresisHWSSensor.Value)*10))
          {
            Work_HWSPumpPowerSet(100);
          } 
          else
          // Если температура ГВС достигла заданной ГВС, то отключаем насос ГВС
          if (sTempHWS.Value >= sTemperatureHWS.Value*10)
          {
            Work_HWSPumpPowerSet(0);
          }
        }
      }
      else
      // Если приоритет бойлера включен
      if (IS_BOILER_PRIORITY)
      {
        // Если термостат включен и температура ГВС выше температуры котла, то
        // отключаем насос ГВС
        if ((sTempHWS.Value > sTempIn.Value) && IS_THERMO_ON)
        {
          Work_HWSPumpPowerSet(0);
          HWS_MORE_SET;
        }
        else
        // Если температура ГВС падает ниже котла минус гистерезис или растет до
        // равной, то идем по алгоритму
        if (((((sTempHWS.Value < sTempIn.Value - 20) && IS_HWS_MORE_SET) ||
              ((sTempHWS.Value <= sTempIn.Value) && ISNT_HWS_MORE_SET)) && IS_THERMO_ON) ||
            IS_THERMO_OFF)
        {
          HWS_LESS_SET;
          // Если температура ГВС упала ниже заданной ГВС минус гистерезис
          if (sTempHWS.Value < ((sTemperatureHWS.Value - sHysteresisHWSSensor.Value)*10))
          {
            Work_HWSPumpPowerSet(100);
          } 
          else
          // Если температура ГВС достигла заданной ГВС, то отключаем насос ГВС
          if (sTempHWS.Value >= sTemperatureHWS.Value*10)
          {
            Work_HWSPumpPowerSet(0);
          }
        }
      }
    }
  }
  else
  // Если температура опустилась ниже 5 градусов, то включаем насос для исключения
  // замерзания системы отопления
  if ((sTempIn.Value <= 50) && (sTempIn.Value > -200) && 
      ISNT_OFF(sTemperatureHWS) && ISNT_HWS_SENSOR_DEAD)
  {
    Work_HWSPumpPowerSet(100);
  }
  else
  // Если температура котла упала ниже 36, то отключаем насос ГВС
  if ((sTempIn.Value <= 360) && (sTempIn.Value >= 70) && 
      ISNT_OFF(sTemperatureHWS) && ISNT_HWS_SENSOR_DEAD)
  {
    Work_HWSPumpPowerSet(0);
  }
  else
  // Если температура ГВС - откл, то насос ГВС отключен
  if (IS_OFF(sTemperatureHWS) || IS_HWS_SENSOR_DEAD)
  {
    Work_HWSPumpPowerSet(0);
  }
}

/*******************************************************************************
  *
  * Цикл работы циркуляционного насоса отопления
  *
*******************************************************************************/

void Work_CirPumpCycle (void)
{
  // Не контролируется в ручном режиме
  if (ISNT_MANUAL_MENU)
  {
    // Если температура котла больше 80 или меньше 5, или ошибка датчика,
    // или разомкнулся аварийный термостат, то насос работает постоянно
    if ((sTempIn.Value >= 800) || 
        (sTempIn.Value <= 50) || 
        (sTempIn.Value == TEMP_SENSOR_CLOSURE) ||
        (sTempIn.Value == TEMP_SENSOR_BREAK) ||
        IS_ALARM_THERMO_TRIG) 
    {
      CIRPUMP_ON;
      CirPumpStart_Counter = 0;
      CirPump_Counter = 0;
    }
    else
    // В остальных случаях работаем по алгоритму
    {
      // Включаем Анти-СТОП раз в неделю на 1 минуту
      if (IS_SWITCHOFF_MODE && (AntiSTOP_Counter == 10079))
      { 
        CIRPUMP_ON;
      }
      else
      // Если температура котла выше температуры включения насоса отопления, то 
      // включаем насос по алгоритму
      if ((sTempIn.Value >= (sTemperatureHeatingPumpStart.Value * 10)) && 
          ISNT_OFF(sTemperatureHeatingPumpStart))
      { 
        // Если приоритет бойлера выключен
        if (ISNT_BOILER_PRIORITY) 
        {
          // Если термостат выключен, то включаем насос постоянно
          if (IS_THERMO_OFF)
          {
            CIRPUMP_ON;
            CirPumpStart_Counter = 0;
            CirPump_Counter = 0;
          }
          else
          // Если термостат включен
          if (IS_THERMO_ON)
          {
            // Если термостат разомнкут
            if (IS_THERMO_OPEN)
            {
              CIRPUMP_ON;
              CirPumpStart_Counter = 0;
              CirPump_Counter = 0;              
            }
            else
            // Если термостат замкнут
            if (IS_THERMO_CLOSE)
            {
              Work_CirPumpCircThermo();            
            }
          }
        }
        else
        // Если приоритет бойлера включен
        if (IS_BOILER_PRIORITY)
        {
          // Если термостат выключен
          if (IS_THERMO_OFF)
          {
            // Если температура ГВС упала ниже заданной ГВС минус гистерезис,
            // то выключаем насос отопления
            if (sTempHWS.Value < ((sTemperatureHWS.Value - sHysteresisHWSSensor.Value)*10))
            {
              CIRPUMP_OFF;
              CirPumpStart_Counter = 0;
              CirPump_Counter = 0; 
            } 
            // Если температура ГВС достигла заданной ГВС, то включаем насос отопления
            else 
            if (sTempHWS.Value >= sTemperatureHWS.Value*10)
            {
              CIRPUMP_ON;
              CirPumpStart_Counter = 0;
              CirPump_Counter = 0;               
            }
          }
          // Если термостат включен
          else
          if (IS_THERMO_ON)
          {
            // Включаем насос отопления, если температура ГВС выше температуры котла
            if (sTempHWS.Value > sTempIn.Value)
            {
              HWS_MORE_SET;
              // Если термостат замкнут, то включаем насос отопления в цикличном
              // режиме
              if (IS_THERMO_CLOSE)
              {
                Work_CirPumpCircThermo();
              }
              // иначе просто включаем
              else
              {
                CIRPUMP_ON;
                CirPumpStart_Counter = 0;
                CirPump_Counter = 0;  
              }
            }
            else
            // Если температура ГВС падает ниже котла минус гистерезис или растет до
            // равной, то идем по алгоритму
            if (((sTempHWS.Value < sTempIn.Value - 20) && (IS_HWS_MORE_SET)) ||
                ((sTempHWS.Value <= sTempIn.Value) && (ISNT_HWS_MORE_SET)))  
            {
              HWS_LESS_SET;
              // Если температура ГВС упала ниже заданной ГВС минус гистерезис,
              // то выключаем насос отопления
              if (sTempHWS.Value < ((sTemperatureHWS.Value - sHysteresisHWSSensor.Value)*10))
              {
                CIRPUMP_OFF;
                CirPumpStart_Counter = 0;
                CirPump_Counter = 0; 
              }
              else
              // Если температура ГВС достигла заданной ГВС, то включаем насос отопления
              if (sTempHWS.Value >= sTemperatureHWS.Value*10)
              {   
                // Если режим КОНТРОЛЬ и термостат замкнут, то включаем насос
                // отопления в цикличном режиме
                if (IS_THERMO_CLOSE && IS_CONTROL_MODE)
                {
                  Work_CirPumpCircThermo();                  
                }       
                // Иначе просто включаем
                else
                {
                  CIRPUMP_ON;
                  CirPumpStart_Counter = 0;
                  CirPump_Counter = 0; 
                }                  
              }
              // Если в промежутке температур
              else
              {
                // Если термостат замкнут и режим КОНТРОЛЬ, то поддерживаем
                // циклический режим работы насоса отопления
                if (IS_THERMO_CLOSE && IS_CONTROL_MODE)
                {
                  Work_CirPumpCircThermo();
                }      
              }
            }
          }
        }
      }
      else
      // Если температура котла упала ниже температуры включения насоса отопления
      // минус 4 градуса, то отключаем насос
      if (((sTempIn.Value <= ((sTemperatureHeatingPumpStart.Value - 4) * 10)) &&
           (sTempIn.Value >= 70)) ||
          IS_OFF(sTemperatureHeatingPumpStart))
      {
        CirPumpStart_Counter = 0;
        CirPump_Counter = 0;
        CIRPUMP_OFF;
      }
    }
  }
}

/*******************************************************************************
  *
  * Обработчик циклического режима работы насоса отопления
  *
  * Используется 
  * - для включенного и замкнутого термостата в режиме КОНТРОЛЬ при 
  *   включенном приоритете бойлера
  * - для включенного и замнкутого термостата при выключенном приоритете бойлера
  *
*******************************************************************************/

void Work_CirPumpCircThermo (void)
{
  // Включаем насос на 25 секунд при замыкании контактов
  if (CirPumpStart_Counter < 25) {CIRPUMP_ON;}
  else
  {
    // Включаем насос на время включения насоса
    if (CirPump_Counter < sTimePumpOn.Value) {CIRPUMP_ON;}
    else                                     {CIRPUMP_OFF;}
  }                    
}

/*******************************************************************************
  *
  * Предупреждения по температуре
  *
  * Приоритеты:
  * 1. Разрыв цепи контактов аварийного термостата при превышении 85-90 С
  * 2. Критическая температура котла более 90 С
  * 3. Перегрев шнека подачи 
  * 4. Высокая температура котла от 80 до 90 С
  * 5. Снижени температуры ниже 5 С
  *
*******************************************************************************/

uint8_t FF_Byte[1] = {0xFF};

void Work_TempAlarmCycle (void)
{
  // Если сработал аварийный термостат, то отключаем шнек и вентилятор, переходим 
  // в режим выключен. Включить обратно можно только, когда температура упадет ниже 50С
  if (SW1_State == GPIO_PIN_RESET)
  {
    if (ISNT_ALARM_THERMOSTAT)
    {
      MODE_FLAG = SWITCHOFF_MODE;
    }
      
    ALARM_THERMOSTAT_ON;
    ALARM_THERMO_TRIG_SET;
    ALARM_ON;
    Buzzer_Counter = 4; // Не сбрасывается, пищит постоянно
  }
  else
  // Если температура выше 90 градусов, то критический перегрев котла
  // включается звуковая сигнализация, шнек и вентилятор отключаются,
  // насос отопления работает в постоянном режиме
  if (sTempIn.Value >= 900) 
  {
    ALARM_ON;
    CRIT_TEMP_ON;
    // Если не в ручном управлении
    if (ISNT_MANUAL_MENU)
    {
      CIRPUMP_ON;
      Work_FanPowerSet(0);
      SCREW_OFF;
    }
    Buzzer_Counter = 4; // Не сбрасывается, пищит постоянно
  }
  else
  // Если температура шнека достигает температуры тревоги шнека и не выключен, включаем пересыпку
  if ((sTempScrew.Value >= sTemperatureScrewAlarm.Value * 10) && ISNT_OFF(sTemperatureScrewAlarm))
  {
    ALARM_ON;
    SCREW_TEMP_ON;
    if (ISNT_OVERFLOW_START) {OVERFLOW_START_ON; OVERFLOW_ON; OVERFLOW_BUZZER_ON;}
    if (IS_OVERFLOW_BUZZER) Buzzer_Counter = 4; // Пищит постоянно, отключается при нажатии любой кнопки
  }
  else
  // Если температура котла от 80 до 90, периодически включается звуковой сигнал, 
  // включается индикатор АВАРИЯ, но котел работает
  if ((sTempIn.Value >= 800) && (sTempIn.Value < 900))
  {
    ALARM_ON;
    HIGH_TEMP_ON; 
    
    // Пищим 1 секунду раз в 30 сек
    if (Buzzer_Counter == 0) 
    {
      if (Buzzer_Period_Counter == 0) 
      {
        Buzzer_Period_Counter = 30;
        Buzzer_Counter = 4;
      }
    }
  }
  else
  // Если температура ГВС выше 80, периодически включается звуковой сигнал,
  // включается индикатор АВАРИЯ, но котел работает
  if ((sTempHWS.Value >= 800))
  {
    ALARM_ON;
    HWS_TEMP_ON;
    
    // Пищим 1 секунду раз в 30 сек
    if (Buzzer_Counter == 0) 
    {
      if (Buzzer_Period_Counter == 0) 
      {
        Buzzer_Period_Counter = 30;
        Buzzer_Counter = 4;
      }
    }
  }
  else
  // Если температура упала ниже 5 С, включаем звуковой сигнал и индикатор АВАРИЯ,
  // насосы работает в постоянном режиме 
  if ((sTempIn.Value < 50) && (sTempIn.Value > -200))
  {
    ALARM_ON;
    LOW_TEMP_ON;
  }
  
  // Если аварийный термостат замкнулся, а температура упала ниже 50С, то отключаем флаг аварийного термостата
  if(SW1_State)
  {
    if (IS_ALARM_THERMOSTAT) {ALARM_THERMOSTAT_OFF;}
    if (IS_ALARM_THERMO_TRIG && (sTempIn.Value < 500)) {ALARM_THERMO_TRIG_RESET;}
  }
    
  // Если температура упала ниже 89 градусов или датчик стал неисправным, то отключаем флаг критической темп
  if ((sTempIn.Value < 890) && IS_CRIT_TEMP) {CRIT_TEMP_OFF;}
  
  // Если температура упала ниже 80 градусов или датчик стал неисправным, отключаем флаг перегрева
  if ((sTempIn.Value < 800) && IS_HIGH_TEMP) {HIGH_TEMP_OFF;}
  
  // Если температура ГВС упала ниже 80 градусов или датчик стал неисправным, отключаем флаг перегрева ГВС
  if ((sTempHWS.Value < 800) && IS_HWS_TEMP) {HWS_TEMP_OFF;}
  
  // Если температура поднялась выше 5 градусов или неисправен датчик, отключаем флаг низкой температуры
  if (((sTempIn.Value > 50) || (sTempIn.Value == TEMP_SENSOR_CLOSURE) || (sTempIn.Value == TEMP_SENSOR_BREAK)) && 
      IS_LOW_TEMP) {LOW_TEMP_OFF;};
  
  // Если температура шнека упала ниже температуры тревоги шнека или неисправен датчик, 
  // отключаем флаг тревоги шнека
  if ((sTempScrew.Value < (sTemperatureScrewAlarm.Value - 2)*10) && IS_SCREW_TEMP) 
  {
    SCREW_TEMP_OFF;
    OVERFLOW_START_OFF;
    //OVERFLOW_OFF;
    OVERFLOW_BUZZER_OFF;
  }
  
  // Если замыкание или обрыв датчик котла, то пищим, переходим в режим выключен, 
  // включаем сигнал авария, включаем насос отопления 
  if ((sTempIn.Value == TEMP_SENSOR_CLOSURE) || (sTempIn.Value == TEMP_SENSOR_BREAK))
  {
    TEMP_SENSOR_SET;
    ALARM_ON;
    MODE_FLAG = SWITCHOFF_MODE;
    Buzzer_Counter = 4;
  }
  else
  // Если замыкание или обрыв датчика шнека, то пищим при работе шнека, включаем аварию,
  // Пищалка не включается при температуре тревоги шнека - откл.
  // Если прошло 7 дней, то переходим в режим отключен. Работа доступна только на дровах
  if ((sTempScrew.Value == TEMP_SENSOR_CLOSURE) || (sTempScrew.Value == TEMP_SENSOR_BREAK))
  {
    SCREW_SENSOR_SET;
    ALARM_ON;
    if (ISNT_OFF(sTemperatureScrewAlarm) && IS_SCREW) Buzzer_Counter = 4;
    
    if (IS_SCREW_SENSOR_DEAD && IS_AUTO_MODE && ISNT_OFF(sTemperatureScrewAlarm)) 
      MODE_FLAG = SWITCHOFF_MODE;
  }
  else
  // Если замыкание или обрыв датчика ГВС, то пищим при работе насоса ГВС, включаем аварию,
  // Пищалка не включается при температуре ГВС - макс (до 7 дней) или выкл
  // Если прошло 7 дней, то установка температуры ГВС - макс не срабатывает, а котел переходит
  // в режим выключен и больше не включается
  if ((sTempHWS.Value == TEMP_SENSOR_CLOSURE) || (sTempHWS.Value == TEMP_SENSOR_BREAK))
  {
    HWS_SENSOR_SET;
    ALARM_ON;
    if (ISNT_OFF(sTemperatureHWS) && 
        ((IS_HWS_SENSOR_DEAD 
          || 
          (ISNT_HWS_SENSOR_DEAD && (sTemperatureHWS.Value != sTemperatureHWS.Max * 10))) 
         &&
         IS_HWSPUMP))
      Buzzer_Counter = 4;
    
    if (IS_HWS_SENSOR_DEAD && IS_AUTO_MODE && ISNT_OFF(sTemperatureHWS))
      MODE_FLAG = SWITCHOFF_MODE;
  }
   
  // Если замыкание или обрыв датчика котла отсутствует, а флаг взведен, то сбрасываем
  if ((sTempIn.Value != TEMP_SENSOR_CLOSURE) && (sTempIn.Value != TEMP_SENSOR_BREAK))
  {
    if (IS_TEMP_SENSOR) {TEMP_SENSOR_RESET;}
  }
  
  // Если замыкание или обрыв датчика шнека отсутствует, а флаг взведен, то сбрасываем
  if ((sTempScrew.Value != TEMP_SENSOR_CLOSURE) && (sTempScrew.Value != TEMP_SENSOR_BREAK))
  {
    if (Screw_Sensor_Counter[0] != 0)
    {
      Screw_Sensor_Counter[0] = 0;
      EEPROM_WriteByte ((uint8_t*) FF_Byte,SCREW_SENS_ADDR);  
    }
    if (IS_SCREW_SENSOR || IS_SCREW_SENSOR_DEAD)
    {
      SCREW_SENSOR_DEAD_RESET;
      SCREW_SENSOR_RESET;
    }
  }
  
  // Если замыкание или обрыв датчика ГВС отсутствует, а флаг взведен, то сбрасываем
  if ((sTempHWS.Value != TEMP_SENSOR_CLOSURE) && (sTempHWS.Value != TEMP_SENSOR_BREAK))
  {
    if (HWS_Sensor_Counter[0] != 0)
    {
      HWS_Sensor_Counter[0] = 0;
      EEPROM_WriteByte ((uint8_t*) FF_Byte,HWS_SENS_ADDR);
    }
    if (IS_HWS_SENSOR || IS_HWS_SENSOR_DEAD)
    {
      HWS_SENSOR_RESET;
      HWS_SENSOR_DEAD_RESET;
    }
  }
  
  // Если нет предупреждений, то отключаем аварию
  if (WARNING_FLAGS == 0x00000000) { ALARM_OFF;}
  
}

/*******************************************************************************
  *
  * Проверка температуры для перехода в регуляцию из розжига
  *
  * Температура перехода - Температура выключения блока + 15 С
  *
*******************************************************************************/

uint8_t Work_TempRegulationAchieve (void)
{
  if (sTempIn.Value >= ((sTemperatureOff.Value * 10) + 150))
  {
    // Ждем 2 секунды для исключения частого переключения
    if (ModeChange_Counter == 2) ModeChange_Counter = 0;
    if (ModeChange_Counter == 1)
    {    
      // Переходим в регуляцию
      MODE_FLAG = REGULATION_MODE;
      ADC_FLAG_SET;
      MODE_Buf[0] = MODE_FLAG;
      EEPROM_WriteByte ((uint8_t*) MODE_Buf,MODE_FLAG_ADDR);
      return 1;
    }
  }
  return 0;
}

/*******************************************************************************
  *
  * Проверка температуры на достижение заданной
  *
*******************************************************************************/

uint8_t Work_TempSetAchieve (void)
{
  // Если приоритет бойлера выключен
  if (ISNT_BOILER_PRIORITY)
  {
    // Если термостат выключен
    if (IS_THERMO_OFF)
    {
      // Если температура котла достигла заданной, то переходим в режим КОНТРОЛЬ
      if (((sTempIn.Value >= (sTemperatureSetting.Value*10)) && ISNT_NIGHT) ||
          ((sTempIn.Value >= (sTempNight.Value*10)) && IS_NIGHT)) {}
      else return 0;
    }
    else
    // Если термостат включен
    if (IS_THERMO_ON)
    {
      // Если температура котла достигла заданной или замкнулись контакты термостата
      // при температуре котла выше минимальной, то переходим в режим КОНТРОЛЬ
      if ((((sTempIn.Value >= (sTemperatureSetting.Value*10)) && ISNT_NIGHT) ||
           ((sTempIn.Value >= (sTempNight.Value*10)) && IS_NIGHT))
          ||
            (IS_THERMO_CLOSE && (sTempIn.Value >= (sTemperatureSetting.Min*10)))) {}
      else return 0;
    }
    else return 0;
  }
  else
  // Если приоритет бойлера включен
  if (IS_BOILER_PRIORITY)
  {
    // Если термостат выключен
    if (IS_THERMO_OFF)
    {
      // Если температура котла больше максимальной или температура ГВС больше
      // заданной ГВС при температуре котла выше заданной, то переходим в КОНТРОЛЬ
      if ((sTempIn.Value >= (sTemperatureSetting.Max*10)) ||
           ((((sTempIn.Value >= (sTemperatureSetting.Value*10)) && ISNT_NIGHT) ||
             ((sTempIn.Value >= (sTempNight.Value*10)) && IS_NIGHT))
             && 
            (sTempHWS.Value >= (sTemperatureHWS.Value*10)))) {}
      else return 0;
    }
    else
    // Если термостат включен
    if (IS_THERMO_ON)
    {
      // Если температура котла больше максимальной или температура ГВС больше
      // заданной ГВС при температуре котла выше заданной, либо при температуре
      // котла выше минимальной и термостат замкнут, то переходим в КОНТРОЛЬ
      if ((sTempIn.Value >= (sTemperatureSetting.Max*10)) ||
          ((sTempHWS.Value >= (sTemperatureHWS.Value*10)) &&
           ((((sTempIn.Value >= (sTemperatureSetting.Value*10)) && ISNT_NIGHT) ||
             ((sTempIn.Value >= (sTempNight.Value*10)) && IS_NIGHT)) ||
             ((sTempIn.Value >= sTemperatureSetting.Min*10) && IS_THERMO_CLOSE)))) {}
      else return 0;      
    }
    else return 0; 
  }
  else return 0;
  
  // Ждем 2 секунды для исключения частого переключения
  if (ModeChange_Counter == 2) ModeChange_Counter = 0;
  if (ModeChange_Counter == 1)
  {
    // Переходим в контроль, записываем режим в память, если переход из розжига
    if (IS_IGNITION_MODE)
    {
      MODE_FLAG = CONTROL_MODE;
      MODE_Buf[0] = MODE_FLAG;
      EEPROM_WriteByte ((uint8_t*) MODE_Buf,MODE_FLAG_ADDR);
    }
    else MODE_FLAG = CONTROL_MODE;
    
    ADC_FLAG_SET;
    FeedTime_Counter = sBlowingTime.Value; // Чтобы выравнять 1 запуск с продувом
    FeedTime_Num = sSupplyRepetition.Value + 1; // На первый запуск
    BlowingTime_Counter = sBlowingTime.Value;
  }
  return 1;
}

/*******************************************************************************
  *
  * Проверка на падение температуры на величину гистерезиса
  *
*******************************************************************************/

uint8_t Work_TempFallHyster (void)
{
  // Если приоритет бойлера выключен
  if (ISNT_BOILER_PRIORITY)
  {
    // Если термостат выключен
    if (IS_THERMO_OFF)
    {
      // Если температура котла упала ниже заданной минус гистерезис
      // с учетом день/ночь, то переходим в режим РЕГУЛЯЦИЯ
      if (((sTempIn.Value < ((sTemperatureSetting.Value - sHysteresisBoilerSensor.Value)*10)) && ISNT_NIGHT) ||
          ((sTempIn.Value < ((sTempNight.Value - sHysteresisBoilerSensor.Value)*10)) && IS_NIGHT)) {}
      else return 0;
    }
    else
    // Если термостат включен
    if (IS_THERMO_ON)
    {
      // Если температура котла упала ниже заданной минус гистерезис с учетом 
      // день/ночь и термостат разомкнут или температура котла упала ниже
      // минимальной, то переходим в режим РЕГУЛЯЦИЯ
      if (((((sTempIn.Value < ((sTemperatureSetting.Value - sHysteresisBoilerSensor.Value)*10)) && ISNT_NIGHT) ||
           ((sTempIn.Value < ((sTempNight.Value - sHysteresisBoilerSensor.Value)*10)) && IS_NIGHT)) &&
           IS_THERMO_OPEN) 
          ||
           (sTempIn.Value < ((sTemperatureSetting.Min - sHysteresisBoilerSensor.Value) * 10))) {}
      else return 0;
    }
    else return 0;
  }
  else
  // Если приоритет бойлера включен
  if (IS_BOILER_PRIORITY)
  {
    // Если термостат выключен
    if (IS_THERMO_OFF)
    {
      // Если температура котла упала ниже заданной минус гистерезис или температура
      // котла ниже максимальной и температура ГВС упала ниже заданной ГВС минус 
      // гистерезис с учетом день/ночь, то переходим в режим РЕГУЛЯЦИЯ
      if ((((sTempIn.Value < ((sTemperatureSetting.Value - sHysteresisBoilerSensor.Value)*10)) && ISNT_NIGHT) ||
           ((sTempIn.Value < ((sTempNight.Value - sHysteresisBoilerSensor.Value)*10)) && IS_NIGHT))
           ||
          ((sTempIn.Value < ((sTemperatureSetting.Max - sHysteresisBoilerSensor.Value)*10)) && 
           (sTempHWS.Value < ((sTemperatureHWS.Value - sHysteresisHWSSensor.Value)*10)))) {}
      else return 0;
    }
    else
    // Если термостат включен
    if (IS_THERMO_ON)
    {
      // Если температура котла упала ниже максимальной и температура ГВС упала 
      // ниже заданной ГВС минус гистерезис или температура котла упала ниже
      // заданной минус гистерезис при разомкнутом термостате или температура 
      // котла упала ниже минимальной с учетом день/ночь, то переходим в режим
      // РЕГУЛЯЦИЯ
      if (((sTempIn.Value < ((sTemperatureSetting.Max - sHysteresisBoilerSensor.Value) * 10)) &&
          (sTempHWS.Value < ((sTemperatureHWS.Value - sHysteresisHWSSensor.Value)*10))) 
          ||
          ((((sTempIn.Value < ((sTemperatureSetting.Value - sHysteresisBoilerSensor.Value)*10)) && ISNT_NIGHT) ||
            ((sTempIn.Value < ((sTempNight.Value - sHysteresisBoilerSensor.Value)*10)) && IS_NIGHT)) &&
            IS_THERMO_OPEN)
          ||
          (sTempIn.Value < ((sTemperatureSetting.Min - sHysteresisBoilerSensor.Value) * 10))) {}
      else return 0; 
    }
    else return 0; 
  }
  else return 0;
  
  // Ждем 2 секунды для исключения частого переключения
  if (ModeChange_Counter == 2) ModeChange_Counter = 0;
  if ((ModeChange_Counter == 1) && (sTempIn.Value > -200))
  { 
    // Переходим в регуляцию
    MODE_FLAG = REGULATION_MODE;
    ADC_FLAG_SET;
    FeedTime_Counter = 0;
    FeedTime_Num = 0;
    BlowingTime_Counter = 0;
  }
  return 1;
}

/*******************************************************************************
  *
  * Проверка на падение температуры до температуры выключения блока
  *
*******************************************************************************/

void Work_TempOffAchieve (void)
{
  if ((sTempIn.Value < sTemperatureOff.Value*10) && (sTempIn.Value > -200))
  {
    // Ждем 2 секунды для исключения частого переключения
    if (ModeChange_Counter == 2) ModeChange_Counter = 0;
    if (ModeChange_Counter == 1)
    {
      // Переходим в тушение
      MODE_FLAG = EXTINGUISHING_MODE;
      ADC_FLAG_SET;
      FeedTime_Counter = 0;
      FeedTime_Num = 0;
      BlowingTime_Counter = 0;    
      Buzzer_Counter = 2; // 0.5 сек
      
      // Выставляем время тушения
      Extinguishing_Counter = sFuelLackTime.Value*60;
    }
  }
}

/*******************************************************************************
  *
  * Проверка на выход из тушения
  *
*******************************************************************************/

void Work_TempOnAchieve (void)
{
  if (sTempIn.Value >= (sTemperatureOff.Value + sHysteresisBoilerSensor.Value)*10)
  {
    // Ждем 2 секунды для исключения частого переключения
    if (ModeChange_Counter == 2) ModeChange_Counter = 0;
    if (ModeChange_Counter == 1)
    {
      // Переходим в регуляцию
      MODE_FLAG = REGULATION_MODE;
      ADC_FLAG_SET;
      FeedTime_Counter = 0;
      FeedTime_Num = 0;
      BlowingTime_Counter = 0;    
      Buzzer_Counter = 0;
    }
  }
}

/*******************************************************************************
  *
  * Проверка на срабатывания внешнего стопа
  *
*******************************************************************************/

void Work_StopEXT(void)
{
  // Если сработал внешний стоп и режим не выключен, то выключаем
  if (Stop_EXT_State && ISNT_SWITCHOFF_MODE)
  {
    MODE_FLAG = SWITCHOFF_MODE;
    MODE_Buf[0] = MODE_FLAG;
    EEPROM_WriteByte ((uint8_t*) MODE_Buf,MODE_FLAG_ADDR);
  }
}