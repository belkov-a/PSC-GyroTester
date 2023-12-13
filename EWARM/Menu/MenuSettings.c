#include "main.h"
#include "Menu.h"
#include "ILI9341.h"
#include "Init.h"
#include "adc.h"

/*******************************************************************************
  *
  * Структуры работы с меню
  *
*******************************************************************************/

// Пустое меню
const char NullMenuName[2] = " ";
sMenu NullMenu = {(void*) 0,(char*) NullMenuName,(char*) NullMenuName, 0, 0, 0, 0};

// Текущий пункт меню
sMenu* Menu_SelectedItem;

#define MENU_MAKE(Name, Parent, MenuNameRus, MenuNameEng, Item, ItemCount, Page, ItemPerPage) \
	extern sMenu Parent;   \
          sMenu Name = {(void*)&Parent, (char*) MenuNameRus, (char*) MenuNameEng, \
                        (uint32_t)Item, (uint32_t)ItemCount, (uint32_t)Page,(uint32_t)ItemPerPage};

const char CheckWindowName[2][20] = {"Проверка устройства","Device check"};
const char MainMenuName[2][19] = {"Основные настройки","Main settings"};
const char ManualControlMenuName[2][18] = {"Ручное управление","Manual control"};
const char ServiceMenuName[2][20] = {"Сервисные настройки","Service settings"};
const char TimeMenuName[2][14] = {"Текущее время","Current time"};
const char ManufacturerMenuName[2][22] = {"О производителе котла","Bolier manufacturer"};
const char DesignerMenuName[2][20] = {"О разработчике","Controller designer"};
const char DeviceMenuName[2][14] = {"Об устройстве","Device data"};
const char ResetSettingsMenuName[2][15] = {"Сброс настроек","Reset settings"};
                        
MENU_MAKE(CheckWindow,StartWindow,(char*) CheckWindowName[0], (char*)CheckWindowName[1],0,1,0,1);
MENU_MAKE(LicenseWindow,LicenseWindow,(char*) NullMenuName,(char*) NullMenuName,0,0,0,0);
MENU_MAKE(StartWindow,NullMenu,(char*) NullMenuName,(char*) NullMenuName,0,0,0,0);
MENU_MAKE(MainWindow,MainMenu,(char*) NullMenuName,(char*) NullMenuName,0,0,0,0);
MENU_MAKE(MainMenu,MainWindow,(char*) MainMenuName[0],(char*) MainMenuName[1],0,19,0,9);
MENU_MAKE(ManualControlMenu,MainMenu,(char*) ManualControlMenuName[0],(char*) ManualControlMenuName[1],0,3,0,3);
MENU_MAKE(ServiceMenu,MainWindow,(char*) ServiceMenuName[0],(char*) ServiceMenuName[1],0,20,0,9);
MENU_MAKE(TimeMenu,ServiceMenu,(char*) TimeMenuName[0],(char*) TimeMenuName[1],0,2,0,2);
MENU_MAKE(ManufacturerMenu,MainMenu,(char*) ManufacturerMenuName[0],(char*) ManufacturerMenuName[1],0,1,0,1);
MENU_MAKE(DesignerMenu,MainMenu,(char*) DesignerMenuName[0],(char*) DesignerMenuName[1],0,1,0,1);
MENU_MAKE(DeviceMenu,MainMenu,(char*) DeviceMenuName[0],(char*) DeviceMenuName[1],0,1,0,1);
MENU_MAKE(ResetSettingsMenu,ServiceMenu,(char*) ResetSettingsMenuName[0],(char*) ResetSettingsMenuName[1],0,2,0,2);

/*******************************************************************************
  *
  * Отрисовка меню настроек
  *
*******************************************************************************/

const char DevInfoText1[2][21] = {"''Актив-Компонент''","Aktiv-Komponent ltd."};
const char DevInfoText2[2][16] = {"620039, Россия,","620039, Russia,"};
const char DevInfoText3[2][17] = {"г. Екатеринбург,","Ekaterinburg,"};
const char DevInfoText4[2][24] = {"ул. Донбасская, 24, 4","Donbasskaya str., 24, 4"};
const char DevInfoText5[2][22] = {"Email: psc@olympus.ru","Email: psc@olympus.ru"};
const char DevInfoText6[2][22] = {"тел: +7(902)444-59-90","tel: +7(902)444-59-90"};
const char DevInfoText7[2][16] = {"www.termokub.ru","www.termokub.ru"};

#ifdef PSC_START_DEVICE
const char ManInfoText1[2][19] = {"Отопительные котлы","Heating boliers"};
const char ManInfoText2[2][10] = {"''СТАРТ''","''START''"};
const char ManInfoText3[2][21] = {"тел: 8 800 550-09-22","tel: 8 800 550-09-22"};
const char ManInfoText4[2][15] = {"www.kotel74.ru","www.kotel74.ru"};
#endif

#ifdef PSC_PELLETOR_DEVICE
const char ManInfoPelletorText1[2][16] = {"Пеллетные котлы","Pellet boilers"};
const char ManInfoPelletorText2[2][9] = {"PELLETOR","PELLETOR"};
const char ManInfoPelletorText3[2][21] = {"тел: 8 800 700-62-21","tel: 8 800 700-62-21"};
const char ManInfoPelletorText4[2][16] = {"www.pelletor.ru","www.pelletor.ru"};
#endif

const char PSC_ABPName[2][11] = {"Контроллер","Controller"};
const char SoftVerName[2][17] = {"Версия ПО","Software version"}; 
const char SoftDateName[2][18] = {"Дата ПО","Software date"};
const char SerialNumName[2][15] = {"Серийный номер","Serial number"};
const char ManufactDateName[2][18] = {"Дата производства","Manufacture date"};
const char NoSettings[2][9] = {"Отменить","Cancel."};
const char YesSettings[2][12] = {"Подтвердить","Confirm."};

uint8_t Check_Stage = 0;

const char OkSettings[2][3] = {"OK","OK"};
const char StartConfirmText[2][26] = {"ПУСК для завершения",  "     START to finish     "};
const char StartSkipText[2][19] =    {"ПУСК к след пункту",   "START to next item"};
const char CheckText1[2][14] = {"Нажмите ПУСК","Press START"};
const char CheckText2[2][13] = {"Нажмите СТОП","Press STOP"};
const char CheckText3[2][14] = {"Нажмите ВВЕРХ","Press UP"};
const char CheckText4[2][13] = {"Нажмите ВНИЗ","Press DOWN"};
const char CheckText5[2][14] = {"Шнек включен","Screw is on"};
const char CheckText6[2][20] = {"Вентилятор включен","Fan is on"};
const char CheckText7[2][20] = {"Насос отопл включен","Heat pump is on"};
const char CheckText8[2][19] = {"Насос ГВС включен","HWS pump is on"};
const char CheckText9[2][17] = {"Авария включена","Alarm is on"};
const char CheckText10[2][20] = {"Звук сигнал включен","Sound signal is on"};
const char CheckText11[2][23] = {"Замк авар термост","Close alarm thermostat"};
const char CheckText12[2][22] = {"Разомк авар термост","Open alarm thermostat"};
const char CheckText13[2][22] = {"Замк комн термост","Close room thermostat"};
const char CheckText14[2][21] = {"Разомк комн термост","Open room thermostat"};
const char CheckText15[2][20] = {"Замк внешний стоп","Close external stop"};
const char CheckText16[2][21] = {"Разомк внешний стоп","Open external stop"};
const char CheckText17[2][22] = {"Замк датчик котла","Close boiler sensor"};
const char CheckText18[2][21] = {"Отсоед датчик котла","Discon boiler sensor"};
const char CheckText19[2][20] = {"Замк датчик ГВС","Close HWS sensor"};
const char CheckText20[2][18] = {"Отсоед датчик ГВС","Discon HWS sensor"};
const char CheckText21[2][22] = {"Замк датчик шнека","Close screw sensor"};
const char CheckText22[2][20] = {"Отсоед датчик шнека","Discon screw sensor"};
const char CheckText23[2][10] = {"Частота","Frequency"};
const char CheckText24[2][20] = {"Завершить проверку?","Finish checking?"};

uint8_t Menu_DrawSettings(sMenu* Item)
{
  // Если хранитель выключен
  if (ISNT_SCREENSAVER)
  {
    // Главное меню
    if (Item == (sMenu*) &MainMenu)
    {
      // 0. Заданная температура котла
      TFT_WriteMenuControl (0, MENU_POSITION(0), 
                            Menu_SelectedItem, 0,
                            &sTemperatureSetting,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri);   
      // 1. Режим работы котла
      TFT_WriteMenuControl (0, MENU_POSITION(1), 
                            Menu_SelectedItem, 1,
                            0,&sMode,
                            TEXT_COLOR,BACK_COLOR,0,
                            Calibri);   
      // 2. Ручное управление
      TFT_WriteMenuString (0, MENU_POSITION(2),
                           Menu_SelectedItem,
                           &ManualControlMenu, 2,
                           (char*) ManualControlMenuName[Language],
                           0,
                           TEXT_COLOR, BACK_COLOR,
                           Calibri); 
      if (Menu_SelectedItem != (sMenu*) &MainMenu) return 1;
      // 3. Суточный режим
      TFT_WriteMenuControl (0, MENU_POSITION(3), 
                            Menu_SelectedItem, 3,
                            0,&sDailyMode,
                            TEXT_COLOR,BACK_COLOR,0,
                            Calibri);      
      // 4. Время подачи
      TFT_WriteMenuControl (0, MENU_POSITION(4), 
                            Menu_SelectedItem, 4,
                            &sSupplyTime,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 5. Время между подачами топлива
      TFT_WriteMenuControl (0, MENU_POSITION(5), 
                            Menu_SelectedItem, 5,
                            &sSupplyPeriod,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 6. Мощность вентилятора
      TFT_WriteMenuControl (0, MENU_POSITION(6), 
                            Menu_SelectedItem, 6,
                            &sFanPower,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 7. Повторение подач
      TFT_WriteMenuControl (0, MENU_POSITION(7), 
                            Menu_SelectedItem, 7,
                            &sSupplyRepetition,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 8. Время продува
      TFT_WriteMenuControl (0, MENU_POSITION(8), 
                            Menu_SelectedItem, 8,
                            &sBlowingTime,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 9. Время между продувами
      TFT_WriteMenuControl (0, MENU_POSITION(9), 
                            Menu_SelectedItem, 9,
                            &sBlowingPeriod,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 10. Мощность вентилятора при продуве
      TFT_WriteMenuControl (0, MENU_POSITION(10), 
                            Menu_SelectedItem, 10,
                            &sFanPowerBlowing,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 11. Мощность вентилятора при дровах
      TFT_WriteMenuControl (0, MENU_POSITION(11), 
                            Menu_SelectedItem, 11,
                            &sFanPowerFirewood,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri);
      // 12. Заданная температура ГВС
      TFT_WriteMenuControl (0, MENU_POSITION(12), 
                            Menu_SelectedItem, 12,
                            &sTemperatureHWS,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 13. Приоритет бойлера
      TFT_WriteMenuControl (0, MENU_POSITION(13), 
                            Menu_SelectedItem, 13,
                            0,&sBoilerPriority,
                            TEXT_COLOR,BACK_COLOR,0,
                            Calibri);   
      // 14. Температура ночная
      TFT_WriteMenuControl (0, MENU_POSITION(14), 
                            Menu_SelectedItem, 14,
                            &sNightCorrection,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 15. Комнатный термостат
      TFT_WriteMenuControl (0, MENU_POSITION(15), 
                            Menu_SelectedItem, 15,
                            0,&sThermostat,
                            TEXT_COLOR,BACK_COLOR,0,
                            Calibri);   
      // 16. Информация о производителе
      TFT_WriteMenuString (0, MENU_POSITION(16),
                           Menu_SelectedItem,
                           &ManufacturerMenu, 16,
                           (char*) ManufacturerMenuName[Language],
                           0,
                           TEXT_COLOR, BACK_COLOR,
                           Calibri); 
      if (Menu_SelectedItem != (sMenu*) &MainMenu) return 1;
      // 17. Информация о разработчике
      TFT_WriteMenuString (0, MENU_POSITION(17),
                           Menu_SelectedItem,
                           &DesignerMenu, 17,
                           (char*) DesignerMenuName[Language],
                           0,
                           TEXT_COLOR, BACK_COLOR,
                           Calibri); 
      if (Menu_SelectedItem != (sMenu*) &MainMenu) return 1;
      // 18. Информация об устройстве
      TFT_WriteMenuString (0, MENU_POSITION(18),
                           Menu_SelectedItem,
                           &DeviceMenu, 18,
                           (char*) DeviceMenuName[Language],
                           0,
                           TEXT_COLOR, BACK_COLOR,
                           Calibri); 
      if (Menu_SelectedItem != (sMenu*) &MainMenu) return 1;
    }
    else
    // Ручное управление
    if (Item == (sMenu*) &ManualControlMenu)
    {
      // 0. Шнек
      TFT_WriteMenuControl (0, MENU_POSITION(1), 
                            Menu_SelectedItem, 4,
                            0,&sScrewManual,
                            TEXT_COLOR,BACK_COLOR,0,
                            Calibri);
      // 1. Вентилятор
      TFT_WriteMenuControl (0, MENU_POSITION(2), 
                            Menu_SelectedItem, 4,
                            0,&sFanManual,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 2. Насос отопления
      TFT_WriteMenuControl (0, MENU_POSITION(3), 
                            Menu_SelectedItem, 4,
                            0,&sCirPumpManual,
                            TEXT_COLOR,BACK_COLOR,0,
                            Calibri);
      
      // Раз в секунду выводим температуру котла
      if (IS_ADC_FLAG) TFT_WriteParamSign (107,183,S_ALIGN,&sTempIn,TEXT_COLOR,BACK_COLOR,CalibriLarge); 
    }
    else
    // Сервисные настройки
    if (Item == (sMenu*) &ServiceMenu)
    {
      // Если язык поменялся, то обновляем интерфейс
      if (Language != sLanguage.Point)
      {
        Language = sLanguage.Point;
        // Очистка дисплея
        TFT_FillScreen(BACK_COLOR);
        // Взводим флаги необходимости обновления
        NEED_UPDATE_MENU_SET;
        NEED_UPDATE_SET;
        NEED_UPDATE_LANG_SET;
        NEED_UPDATE_BAT_SET;
      }
      
      // 0. Минимальная температура котла
      TFT_WriteMenuControl (0, MENU_POSITION(0), 
                            Menu_SelectedItem, 0,
                            &sTemperatureSettingMin,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 1. Максимальная температура котла
      TFT_WriteMenuControl (0, MENU_POSITION(1), 
                            Menu_SelectedItem, 1,
                            &sTemperatureSettingMax,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 2. Гистерезис датчика котла
      TFT_WriteMenuControl (0, MENU_POSITION(2), 
                            Menu_SelectedItem, 2,
                            &sHysteresisBoilerSensor,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 3. Гистерезис датчика ГВС
      TFT_WriteMenuControl (0, MENU_POSITION(3), 
                            Menu_SelectedItem, 3,
                            &sHysteresisHWSSensor,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 4. Минимальная мощность вентилятора
      TFT_WriteMenuControl (0, MENU_POSITION(4), 
                            Menu_SelectedItem, 4,
                            &sFanPowerMin,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 5. Максимальная мощность вентилятора
      TFT_WriteMenuControl (0, MENU_POSITION(5), 
                            Menu_SelectedItem, 5,
                            &sFanPowerMax,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 6. Время обнаружения нехватки пламени
      TFT_WriteMenuControl (0, MENU_POSITION(6), 
                            Menu_SelectedItem, 6,
                            &sFuelLackTime,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 7. Температура включения насоса отопления
      TFT_WriteMenuControl (0, MENU_POSITION(7), 
                            Menu_SelectedItem, 7,
                            &sTemperatureHeatingPumpStart,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 8. Время отключения насоса
      TFT_WriteMenuControl (0, MENU_POSITION(8), 
                            Menu_SelectedItem, 8,
                            &sTimePumpOff,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 9. Время включения насоса
      TFT_WriteMenuControl (0, MENU_POSITION(9), 
                            Menu_SelectedItem, 9,
                            &sTimePumpOn,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 10. Температура выключения блока
      TFT_WriteMenuControl (0, MENU_POSITION(10), 
                            Menu_SelectedItem, 10,
                            &sTemperatureOff,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 11. Температура тревоги шнека
      TFT_WriteMenuControl (0, MENU_POSITION(11), 
                            Menu_SelectedItem, 11,
                            &sTemperatureScrewAlarm,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 12. Время пересыпки
      TFT_WriteMenuControl (0, MENU_POSITION(12), 
                            Menu_SelectedItem, 12,
                            &sTimeTransfer,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 13. Текущее время
      TFT_WriteMenuString (0, MENU_POSITION(13),
                           Menu_SelectedItem,
                           &TimeMenu, 13,
                           (char*) TimeMenuName[Language],
                           0,
                           TEXT_COLOR, BACK_COLOR,
                           Calibri); 
      if (Menu_SelectedItem != (sMenu*) &ServiceMenu) return 1;
      // 14. Начало ночного периода
      TFT_WriteMenuControl (0, MENU_POSITION(14), 
                            Menu_SelectedItem, 14,
                            &sNightPeriodStartTime,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 15. Начало дневного периода
      TFT_WriteMenuControl (0, MENU_POSITION(15), 
                            Menu_SelectedItem, 15,
                            &sDayPeriodStartTime,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 16. Язык
      TFT_WriteMenuControl (0, MENU_POSITION(16), 
                            Menu_SelectedItem, 16,
                            0,&sLanguage,
                            TEXT_COLOR,BACK_COLOR,0,
                            Calibri); 
      // 17. Энергосбережение
      TFT_WriteMenuControl (0, MENU_POSITION(17), 
                            Menu_SelectedItem, 17,
                            &sEnergySaving,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 18. Хранитель экрана
      TFT_WriteMenuControl (0, MENU_POSITION(18), 
                            Menu_SelectedItem, 18,
                            &sScreenSaver,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 19. Сброс настроек
      TFT_WriteMenuString (0, MENU_POSITION(19),
                           Menu_SelectedItem,
                           &ResetSettingsMenu, 19,
                           (char*) ResetSettingsMenuName[Language],
                           0,
                           TEXT_COLOR, BACK_COLOR,
                           Calibri);
      if (Menu_SelectedItem != (sMenu*) &ServiceMenu) return 1;
    }
    else
    // Текущее время
    if (Item == (sMenu*) &TimeMenu)
    {
      // 0. Часы
      TFT_WriteMenuControl (0, MENU_POSITION(1), 
                            Menu_SelectedItem, 0,
                            &sHours,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 1. Минуты
      TFT_WriteMenuControl (0, MENU_POSITION(2), 
                            Menu_SelectedItem, 1,
                            &sMinutes,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
    }
    else
    // О производителе котла
    if (Item == (sMenu*) &ManufacturerMenu)
    {
      if (IS_NEED_UPDATE)
      {
        // Текстовая информация
        // Наименование компании 
        // адрес
        // почта
        // телефон
        // сайт
        char *ManInfo1;
        char *ManInfo2;
        char *ManInfo3;
        char *ManInfo4;
        #ifdef PSC_PELLETOR_DEVICE
          ManInfo1 = (char*) ManInfoPelletorText1[Language];
          ManInfo2 = (char*) ManInfoPelletorText2[Language];
          ManInfo3 = (char*) ManInfoPelletorText3[Language];
          ManInfo4 = (char*) ManInfoPelletorText4[Language];
        #endif
        #ifdef PSC_START_DEVICE
          ManInfo1 = (char*) ManInfoText1[Language];
          ManInfo2 = (char*) ManInfoText2[Language];
          ManInfo3 = (char*) ManInfoText3[Language];
          ManInfo4 = (char*) ManInfoText4[Language];
        #endif
        
        TFT_WriteString(5,MENU_POSITION(2),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        ManInfo1,Calibri);
        TFT_WriteString(5,MENU_POSITION(3),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        ManInfo2,Calibri);
        TFT_WriteString(5,MENU_POSITION(5),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        ManInfo3,Calibri);
        TFT_WriteString(5,MENU_POSITION(6),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        ManInfo4,Calibri);
      }
    }
    else
    // О разработчике
    if (Item == (sMenu*) &DesignerMenu)
    {
      if (IS_NEED_UPDATE)
      {
        // Текстовая информация 
        // Наименование компании 
        // адрес
        // почта
        // телефон
        // сайт
        TFT_WriteString(5,MENU_POSITION(1),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        (char*) DevInfoText1[Language],Calibri);
        TFT_WriteString(5,MENU_POSITION(2),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        (char*) DevInfoText2[Language],Calibri);
        TFT_WriteString(5,MENU_POSITION(3),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        (char*) DevInfoText3[Language],Calibri);
        TFT_WriteString(5,MENU_POSITION(4),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        (char*) DevInfoText4[Language],Calibri);
        TFT_WriteString(5,MENU_POSITION(6),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        (char*) DevInfoText5[Language],Calibri);
        TFT_WriteString(5,MENU_POSITION(7),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        (char*) DevInfoText6[Language],Calibri);
        TFT_WriteString(5,MENU_POSITION(8),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        (char*) DevInfoText7[Language],Calibri);
      }
    }
    else
    // Информация об устройстве
    if (Item == (sMenu*) &DeviceMenu)
    {
      // 0. Блок управления
      TFT_WriteMenuNullText (0,MENU_POSITION(1),
                             Menu_SelectedItem, 0,
                             (char*) PSC_ABPName[Language],
                             DeviceNamePoint,
                             TEXT_COLOR,BACK_COLOR,Calibri);
      // 1. Версия ПО
      TFT_WriteMenuNullText (0, MENU_POSITION(2),
                             Menu_SelectedItem, 1,
                             (char*) SoftVerName[Language],
                             (char*) SoftVersion,
                             TEXT_COLOR,BACK_COLOR,Calibri);
      // 2. Дата ПО
      TFT_WriteMenuNullText (0, MENU_POSITION(3),
                             Menu_SelectedItem, 2,
                             (char*) SoftDateName[Language],
                             (char*) SoftDate,
                             TEXT_COLOR,BACK_COLOR,Calibri); 
      // 3. Серийный номер
      TFT_WriteMenuNullText (0, MENU_POSITION(4),
                             Menu_SelectedItem, 3,
                             (char*) SerialNumName[Language],
                             (char*) SerialNum,
                             TEXT_COLOR,BACK_COLOR,Calibri); 
      // 4. Дата производства
      TFT_WriteMenuNullText (0, MENU_POSITION(5),
                             Menu_SelectedItem, 4,
                             (char*) ManufactDateName[Language],
                             (char*) ManufactDate,
                             TEXT_COLOR,BACK_COLOR,Calibri); 
      NEED_UPDATE_SET;
      // 5. Напряжение батареи
      TFT_WriteMenuControl (0, MENU_POSITION(6), 
                            Menu_SelectedItem, 5,
                            &sBattery,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri);   
      // 6. Частота сети
      TFT_WriteMenuControl (0, MENU_POSITION(7),
                            Menu_SelectedItem, 6,
                            &sFrequency,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
    }
    else
    // Меню проверки
    if (Item == (sMenu*) &CheckWindow)
    {
      switch (Check_Stage)
      {
        // Кнопка СТАРТ
        case 0:
          TFT_WriteString(0,MENU_POSITION(0),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText1[Language],Calibri);   
          break;
        // Кнопка СТОП
        case 1:
          TFT_WriteString(0,MENU_POSITION(0),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri);  
          TFT_WriteString(0,MENU_POSITION(1),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText2[Language],Calibri);  
          break;
        // Кнопка ВВЕРХ
        case 2:
          TFT_WriteString(0,MENU_POSITION(1),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri);  
          TFT_WriteString(0,MENU_POSITION(2),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText3[Language],Calibri);
          TFT_WriteString(0,MENU_POSITION(8),
                          C_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) StartSkipText[Language],Calibri);
          break;
        // Кнопка ВНИЗ
        case 3:
          TFT_WriteString(0,MENU_POSITION(2),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri);  
          TFT_WriteString(0,MENU_POSITION(3),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText4[Language],Calibri);
          break;
        // Шнек
        case 4:
          TFT_WriteString(0,MENU_POSITION(3),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri);  
          Work_SupplyPowerSet(100);
          TFT_WriteString(0,MENU_POSITION(4),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText5[Language],Calibri);
          break;
        // Вентилятор
        case 5:
          TFT_WriteString(0,MENU_POSITION(4),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          Work_SupplyPowerSet(0);
          Work_FanPowerSet(50);
          TFT_WriteString(0,MENU_POSITION(5),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText6[Language],Calibri);   
          break;
        // Насос отопления
        case 6:
          TFT_WriteString(0,MENU_POSITION(5),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          Work_FanPowerSet(0);
          CIRPUMP_ON;
          TFT_WriteString(0,MENU_POSITION(6),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText7[Language],Calibri);  
          break;
        // Насос ГВС
        case 7:
          TFT_WriteString(0,MENU_POSITION(6),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          CIRPUMP_OFF;
          Work_HWSPumpPowerSet(100);
          TFT_WriteString(0,MENU_POSITION(7),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText8[Language],Calibri);  
          break;
        // Повтор насос ГВС после стирания
        case 8:
          TFT_WriteString(0,MENU_POSITION(0),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText8[Language],Calibri);  
          TFT_WriteString(0,MENU_POSITION(0),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          Work_HWSPumpPowerSet(0);
          Check_Stage++;
        // Авария
        case 9:
          TFT_WriteString(0,MENU_POSITION(1),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText9[Language],Calibri);  
          ALARM_ON;
          TFT_WriteString(0,MENU_POSITION(8),
                          C_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) StartSkipText[Language],Calibri);
          break;
        // Пищалка
        case 10:
          TFT_WriteString(0,MENU_POSITION(1),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          ALARM_OFF;
          BUZZER_ON;
          TFT_WriteString(0,MENU_POSITION(2),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText10[Language],Calibri); 
          break;
       // Аварийний термостат
       case 11:
          TFT_WriteString(0,MENU_POSITION(2),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          BUZZER_OFF;
          TFT_WriteString(0,MENU_POSITION(3),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText11[Language],Calibri); 
          if (SW1_State == GPIO_PIN_SET) {Check_Stage++; HAL_Delay(500);}
          break;
        // Аварийный термостат
        case 12:
          TFT_WriteString(0,MENU_POSITION(3),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(4),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText12[Language],Calibri);  
          if (SW1_State == GPIO_PIN_RESET) {Check_Stage++; HAL_Delay(500);}
          break;
        // Комнатный термостат
        case 13:
          TFT_WriteString(0,MENU_POSITION(4),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(5),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText13[Language],Calibri);  
          if (SW2_State == GPIO_PIN_SET) {Check_Stage++; HAL_Delay(500);}
          break;
        // Комнатный термостат
        case 14:
          TFT_WriteString(0,MENU_POSITION(5),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(6),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText14[Language],Calibri);  
          if (SW2_State == GPIO_PIN_RESET) {Check_Stage++; HAL_Delay(500);}
          break;
        // Внешний стоп
        case 15:
          TFT_WriteString(0,MENU_POSITION(6),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(7),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText15[Language],Calibri);  
          if (Stop_EXT_State == GPIO_PIN_SET) 
          {
            Check_Stage++; HAL_Delay(500);
            // Очистка дисплея
            TFT_FillScreen(BACK_COLOR);
            // Взводим флаги необходимости обновления
            NEED_UPDATE_MENU_SET;
            NEED_UPDATE_SET;
            NEED_UPDATE_LANG_SET;
            NEED_UPDATE_BAT_SET;
          }
          break;
        // Повтор внешний стоп
        case 16:
          TFT_WriteString(0,MENU_POSITION(0),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText15[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(0),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          Check_Stage++;
          break;
        // Внешний стоп
        case 17:
          TFT_WriteString(0,MENU_POSITION(8),
                          C_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) StartSkipText[Language],Calibri);
          TFT_WriteString(0,MENU_POSITION(1),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText16[Language],Calibri);  
          if (Stop_EXT_State == GPIO_PIN_RESET) {Check_Stage++; HAL_Delay(500);}
          break;

        // Замкнуть датчик котла
        case 18:
          TFT_WriteString(0,MENU_POSITION(1),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(2),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText17[Language],Calibri);
          if (sTempIn.Value == TEMP_SENSOR_CLOSURE) {Check_Stage++; HAL_Delay(500);}
          break;
        // Отсоединить датчик котла
        case 19:
          TFT_WriteString(0,MENU_POSITION(2),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(3),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText18[Language],Calibri);
          if (sTempIn.Value == TEMP_SENSOR_BREAK) {Check_Stage++; HAL_Delay(500);}
          break;
        // Замкнуть датчик ГВС
        case 20:
          TFT_WriteString(0,MENU_POSITION(3),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(4),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText19[Language],Calibri);
          if (sTempHWS.Value == TEMP_SENSOR_CLOSURE) {Check_Stage++; HAL_Delay(500);}
          break;
        // Отсоединить датчик ГВС
        case 21:
          TFT_WriteString(0,MENU_POSITION(4),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(5),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText20[Language],Calibri);
          if (sTempHWS.Value == TEMP_SENSOR_BREAK) {Check_Stage++; HAL_Delay(500);}
          break;
        // Замкнуть датчик шнека
        case 22:
          TFT_WriteString(0,MENU_POSITION(5),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(6),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText21[Language],Calibri);
          if (sTempScrew.Value == TEMP_SENSOR_CLOSURE) {Check_Stage++; HAL_Delay(500);}
          break;
        // Отсоединить датчик шнека
        case 23:
          TFT_WriteString(0,MENU_POSITION(6),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(7),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText22[Language],Calibri);
          if (sTempScrew.Value == TEMP_SENSOR_BREAK) 
          {
            Check_Stage++; HAL_Delay(500);
            // Очистка дисплея
            TFT_FillScreen(BACK_COLOR);
            // Взводим флаги необходимости обновления
            NEED_UPDATE_MENU_SET;
            NEED_UPDATE_SET;
            NEED_UPDATE_LANG_SET;
            NEED_UPDATE_BAT_SET;          
          }
          break;
        // Повтор отсоединить датчик шнека
        case 24:
          TFT_WriteString(0,MENU_POSITION(0),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText22[Language],Calibri);
          TFT_WriteString(0,MENU_POSITION(0),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          Check_Stage++;
        // Частота
        case 25:
          TFT_WriteString(0,MENU_POSITION(1),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText23[Language],Calibri);
          TFT_WriteFloatValue(0, MENU_POSITION(1),
                             R_ALIGN,sFrequency.Value,2,0,0,  
                             (char*) UnitNullName[Language],TEXT_COLOR,BACK_COLOR, 
                             Calibri);
          TFT_WriteString(0,MENU_POSITION(8),
                          C_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) StartSkipText[Language],Calibri);
          break;
        // Завершить проверку
        case 26:
          TFT_WriteString(0,MENU_POSITION(2),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText24[Language],Calibri);
          TFT_WriteString(0,MENU_POSITION(8),
                          C_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) StartConfirmText[Language],Calibri);
          break;
      }
    }
    else
    // Сброс настроек
    if (Item == (sMenu*) &ResetSettingsMenu)
    {
      void (*ConfirmVoidFunc) (void);
      sMenu *MenuChildNo;
      sMenu *MenuChildYes;
      
      if (Item == (sMenu*) &ResetSettingsMenu)
      {
        ConfirmVoidFunc = Menu_ResetSettings;
        MenuChildNo = &ServiceMenu;
        MenuChildYes = &ServiceMenu; 
      }
      
      // 0. Нет
      TFT_WriteMenuString (0, MENU_POSITION(1),
                           Menu_SelectedItem,
                           MenuChildNo, 0,
                           (char*) NoSettings[Language],
                           0,
                           TEXT_COLOR, BACK_COLOR,
                           Calibri);    
      // 1. Да 
      TFT_WriteMenuString (0, MENU_POSITION(2),
                           Menu_SelectedItem,
                           MenuChildYes, 1,
                           (char*) YesSettings[Language],
                           ConfirmVoidFunc,
                           TEXT_COLOR, BACK_COLOR,
                           Calibri);    
    }
  }
  return 0;
}