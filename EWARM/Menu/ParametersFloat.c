#include "main.h"
#include "Menu.h"

/*******************************************************************************
  *
  * Структуры работы с числовыми параметрами
  *
*******************************************************************************/

/* Основное меню */

// Заданная температура котла
const char sTemperatureSettingName[2][20] = {"Темп-ра котла, Tуст","Boiler temper, Tset"};
sFloat sTemperatureSetting = {
  {sTemperatureSettingName[0], sTemperatureSettingName[1]},
  60,40,80,
  0,0,
  UNIT_DEG
};

// Время подачи топлива
const char sSupplyTimeName[2][17] = {"Время подачи, tп","Supply time, ts"};
sFloat sSupplyTime = {
  {sSupplyTimeName[0], sSupplyTimeName[1]},
  10,5,120,
  0,0,
  UNIT_SEC
};

// Время между подачами топлива
const char sSupplyPeriodName[2][21] = {"Время между под, Pп","Time between sup, Ps"};
sFloat sSupplyPeriod = {
  {sSupplyPeriodName[0], sSupplyPeriodName[1]},
  15,1,300,
  0,0,
  UNIT_SEC
};

// Мощность вентилятора
const char sFanPowerName[2][21] = {"Мощность вент-ра, Wв","Fan power, Wf"};
sFloat sFanPower = {
  {sFanPowerName[0], sFanPowerName[1]},
  40,10,100,
  0,0,
  UNIT_PERC
};

// Повторение подач
const char sSupplyRepetName[2][18] = {"Повтор подач, Nп","Supply repeat, Ns"}; 
sFloat sSupplyRepetition = {
  {sSupplyRepetName[0], sSupplyRepetName[1]},
  2,0,20,
  0,0,
  UNIT_NULL
};

// Время продува
const char sBlowingTimeName[2][19] = {"Время продува, tпр","Blowing time, tb"};
sFloat sBlowingTime = {
  {sBlowingTimeName[0], sBlowingTimeName[1]},
  15,5,59,
  0,OFF_SUPPORT,
  UNIT_SEC
};

// Время между продувами
const char sBlowingPeriodName[2][22] = {"Время между прод, Pпр","Time between blow, Pb"};
sFloat sBlowingPeriod = {
  {sBlowingPeriodName[0], sBlowingPeriodName[1]},
  2,1,99,
  0,0,
  UNIT_MIN
};

// Мощность вентилятора в продувах
const char sFanPowerBlowingName[2][22] = {"Мощн вент продув, Wпр","Blowing fan power, Wb"};
sFloat sFanPowerBlowing = {
  {sFanPowerBlowingName[0], sFanPowerBlowingName[1]},
  50,10,100,
  0,0,
  UNIT_PERC
};

// Мощность вентилятора дрова
const char sFanPowerFirewoodName[2][23] = {"Мощн вент дрова, Wд","Firewood fan power, Wf"};
sFloat sFanPowerFirewood = {
  {(char*)sFanPowerFirewoodName[0],(char*)sFanPowerFirewoodName[1]},
  40,10,100,
  0,0,
  UNIT_PERC
};

// Температура заданная ГВС
const char sTemperatureHWSName[2][22] = {"Темп-ра ГВС, Tгвс.зад","Temper HWS, Thws.set"};
sFloat sTemperatureHWS = {
  {sTemperatureHWSName[0], sTemperatureHWSName[1]},
  30,30,70,
  0,OFF_SUPPORT + OFF_STATE,
  UNIT_DEG
};

// Ночная коррекция
const char sNightCorrectionName[2][21] = {"Ночная коррекция, Tн","Night correction, Tn"};
sFloat sNightCorrection = {
  {sNightCorrectionName[0], sNightCorrectionName[1]},
  5,-20,20,
  0,0,
  UNIT_DEG
};

/******************************** Сервисное меню ******************************/

// Мин температура котла
const char sTemperatureSettingMinName[2][25] = {"Мин тем котла, Tмин","Min temp boiler, Tmax"};
sFloat sTemperatureSettingMin = {
  {sTemperatureSettingMinName[0], sTemperatureSettingMinName[1]},
  40,40,55,
  0,0,
  UNIT_DEG
};

// Макс темпертаура котла
const char sTemperatureSettingMaxName[2][25] = {"Макс тем котла, Tмакс","Max temp boiler, Tmax"};
sFloat sTemperatureSettingMax = {
  {sTemperatureSettingMaxName[0], sTemperatureSettingMaxName[1]},
  80,60,80,
  0,0,
  UNIT_DEG
};

// Гистерезис датчика котла
const char sHysteresisBoilerSensorName[2][24] = {"Гистер. датч. котла, Hк","Hyster. boiler sens, Hb"};
sFloat sHysteresisBoilerSensor = {
  {sHysteresisBoilerSensorName[0], sHysteresisBoilerSensorName[1]},
  1,1,5,
  0,0,
  UNIT_DEG
};

// Гистерезис датчика ГВС
const char sHysteresisHWSSensorName[2][24] = {"Гистер. датч. ГВС, Hгвс","Hyster. HWS sens, Hhws"};
sFloat sHysteresisHWSSensor = {
  {sHysteresisHWSSensorName[0],sHysteresisHWSSensorName[1]},
  3,1,5,
  0,0,
  UNIT_DEG
};

// Минимальная мощность вентилятора
const char sFanPowerMinName[2][20] = {"Мин мощн вент, Wмин","Min fan power, Wmin"};
sFloat sFanPowerMin = {
  {sFanPowerMinName[0], sFanPowerMinName[1]},
  25,20,70,
  0,0,
  UNIT_PERC
};

// Максимальная мощность вентилятора
const char sFanPowerMaxName[2][22] = {"Макс мощн вент, Wмакс","Max fan power, Wmax"};
sFloat sFanPowerMax = {
  {sFanPowerMaxName[0], sFanPowerMaxName[1]},
  55,20,100,
  0,0,
  UNIT_PERC
};

// Время обнаружения нехватки топлива
const char sFuelLackTimeName[2][21] = {"Время нехв топл, tнт","Fuel lack time, tfl"};
sFloat sFuelLackTime = {
  {sFuelLackTimeName[0], sFuelLackTimeName[1]},
  30,5,120,
  0,OFF_SUPPORT,
  UNIT_MIN
};

// Температура включения насоса отопления
const char sTemperatureHeatingPumpStartName[2][23] = {"Твкл насоса отопл, Tно","Heat pump Ton, Thp"};
sFloat sTemperatureHeatingPumpStart = {
  {sTemperatureHeatingPumpStartName[0], sTemperatureHeatingPumpStartName[1]},
  40,25,70,
  0,OFF_SUPPORT,
  UNIT_DEG
};

// Время отключения насоса
const char sTimePumpOffName[2][23] = {"Время откл насоса, tон","Pump off time, toffp"};
sFloat sTimePumpOff = {
  {sTimePumpOffName[0], sTimePumpOffName[1]},
  3,1,250,
  0,0,
  UNIT_MIN
};

// Время включения насоса
const char sTimePumpOnName[2][22] = {"Время вкл насоса, tвн","Pump on time, tonp"};
sFloat sTimePumpOn = {
  {sTimePumpOnName[0], sTimePumpOnName[1]},
  30,10,60,
  0,0,
  UNIT_SEC
};

// Температура отключения блока
const char sTemperatureOffName[2][20] = {"Темп-ра откл, Tоткл","Off temper, Toff"};
sFloat sTemperatureOff = {
  {sTemperatureOffName[0], sTemperatureOffName[1]},
  30,15,50,
  0,0,
  UNIT_DEG
};

// Температура тревоги шнека
const char sTemperatureScrewAlarmName[2][22] = {"Темп трев шнека, Tтш","Temp alarm screw, Tas"};
sFloat sTemperatureScrewAlarm = {
  {sTemperatureScrewAlarmName[0], sTemperatureScrewAlarmName[1]},
  70,50,90,
  0,OFF_SUPPORT,
  UNIT_DEG
};

// Время пересыпки
const char sTimeTransferName[2][25] = {"Время пересыпки, tпер","Transfer time, tt"};
sFloat sTimeTransfer = {
  {sTimeTransferName[0],sTimeTransferName[1]},
  10,1,30,
  0,0,
  UNIT_MIN
};

// Часы
const char sHoursName[2][12] = {"Время, часы","Time, hours"};
sFloat sHours = {
  {sHoursName[0],sHoursName[1]},
  12,0,23,
  0,0,
  UNIT_NULL
};

// Минуты
const char sMinutesName[2][14] = {"Время, минуты","Time, minutes"};
sFloat sMinutes = {
  {sMinutesName[0],sMinutesName[1]},
  0,0,59,
  0,0,
  UNIT_NULL
};

// Начало ночного периода
const char sNightPeriodStartTimeName[2][18] = {"Ночной период, tн","Night period, tn"};
sFloat sNightPeriodStartTime = {
  {sNightPeriodStartTimeName[0], sNightPeriodStartTimeName[1]},
  22,18,23,
  0,0,
  UNIT_HOUR
};

// Начало дневного периода
const char sDayPeriodStartTimeName[2][19] = {"Дневной период, tд","Day period, td"};
sFloat sDayPeriodStartTime = {
  {sDayPeriodStartTimeName[0], sDayPeriodStartTimeName[1]},
  6,5,12,
  0,0,
  UNIT_HOUR
};

// Энергосбережение
const char sEnergySavingName[2][18] = {"Энергосбереж, tэ","Energy saving, te"};
sFloat sEnergySaving = {
  {sEnergySavingName[0], sEnergySavingName[1]},
  5,5,60,
  0,OFF_SUPPORT + OFF_STATE,
  UNIT_MIN
};

// Хранитель экрана
const char sScreenSaverName[2][20] = {"Хранитель экрана","Screen saver"};
sFloat sScreenSaver = {
  {sScreenSaverName[0],sScreenSaverName[1]},
  15,15,120,
  0,OFF_SUPPORT + OFF_STATE,
  UNIT_MIN
};

// Напряжение батареи
const char BatteryVoltageName[2][20] = {"Напряжение батареи","Battery voltage"};
sFloat sBattery = {
  {BatteryVoltageName[0], BatteryVoltageName[1]},
  300,100,400,
  2,0,
  UNIT_VOLT
};

// Частота сети
const char FrequencyName[2][13] = {"Частота сети","Frequency"};
sFloat sFrequency = {
  {FrequencyName[0], FrequencyName[1]},
  0,0,200,
  0,0,
  UNIT_HZ
};
