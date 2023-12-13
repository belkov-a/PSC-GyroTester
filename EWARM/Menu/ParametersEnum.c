#include "main.h"
#include "Menu.h"

/*******************************************************************************
  *
  * ��������� ������ � ����������� ������������
  *
*******************************************************************************/

const char sOnOffValue[2][2][5] = {
  {"����", "���"},
  {"Off",  "On"}
};

// ����
const char sLanguageName[2][16] = {"���� ����������","Language"};
const char sLanguageValue[2][2][10] = {
  {"�������", "English"},
  {"�������", "English"}
};
sEnum sLanguage = {
  {sLanguageName[0],sLanguageName[1]},
  0, 2,
  {{sLanguageValue[0][0],sLanguageValue[0][1]},
   {sLanguageValue[1][0],sLanguageValue[1][1]}}
};

// ����� ������ �����
const char sModeName[2][13] = {"����� �����","Boiler mode"};
const char sModeValue[2][2][15] = {
  {"��������������", "�����"},
  {"Automatic",      "Firewood"}
};
sEnum sMode = {
  {sModeName[0],sModeName[1]},
  0, 2,
  {{sModeValue[0][0],sModeValue[0][1]},
   {sModeValue[1][0],sModeValue[1][1]}}
};

// �������� ����� 
const char sDailyModeName[2][15] = {"�������� �����","Daily mode"};
const char sDailyModeValue[2][2][15] = {
  {"����������","����/����"},
  {"Normal",    "Day/Night"}
};
sEnum sDailyMode = {
  {sDailyModeName[0],sDailyModeName[1]},
  0, 2,
  {{sDailyModeValue[0][0],sDailyModeValue[0][1]},
   {sDailyModeValue[1][0],sDailyModeValue[1][1]}}
};

// ��������� �������
const char sBoilerPriorityName[2][20] = {"��������� �������","Boiler priority"};
sEnum sBoilerPriority = {
  {sBoilerPriorityName[0],sBoilerPriorityName[1]},
  0, 2,
  {{sOnOffValue[0][0],sOnOffValue[0][1]},
   {sOnOffValue[1][0],sOnOffValue[1][1]}}
};

// ��������� ���������
const char sThermostatName[2][20] = {"��������� ���������","Room thermostat"};
sEnum sThermostat = {
  {sThermostatName[0],sThermostatName[1]},
  0, 2,
  {{sOnOffValue[0][0],sOnOffValue[0][1]},
   {sOnOffValue[1][0],sOnOffValue[1][1]}}
};

// ���������� � ������ ����������
const char sFanManualName[2][20] = {"���������� (�����)","Fan (UP)"};
sEnum sFanManual = {
  {sFanManualName[0],sFanManualName[1]},
  0, 2,
  {{sOnOffValue[0][0],sOnOffValue[0][1]},
   {sOnOffValue[1][0],sOnOffValue[1][1]}}  
};

// ���� � ������ ����������
const char sScrewManualName[2][15] = {"���� (����)","Screw (START)"};
sEnum sScrewManual = {
  {sScrewManualName[0],sScrewManualName[1]},
  0, 2,
  {{sOnOffValue[0][0],sOnOffValue[0][1]},
   {sOnOffValue[1][0],sOnOffValue[1][1]}}
};

// ����� ��������� � ������ ����������
const char sCirPumpManualName[2][25] = {"����� ��������� (����)","Heating pump (DOWN)"};
sEnum sCirPumpManual = {
  {sCirPumpManualName[0],sCirPumpManualName[1]},
  0, 2,
  {{sOnOffValue[0][0],sOnOffValue[0][1]},
   {sOnOffValue[1][0],sOnOffValue[1][1]}}
};