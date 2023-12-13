
#include "main.h"
#include "Menu.h"
#include "ILI9341.h"
#include "Init.h"
#include "24C_EEPROM.h"
#include "adc.h"

/*******************************************************************************
  *
  * Обработка изменения параметров
  *
*******************************************************************************/

void Menu_ProduceParameters (void)
{
  // Определение наименования устройства
  DeviceNamePoint = (char*) DeviceNameSTART[Language];
  
  // Время
  if ((TimeMenu.Item & mActive) != mActive)
  {
    // Если не изменяем время и дату в данный момент, то обновлять меню
    sHours.Value   = sTimeCurrent.Hours;
    sMinutes.Value = sTimeCurrent.Minutes;
  }
  else
  {
    // Если изменяем дату и время, то записываем в контроллер
    switch (TimeMenu.Item & ~mActive)
    {
      // Часы
      case 0: 
        sTimeCurrent.Hours = sHours.Value;
        HAL_RTC_SetTime(&hrtc, &sTimeCurrent, RTC_FORMAT_BIN);
        break;
      // Минуты
      case 1:
        sTimeCurrent.Minutes = sMinutes.Value;
        HAL_RTC_SetTime(&hrtc, &sTimeCurrent, RTC_FORMAT_BIN);
        break;
    }
  }
  
  // Мин и макс значения из сервисных настроек
  // - Заданная температура котла
  // - Мощность вентилятора
  // - Мощность вентилятора в продувах
  // - Вентилятор в ручном управлении
  if (sTemperatureSetting.Min != sTemperatureSettingMin.Value)
    sTemperatureSetting.Min = sTemperatureSettingMin.Value;
  if (sTemperatureSetting.Max != sTemperatureSettingMax.Value)
    sTemperatureSetting.Max = sTemperatureSettingMax.Value;
  
  // Основные параметры
  if (sTemperatureSetting.Value != sTemperatureSet.Value)
    sTemperatureSet.Value = sTemperatureSetting.Value;
  
  // Напряжение батареи
  // Обновляем, если в меню "Об устройстве" и значение изменилось
  if ((sBattery.Value != BatteryVoltage) & 
      (Menu_SelectedItem == (sMenu*) &DeviceMenu))
  {
    sBattery.Value = BatteryVoltage; 
    //NEED_UPDATE_SET;
  }
  
  // Меню ручного управления
  if (Menu_SelectedItem == (sMenu*) &ManualControlMenu)
  {
    // Вентилятор
    if (sFanManual.Point == 0) {Work_FanPowerSet(0);}
    else                       {Work_FanPowerSet(sFanPower.Value);}
    // Шнек
    if (sScrewManual.Point == 0) {Work_SupplyPowerSet(0);}
    else                         {Work_SupplyPowerSet(100);}
    // Циркуляционный насос отопления
    if (sCirPumpManual.Point == 0) {CIRPUMP_OFF;}
    else                           {CIRPUMP_ON;}
    
    // Насос ГВС принудительно отключаем
    HWSPUMP_OFF;
  }
  
  // Подрежимы
  if (IS_AUTO_MODE)           {FIREWOOD_OFF;}
  else                        {FIREWOOD_ON;}
  if (sThermostat.Point == 0) {THERMO_OFF;}
  else                        {THERMO_ON;}
  
  // Ночная температура
  sTempNight.Value = sTemperatureSetting.Value + sNightCorrection.Value;
  if (sTempNight.Value < sTemperatureSettingMin.Value) 
    sTempNight.Value = sTemperatureSettingMin.Value;
  if (sTempNight.Value > sTemperatureSettingMax.Value)
    sTempNight.Value = sTemperatureSettingMax.Value;
  
  // Если режим нормальный и флаг день/ночь взведен, то включаем день
  if ((IS_NIGHT) && (sDailyMode.Point == NORMAL)) DAY_SET;
  
  // Ограничение температуры бойлера ГВС, максимальное значение должно быть
  // меньше температуры котла заданной на 3 градуса в приоритете котла
  //
  // В приоритете бойлера ограничение температуры бойлера ГВС - 75 градусов
  
  if (ISNT_BOILER_PRIORITY)
  {
    sTemperatureHWS.Max = sTemperatureSetting.Value - 3;
    
    if (sTemperatureHWS.Value > sTemperatureHWS.Max) 
    {
      sTemperatureHWS.Value = sTemperatureHWS.Max;
    }
  }
  else
  if (IS_BOILER_PRIORITY)
  {
    sTemperatureHWS.Max = 75;
  }
  
  // Если температура заданная ГВС выключена, то выключаем приоритет бойлера и 
  // не даем включиться
  if (IS_OFF(sTemperatureHWS) && IS_BOILER_PRIORITY &&
      (((*Menu_SelectedItem).Item & mActive) != mActive))
  {
    sBoilerPriority.Point = OFF;
    //NEED_UPDATE_MENU_SET;
    NEED_UPDATE_SET;
  }
}

/*******************************************************************************
  *
  * Функция установки флагов кнопок
  *
*******************************************************************************/

uint16_t InButt[4] = {0,0,0,0};
uint16_t AndInButt = 0;
uint16_t OrInButt = 0;
uint16_t OldButtState, NewButtState, RepeatButtons;
uint8_t  RepeatButtons20;

void Menu_ProduceButtonFlags(void)
{
  for (uint8_t i = 0; i < 4; i++)
  {
    // Считываем состояние кнопок с портов
    uint16_t ButtReadFlag = 0;
    ButtReadFlag |= But_START_State * But_START;
    ButtReadFlag |= But_STOP_State  * But_STOP;
    ButtReadFlag |= But_MORE_State  * But_MORE;
    ButtReadFlag |= But_LESS_State  * But_LESS;
    
    // Читаем код нажатых кнопок в массив со сдвигом
    InButt[3] = InButt[2];
    InButt[2] = InButt[1];
    InButt[1] = InButt[0]; 
    InButt[0] = ButtReadFlag;
    
    // Логическое И над массивом
    AndInButt  = InButt[0];
    AndInButt &= InButt[1]; 
    AndInButt &= InButt[2];
    AndInButt &= InButt[3];
    
    // Логическое ИЛИ над массивом
    OrInButt  = InButt[0];  
    OrInButt |= InButt[1];
    OrInButt |= InButt[2];
    OrInButt |= InButt[3];
    
    // Определение нового и старого состояний кнопок  
    OldButtState = NewButtState;
    NewButtState |= AndInButt;
    NewButtState &= OrInButt;
   
    // Определение начального перехода 0->1  в состоянии кнопок
    BUTTONS = (OldButtState ^ 0x0FFFF) & NewButtState;
  }
  
  // Создание автоповторов при удерживании кнопки,
  // если все биты BUTTONS = 0, очистка RepeatButtons,
  // если кнопка нажата, то инкремент RepeatBottons
  if (NewButtState == 0x0000) 
  {
    RepeatButtons = 0; 
    RepeatButtons20 = 0;
  }
  else RepeatButtons++;
  
  // Проверка бита первого повтора кнопок Вверх и Вниз, если меню не ручное управление
  if ((((NewButtState == But_MORE) || (NewButtState == But_LESS)) ||
       ((NewButtState == But_STOP) && (Menu_SelectedItem == (sMenu*) &MainWindow))) &&
       (Menu_SelectedItem != (sMenu*) &ManualControlMenu))
  {
    if (RepeatButtons == 0x0008)
    {
      RepeatButtons = 0x1000; //взводим бит автоудерживания 
      BUTTONS = NewButtState; //первый повтор
    }
  
    // Следующие повторы
    if (RepeatButtons == 0x1001)
    { 
      RepeatButtons = 0x1000; //взводим бит автоудерживания 
      BUTTONS = NewButtState; //первый повтор
      RepeatButtons20++;
      if (RepeatButtons20 > 20) RepeatButtons20 = 20;
    }
  }
}

/*******************************************************************************
  *
  * Изменение цифровых данных (элемент активен)
  *
  * Кнопки ВВЕРХ и ВНИЗ меняют значение элемента
  * Кнопки СТАРТ и СТОП принимают значение
  *
*******************************************************************************/

uint8_t Menu_CorrectFloatUpDown(sFloat *Element, // ссылка на структуру элемента
                                sMenu *Menu)     // ссылка на структуру меню
{
  uint8_t Alarm = 0;
  int Res = (*Element).Value;
  int Act = (*Menu).Item & mActive;
  int Rac = (*Menu).Item & ~mActive;
  
  switch (BUTTONS)
  {
    case But_START: // Подтверждение
    case But_STOP:
      Act &= ~mActive;
      (*Menu).Item = Rac | Act;
      BUTTONS &= ~(But_START | But_STOP);
      break;
      
    case But_MORE: // Переход вверх
      // Если счетчик повтора больше 20, то прибавляем 10, иначе 1
      if (RepeatButtons20 >= 20) Res = (int) (Res + 10);
      else                       
      {
        if ((Element->Off & OFF_STATE) == OFF_STATE) Element->Off = OFF_SUPPORT;
        else                                         Res = (int) (Res + 1);
      }
      BUTTONS = 0;
      break;
      
    case But_LESS: // Переход вниз
      // Если счетчик повтора больше 20, то вычитаем 10, иначе 1
      if (RepeatButtons20 >= 20) Res = (int) (Res - 10);
      else                       Res = (int) (Res - 1);
      BUTTONS = 0;
      break;
  }
  
  if (Res > (*Element).Max) {Res = (*Element).Max; Alarm = ALARM_TIME;}
  if (Res < (*Element).Min) {
    Res = (*Element).Min; 
    // Если есть функция выключения, то активируем её
    if ((Element->Off & OFF_SUPPORT) == OFF_SUPPORT) 
    {
      if ((Element->Off & OFF_STATE) == OFF_STATE) Alarm = ALARM_TIME;
      Element->Off = OFF_STATE + OFF_SUPPORT;
    }
    else Alarm = ALARM_TIME; 
  }
  (*Element).Value = Res;
  return Alarm;
}

/*******************************************************************************
  *
  * Изменение перечисляемого параметра с циклическими переходами (элемент активен)
  *
  * Кнопки ВВЕРХ и ВНИЗ меняют значение элемента
  *
*******************************************************************************/

uint8_t Menu_CorrectEnumUpDown(sEnum *Element, // ссылка на структуру элемента
                               sMenu *Menu)    // ссылка на структуру меню
{
  uint8_t Alarm = 0;
  uint8_t Res = (*Element).Point;
  int Act = (*Menu).Item & ~mActive;
  
  // Если элемент имеет только состояние вкл/откл, то не переходим в редактирование,
  // а только меняем значение, иначе переходим
  if (NON_ONOFF)
  {
    switch (BUTTONS)
    {
      // Подтверждение выбора
      case But_START:
      case But_STOP:
        (*Menu).Item = Act;
        BUTTONS = 0;
        break;
      
      // Переход вверх
      case But_MORE:
        Res++;
        if (Res > (*Element).MaxPoints - 1) {Res = 0; Alarm = ALARM_TIME;}
        BUTTONS = 0;
        break;
        
      // Переход вниз
      case But_LESS:
        if (Res > 0) Res--;
        else {Res = (*Element).MaxPoints - 1; Alarm = ALARM_TIME;}
        BUTTONS = 0;
        break;
    }
  }
  else
  {
    (*Menu).Item = Act;
    if (Res == 1) Res = 0;
    else          Res = 1;    
  }
  
  (*Element).Point = Res;
  return Alarm;
}

/*******************************************************************************
  *
  * Реакция на кнопки в меню настроек
  *
*******************************************************************************/

void Menu_StartStopMoreLess(sMenu* Menu)
{ 
  if ((ISNT_ENERGY) && (ISNT_SCREENSAVER)) 
  {
    if (BUTTONS > 0) {EnergySaving_Counter = 0;ScreenSaver_Counter = 0;ScreenSaverChange_Counter = 0;}
    
    // Если редактирование не активно
    if(((*Menu).Item & mActive) != mActive)
    {
      int Act,Res,Max,Page;
      Act  = (*Menu).Item & mActive;
      Res  = (*Menu).Item & ~mActive;
      Max  = (*Menu).ItemCount;
      Page = (*Menu).Page;  
      
      // Если включена тревога шнека и пищалка, любой кнопкой отключаем пищалку
      if ((BUTTONS > 0) && (IS_OVERFLOW_BUZZER)) OVERFLOW_BUZZER_OFF;
      
      // Если только перешли в меню, то кнопки не реагируем
      if (MoreLessCounter > 314)
      {
        if (BUTTONS == 0) MoreLessCounter = 0;
      }
      else
      {
        switch (BUTTONS)
        {
          // Кнопка старт
          // активирует пункт меню
          case But_START:
            // Нет реакции в меню разработчика и производителя
            if ((Menu_SelectedItem != (sMenu*) &DesignerMenu) &&
                (Menu_SelectedItem != (sMenu*) &ManufacturerMenu) &&
                (Menu_SelectedItem != (sMenu*) &CheckWindow))
            {
              Act ^= mActive;
              Act &= mActive;
              NEED_UPDATE_SET;
            }
            if (Check_Stage != 1) Check_Stage++;
            // В меню проверки подтверждаем или пролистываем пункты
            if (Menu_SelectedItem == (sMenu*) &CheckWindow)
            {
              if ((Check_Stage == 8)  || 
                  (Check_Stage == 16) || 
                  (Check_Stage == 24) ||
                  (Check_Stage == 27))
              {
                // Очистка дисплея
                TFT_FillScreen(BACK_COLOR);
                // Взводим флаги необходимости обновления
                NEED_UPDATE_MENU_SET;
                NEED_UPDATE_HEAD_SET;
                NEED_UPDATE_LANG_SET;
                NEED_UPDATE_BAT_SET;
                
                if (Check_Stage == 27)
                {
                  Menu_SelectedItem = (sMenu*)((*Menu_SelectedItem).Parent);
                  NEED_UPDATE_SET;
                }
              }
            }
            
            BUTTONS = 0;
            // В ручном режиме включаем/отключаем шнек
            if (Menu_SelectedItem == (sMenu*) &ManualControlMenu)
            {
              Act &= ~mActive;
              if (ISNT_SCREW)    sScrewManual.Point = 1;
              else if (IS_SCREW) sScrewManual.Point = 0;
            }
            break;
          
          // Кнопка стоп
          // выходим в родительское меню
          case But_STOP:
            Act &= ~mActive;
            // Если выходим из меню ручного управления, то отключаем все нагрузки
            if (Menu_SelectedItem == (sMenu*) &ManualControlMenu)
            {
              // Выключаем нагрузки
              CIRPUMP_OFF;
              SCREW_OFF;
              FAN_OFF;
              // Мощность шнека 0%
              Work_SupplyPowerSet(0);
              // Мощность вентилятора 0%
              Work_FanPowerSet(0);
              // Сбрасываем меню в состояние выкл
              sFanManual.Point = 0;
              sScrewManual.Point = 0;
              sCirPumpManual.Point = 0;
            }

            // Если выходим из настроек 
            if (Menu_SelectedItem != (sMenu*) &CheckWindow)
            {
              // Очистка дисплея
              TFT_FillScreen(BACK_COLOR);
              
              Menu_SelectedItem = (sMenu*)((*Menu_SelectedItem).Parent);
                
              if (Menu_SelectedItem != (sMenu*) &MainWindow)
              {
                // Взводим флаги обновления
                NEED_UPDATE_SET;
                NEED_UPDATE_HEAD_SET;
                NEED_UPDATE_SCROLL_SET;
                NEED_UPDATE_BAT_SET;
              }
              else
              {
                NEED_UPDATE_BAT_SET;
                MENU_EXIT_SET;
                // Сохраняем настройки в память
                EEPROM_WriteSettings();          
              }
            }
            else
            // В меню проверки
            {
              if (Check_Stage == 1) Check_Stage++;
            }
            BUTTONS = 0;
            break;
            
          // Кнопка больше
          case But_MORE:
            // Если меню ручного управления, то включаем/отключаем вентилятор
            if (Menu_SelectedItem == (sMenu*) &ManualControlMenu)
            {
              if (ISNT_FAN)    sFanManual.Point = 1;
              else if (IS_FAN) sFanManual.Point = 0;
            }
            else
            // В меню проверки проверяется кнопка вверх
            if ((Menu_SelectedItem == (sMenu*) &CheckWindow) && (Check_Stage == 2)) Check_Stage++;
            else
            // В остальных случаях - движение по меню
            {
              // Если пункт не первый
              if (Res > 0)
              {
                // Если выбран первый пункт на текущей странице то двигаемся вверх
                // и смещаем страницу, иначе только пункт
                if (Res == Page) 
                {
                  Page--; Res--;
                  NEED_UPDATE_SCROLL_SET;
                }
                else Res--;
              }
              // Если пункт первый 
              else
              {
                // Курсор на последний пункт
                Res = Max - 1;
                if (Page == 0)
                {
                  // Если число пунктов на страницу больше одного
                  if ((*Menu).ItemPerPage > 1)
                  {
                    Page = Max - (*Menu).ItemPerPage;
                    NEED_UPDATE_SCROLL_SET;
                  }
                  // Если один пункт на страницу
                  else
                  {
                    Page = Max - (*Menu).ItemPerPage;
                    NEED_UPDATE_SCROLL_SET;
                  }
                }
                if (Page < 0) 
                {
                  Page = 0;
                  NEED_UPDATE_SCROLL_SET;
                }
              }
            }
            BUTTONS = 0;
            NEED_UPDATE_SET;
            break;
            
          // Кнопка меньше
          case But_LESS:
            // Если меню ручного управления, то включаем/отключаем насос отопления
            if (Menu_SelectedItem == (sMenu*) &ManualControlMenu)
            {
              if (ISNT_CIRPUMP)    sCirPumpManual.Point = 1;
              else if (IS_CIRPUMP) sCirPumpManual.Point = 0;
            }
            // В меню проверки проверяем кнопку
            if ((Menu_SelectedItem == (sMenu*) &CheckWindow) && (Check_Stage == 3)) Check_Stage++;
            else
            // В остальных случаях - движение по меню
            {
              // Если пункт не последний
              if (Res < Max - 1)
              {
                // Если число пунктов на страницу больше одного
                if ((*Menu).ItemPerPage > 1)
                {
                  // Если последний пункт на текущей странице то двигаемся вниз 
                  // и смещаем страницу, иначе только пункт
                  if (Res >= (*Menu).ItemPerPage - 1 + Page) 
                  {
                    Page++; Res++;
                    NEED_UPDATE_SCROLL_SET;
                  }
                  else Res++;
                }
                // Если один пункт на страницу
                else 
                {
                  Page++; Res++;
                  NEED_UPDATE_SCROLL_SET;
                }
              }
              // Если пункт последний
              else
              {
                Res = 0; Page = 0;
                NEED_UPDATE_SCROLL_SET;
              }
            }
            BUTTONS = 0;
            NEED_UPDATE_SET;
            break;
        }
      }
      
      (*Menu).Page = Page;
      (*Menu).Item = Res|Act;
    }
  }
  else
  {
    // Включение подсветки
    if ((IS_ENERGY) && (BUTTONS > 0))
    {
      TFT_LED_Set_Brightness(100);      
      ENERGY_RESET;
      
      BUTTONS = 0;
      EnergySaving_Counter = 0;
    }
    
    // Выход из хранителя
    if ((IS_SCREENSAVER) && ((BUTTONS > 0) || (WARNING_FLAGS > 0)))
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
  * Отрисовка меню числового параметра или параметра перечисления
  *
  * Рисует и редактирует параметр 
  * *Dig  - числовой параметр (0 - если не используется)
  * *Enum - параметр перечисления (0 - если не используется)
  *
*******************************************************************************/

void TFT_WriteMenuControl (uint16_t x, uint16_t y,  // координаты пункта меню
                           sMenu *Menu,             // ссылка на структуру меню управления
                           uint8_t MenuItem,        // номер элемента меню, соотв. выбору элемента
                           sFloat *Dig,             // ссылка на структуру числового элемента (0 - если не исп.)
                           sEnum  *Enum,            // ссылка на структуру элемента перечисления (0 - если не исп.)
                           uint16_t Color,          // цвет символов пункта меню
                           uint32_t Background,     // цвет фона пункта меню
                           uint16_t AlarmColor,     // цвет предупреждений (выход параметра за границу)
                           sFont font)              // ссылка на структуру шрифта
{
  // Отображается, если положение пунктов меню попадает на страницу
  uint8_t Shift;
  if ((Menu == (sMenu*)&MainMenu) ||
      (Menu == (sMenu*)&ServiceMenu)) Shift = MENU_BASE_Y + 0*MENU_SHIFT_Y;
  else                                Shift = MENU_BASE_Y + 1*MENU_SHIFT_Y;
  
  if ((y >= Shift) && (y <= MENU_BASE_Y + MENU_MAX_POINT*MENU_SHIFT_Y - 10))
  {
    while(IS_SPI_DMA_BUSY) {}
    
    char *str;
    uint16_t x0 = 0;
    
    // Выбор наименования числового элемента или элемента перечисления с учетом языка
    if (Enum != 0) str = (char*) Enum->Name[Language];
    else
    if (Dig != 0) str = (char*) Dig->Name[Language];
    
    // Заливка фона и вывод наименования параметра
    if (((*Menu).Item & ~mActive) != MenuItem) // выбран другой элемент, заливка прямыми цветами
    {
      if (((((*Menu).Item & mActive) != mActive) && IS_NEED_UPDATE) || IS_NEED_UPDATE_LANG)
      {
        // Вывод наименования
        x0 += TFT_WriteString(0,y,L_ALIGN,Color,NO_BACKGROUND,str,font);
      }
    }
    else
    if ((*Menu).Item == MenuItem) // элемент выбран, но не активен (замена цветов)
    {
      // Вывод наименования
      x0 += TFT_WriteString(x,y,L_ALIGN,Background,Color,str,font);
    }
    else
    if ((*Menu).Item == MenuItem + mActive) //элемент активен. Редактирование 
    {
      // Вывод наименования
      x0 += TFT_WriteString(x,y,L_ALIGN,Color,NO_BACKGROUND,str,font);
    }
    
    // Вывод параметра

    if (Enum != 0) TFT_WriteMenuEnumControl (x0,y,RIS_ALIGN,Menu,MenuItem,Enum,Color,Background,AlarmColor,font);
    else
    if (Dig != 0) TFT_WriteMenuDigitalControl (x0,y,RI_ALIGN,Menu,MenuItem,Dig,Color,Background,AlarmColor,font);
  }
}


/*******************************************************************************
  *
  * Отрисовка пункта меню со вложением
  *
  * Рисует название вложенного меню *Str
  *
*******************************************************************************/

void TFT_WriteMenuString (uint16_t x, int y,       // координаты пункта меню
                          sMenu *Menu,            // ссылка на структуру текущего меню управления
                          sMenu *ChildMenu,       // ссылка на дочернее меню
                          uint8_t MenuItem,       // номер элемента меню, соотв. выбору элемента
                          char *Str,              // ссылка на структуру строкового элемента
                          void ConfirmVoid(void), // функция, вызываемая по подтверждению
                          uint16_t Color,         // цвет символов пункта меню
                          uint32_t Background,    // цвет фона пункта меню
                          sFont font)             // ссылка на структуру шрифта
{
  // Отображается, если положение пунктов меню попадает на страницу
  uint8_t Shift;
  if ((Menu == (sMenu*)&MainMenu) ||
      (Menu == (sMenu*)&ServiceMenu) ||
      (Menu == (sMenu*)&CheckWindow)) Shift = MENU_BASE_Y + 0*MENU_SHIFT_Y;
  else                                Shift = MENU_BASE_Y + 1*MENU_SHIFT_Y;
  if (y < 0) y = 0;
  
  // Отображается, если положение пунктов меню попадает на страницу
  if ((y >= Shift) && (y <= MENU_BASE_Y + MENU_MAX_POINT*MENU_SHIFT_Y - 10))
  {
    while(IS_SPI_DMA_BUSY) {}
    
    uint8_t x0 = 0;
    uint16_t dot_width = TFT_WIDTH - R_INDENT - TFT_CalcStringWidth(Str,font) - font.Space;
    // Заливка фона и вывод наименования параметра
    if (((*Menu).Item & ~mActive) != MenuItem) // выбран другой элемент, заливка прямыми цветами
    {
      if (((((*Menu).Item & mActive) != mActive) && IS_NEED_UPDATE) || IS_NEED_UPDATE_LANG)
      {      
        // Вывод наименования
        x0 += TFT_WriteString(x,y,L_ALIGN,Color,NO_BACKGROUND,Str,font);
        // Вывод пунктира до конца строки
        TFT_HDotLine(0,dot_width,dot_width,2,Color,Background);
        // Устанавливаем поле вывода
        TFT_ILI9341_SetAddrWindow(x0,y,TFT_WIDTH-R_INDENT-1,y+font.Height);
        // Передаем буффер
        TFT_SendBuffer(TFT_BUF,dot_width*2*font.Height);        
      }
    }
    else
    if ((*Menu).Item == MenuItem) // элемент выбран, но не активен (замена цветов)
    {
      // Вывод наименования
      x0 += TFT_WriteString(x,y,L_ALIGN,Background,Color,Str,font);
      // Вывод пунктира до конца строки
      TFT_HDotLine(0,dot_width,dot_width,2,Background,Color);
      // Устанавливаем поле вывода
      TFT_ILI9341_SetAddrWindow(x0,y,TFT_WIDTH-R_INDENT-1,y+font.Height);
      // Передаем буффер
      TFT_SendBuffer(TFT_BUF,dot_width*2*font.Height);
    }
    else
    if ((*Menu).Item == MenuItem + mActive) //элемент активен. Редактирование 
    {
      (*Menu).Item &= ~mActive; // убрать курсор
      // Если функция, вызываемая по подтверждению, не пустая
      if (ConfirmVoid)
      {
        ConfirmVoid();
      }
      // Переход в дочернее меню и обновление флагов, если не пустое
      if (ChildMenu != &NULL_ENTRY) 
      {
        Menu_SelectedItem = (sMenu*) ChildMenu;
        TFT_FillScreen(Background);
        NEED_UPDATE_MENU_SET;
        NEED_UPDATE_BAT_SET;
      }
    }
  }  
}

/*******************************************************************************
  *
  * Отрисовка пункта меню со вложением и отображением текущего числового 
  * параметра
  *
  * Рисует название вложенного меню и текущее состояние параметра
  *
*******************************************************************************/

void TFT_WriteMenuStringNested (uint16_t x, uint16_t y, // координаты пункта меню
                                sMenu *Menu,            // ссылка на структуру меню
                                sMenu *ChildMenu,       // ссылка на структуру дочернего меню
                                uint8_t MenuItem,       // номер элемента меню, соотв. выбору элемента
                                sEnum *Enum,            // ссылка на структуру элемента перечисления 
                                char  *str,             // указатель на текст пункта 
                                uint16_t color,         // цвет символов пункта меню
                                uint32_t background,    // цвет фона
                                sFont font)             // ссылка на шрифт 
{
  // Отображается, если положение пунктов меню попадает на страницу
  uint8_t Shift;
  if (Menu == (sMenu*)&MainMenu) Shift = MENU_BASE_Y + 0*MENU_SHIFT_Y;
  else                           Shift = MENU_BASE_Y + 1*MENU_SHIFT_Y;
  
  // Отображается, если положение пунктов меню попадает на страницу
  if ((y >= Shift) && (y <= MENU_BASE_Y + MENU_MAX_POINT*MENU_SHIFT_Y))
  {
    uint8_t x0 = 0;
    uint16_t x_indent = 0;
    
    // Заливка фона и вывод наименования параметра
    if (((*Menu).Item & ~mActive) != MenuItem) // выбран другой элемент, заливка прямыми цветами
    {
      if ((((*Menu).Item & mActive) != mActive) && (IS_NEED_UPDATE)) 
      {   
        // Вывод состояния
        x_indent = TFT_WriteString(x+font.Space,y,RIS_ALIGN,color,background,(char*) (*Enum).Value[Language][(*Enum).Point],font);
        // Вывод наименования
        x0 += TFT_WriteString(x,y,NO_ALIGN,color,NO_BACKGROUND,str,font);
        // Вывод пунктира до конца строки
        TFT_HDotLine(x0,y,TFT_WIDTH-R_INDENT-x0-x_indent+2,2,color,background);   
      }
    }
    else
    if ((*Menu).Item == MenuItem) // элемент выбран, но не активен (замена цветов)
    {
      // Вывод состояния
      x_indent = TFT_WriteString(x+font.Space,y,RIS_ALIGN,background,color,(char*) (*Enum).Value[Language][(*Enum).Point],font);
      // Вывод наименования
      x0 += TFT_WriteString(x,y,NO_ALIGN,background,color,str,font);
      // Вывод пунктира до конца строки
      TFT_HDotLine(x0,y,TFT_WIDTH-R_INDENT-x0-x_indent+2,2,background,color);
    }
    else
    if ((*Menu).Item == MenuItem + mActive) //элемент активен. Редактирование 
    {
      (*Menu).Item &= ~mActive; // убрать курсор
      // Переход в дочернее меню
      Menu_SelectedItem = (sMenu*) ChildMenu;
      // Очистка дисплея
      TFT_FillScreen(BACK_COLOR);
      // Взводим флаг необходимости обновления
      NEED_UPDATE_MENU_SET;
    }    
  }
}

/*******************************************************************************
  *
  * Отрисовка пункта меню с текстовым параметром без действия по нажатию
  *
*******************************************************************************/

void TFT_WriteMenuNullText (uint8_t x, uint8_t y,       // координаты текста
                            sMenu *Menu,                // ссылка на структуру текущего меню
                            uint8_t MenuItem,           // номер элемента меню, соотв. выбору элемента
                            char *TextName,             // указатель на наименование пункта
                            char *Text,                 // указатель на строку
                            uint16_t Color,             // цвет символов пункта меню
                            uint32_t Background,        // цвет фона пункта меню
                            sFont font)                 // ссылка на структуру шрифта
{
  // Отображается, если положение пунктов меню попадает на страницу
  uint8_t Shift;
  if ((Menu == (sMenu*)&MainMenu) ||
      (Menu == (sMenu*)&ServiceMenu) ||
      (Menu == (sMenu*)&CheckWindow))   Shift = MENU_BASE_Y + 0*MENU_SHIFT_Y;
  else                                  Shift = MENU_BASE_Y + 1*MENU_SHIFT_Y;
  
  if ((y >= Shift) && (y <= MENU_BASE_Y + MENU_MAX_POINT*MENU_SHIFT_Y - 10))
  {
    if (IS_NEED_UPDATE)
    {
      while(IS_SPI_DMA_BUSY) {}
      
      uint8_t x0 = 0;
      
      // Вывод наименования
      x0 += TFT_WriteString(x,y,L_ALIGN,Color,NO_BACKGROUND,TextName,font);
      
      uint16_t str_width = TFT_CalcStringWidth(Text,font) + font.Space;
      uint16_t dot_width = TFT_WIDTH - R_INDENT - str_width - x0;
      uint16_t buf_size = str_width + dot_width;
      
      // Вывод пунктира до конца строки
      TFT_HDotLine(0,buf_size,dot_width,2,Color,NO_BACKGROUND);   
      
      // Вывод текста
      if (Text != 0) TFT_PutString(dot_width,y,RIS_ALIGN,Color,NO_BACKGROUND,Text,font);    
      
      // Устанавливаем поле вывода
      TFT_ILI9341_SetAddrWindow(x0,y,TFT_WIDTH-R_INDENT-1,y+font.Height);

      // Передаем буффер
      TFT_SendBuffer(TFT_BUF,buf_size*2*font.Height);
    }
    
    // Если элемент активен, то убираем курсор
    if ((*Menu).Item == MenuItem + mActive) (*Menu).Item &= ~mActive;
  }
}

/*******************************************************************************
  *
  * Отрисовка изображения с выравниванием
  *
*******************************************************************************/

void TFT_WriteMenuImage (uint8_t x, uint8_t y,  // координаты изображения
                         uint8_t align,         // выравнивание по ширине
                         sMenu *Menu,           // ссылка на структуру меню
                         uint8_t MenuItem,      // номер элемента меню, соотв. выбору элемента
                         sImage *img)           // ссылка на структуру изображения
{
  // Ждем, пока DMA закончит передачу
  while(IS_SPI_DMA_BUSY) {}
  
  uint16_t img_width = (*img).Width;
  // выравнивание изображения по ширине
  switch (align)
  {
    case L_ALIGN:  x = 0; break;
    case R_ALIGN:  x = TFT_WIDTH - img_width; break;
    case C_ALIGN:  x = (TFT_WIDTH - img_width)/2; break;
    case RI_ALIGN: x = (TFT_WIDTH - R_INDENT) - img_width; break;
    case CI_ALIGN: x = (TFT_WIDTH - img_width)/2 - R_INDENT; break;
  }
  // Вывод изображения
  TFT_DrawImage(x, y, 0, img);
  
  // Если элемент активен, то убираем курсор
  if ((*Menu).Item == MenuItem + mActive) (*Menu).Item &= ~mActive;
}

/*******************************************************************************
  *
  * Отрисовка загрузочной линейки
  *
*******************************************************************************/

void Menu_DrawLoadBar (void)
{
  // Если не хранитель экрана
  if (ISNT_SCREENSAVER)
  {
    // Ждем, пока DMA закончит передачу
    while(IS_SPI_DMA_BUSY) {}
    
    if (Menu_SelectedItem == (sMenu*) &MainWindow)
    {
      switch (BUTTONS)
      {
        // Если удерживаются БОЛЬШИЕ И МЕНЬШЕ, то рисуем шкалу загрузки для
        // перехода в меню настроек
        case (But_LESS + But_MORE):
        case (But_LESS):
        case (But_MORE):
          if (MoreLessCounter > 0)
          {
            TFT_Rectangle(1,225,318,14,1,TEXT_COLOR);
            TFT_FillRect(3,227,MoreLessCounter,11,GREEN);
          }
          break;
        // В других случаях очищаем шкалу загрузки, если она присутствует
        default:
          if (MoreLessCounter > 0) 
          {
            TFT_Rectangle(1,225,318,14,1,BACK_COLOR);
            TFT_FillRect(3,227,MoreLessCounter,11,BACK_COLOR);
            MoreLessCounter = 0;
          }
          break;
      }
    }
  }
}

/*******************************************************************************
  *
  * Заливка фоном в предупреждениях и вывод предупреждений
  *
*******************************************************************************/

const char AlarmThermostatTitle[2][25] = {"         УГРОЗА         ","         DANGER         "};
const char CritTempTitle[2][20] =  {" КРИТ. ТЕМПЕРАТУРА ", " CRIT. TEMPERATURE "};
const char ScrewTempTitle[2][19] = {" ТЕМПЕРАТУРА ШНЕКА", " SCREW TEMPERATURE"};
const char HighTempTitle[2][30] =  {"          ПЕРЕГРЕВ           ","         OVERHEAT          "};
const char HWSTempTitle[2][30] =   {"        ПЕРЕГРЕВ ГВС         ","       HWS OVERHEAT        "};
const char LowTempTitle[2][21] =   {" НИЗКАЯ ТЕМПЕРАТУРА ","  LOW TEMPERATURE  "}; 

const char TempSensorTitle[2][22]  = {"ПОВРЕЖ. ДАТЧ. КОТЛА","BOILER SENS IS BROKEN"};
const char ScrewSensorTitle[2][21] = {"ПОВРЕЖ. ДАТЧ. ШНЕКА","SCREW SENS IS BROKEN"};
const char HWSSensorTitle[2][21]   = {" ПОВРЕЖ. ДАТЧ. ГВС "," HWS SENS IS BROKEN "};

void Menu_AlarmBackground (void)
{
  // Если хранитель экрана выключен
  if (ISNT_SCREENSAVER)
  {
    // Если сработал аварийный термостат, то пишем УГРОЗА
    if (IS_ALARM_THERMOSTAT)
    {
      // Если фон не красный, заливаем
      if (BACK_COLOR != RED)
      {
        BACK_COLOR = RED;
        TFT_FillScreen(BACK_COLOR);
        NEED_UPDATE_BAT_SET;
        ADC_FLAG_SET;
      }   
      
      TFT_WriteString(0,72,C_ALIGN,TEXT_COLOR,BACK_COLOR,(char*) AlarmThermostatTitle[Language],Calibri);
    }
    else
    // Если температура выше 90 градусов, заливаем фон красным
    if (IS_CRIT_TEMP)
    {
      // Если фон не красный, заливаем
      if (BACK_COLOR != RED)
      {
        BACK_COLOR = RED;
        TFT_FillScreen(BACK_COLOR);
        NEED_UPDATE_BAT_SET;
        ADC_FLAG_SET;
      }
      
      TFT_WriteString(0,72,C_ALIGN,TEXT_COLOR,BACK_COLOR,(char*) CritTempTitle[Language],Calibri);
    }
    else
    // Если температура шнека больше температуры тревоги шнека или температура котла
    // от 80 до 90 градусов или температура ГВС выше 80, заливаем желтым
    if ((IS_SCREW_TEMP) || (IS_HIGH_TEMP) || (IS_HWS_TEMP))
    {
      if (BACK_COLOR != LIGHT_ORANGE)
      {
        BACK_COLOR = LIGHT_ORANGE;
        TFT_FillScreen(BACK_COLOR);
        NEED_UPDATE_BAT_SET;
        ADC_FLAG_SET;
      }
      
      if (IS_SCREW_TEMP) TFT_WriteString(0,72,C_ALIGN,RED,BACK_COLOR,(char*) ScrewTempTitle[Language],Calibri);
      else
      if (IS_HIGH_TEMP) TFT_WriteString(0,72,C_ALIGN,RED,BACK_COLOR,(char*) HighTempTitle[Language],Calibri);
      else
      if (IS_HWS_TEMP) TFT_WriteString(0,72,C_ALIGN,RED,BACK_COLOR,(char*) HWSTempTitle[Language],Calibri);
    }
    else
    // Если температура шнека меньше 5 градусов, заливаем синим
    if (IS_LOW_TEMP)
    {
      if (BACK_COLOR != DARK_BLUE)
      {
        BACK_COLOR = DARK_BLUE;
        TFT_FillScreen(BACK_COLOR);
        NEED_UPDATE_BAT_SET;
        ADC_FLAG_SET;
        
        TFT_WriteString(0,72,C_ALIGN,RED,BACK_COLOR,(char*) LowTempTitle[Language],Calibri);
      }
    }
    else
    // В остальных случаях фон черный и стираем предупреждение
    {
      if (BACK_COLOR != BLACK)
      {
        BACK_COLOR = BLACK;
        TFT_FillScreen(BACK_COLOR);
        NEED_UPDATE_BAT_SET;
        ADC_FLAG_SET;
      }
      TFT_FillRect(100,72,120,18,BACK_COLOR);
    }
    
    // Если датчик котла поврежден
    if (IS_TEMP_SENSOR)
    {
      TFT_WriteString(0,30,C_ALIGN,ORANGE,BACK_COLOR,(char*) TempSensorTitle[Language],Calibri);
    }
    else
    // Если датчик шнека поврежден
    if (IS_SCREW_SENSOR)
    { 
      TFT_WriteString(0,30,C_ALIGN,ORANGE,BACK_COLOR,(char*) ScrewSensorTitle[Language],Calibri);
    }
    else
    // Если датчик ГВС поврежден
    if (IS_HWS_SENSOR)
    {   
      TFT_WriteString(0,30,C_ALIGN,ORANGE,BACK_COLOR,(char*) HWSSensorTitle[Language],Calibri);    
    }
    else
    // Иначе стираем
    {
      TFT_FillRect(45,30,240,18,BACK_COLOR);
    }
  }
}

/*******************************************************************************
  *
  * Отрисовка хранителя экрана
  *
*******************************************************************************/

uint8_t ScreenSaverChanged = 0;

void Menu_ScreenSaver(void)
{
  if (IS_SCREENSAVER && ((ScreenSaverChange_Counter % 5) == 0) && (WARNING_FLAGS == 0))
  {
    if (ScreenSaverChanged == 0)
    {
      TFT_FillScreen(BACK_COLOR);
      
      uint16_t x0 = 0;
      uint16_t y0 = 0;
      switch (ScreenSaverChange_Counter)
      {
        case 0:  x0 = 5;   y0 = 20;  break;
        case 5:  x0 = 107; y0 = 180; break; 
        case 10: x0 = 205; y0 = 70;  break; 
        case 15: x0 = 5;   y0 = 120; break;
        case 20: x0 = 107; y0 = 20;  break; 
        case 25: x0 = 205; y0 = 180; break; 
        case 30: x0 = 5;   y0 = 70;  break; 
        case 35: x0 = 107; y0 = 120; break; 
        case 40: x0 = 205; y0 = 20;  break; 
        case 45: x0 = 7;   y0 = 180; break;
        case 50: x0 = 107; y0 = 70;  break; 
        case 55: x0 = 205; y0 = 120; break; 
        case 60: x0 = 7;   y0 = 20;  break; 
      }
      TFT_WriteParamSign (x0, y0, S_ALIGN,&sTempIn,TEXT_COLOR,BACK_COLOR,CalibriLarge);
      
      ScreenSaverChanged = 1;
    }
  }
  else ScreenSaverChanged = 0;
}

/*******************************************************************************
  *
  * Сброс настроек
  *
*******************************************************************************/

void Menu_ResetSettings(void)
{
  // Основные настройки
  sMode.Point                           = 0;   // Режим - автоматический
  sDailyMode.Point                      = 0;   // Суточный режим - нормальный
  sTemperatureSetting.Value             = 60;  // Заданная температура котла
  sTemperatureSetting.Off               = 0;
  sSupplyTime.Value                     = 10;  // Время подачи
  sSupplyTime.Off                       = 0;
  sSupplyPeriod.Value                   = 15;  // Период подач
  sSupplyPeriod.Off                     = 0;
  sFanPower.Value                       = 40;  // Мощность вентилятора
  sFanPower.Off                         = 0;
  sSupplyRepetition.Value               = 2;   // Повторение подач
  sSupplyRepetition.Off                 = 0;
  sBlowingTime.Value                    = 15;  // Время продува
  sBlowingTime.Off                      = OFF_SUPPORT;    
  sBlowingPeriod.Value                  = 2;   // Период продувов
  sBlowingPeriod.Off                    = 0;
  sFanPowerBlowing.Value                = 50;  // Мощность вентилятора в продувах
  sFanPowerBlowing.Off                  = 0;
  sFanPowerFirewood.Value               = 40;  // Мощность вентилятора на дровах
  sFanPowerFirewood.Off                 = 0;
  sTemperatureHWS.Value = sTemperatureHWS.Min; // Заданная температура ГВС
  sTemperatureHWS.Off                   = OFF_SUPPORT + OFF_STATE;
  sBoilerPriority.Point                 = 0;   // Приоритет бойлера - откл
  sNightCorrection.Value                = 5;   // Ночная коррекция
  sNightCorrection.Off                  = 0;
  sThermostat.Point                     = 0;   // Комнатный термостат - откл
  
  // Сервисные настройки
  sTemperatureSettingMin.Value          = 40;  // Мин. температура котла
  sTemperatureSettingMin.Off            = 0;
  sTemperatureSettingMax.Value          = 80;  // Макс. температура котла
  sTemperatureSettingMax.Off            = 0;
  sHysteresisBoilerSensor.Value         = 1;   // Гистерезис датчика котла
  sHysteresisBoilerSensor.Off           = 0;
  sHysteresisHWSSensor.Value            = 3;   // Гистерезис датчика ГВС
  sHysteresisHWSSensor.Off              = 0;
  sFanPowerMin.Value                    = 25;  // Минимальная мощность вентилятора
  sFanPowerMin.Off                      = 0;
  sFanPowerMax.Value                    = 55;  // Максимальная мощность вентилятора
  sFanPowerMax.Off                      = 0;
  sFuelLackTime.Value                   = 30;  // Время обнаружения нехватки топлива
  sFuelLackTime.Off                     = OFF_SUPPORT;   
  sTemperatureHeatingPumpStart.Value    = 40;  // Температура включения насоса отопления
  sTemperatureHeatingPumpStart.Off      = OFF_SUPPORT;
  sTimePumpOff.Value                    = 3;   // Время отключения насоса
  sTimePumpOff.Off                      = 0;
  sTimePumpOn.Value                     = 30;  // Время включения насоса
  sTimePumpOn.Off                       = 0;
  sTemperatureOff.Value                 = 30;  // Температура выключения блока
  sTemperatureOff.Off                   = 0;
  sTemperatureScrewAlarm.Value          = 70;  // Температура тревоги шнека
  sTemperatureScrewAlarm.Off            = OFF_SUPPORT;  
  sTimeTransfer.Value                   = 10;  // Время пересыпки
  sTimeTransfer.Off                     = 0;
  sNightPeriodStartTime.Value           = 22;  // Начало ночного периода
  sNightPeriodStartTime.Off             = 0;   
  sDayPeriodStartTime.Value             = 6;   // Начало дневного периода
  sDayPeriodStartTime.Off               = 0;
  sEnergySaving.Value                   = sEnergySaving.Min;   // Энергосбережение
  sEnergySaving.Off                     = OFF_SUPPORT + OFF_STATE;
  sScreenSaver.Value                    = sScreenSaver.Min;    // Хранитель экрана
  sScreenSaver.Off                      = OFF_SUPPORT + OFF_STATE;
}

/*******************************************************************************
  *
  * Структуры работы с числовыми параметрами
  *
*******************************************************************************/

const char UnitSecName[2][4]  = {"сек","sec"}; // секунды
const char UnitMinName[2][4]  = {"мин","min"}; // минуты
const char UnitHourName[2][3] = {"ч","hr"};    // часы
const char UnitPercName[2][2] = {"%",  "%"};   // проценты
const char UnitDegName[2][2]  = {"C",  "C"};   // градусы цельсия
const char UnitkHzName[2][4]  = {"кГц","kHz"}; // килогерцы
const char UnitHzName[2][3]   = {"Гц","Hz"};   // герцы
const char UnitVoltName[2][2] = {"В",  "V"};   // вольты
const char UnitNullName[2][2] = {0,    0};     // нулевая единица измерения