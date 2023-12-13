#include "main.h"
#include "Init.h"
#include "adc.h"
#include "Menu.h"

uint32_t ADC_Result[5];

const uint16_t KY81_220_ResistValue[24] = {
   980, 1030, 1135, 1247, 1367, 1495, 1630, 1772, 1922, 2000, 2080, 2245, 
  2417, 2663, 2860, 3065, 3278, 3500, 3728, 3950, 4055, 4154, 4325, 4450};

const int16_t  KY81_220_TempValue[24] = {
   -55,  -50,  -40,  -30,  -20,  -10,    0,   10,   20,   25,   30,   40,
    50,   60,   70,   80,   90,  100,  110,  120,  125,  130,  140,  150};

/*******************************************************************************
  *
  * Получение и преобразования значений датчиков и батареи
  *
*******************************************************************************/

void ADC_ReadValues(void)
{  
  if (IS_ADC_READ_FLAG)
  {
    for (int i = 0; i < 15; i++)
    {
      ADC_Result[0] += (ADC_GetValue_Channel(ADC_CHANNEL_1));
      ADC_Result[1] += (ADC_GetValue_Channel(ADC_CHANNEL_2));
      ADC_Result[2] += (ADC_GetValue_Channel(ADC_CHANNEL_3));
      ADC_Result[3] += (ADC_GetValue_Channel(ADC_CHANNEL_4));
      ADC_Result[4] += 2*ADC_GetValue_Channel(ADC_CHANNEL_VBAT);
    }
    
    sTempIn.Value    = ADC_CalcTempKY81Value (ADC_Result[1]/15);
    sTempHWS.Value   = ADC_CalcTempKY81Value (ADC_Result[2]/15);
    sTempScrew.Value = ADC_CalcTempKY81Value (ADC_Result[3]/15);
    BatteryVoltage = (uint16_t) ((ADC_Result[4]*330)/4096)/15;
    
    for (int i = 0; i < 5; i++) ADC_Result[i] = 0;
    
    ADC_READ_FLAG_RESET;
  }
}

/*******************************************************************************
  *
  * Расчет температуры датчика KY81/220 (2кОм)  
  *
  * Нет контакта - возвращаем TEMP_SENSOR_LACK
  * Замыкание    - возвращаем TEMP_SENSOR_CLOSURE
  * 
  * Аппроксимация по табличным значениям
  *
*******************************************************************************/

int ADC_CalcTempKY81Value (float ADC_Value)
{ 
  // Определяем замыкание или отсутствие контакта
  if (ADC_Value < ADC_VOLTAGE_MIN) return TEMP_SENSOR_CLOSURE;
  if (ADC_Value > ADC_VOLTAGE_MAX) return TEMP_SENSOR_BREAK;
  
  // Считаем сопротивление датчика
  float SensorResist = (float) ((ADC_Value * ADC_DIVIDER_RESIST) / (ADC_POWER_SUPPLY - ADC_Value));
    
  // Координаты аппроксимации
  float y1 = 0;
  float y0 = 0;
  float  x1 = 0;
  float  x0 = 0;
  
  // Если сопротивление выше максимального табличного, то аппроксимируем по 
  // последним двум точкам
  if (SensorResist > KY81_220_ResistValue[23])
  {
    y1 = KY81_220_ResistValue[23];
    y0 = KY81_220_ResistValue[22];
    x1 = KY81_220_TempValue[23];
    x0 = KY81_220_TempValue[22];
  }
  else
  // Если сопротивление ниже минимального табличного, то аппроксимируем по
  // первым двум точкам
  if (SensorResist <= KY81_220_ResistValue[0])
  {
    y1 = KY81_220_ResistValue[1];
    y0 = KY81_220_ResistValue[0];
    x1 = KY81_220_TempValue[1];
    x0 = KY81_220_TempValue[0];
  }
  else
  // Иначе сканируем таблицу значений и аппроксимируем по диапазонам
  for (uint8_t i = 1; i < 24; i++)
  {
    if ((SensorResist <= KY81_220_ResistValue[i]) && (SensorResist > KY81_220_ResistValue[i-1]))
    {
      y1 = KY81_220_ResistValue[i];
      y0 = KY81_220_ResistValue[i-1];
      x1 = KY81_220_TempValue[i];
      x0 = KY81_220_TempValue[i-1];
      break;
    }
  }
  
  // Считаем коэффициенты аппроксимации 
  float k = (y1 - y0) / (x1 - x0);
  float b = (y0*x1 - y1*x0) / (x1 - x0); 
  
  int Temp = (int) (((SensorResist - b) / k) * 10);
  if (Temp < -990) return -990;
  if (Temp > 3000) return 3000;
  return Temp;
}

/*******************************************************************************
  *
  * Чтение значения канала АЦП
  *
*******************************************************************************/

uint16_t ADC_GetValue_Channel(uint32_t Channel)
{
  ADC_ChannelConfTypeDef sConfig;
  
  uint16_t val = 0;
  
  // Конфигурация выбранного канала
  sConfig.Channel = Channel;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5 ;
  HAL_ADC_ConfigChannel(&hadc,&sConfig);
  
  // Автокалибровка АЦП
  HAL_ADCEx_Calibration_Start(&hadc);
    
  // Старт преобразования и ждем завершения
  HAL_ADC_Start(&hadc);
  HAL_ADC_PollForConversion(&hadc,1000);
  
  // Считываем значение	
  val = HAL_ADC_GetValue(&hadc);
  
  // Останавливаем преобразование
  HAL_ADC_Stop(&hadc);
  
  // Сбрасываем конфигурацию
  sConfig.Rank = ADC_RANK_NONE;
  HAL_ADC_ConfigChannel(&hadc,&sConfig);
  
  return val;
}