
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define ADC_POWER_SUPPLY   4096 // Питание      - 3.3 В
#define ADC_VOLTAGE_MAX    3972 // Нет контакта - 3.2 В
#define ADC_VOLTAGE_MIN     125 // Замыкание    - 0.1 В
#define ADC_DIVIDER_RESIST 1000 // Делитель     - 1 кОм

#define TEMP_SENSOR_CLOSURE -1000 // Замыкание датчика
#define TEMP_SENSOR_BREAK   -2000 // Нет контакта датчика

void ADC_ReadValues(void);
uint16_t ADC_GetValue_Channel(uint32_t Channel);
int ADC_CalcTempKY81Value (float ADC_Value);