#include "main.h"
#include "Menu.h"

/*******************************************************************************
  *
  * Структуры работы с параметрами вывода
  *
*******************************************************************************/


// Температура котла
const char sTemperatureSetName[2][5] = {"Tуст","Tset"};
sFloatSigned sTemperatureSet = {
  {sTemperatureSetName[0],sTemperatureSetName[1]},
  80, 
  UNIT_DEG
};

// Мощность вентилятора текущая
const char sFanCurrentName[2][6] = {"Wвент","Wfan"};
sFloatSigned sFanCurrent = {
  {sFanCurrentName[0],sFanCurrentName[1]},
  0,
  UNIT_PERC
};

// Температура ГВС, целое с точностью 0.1
const char sTempHWSName[2][5] = {"Tгвс","Thws"};
sFloatSigned sTempHWS = {
  {sTempHWSName[0],sTempHWSName[1]},
  200,
  UNIT_DEG
};

// Температура подачи, целое с точностью 0.1
const char sTempInName[2][8] = {"Tподачи","Tsupply"};
sFloatSigned sTempIn = {
  {sTempInName[0],sTempInName[1]},
  200,
  UNIT_NULL
};

// Температура шнека, целое с точностью 0.1
const char sTempScrewName[2][7] = {"Tшнек","Tscrew"};
sFloatSigned sTempScrew = {
  {sTempScrewName[0],sTempScrewName[1]},
  200,
  UNIT_DEG
};

// Температура ночная
const char sTempNightName[2][7] = {"Tночь","Tnight"};
sFloatSigned sTempNight = {
  {sTempNightName[0],sTempNightName[1]},
  60,
  UNIT_DEG
};

// Подача
const char sSupplyCurrentName[2][8] = {"Подача ","Supply "};
sFloatSigned sSupplyCurrent = {
  {sSupplyCurrentName[0],sSupplyCurrentName[1]},
  5,
  UNIT_SEC
};