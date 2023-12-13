#include "main.h"
#include "Menu.h"

/*******************************************************************************
  *
  * ��������� ������ � ��������� �����������
  *
*******************************************************************************/

/* �������� ���� */

// �������� ����������� �����
const char sTemperatureSettingName[2][20] = {"����-�� �����, T���","Boiler temper, Tset"};
sFloat sTemperatureSetting = {
  {sTemperatureSettingName[0], sTemperatureSettingName[1]},
  60,40,80,
  0,0,
  UNIT_DEG
};

// ����� ������ �������
const char sSupplyTimeName[2][17] = {"����� ������, t�","Supply time, ts"};
sFloat sSupplyTime = {
  {sSupplyTimeName[0], sSupplyTimeName[1]},
  10,5,120,
  0,0,
  UNIT_SEC
};

// ����� ����� �������� �������
const char sSupplyPeriodName[2][21] = {"����� ����� ���, P�","Time between sup, Ps"};
sFloat sSupplyPeriod = {
  {sSupplyPeriodName[0], sSupplyPeriodName[1]},
  15,1,300,
  0,0,
  UNIT_SEC
};

// �������� �����������
const char sFanPowerName[2][21] = {"�������� ����-��, W�","Fan power, Wf"};
sFloat sFanPower = {
  {sFanPowerName[0], sFanPowerName[1]},
  40,10,100,
  0,0,
  UNIT_PERC
};

// ���������� �����
const char sSupplyRepetName[2][18] = {"������ �����, N�","Supply repeat, Ns"}; 
sFloat sSupplyRepetition = {
  {sSupplyRepetName[0], sSupplyRepetName[1]},
  2,0,20,
  0,0,
  UNIT_NULL
};

// ����� �������
const char sBlowingTimeName[2][19] = {"����� �������, t��","Blowing time, tb"};
sFloat sBlowingTime = {
  {sBlowingTimeName[0], sBlowingTimeName[1]},
  15,5,59,
  0,OFF_SUPPORT,
  UNIT_SEC
};

// ����� ����� ���������
const char sBlowingPeriodName[2][22] = {"����� ����� ����, P��","Time between blow, Pb"};
sFloat sBlowingPeriod = {
  {sBlowingPeriodName[0], sBlowingPeriodName[1]},
  2,1,99,
  0,0,
  UNIT_MIN
};

// �������� ����������� � ��������
const char sFanPowerBlowingName[2][22] = {"���� ���� ������, W��","Blowing fan power, Wb"};
sFloat sFanPowerBlowing = {
  {sFanPowerBlowingName[0], sFanPowerBlowingName[1]},
  50,10,100,
  0,0,
  UNIT_PERC
};

// �������� ����������� �����
const char sFanPowerFirewoodName[2][23] = {"���� ���� �����, W�","Firewood fan power, Wf"};
sFloat sFanPowerFirewood = {
  {(char*)sFanPowerFirewoodName[0],(char*)sFanPowerFirewoodName[1]},
  40,10,100,
  0,0,
  UNIT_PERC
};

// ����������� �������� ���
const char sTemperatureHWSName[2][22] = {"����-�� ���, T���.���","Temper HWS, Thws.set"};
sFloat sTemperatureHWS = {
  {sTemperatureHWSName[0], sTemperatureHWSName[1]},
  30,30,70,
  0,OFF_SUPPORT + OFF_STATE,
  UNIT_DEG
};

// ������ ���������
const char sNightCorrectionName[2][21] = {"������ ���������, T�","Night correction, Tn"};
sFloat sNightCorrection = {
  {sNightCorrectionName[0], sNightCorrectionName[1]},
  5,-20,20,
  0,0,
  UNIT_DEG
};

/******************************** ��������� ���� ******************************/

// ��� ����������� �����
const char sTemperatureSettingMinName[2][25] = {"��� ��� �����, T���","Min temp boiler, Tmax"};
sFloat sTemperatureSettingMin = {
  {sTemperatureSettingMinName[0], sTemperatureSettingMinName[1]},
  40,40,55,
  0,0,
  UNIT_DEG
};

// ���� ����������� �����
const char sTemperatureSettingMaxName[2][25] = {"���� ��� �����, T����","Max temp boiler, Tmax"};
sFloat sTemperatureSettingMax = {
  {sTemperatureSettingMaxName[0], sTemperatureSettingMaxName[1]},
  80,60,80,
  0,0,
  UNIT_DEG
};

// ���������� ������� �����
const char sHysteresisBoilerSensorName[2][24] = {"������. ����. �����, H�","Hyster. boiler sens, Hb"};
sFloat sHysteresisBoilerSensor = {
  {sHysteresisBoilerSensorName[0], sHysteresisBoilerSensorName[1]},
  1,1,5,
  0,0,
  UNIT_DEG
};

// ���������� ������� ���
const char sHysteresisHWSSensorName[2][24] = {"������. ����. ���, H���","Hyster. HWS sens, Hhws"};
sFloat sHysteresisHWSSensor = {
  {sHysteresisHWSSensorName[0],sHysteresisHWSSensorName[1]},
  3,1,5,
  0,0,
  UNIT_DEG
};

// ����������� �������� �����������
const char sFanPowerMinName[2][20] = {"��� ���� ����, W���","Min fan power, Wmin"};
sFloat sFanPowerMin = {
  {sFanPowerMinName[0], sFanPowerMinName[1]},
  25,20,70,
  0,0,
  UNIT_PERC
};

// ������������ �������� �����������
const char sFanPowerMaxName[2][22] = {"���� ���� ����, W����","Max fan power, Wmax"};
sFloat sFanPowerMax = {
  {sFanPowerMaxName[0], sFanPowerMaxName[1]},
  55,20,100,
  0,0,
  UNIT_PERC
};

// ����� ����������� �������� �������
const char sFuelLackTimeName[2][21] = {"����� ���� ����, t��","Fuel lack time, tfl"};
sFloat sFuelLackTime = {
  {sFuelLackTimeName[0], sFuelLackTimeName[1]},
  30,5,120,
  0,OFF_SUPPORT,
  UNIT_MIN
};

// ����������� ��������� ������ ���������
const char sTemperatureHeatingPumpStartName[2][23] = {"���� ������ �����, T��","Heat pump Ton, Thp"};
sFloat sTemperatureHeatingPumpStart = {
  {sTemperatureHeatingPumpStartName[0], sTemperatureHeatingPumpStartName[1]},
  40,25,70,
  0,OFF_SUPPORT,
  UNIT_DEG
};

// ����� ���������� ������
const char sTimePumpOffName[2][23] = {"����� ���� ������, t��","Pump off time, toffp"};
sFloat sTimePumpOff = {
  {sTimePumpOffName[0], sTimePumpOffName[1]},
  3,1,250,
  0,0,
  UNIT_MIN
};

// ����� ��������� ������
const char sTimePumpOnName[2][22] = {"����� ��� ������, t��","Pump on time, tonp"};
sFloat sTimePumpOn = {
  {sTimePumpOnName[0], sTimePumpOnName[1]},
  30,10,60,
  0,0,
  UNIT_SEC
};

// ����������� ���������� �����
const char sTemperatureOffName[2][20] = {"����-�� ����, T����","Off temper, Toff"};
sFloat sTemperatureOff = {
  {sTemperatureOffName[0], sTemperatureOffName[1]},
  30,15,50,
  0,0,
  UNIT_DEG
};

// ����������� ������� �����
const char sTemperatureScrewAlarmName[2][22] = {"���� ���� �����, T��","Temp alarm screw, Tas"};
sFloat sTemperatureScrewAlarm = {
  {sTemperatureScrewAlarmName[0], sTemperatureScrewAlarmName[1]},
  70,50,90,
  0,OFF_SUPPORT,
  UNIT_DEG
};

// ����� ���������
const char sTimeTransferName[2][25] = {"����� ���������, t���","Transfer time, tt"};
sFloat sTimeTransfer = {
  {sTimeTransferName[0],sTimeTransferName[1]},
  10,1,30,
  0,0,
  UNIT_MIN
};

// ����
const char sHoursName[2][12] = {"�����, ����","Time, hours"};
sFloat sHours = {
  {sHoursName[0],sHoursName[1]},
  12,0,23,
  0,0,
  UNIT_NULL
};

// ������
const char sMinutesName[2][14] = {"�����, ������","Time, minutes"};
sFloat sMinutes = {
  {sMinutesName[0],sMinutesName[1]},
  0,0,59,
  0,0,
  UNIT_NULL
};

// ������ ������� �������
const char sNightPeriodStartTimeName[2][18] = {"������ ������, t�","Night period, tn"};
sFloat sNightPeriodStartTime = {
  {sNightPeriodStartTimeName[0], sNightPeriodStartTimeName[1]},
  22,18,23,
  0,0,
  UNIT_HOUR
};

// ������ �������� �������
const char sDayPeriodStartTimeName[2][19] = {"������� ������, t�","Day period, td"};
sFloat sDayPeriodStartTime = {
  {sDayPeriodStartTimeName[0], sDayPeriodStartTimeName[1]},
  6,5,12,
  0,0,
  UNIT_HOUR
};

// ����������������
const char sEnergySavingName[2][18] = {"������������, t�","Energy saving, te"};
sFloat sEnergySaving = {
  {sEnergySavingName[0], sEnergySavingName[1]},
  5,5,60,
  0,OFF_SUPPORT + OFF_STATE,
  UNIT_MIN
};

// ��������� ������
const char sScreenSaverName[2][20] = {"��������� ������","Screen saver"};
sFloat sScreenSaver = {
  {sScreenSaverName[0],sScreenSaverName[1]},
  15,15,120,
  0,OFF_SUPPORT + OFF_STATE,
  UNIT_MIN
};

// ���������� �������
const char BatteryVoltageName[2][20] = {"���������� �������","Battery voltage"};
sFloat sBattery = {
  {BatteryVoltageName[0], BatteryVoltageName[1]},
  300,100,400,
  2,0,
  UNIT_VOLT
};

// ������� ����
const char FrequencyName[2][13] = {"������� ����","Frequency"};
sFloat sFrequency = {
  {FrequencyName[0], FrequencyName[1]},
  0,0,200,
  0,0,
  UNIT_HZ
};
