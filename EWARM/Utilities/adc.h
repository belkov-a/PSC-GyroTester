
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define ADC_POWER_SUPPLY   4096 // �������      - 3.3 �
#define ADC_VOLTAGE_MAX    3972 // ��� �������� - 3.2 �
#define ADC_VOLTAGE_MIN     125 // ���������    - 0.1 �
#define ADC_DIVIDER_RESIST 1000 // ��������     - 1 ���

#define TEMP_SENSOR_CLOSURE -1000 // ��������� �������
#define TEMP_SENSOR_BREAK   -2000 // ��� �������� �������

void ADC_ReadValues(void);
uint16_t ADC_GetValue_Channel(uint32_t Channel);
int ADC_CalcTempKY81Value (float ADC_Value);