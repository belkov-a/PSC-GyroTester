#include "main.h"
#include "Menu.h"

/*******************************************************************************
  *
  * ��������� ������ � ����������� ������
  *
*******************************************************************************/


// ����������� �����
const char sTemperatureSetName[2][5] = {"T���","Tset"};
sFloatSigned sTemperatureSet = {
  {sTemperatureSetName[0],sTemperatureSetName[1]},
  80, 
  UNIT_DEG
};

// �������� ����������� �������
const char sFanCurrentName[2][6] = {"W����","Wfan"};
sFloatSigned sFanCurrent = {
  {sFanCurrentName[0],sFanCurrentName[1]},
  0,
  UNIT_PERC
};

// ����������� ���, ����� � ��������� 0.1
const char sTempHWSName[2][5] = {"T���","Thws"};
sFloatSigned sTempHWS = {
  {sTempHWSName[0],sTempHWSName[1]},
  200,
  UNIT_DEG
};

// ����������� ������, ����� � ��������� 0.1
const char sTempInName[2][8] = {"T������","Tsupply"};
sFloatSigned sTempIn = {
  {sTempInName[0],sTempInName[1]},
  200,
  UNIT_NULL
};

// ����������� �����, ����� � ��������� 0.1
const char sTempScrewName[2][7] = {"T����","Tscrew"};
sFloatSigned sTempScrew = {
  {sTempScrewName[0],sTempScrewName[1]},
  200,
  UNIT_DEG
};

// ����������� ������
const char sTempNightName[2][7] = {"T����","Tnight"};
sFloatSigned sTempNight = {
  {sTempNightName[0],sTempNightName[1]},
  60,
  UNIT_DEG
};

// ������
const char sSupplyCurrentName[2][8] = {"������ ","Supply "};
sFloatSigned sSupplyCurrent = {
  {sSupplyCurrentName[0],sSupplyCurrentName[1]},
  5,
  UNIT_SEC
};