#include "main.h"
#include "Menu.h"

/*******************************************************************************
  *
  * Структуры работы с параметрами перечисления
  *
*******************************************************************************/

const char sOnOffValue[2][2][5] = {
  {"Откл", "Вкл"},
  {"Off",  "On"}
};

// Язык
const char sLanguageName[2][16] = {"Язык интерфейса","Language"};
const char sLanguageValue[2][2][10] = {
  {"Русский", "English"},
  {"Русский", "English"}
};
sEnum sLanguage = {
  {sLanguageName[0],sLanguageName[1]},
  0, 2,
  {{sLanguageValue[0][0],sLanguageValue[0][1]},
   {sLanguageValue[1][0],sLanguageValue[1][1]}}
};

// Режим работы котла
const char sModeName[2][13] = {"Режим котла","Boiler mode"};
const char sModeValue[2][2][15] = {
  {"Автоматический", "Дрова"},
  {"Automatic",      "Firewood"}
};
sEnum sMode = {
  {sModeName[0],sModeName[1]},
  0, 2,
  {{sModeValue[0][0],sModeValue[0][1]},
   {sModeValue[1][0],sModeValue[1][1]}}
};

// Суточный режим 
const char sDailyModeName[2][15] = {"Суточный режим","Daily mode"};
const char sDailyModeValue[2][2][15] = {
  {"Нормальный","День/Ночь"},
  {"Normal",    "Day/Night"}
};
sEnum sDailyMode = {
  {sDailyModeName[0],sDailyModeName[1]},
  0, 2,
  {{sDailyModeValue[0][0],sDailyModeValue[0][1]},
   {sDailyModeValue[1][0],sDailyModeValue[1][1]}}
};

// Приоритет бойлера
const char sBoilerPriorityName[2][20] = {"Приоритет бойлера","Boiler priority"};
sEnum sBoilerPriority = {
  {sBoilerPriorityName[0],sBoilerPriorityName[1]},
  0, 2,
  {{sOnOffValue[0][0],sOnOffValue[0][1]},
   {sOnOffValue[1][0],sOnOffValue[1][1]}}
};

// Комнатный термостат
const char sThermostatName[2][20] = {"Комнатный термостат","Room thermostat"};
sEnum sThermostat = {
  {sThermostatName[0],sThermostatName[1]},
  0, 2,
  {{sOnOffValue[0][0],sOnOffValue[0][1]},
   {sOnOffValue[1][0],sOnOffValue[1][1]}}
};

// Вентилятор в ручном управлении
const char sFanManualName[2][20] = {"Вентилятор (ВВЕРХ)","Fan (UP)"};
sEnum sFanManual = {
  {sFanManualName[0],sFanManualName[1]},
  0, 2,
  {{sOnOffValue[0][0],sOnOffValue[0][1]},
   {sOnOffValue[1][0],sOnOffValue[1][1]}}  
};

// Шнек в ручном управлении
const char sScrewManualName[2][15] = {"Шнек (ПУСК)","Screw (START)"};
sEnum sScrewManual = {
  {sScrewManualName[0],sScrewManualName[1]},
  0, 2,
  {{sOnOffValue[0][0],sOnOffValue[0][1]},
   {sOnOffValue[1][0],sOnOffValue[1][1]}}
};

// Насос отопления в ручном управлении
const char sCirPumpManualName[2][25] = {"Насос отопления (ВНИЗ)","Heating pump (DOWN)"};
sEnum sCirPumpManual = {
  {sCirPumpManualName[0],sCirPumpManualName[1]},
  0, 2,
  {{sOnOffValue[0][0],sOnOffValue[0][1]},
   {sOnOffValue[1][0],sOnOffValue[1][1]}}
};