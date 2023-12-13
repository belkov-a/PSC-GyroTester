
#include "main.h"
#include "Menu.h"
#include "ILI9341.h"
#include "Init.h"
#include "adc.h"
#include "24C_EEPROM.h"

/*******************************************************************************
  *
  * Отрисовка всех окон
  *
*******************************************************************************/

uint8_t Menu_DrawWindows (sMenu* Item)
{
  // Страница нелицензионного ПО
  if (Item == (sMenu*) &LicenseWindow)
  {
    // Отрисовка страницы нелицензионного ПО
    Menu_DrawLicenseWindow();
  }
  else
  // Стартовая страница
  if (Item == (sMenu*) &StartWindow)
  {
    // Отрисовка стартовой страницы
    Menu_DrawStartWindow();
    // Сброс флагов обновления
    NEED_UPDATE_RESET;
    NEED_UPDATE_HEAD_RESET;
    // Реакция на кнопки
    Window_EnterEscapeUpDown (Item);
  }
  else
  if (Item == (sMenu*) &MainWindow)
  {
    // Заливка фона при предупреждениях
    Menu_AlarmBackground();
    
    // Обработка строки загрузки
    Menu_DrawLoadBar();

    // Отрисовка режимов
    Menu_DrawMainWindow();
    
    // Реакция на кнопки
    Window_EnterEscapeUpDown (Item);
    // Сброс флагов обновления 
    NEED_UPDATE_HEAD_RESET;
  }
  else
  // Меню настроек
  { 
    if (IS_NEED_UPDATE_MENU)
    {
      NEED_UPDATE_SET;
      NEED_UPDATE_HEAD_SET;
      NEED_UPDATE_SCROLL_SET;
      NEED_UPDATE_MENU_RESET;
    }
    // Обработка изменения параметров
    Menu_ProduceParameters();
    // Обработка кнопок в меню
    Menu_StartStopMoreLess(Item);
    if (Menu_SelectedItem == (sMenu*) &StartWindow) return 1;
    // Отрисовка шаблона меню
    Menu_DrawTemplate(Menu_SelectedItem);
    // Отрисовка заголовка страницы
    Menu_DrawHead(Menu_SelectedItem);
    // Отрисовка скрола
    Menu_DrawScroll(Menu_SelectedItem); 
    // Отрисовка меню
    Menu_DrawSettings(Menu_SelectedItem);
    // Сброс флагов обновления
    NEED_UPDATE_RESET;
    NEED_UPDATE_HEAD_RESET;
    NEED_UPDATE_SCROLL_RESET;
    NEED_UPDATE_LANG_RESET;
  }
  
  return 0;
}

/*******************************************************************************
  *
  * Переходы между окнами
  *
*******************************************************************************/
uint16_t MoreLessCounter;

void Window_EnterEscapeUpDown (sMenu *Menu)
{
  int Act,Res;
  Act  = (*Menu).Item & mActive;
  Res  = (*Menu).Item & ~mActive;
  
  // Если не взведен флаг энергосбережения
  if (ISNT_ENERGY && ISNT_SCREENSAVER)
  {
    if (BUTTONS > 0) {EnergySaving_Counter = 0;ScreenSaver_Counter = 0;ScreenSaverChange_Counter = 0;}
    
    // Если включена тревога шнека и пищалка, любой кнопкой отключаем пищалку
    if ((BUTTONS > 0) && (IS_OVERFLOW_BUZZER)) OVERFLOW_BUZZER_OFF;
    
    // Если не нажаты кнопки, то сбрасываем флаг выхода из меню
    // используется для ложного выхода из режима
    if ((BUTTONS == 0) && (RepeatButtons == 0)) MENU_EXIT_RESET;
    
    // Если основное окно
    if (Menu == (sMenu*)&MainWindow)
    {
      switch (BUTTONS)
      {
        // СТОП, работает в режимах кроме ВЫКЛЮЧЕН
        case (But_STOP):
          if (ISNT_SWITCHOFF_MODE)
          {
            // Если удерживактся и не вышли из меню, переходим в ВЫКЛЮЧЕН
            if ((RepeatButtons >= 1000) && ISNT_MENU_EXIT)
            {
              MoreLessCounter = 0;
              MODE_FLAG = SWITCHOFF_MODE;
              MODE_Buf[0] = MODE_FLAG;
              EEPROM_WriteByte ((uint8_t*) MODE_Buf,MODE_FLAG_ADDR);
              ADC_FLAG_SET;
              NEED_UPDATE_BAT_SET;  
              BUTTONS = 0;
            }
          }

          // Отмена пересыпки
          if (IS_OVERFLOW)
          {
            Overflow_Counter = 0;
            OVERFLOW_OFF;
          }
          break;
        // МЕНЬШЕ, работает только в режиме ВЫКЛЮЧЕН
        case (But_LESS):
            // Если удерживаются, заполняем шкалу
            if (RepeatButtons >= 1000) MoreLessCounter += 15;
            // Если шкала заполнилась переходим в сервисные настройки 
            if (MoreLessCounter > 314) 
            {
              //MoreLessCounter = 0;
              if (IS_SWITCHOFF_MODE || IS_WAITING_MODE)
                Menu_SelectedItem = (sMenu*)&ServiceMenu;
              else
                Menu_SelectedItem = (sMenu*)&MainMenu;   
              BACK_COLOR = BLACK;
              TFT_FillScreen(BACK_COLOR);
              NEED_UPDATE_MENU_SET;
              NEED_UPDATE_BAT_SET;
            }
            break;        
        // БОЛЬШЕ 
        case (But_MORE):
          // Если удерживаются, заполняем шкалу
          if (RepeatButtons >= 1000) MoreLessCounter += 15;
          // Если шкала заполнилась переходим в основные настройки
          if (MoreLessCounter > 314) 
          {
            //MoreLessCounter = 0;
            Menu_SelectedItem = (sMenu*)&MainMenu;
            BACK_COLOR = BLACK;
            TFT_FillScreen(BACK_COLOR);
            NEED_UPDATE_MENU_SET;
            NEED_UPDATE_BAT_SET;
          }
          break;
        // СТАРТ
        case (But_START):
          // Если ВЫКЛЮЧЕН или ОЖИДАНИЕ, переходим в РОЗЖИГ, если температура
          // ниже заданной, иначе КОНТРОЛЬ
          // Не переходим, если сработал аварийный термостат и температура 
          // Не переходим, если поврежден датчик котла
          // Не переходим, если прошло 7 дней после повреждения датчика шнека
          // Не переходим, если замкнут внешний стоп
          if (ISNT_ALARM_THERMO_TRIG)
          {
            if (Stop_EXT_State == GPIO_PIN_RESET)
            {
              if ((IS_SWITCHOFF_MODE || IS_WAITING_MODE || IS_EXTINGUISHING_MODE) &&
                   ISNT_TEMP_SENSOR)
              {
                if (((IS_SCREW_SENSOR_DEAD && ISNT_OFF(sTemperatureScrewAlarm))
                     || (IS_HWS_SENSOR_DEAD && ISNT_OFF(sTemperatureHWS))) && 
                    IS_AUTO_MODE) {}
                else 
                {
                  if (sTempIn.Value >= sTemperatureSetting.Value * 10) 
                  {
                    MODE_FLAG = CONTROL_MODE;
                    FeedTime_Counter = sBlowingTime.Value; // Чтобы выравнять 1 запуск с продувом
                    FeedTime_Num = sSupplyRepetition.Value + 1; // На первый запуск
                    BlowingTime_Counter = sBlowingTime.Value;
                    CirPumpStart_Counter = 25; // включаем насос на 25 сек 
                    CirPump_Counter = 0;
                  }
                  else MODE_FLAG = IGNITION_MODE;
                  
                  MODE_Buf[0] = MODE_FLAG;
                  EEPROM_WriteByte ((uint8_t*) MODE_Buf,MODE_FLAG_ADDR);
                }
              }
            }
          }
          break;
      }
      (*Menu).Item = Res | Act;
    }
    
    // Если стартовая заставка, по нажатию переходим к главному экрану
    if ((Menu == (sMenu*)&StartWindow) && (BUTTONS > 0))
    {
      //Act &= ~mActive;
      Menu_SelectedItem = (sMenu*)&MainWindow;
      TFT_FillScreen(BACK_COLOR);
      NEED_UPDATE_HEAD_SET;
      NEED_UPDATE_BAT_SET;
      BUTTONS = 0;
      (*Menu).Item = Res | Act;
    }
  }
  else
  // Если в энергосбережении или в хранителе, по нажатию кнопки включаем подсветку
  // или отображаем информацию заново по нажатию кнопки либо предупреждению
  {
    // Включение подсветки
    if (IS_ENERGY && (BUTTONS > 0))
    {
      TFT_LED_Set_Brightness(100);      
      ENERGY_RESET;
      
      BUTTONS = 0;
      EnergySaving_Counter = 0;
    }
    
    // Выход из хранителя
    if (IS_SCREENSAVER && ((BUTTONS > 0) || (WARNING_FLAGS > 0)))
    {
      // Очистка дисплея
      TFT_FillScreen(BACK_COLOR);
      // Взводим флаги необходимости обновления
      NEED_UPDATE_MENU_SET;
      NEED_UPDATE_SET;
      NEED_UPDATE_LANG_SET;
      NEED_UPDATE_BAT_SET;
      
      SCREENSAVER_RESET;
      
      BUTTONS = 0;
      ScreenSaver_Counter = 0;
      ScreenSaverChange_Counter = 0;
    }  
  }
}

/*******************************************************************************
  *
  * Отрисовка верхней панели
  *
*******************************************************************************/

void Menu_DrawHeaderPanel(void)
{
  // Если хранитель выключен
  if (ISNT_SCREENSAVER)
  {  
    // Отрисовка текущего времени по центру
    RTC_Write(TIME,0,2,C_ALIGN,TEXT_COLOR,NO_BACKGROUND,Calibri);
    
    // Отрисовка батареи
    Menu_DrawBattery (289, 2);
       
    // Значок аварии
    if      (IS_ALARM_FLAG)   TFT_DrawImage(3, 2, 1, (sImage*) &AlarmImg);
    else if (ISNT_ALARM_FLAG) TFT_FillRect (3, 2, AlarmImg.Width, AlarmImg.Height, BACK_COLOR);

    // Значок вентилятора
    if      (IS_FAN)          TFT_DrawImage(22, 2, 1, (sImage*) &FanImg);
    else if (ISNT_FAN)        TFT_FillRect (22, 2, FanImg.Width, FanImg.Height, BACK_COLOR);
    
    // Значок шнека
    if      (IS_SCREW)        TFT_DrawImage(41, 2, 1, (sImage*) &ScrewImg);
    else if (ISNT_SCREW)      TFT_FillRect (41, 2, ScrewImg.Width, ScrewImg.Height, BACK_COLOR);
     
    // Значок насоса ГВС
    if      (IS_HWSPUMP)      TFT_DrawImage(60, 2, 1, (sImage*) &HWSpumpImg);
    else if (ISNT_HWSPUMP)    TFT_FillRect (60, 2, HWSpumpImg.Width, HWSpumpImg.Height, BACK_COLOR);
    
    // Значок циркуляционного насоса отопления
    if      (IS_CIRPUMP)      TFT_DrawImage(79, 2, 1, (sImage*) &CircularPumpImg);
    else if (ISNT_CIRPUMP)    TFT_FillRect (79, 2, CircularPumpImg.Width, CircularPumpImg.Height, BACK_COLOR);
    
    // Значок аварийного термостата
    if      (IS_ALARM_THERMO_TRIG)   TFT_DrawImage(98, 2, 1, (sImage*) &AlarmThermostatImg);
    else if (ISNT_ALARM_THERMO_TRIG) TFT_FillRect (98, 2, AlarmThermostatImg.Width, AlarmThermostatImg.Height, BACK_COLOR);
    
    // Значок работы на дровах
    if      (IS_FIREWOOD)     TFT_DrawImage(210, 2, 1, (sImage*) &FirewoodImg);
    else if (ISNT_FIREWOOD)   TFT_FillRect (210, 2, FirewoodImg.Width, FirewoodImg.Height, BACK_COLOR);
    
    // Значок термостата
    if (IS_THERMO)
    {
      if (IS_THERMO_OPEN)       TFT_DrawImage(229, 2, 1, (sImage*) &ThermostatOpenImg);
      else if (IS_THERMO_CLOSE) TFT_DrawImage(229, 2, 1, (sImage*) &ThermostatCloseImg);
    }
    else if (ISNT_THERMO)       TFT_FillRect (229, 2, ThermostatOpenImg.Width, ThermostatOpenImg.Height, BACK_COLOR);
    
    // Значок бойлера
    if (IS_BOILER_PRIORITY)        TFT_DrawImage(248, 2, 1, (sImage*) &BoilerImg);
    else if (ISNT_BOILER_PRIORITY) TFT_FillRect (248, 2, BoilerImg.Width, BoilerImg.Height, BACK_COLOR);
    
    // Значок дня/ночи
    if (IS_DAYNIGHT)
    {
      if      (IS_DAY)   TFT_DrawImage(267, 2, 1, (sImage*) &DayImg);
      else if (IS_NIGHT) TFT_DrawImage(267, 2, 1, (sImage*) &NightImg);
    }
    else TFT_FillRect (267, 2, DayImg.Width, DayImg.Height, BACK_COLOR);
  }
}

/*******************************************************************************
  *
  * Отрисовка страницы нелицензионного ПО
  *
*******************************************************************************/

const char LicenseTitle[2][15] = {"НЕЛИЦЕНЗИОННОЕ","UNLICENSED"};
const char LicenseTitle1[2][9] = {"ПО","SOFTWARE"};

void Menu_DrawLicenseWindow(void)
{  
  // Нелицензионное программное обеспечение
  TFT_WriteString(0,95, C_ALIGN,HEAD_COLOR,BACK_COLOR,(char*) LicenseTitle[Language],Calibri); 
  TFT_WriteString(0,135,C_ALIGN,HEAD_COLOR,BACK_COLOR,(char*) LicenseTitle1[Language],Calibri);
}

/*******************************************************************************
  *
  * Отрисовка стартовой страницы
  *
  * Рисует логотип, наименование блока управления, версия и дата ПО, сайт
  *
*******************************************************************************/

#ifdef PSC_START_DEVICE
const char DeviceTitleStart[2][25] = {"Контроллер пеллетного","Pellet boiler controller"};
const char DeviceTitleStart1[2][16] = {"котла ''СТАРТ''","''START''"};
#endif
#ifdef PSC_PELLETOR_DEVICE
const char DeviceTitlePelletor[2][16] = {"Пеллетный котел","Pellet boiler"};
const char DeviceTitlePelletor1[2][9] = {"PELLETOR","PELLETOR"};
#endif
const char SoftVerStart[2][6]  = {"Вер. ","Ver. "};
const char SoftDateStart[2][7] = {" от "," from "};
const char WebsiteTermokub[2][16] = {"www.termokub.ru","www.termokub.ru"};

void Menu_DrawStartWindow(void)
{  
  // Отрисовка главной страницы, если не хранитель экрана
  if (ISNT_SCREENSAVER)
  {
    sImage *DeviceLogo;
    char *Title1;
    char *Title2;
    #ifdef PSC_PELLETOR_DEVICE
    DeviceLogo = &logo_pelletor;
    Title1 = (char*) DeviceTitlePelletor[Language];
    Title2 = (char*) DeviceTitlePelletor1[Language];
    #endif
    #ifdef PSC_START_DEVICE
    DeviceLogo = &logo;
    Title1 = (char*) DeviceTitleStart[Language];
    Title2 = (char*) DeviceTitleStart1[Language];
    #endif
    
    // Логотип компании
    TFT_WriteMenuImage(0, 30, C_ALIGN, Menu_SelectedItem, 1,DeviceLogo);
    
    if (IS_NEED_UPDATE)
    {      
      // Наименование блока управления
      TFT_WriteString(0,122,C_ALIGN,TEXT_COLOR,BACK_COLOR,Title1,Calibri);
      TFT_WriteString(0,145,C_ALIGN,TEXT_COLOR,BACK_COLOR,Title2,Calibri);
      
      // Вывод наименования устройства
      TFT_WriteString(0,173,C_ALIGN,TEXT_COLOR,BACK_COLOR,DeviceNamePoint,Calibri);
      
      // Версия и дата софта
      char SoftText[24] ;
      for (int i = 0; i < 32; i++)
      {
        if (i < 5)                   SoftText[i] = SoftVerStart[Language][i];
        if ((i >= 5) && (i < 8))     SoftText[i] = SoftVersion[i-5];
        if (Language == RUS) 
        {
          if ((i >= 8) && (i < 12))  SoftText[i] = SoftDateStart[Language][i-8];
          if ((i >= 12) && (i < 20)) SoftText[i] = SoftDate[i-12];
          if (i == 20) SoftText[i] = 0;
        }
        else
        if (Language == ENG) 
        {
          if ((i >= 8) && (i < 14))  SoftText[i] = SoftDateStart[Language][i-8];
          if ((i >= 14) && (i < 22)) SoftText[i] = SoftDate[i-14];
          if (i == 22) SoftText[i] = 0;
        }
        
      }
      TFT_WriteString(0,198,C_ALIGN,TEXT_COLOR,BACK_COLOR,(char*) SoftText,Calibri);
      
      // Сайт термокуб
      TFT_WriteString(0,220,C_ALIGN,TEXT_COLOR,BACK_COLOR,(char*) WebsiteTermokub,Calibri);  

    }
  }
}

/*******************************************************************************
  *
  * Отрисовка основного окна
  *
*******************************************************************************/

const char AutoModeName[2][10]          = {"АВТОРЕЖИМ","AUTOMODE"};
const char FirewoodModeName[2][9]      = {"ДРОВА","FIREWOOD"};
const char ThermostatModeName[2][11]    = {"ТЕРМОСТАТ","THERMOSTAT"};

const char SwitchOffModeName[2][15]     = {"   ВЫКЛЮЧЕН   "," SWITCHED OFF "};
const char IgnitionModeName[2][21]      = {"     РОЗЖИГ     ","      IGNITION      "};
const char ControlModeName[2][18]       = {"   КОНТРОЛЬ   ","     CONTROL     "};
const char RegulationModeName[2][17]    = {"  РЕГУЛЯЦИЯ  ", "   REGULATION   "};
const char ExtinguishingModeName[2][14] = {"   ТУШЕНИЕ   ", "EXTINGUISHING"};
const char WaitingModeName[2][20]       = {"   ОЖИДАНИЕ   ","      WAITING      "};
const char ExtrusionModeName[2][17]     = {"  ПЕРЕСЫПКА  ", "    OVERFLOW    "};

void Menu_DrawMainWindow(void)
{
  char* ModeName;
  
  // Если хранитель экрана выключен
  if (ISNT_SCREENSAVER)
  {
    switch(MODE_FLAG)
    {
      // Режим выключен
      case SWITCHOFF_MODE:
        ModeName = (char*) SwitchOffModeName[Language];
        if (ISNT_OVERFLOW && ISNT_CRIT_TEMP && ISNT_ALARM_THERMOSTAT)
        {
          TFT_FillRect(111,130,TFT_SIGN_WIDTH,18,BACK_COLOR);
          TFT_FillRect(111,155,TFT_SIGN_WIDTH,18,BACK_COLOR);
        }
        break;      
        
      // Режим разжигания
      case IGNITION_MODE:
        ModeName = (char*) IgnitionModeName[Language];
        // Выводим "Подача ХХ сек", если не дрова
        if (ISNT_OVERFLOW && ISNT_CRIT_TEMP && ISNT_ALARM_THERMOSTAT)
        {
          if ((FeedTime_Counter < sSupplyTime.Value) && ISNT_FIREWOOD)
          {
            TFT_WriteParamName (111,130,S_ALIGN,&sSupplyCurrent,TEXT_COLOR,BACK_COLOR,Calibri);
            TFT_WriteParamSign (111,155,S_ALIGN,&sSupplyCurrent,TEXT_COLOR,BACK_COLOR,Calibri);
          }
          // Иначе пустое поле
          else
          {
            TFT_FillRect(111,130,TFT_SIGN_WIDTH,18,BACK_COLOR);
            TFT_FillRect(111,155,TFT_SIGN_WIDTH,18,BACK_COLOR);
          }
        }
        break;

      // Режим контроль
      case CONTROL_MODE:
        ModeName = (char*) ControlModeName[Language];
        if (ISNT_OVERFLOW && ISNT_CRIT_TEMP && ISNT_ALARM_THERMOSTAT)
        {
          // Выводим "Подача ХХ сек" через повторение подач, если не дрова
          if ((FeedTime_Counter < sSupplyTime.Value) &&
              (FeedTime_Num == 0) && ISNT_FIREWOOD)
          {
            TFT_WriteParamName (111,130,S_ALIGN,&sSupplyCurrent,TEXT_COLOR,BACK_COLOR,Calibri);
            TFT_WriteParamSign (111,155,S_ALIGN,&sSupplyCurrent,TEXT_COLOR,BACK_COLOR,Calibri);
          }
          // Иначе пустое поле
          else
          {
            TFT_FillRect(111,135,TFT_SIGN_WIDTH,18,BACK_COLOR);
            TFT_FillRect(111,155,TFT_SIGN_WIDTH,18,BACK_COLOR);
          }
        }
        break;
        
      // Режим регуляции
      case REGULATION_MODE:
        ModeName = (char*) RegulationModeName[Language];
        if (ISNT_OVERFLOW && ISNT_CRIT_TEMP && ISNT_ALARM_THERMOSTAT)
        {
          // Выводим "Подача ХХ сек", если не дрова
          if ((FeedTime_Counter < sSupplyTime.Value) && ISNT_FIREWOOD)
          {
            TFT_WriteParamName (111,130,S_ALIGN,&sSupplyCurrent,TEXT_COLOR,BACK_COLOR,Calibri);
            TFT_WriteParamSign (111,155,S_ALIGN,&sSupplyCurrent,TEXT_COLOR,BACK_COLOR,Calibri);
          }
          // Иначе пустое поле
          else
          {
            TFT_FillRect(111,130,TFT_SIGN_WIDTH,18,BACK_COLOR);
            TFT_FillRect(111,155,TFT_SIGN_WIDTH,18,BACK_COLOR);
          }
        }
        break;
      
      // Режим тушения
      case EXTINGUISHING_MODE:
        ModeName = (char*) ExtinguishingModeName[Language];
        if (ISNT_OVERFLOW && ISNT_CRIT_TEMP && ISNT_ALARM_THERMOSTAT)
        {
          // Выводим "Подача ХХ сек", если не дрова
          if ((FeedTime_Counter < sSupplyTime.Value) && ISNT_FIREWOOD)
          {
            TFT_WriteParamName (111,130,S_ALIGN,&sSupplyCurrent,TEXT_COLOR,BACK_COLOR,Calibri);
            TFT_WriteParamSign (111,155,S_ALIGN,&sSupplyCurrent,TEXT_COLOR,BACK_COLOR,Calibri);
          }
          // Иначе пустое поле
          else
          {
            TFT_FillRect(111,130,TFT_SIGN_WIDTH,18,BACK_COLOR);
            TFT_FillRect(111,155,TFT_SIGN_WIDTH,18,BACK_COLOR);
          }
        }
        break;
        
      // Режим ожидания
      case WAITING_MODE:
        ModeName = (char*) WaitingModeName[Language];
        if (ISNT_OVERFLOW && ISNT_CRIT_TEMP && ISNT_ALARM_THERMOSTAT)
        {
          TFT_FillRect(111,130,TFT_SIGN_WIDTH,18,BACK_COLOR);
          TFT_FillRect(111,155,TFT_SIGN_WIDTH,18,BACK_COLOR);
        }
        break;
    }
    
    // Если крит температура или аварийный термостат, то чистим
    if (IS_CRIT_TEMP && IS_ALARM_THERMOSTAT)
    {
      TFT_FillRect(111,130,TFT_SIGN_WIDTH,18,BACK_COLOR);
      TFT_FillRect(111,155,TFT_SIGN_WIDTH,18,BACK_COLOR);    
    }
    else
    // Если пересыпка
    if (IS_OVERFLOW)
    {
      TFT_WriteParamName (111,130,S_ALIGN,&sSupplyCurrent,TEXT_COLOR,BACK_COLOR,Calibri);
      TFT_WriteParamSign (111,155,S_ALIGN,&sSupplyCurrent,TEXT_COLOR,BACK_COLOR,Calibri);    
    }
    
    // Выводим режим, если не пересыпка
    if (ISNT_OVERFLOW) TFT_WriteString(0,100,C_ALIGN,HEAD_COLOR,BACK_COLOR,ModeName,Calibri);
    // Иначе выводим ВЫТАЛКИВАНИЕ ТОПЛИВА
    else TFT_WriteString(0,100,C_ALIGN,HEAD_COLOR,BACK_COLOR,(char*) ExtrusionModeName[Language],Calibri);
    
    // Раз в секунду выводим параметры
    if (IS_ADC_FLAG)
    {
      // Если режим нормальный или день, выводим температуру заданную
      if ((sDailyMode.Point == NORMAL) || (IS_DAY))
      {
        TFT_WriteParamName (5,130,S_ALIGN,&sTemperatureSet,ORANGE,BACK_COLOR,Calibri);
        TFT_WriteParamSign (5,155,S_ALIGN,&sTemperatureSet,TEXT_COLOR,BACK_COLOR,Calibri);
      }
      else
      // Если режим день/ночь и ночь, выводим ночную
      if ((sDailyMode.Point == DAYNIGHT) && (IS_NIGHT))
      {
        TFT_WriteParamName (5,130,S_ALIGN,&sTempNight,ORANGE,BACK_COLOR,Calibri);
        TFT_WriteParamSign (5,155,S_ALIGN,&sTempNight,TEXT_COLOR,BACK_COLOR,Calibri);
      }
      
      // Температура ГВС
      TFT_WriteParamName (5,180,S_ALIGN,&sTempHWS,ORANGE,BACK_COLOR,Calibri);
      TFT_WriteParamSign (5,205,S_ALIGN,&sTempHWS,TEXT_COLOR,BACK_COLOR,Calibri);    

      // Температура котла
      TFT_WriteParamSign (107,186,S_ALIGN,&sTempIn,TEXT_COLOR,BACK_COLOR,CalibriLarge);   
      
      // Мощность вентилятора
      TFT_WriteParamName (210,130,S_ALIGN,&sFanCurrent,ORANGE,BACK_COLOR,Calibri);
      TFT_WriteParamSign (210,155,S_ALIGN,&sFanCurrent,TEXT_COLOR,BACK_COLOR,Calibri);
      
      // Температура шнека
      TFT_WriteParamName (210,180,S_ALIGN,&sTempScrew,ORANGE,BACK_COLOR,Calibri);
      TFT_WriteParamSign (210,205,S_ALIGN,&sTempScrew,TEXT_COLOR,BACK_COLOR,Calibri);       
      
      ADC_FLAG_RESET;
    }
  }
}

/*******************************************************************************
  *
  * Отрисовка шаблона меню
  *
  * Рисуем линии заголовков и контур скрола
  *
*******************************************************************************/
    
void Menu_DrawTemplate(sMenu* Item)
{
  if (IS_NEED_UPDATE_HEAD)
  {    
    // Шаблон для основного меню и сервисного меню
    if ((Item == (sMenu*) &MainMenu) || 
        (Item == (sMenu*) &ServiceMenu) ||
        (Item == (sMenu*) &CheckWindow))
    {
      // Отрисовка линий разделителя
      TFT_HLineDivider(22,1,TEXT_COLOR);
      TFT_HLineDivider(44,1,TEXT_COLOR);
      
      // Отрисовка рамки скролла
      // рисуется справа от пунктов меню, 
      // положение рамки по вертикали MENU_BASE_Y, 
      // размер рамки HEIGHT - MENU_BASE_Y - 3
      // ширина рамки 9 пикселов, толщина линии 1
      // цвет TEXT_COLOR (соответствует цвету текста)
      // Рисуем, если не режим проверки
      if (Item != (sMenu*) &CheckWindow)
        TFT_Rectangle(TFT_WIDTH - R_INDENT + 5,
                      MENU_BASE_Y,
                      9,
                      TFT_HEIGHT - MENU_BASE_Y - 3,
                      1,
                      TEXT_COLOR);  
    }
    // Шаблон подменю
    else
    {      
      // Отрисовка линий разделителя
      TFT_HLineDivider(22,2,TEXT_COLOR);
      TFT_HLineDivider(44,2,TEXT_COLOR);  
      TFT_HLineDivider(67,1,TEXT_COLOR);  
      
      // Отрисовка рамки скролла
      // рисуется справа от пунктов меню, 
      // положение рамки по вертикали MENU_BASE_Y + MENU_SHIFT_Y, 
      // размер рамки HEIGHT - MENU_BASE_Y - MENU_SHIFT_Y - 3
      // ширина рамки 9 пикселов, толщина линии 1
      // цвет TEXT_COLOR (соответствует цвету текста)
      TFT_Rectangle(TFT_WIDTH - R_INDENT + 5,
                    MENU_BASE_Y + MENU_SHIFT_Y,
                    9,
                    TFT_HEIGHT - MENU_BASE_Y - MENU_SHIFT_Y - 3,
                    1,
                    TEXT_COLOR);   
    }
  }
}

/*******************************************************************************
  *
  * Отрисовка заголовков меню
  *
*******************************************************************************/

void Menu_DrawHead (sMenu* Item)
{
  // Если требуется обновление и хранитель выключен
  if (IS_NEED_UPDATE_HEAD && ISNT_SCREENSAVER)
  {
    // Заголовок для основного меню и сервисного меню
    if ((Item == (sMenu*) &MainMenu) || 
        (Item == (sMenu*) &ServiceMenu) || 
        (Item == (sMenu*) &CheckWindow))
    {
      // Выводим заголовок
      switch (Language)
      {
        case RUS: TFT_WriteString(0,24,C_ALIGN,HEAD_COLOR,NO_BACKGROUND,(*Item).MenuNameRus,Calibri); break;
        case ENG: TFT_WriteString(0,24,C_ALIGN,HEAD_COLOR,NO_BACKGROUND,(*Item).MenuNameEng,Calibri); break;
      }
      
    }
    // Заголовок для меню
    else
    {
      sMenu* ParentMenu = (sMenu*) (*Item).Parent;
      
      // Выводим заголовки
      switch (Language)
      {
        case RUS: 
          TFT_WriteString(0,24,C_ALIGN,HEAD_COLOR,NO_BACKGROUND,(*ParentMenu).MenuNameRus,Calibri); 
          TFT_WriteString(0,47,C_ALIGN,HEAD_COLOR,NO_BACKGROUND,(*Item).MenuNameRus,Calibri); 
          break;
        case ENG: 
          TFT_WriteString(0,24,C_ALIGN,HEAD_COLOR,NO_BACKGROUND,(*ParentMenu).MenuNameEng,Calibri); 
          TFT_WriteString(0,47,C_ALIGN,HEAD_COLOR,NO_BACKGROUND,(*Item).MenuNameEng,Calibri);
          break;
      }
    }
  }
}

/*******************************************************************************
  *
  * Отрисовка скролла
  *
*******************************************************************************/

void Menu_DrawScroll(sMenu *Menu)
{
  // Если необходимо обновить и хранитель выключен
  if (IS_NEED_UPDATE_SCROLL && ISNT_SCREENSAVER && (Menu != (sMenu*)&CheckWindow))
  {
    
    uint8_t PageCount = 0;
    uint16_t Scroll_Size = 0;
    uint16_t Scroll_Start = 0;
    
    // Если главное меню или сервисное меню (11 пунктов на страницу)
    if ((Menu == (sMenu*) &MainMenu) || (Menu == (sMenu*) &ServiceMenu))
    {
      // Чистим скролл
      TFT_VLine(TFT_WIDTH - R_INDENT + 8,
                MENU_SCROLL_START,
                MENU_SCROLL_FULL,
                3,
                BACK_COLOR);
      
      // Расчет высоты скролла в зависимости от количества пунктов меню (*Menu).ItemCount
      // и ограничение максимальным значением MENU_SCROLL_FULL
      Scroll_Size = (MENU_SCROLL_FULL * MENU_MAX_POINT) / (*Menu).ItemCount;
      if (Scroll_Size > MENU_SCROLL_FULL) Scroll_Size = MENU_SCROLL_FULL;
      
      // Расчет позиции скролла по вертикали в зависимости от текущей страницы (*Menu).Page
      // и числа страниц PageCount
      if (((*Menu).ItemCount - MENU_MAX_POINT) < 1) PageCount = 1;
      else PageCount = (*Menu).ItemCount - MENU_MAX_POINT;
      Scroll_Start = MENU_SCROLL_START + (*Menu).Page*(MENU_SCROLL_FULL - Scroll_Size)/(PageCount);    
    }
    // Если дочернее меню (10 пунктов на страницу)
    else
    {
      // Чистим скролл
      TFT_VLine(TFT_WIDTH - R_INDENT + 8,
                MENU_SCROLL_START_CHILD,
                MENU_SCROLL_FULL_CHILD,
                3,
                BACK_COLOR);
      
      // Расчет высоты скролла в зависимости от количества пунктов меню (*Menu).ItemCount
      // и ограничение максимальным значением MENU_SCROLL_FULL_CHILD
      Scroll_Size = (MENU_SCROLL_FULL_CHILD * ((*Menu).ItemPerPage)) / (*Menu).ItemCount;
      if (Scroll_Size > MENU_SCROLL_FULL_CHILD) Scroll_Size = MENU_SCROLL_FULL_CHILD;
      
      // Расчет позиции скролла по вертикали в зависимости от текущей страницы (*Menu).Page
      // и числа страниц PageCount
      PageCount = (*Menu).ItemCount - (*Menu).ItemPerPage;
      Scroll_Start = MENU_SCROLL_START_CHILD + (*Menu).Page*(MENU_SCROLL_FULL_CHILD - Scroll_Size)/(PageCount); 
    }
    
    // Отрисовка скролла
    // рисуется справа от пунктов меню, 
    // положение скролла по вертикали Scroll_Start, размер скролла Scroll_Size
    // ширина скролла 3 пиксела, радиус скругления 1
    // цвет TEXT_COLOR (соответствует цвету текста)
    TFT_VLine(TFT_WIDTH - R_INDENT + 8,
              Scroll_Start,
              Scroll_Size,
              3,
              TEXT_COLOR);
  }
}

/*******************************************************************************
  *
  * Отрисовка батареи
  *
  * Рассчитывается уровень заряда в процентах
  *
*******************************************************************************/

uint16_t BatteryVoltage = 0;
int BatteryLevel = 0;

void Menu_DrawBattery (uint16_t x, uint16_t y) // координаты расположения батареи
{   
  int BatteryLevel_new = 0;
  // Расчет уровня заряда в процентах
  // Границы
  // Минимальная  - 2.20 В
  // Максимальная - 2.75 В
  BatteryLevel_new = (int) (1.82 * BatteryVoltage - 400);
  // Ограничение уровня заряда
  if (BatteryLevel_new < 0)   BatteryLevel_new = 0;
  if (BatteryLevel_new > 100) BatteryLevel_new = 100;
  
  // Обновляем батарею, если изменился уровень заряда или он меньше 15%
  if ((BatteryLevel_new != BatteryLevel) || 
      (BatteryLevel_new < 15) ||
      IS_NEED_UPDATE_BAT)
  {
    BatteryLevel = BatteryLevel_new;
    
    uint16_t BatteryFillColor = 0;
    uint16_t BatterySymbolColor = 0;
      
    // Если заряд меньше 15% и не заряжаем, то мигаем красным и взвоим флаги
    if (BatteryLevel < 15)
    {
      if (IS_BLINK_BATTERY) {BatterySymbolColor = TEXT_COLOR; BatteryFillColor = RED;}
      else
      if (ISNT_BLINK_BATTERY) {BatterySymbolColor = BACK_COLOR; BatteryFillColor = BACK_COLOR;}
    }
    else
    { 
      BatterySymbolColor = TEXT_COLOR;
      
      // Если заряд больше 15% и меньше 25%, то выводим уровень красным
      if ((BatteryLevel >= 15) && (BatteryLevel < 25)) {BatteryFillColor = RED;}
      else
      // Если заряд больше 25% и меньше 40%, то выводим уровень желто-оранжевым
      if ((BatteryLevel >= 25) && (BatteryLevel < 40)) {BatteryFillColor = YELLOW_ORANGE;}
      else
      // Если заряд больше 40%, то выводим уровень зеленым
      if (BatteryLevel >= 40) {BatteryFillColor = GREEN;}
    }
    
    // Отрисовка пустого логотипа батареи Battery_Symbol
    TFT_WriteString(x,y,NO_ALIGN,BatterySymbolColor,NO_BACKGROUND," ",Battery_Symbol); 
    // Отрисовка уровня заряда красным
    Menu_FillBatteryLevel (x+6,y+4,BatteryLevel, BatteryFillColor);  
    
    NEED_UPDATE_BAT_RESET;
  }
}

/*******************************************************************************
  *
  * Заливка уровня заряда
  *
*******************************************************************************/

void Menu_FillBatteryLevel (uint16_t x, uint16_t y,  // координаты уровня
                            uint16_t Level,          // значение уровня заряда
                            uint16_t color)          // цвет заливки
{
  uint16_t BatteryLevelPoint = (uint16_t) (Level * 0.18);
  // Отрисовка уровня заряда
  TFT_FillRect(x + (18-BatteryLevelPoint),y,BatteryLevelPoint,10,color);
}