
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f0xx_hal.h"
  
#define L3G4200D_ADDR 0xD0
  
#define WHO_AM_I                                                                         0x0F
#define CTRL_REG1                                                                        0x20
#define CTRL_REG2                                                                        0x21
#define CTRL_REG3                                                                        0x22
#define CTRL_REG4                                                                        0x23
#define CTRL_REG5                                                                        0x24
#define REFERENCE                                                                        0x25
#define OUT_TEMP                                                                         0x26
#define STATUS_REG                                                                       0x27
#define OUT_X_L                                                                          0xA8
#define OUT_X_H                                                                          0x29
#define OUT_Y_L                                                                          0x2A
#define OUT_Y_H                                                                          0x2B
#define OUT_Z_L                                                                          0x2C
#define OUT_Z_H                                                                          0x2D
#define FIFO_CTRL_REG                                                                    0x2E
#define FIFO_SRC_REG                                                                     0x2F
#define INT1_CFG                                                                         0x30
#define INT1_SRC                                                                         0x31
#define INT1_TSH_XH                                                                      0x32
#define INT1_TSH_XL                                                                      0x33
#define INT1_TSH_YH                                                                      0x34
#define INT1_TSH_YL                                                                      0x35
#define INT1_TSH_ZH                                                                      0x36
#define INT1_TSH_ZL                                                                      0x37
#define INT1_DURATION                                                                    0x38
  
  void WriteReg (uint8_t reg, uint8_t value);
  uint8_t ReadReg (uint8_t reg);

// Наименование блока управления
#define PSC_PELLETOR_DEVICE
//#define PSC_START_DEVICE
  

extern const char SoftVersion[]; 
extern const char SoftDate[];
extern char ManufactDate[];
extern char SerialNum[];

// Глобальные переменные
extern uint16_t BatteryVoltage;
extern char* DeviceNamePoint;

// Режимы работы
extern uint8_t MODE_FLAG;
#define SWITCHOFF_MODE      0 // Режим выключен
#define IGNITION_MODE       1 // Режим разжигания
#define CONTROL_MODE        2 // Режим контроль
#define REGULATION_MODE     3 // Режим регуляции
#define EXTINGUISHING_MODE  4 // Режим тушения
#define WAITING_MODE        5 // Режим ожидания
#define IS_SWITCHOFF_MODE     (MODE_FLAG == SWITCHOFF_MODE)
#define ISNT_SWITCHOFF_MODE   (MODE_FLAG != SWITCHOFF_MODE)
#define IS_IGNITION_MODE      (MODE_FLAG == IGNITION_MODE)
#define IS_CONTROL_MODE       (MODE_FLAG == CONTROL_MODE)
#define IS_REGULATION_MODE    (MODE_FLAG == REGULATION_MODE)
#define IS_EXTINGUISHING_MODE (MODE_FLAG == EXTINGUISHING_MODE)
#define IS_WAITING_MODE       (MODE_FLAG == WAITING_MODE)

#define OFF       0 // Отключен
#define ON        1 // Включен

// Коды глобальных флагов
extern uint32_t GLOBAL_FLAGS;

extern uint32_t Frequancy;

extern uint32_t FrameCount;
extern uint8_t  LED_Counter;

extern uint8_t FeedTime_Counter;
extern uint8_t FeedTime_Num;
extern uint8_t BlowingTime_Counter;
extern uint8_t ModeChange_Counter;
extern uint16_t Buzzer_Counter;
extern uint16_t Buzzer_Period_Counter;
extern uint16_t Extinguishing_Counter;
extern uint16_t CirPump_Counter;
extern uint16_t CirPumpStart_Counter;
extern uint16_t Overflow_Counter;
extern uint16_t EnergySaving_Counter;
extern uint16_t ScreenSaver_Counter;
extern uint8_t ScreenSaverChange_Counter;
extern uint16_t AntiSTOP_Counter;
extern uint32_t FreqCount;
extern uint16_t  FreqPoint;
extern uint32_t Period;

extern uint8_t Screw_Sensor_Counter[];
extern uint8_t HWS_Sensor_Counter[];
extern uint8_t AntiSTOPHour_Counter[];
extern uint8_t MODE_Buf[];
extern uint8_t Skip_FLAG;

extern float Power_Fan_mdf;
extern uint8_t Power_Screw;
extern uint8_t Power_HWS;

#define NONE 0x00

#define FLAG_RESET         0
#define FLAG_SET           1

#define SPI_DMA_BUSY       (0x00000001) // Флаг занятости SPI

#define EVENT_1HZ          (0x00000004) // Флаг события 1 Гц
#define EVENT_4HZ          (0x00000008) // Флаг события 4 Гц
#define ADC_READ_FLAG      (0x00000010) // Флаг чтения АЦП
#define BUZZER_NEED        (0x00000020) // Флаг необходимости включения пищалки
#define OVERFLOW_FLAG      (0x00000040) // Флаг включения пересыпки
#define OVERFLOW_START     (0x00000080) // Флаг начала пересыпки
#define OVERFLOW_BUZZER    (0x00000100) // Флаг пищалки в тревоге шнека
#define ENERGY_FLAG        (0x00000200) // Флаг энергосбережения
#define SCREENSAVER_FLAG   (0x00000400) // Флаг хранителя экрана
#define THERMO_FLAG        (0x00001000) // Флаг термостата
#define THERMO_STATE       (0x00002000) // Флаг состояния термостата
#define FIREWOOD_FLAG      (0x00004000) // Флаг режима дров
#define DAYNIGNT_FLAG      (0x00008000) // Флаг режима день/ночь
#define ALARM_FLAG         (0x00010000) // Флаг аварии
#define FAN_FLAG           (0x00020000) // Флаг включения вентилятора
#define SCREW_FLAG         (0x00040000) // Флаг включения шнека
#define CIRPUMP_FLAG       (0x00080000) // Флаг включения насоса циркуляции
#define HWSPUMP_FLAG       (0x00100000) // Флаг включения насоса ГВС
#define HEATFLOOR_FLAG     (0x00200000) // Флаг включения теплого пола
#define CHPUMP_FLAG        (0x00400000) // Флаг включения центрального отопления
#define BUZZER_FLAG        (0x00800000) // Флаг включения пищалки
#define HWS_MORE_SET_FLAG  (0x01000000) // Флаг температура ГВС > температуры котла
#define TEMP_MORE_SET_FLAG (0x02000000) // Флаг температура котла выше заданной
#define CLEAR_TFT_BUF      (0x04000000) // Флаг окончательной очистки буфера

#define ISNT_SPI_DMA_BUSY  ((GLOBAL_FLAGS & SPI_DMA_BUSY) == 0)
#define IS_SPI_DMA_BUSY    ((GLOBAL_FLAGS & SPI_DMA_BUSY) == SPI_DMA_BUSY)
#define SPI_DMA_BUSY_SET   (GLOBAL_FLAGS |= SPI_DMA_BUSY)
#define SPI_DMA_BUSY_RESET (GLOBAL_FLAGS &= ~SPI_DMA_BUSY)
#define ISNT_CLEAR_TFT_BUF ((GLOBAL_FLAGS & CLEAR_TFT_BUF) == 0)
#define IS_CLEAR_TFT_BUF   ((GLOBAL_FLAGS & CLEAR_TFT_BUF) == CLEAR_TFT_BUF)
#define CLEAR_TFT_BUF_SET  (GLOBAL_FLAGS |= CLEAR_TFT_BUF)
#define CLEAR_TFT_BUF_RESET (GLOBAL_FLAGS &= ~CLEAR_TFT_BUF)
#define ISNT_EVENT_1HZ     ((GLOBAL_FLAGS & EVENT_1HZ) == 0)
#define IS_EVENT_1HZ       ((GLOBAL_FLAGS & EVENT_1HZ) == EVENT_1HZ)
#define EVENT_1HZ_SET      (GLOBAL_FLAGS |= EVENT_1HZ)
#define EVENT_1HZ_RESET    (GLOBAL_FLAGS &= ~EVENT_1HZ)
#define ISNT_EVENT_4HZ     ((GLOBAL_FLAGS & EVENT_4HZ) == 0)
#define IS_EVENT_4HZ       ((GLOBAL_FLAGS & EVENT_4HZ) == EVENT_4HZ)
#define EVENT_4HZ_SET      (GLOBAL_FLAGS |= EVENT_4HZ)
#define EVENT_4HZ_RESET    (GLOBAL_FLAGS &= ~EVENT_4HZ)
#define ISNT_ADC_READ_FLAG ((GLOBAL_FLAGS & ADC_READ_FLAG) == 0)
#define IS_ADC_READ_FLAG   ((GLOBAL_FLAGS & ADC_READ_FLAG) == ADC_READ_FLAG)
#define ADC_READ_FLAG_SET  (GLOBAL_FLAGS |= ADC_READ_FLAG)
#define ADC_READ_FLAG_RESET (GLOBAL_FLAGS &= ~ADC_READ_FLAG)
#define ISNT_BUZZER_NEED   ((GLOBAL_FLAGS & BUZZER_NEED) == 0)
#define IS_BUZZER_NEED     ((GLOBAL_FLAGS & BUZZER_NEED) == BUZZER_NEED)
#define BUZZER_NEED_SET    (GLOBAL_FLAGS |= BUZZER_NEED)
#define BUZZER_NEED_RESET  (GLOBAL_FLAGS &= ~BUZZER_NEED)
#define IS_DAY             ((GLOBAL_FLAGS & DAYNIGNT_FLAG) == 0)
#define IS_NIGHT           ((GLOBAL_FLAGS & DAYNIGNT_FLAG) == DAYNIGNT_FLAG)
#define NIGHT_SET          (GLOBAL_FLAGS |= DAYNIGNT_FLAG)
#define DAY_SET            (GLOBAL_FLAGS &= ~DAYNIGNT_FLAG)
#define ISNT_OVERFLOW      ((GLOBAL_FLAGS & OVERFLOW_FLAG) == 0)
#define IS_OVERFLOW        ((GLOBAL_FLAGS & OVERFLOW_FLAG) == OVERFLOW_FLAG)
#define OVERFLOW_ON        (GLOBAL_FLAGS |= OVERFLOW_FLAG)
#define OVERFLOW_OFF       (GLOBAL_FLAGS &= ~OVERFLOW_FLAG)
#define ISNT_OVERFLOW_START ((GLOBAL_FLAGS & OVERFLOW_START) == 0)
#define IS_OVERFLOW_START   ((GLOBAL_FLAGS & OVERFLOW_START) == OVERFLOW_START)
#define OVERFLOW_START_ON   (GLOBAL_FLAGS |= OVERFLOW_START)
#define OVERFLOW_START_OFF  (GLOBAL_FLAGS &= ~OVERFLOW_START)
#define ISNT_OVERFLOW_BUZZER ((GLOBAL_FLAGS & OVERFLOW_BUZZER) == 0)
#define IS_OVERFLOW_BUZZER   ((GLOBAL_FLAGS & OVERFLOW_BUZZER) == OVERFLOW_BUZZER)
#define OVERFLOW_BUZZER_ON   (GLOBAL_FLAGS |= OVERFLOW_BUZZER)
#define OVERFLOW_BUZZER_OFF  (GLOBAL_FLAGS &= ~OVERFLOW_BUZZER)
#define ISNT_ENERGY        ((GLOBAL_FLAGS & ENERGY_FLAG) == 0)
#define IS_ENERGY          ((GLOBAL_FLAGS & ENERGY_FLAG) == ENERGY_FLAG)
#define ENERGY_SET         (GLOBAL_FLAGS |= ENERGY_FLAG)
#define ENERGY_RESET       (GLOBAL_FLAGS &= ~ENERGY_FLAG)
#define ISNT_SCREENSAVER   ((GLOBAL_FLAGS & SCREENSAVER_FLAG) == 0)
#define IS_SCREENSAVER     ((GLOBAL_FLAGS & SCREENSAVER_FLAG) == SCREENSAVER_FLAG)
#define SCREENSAVER_SET    (GLOBAL_FLAGS |= SCREENSAVER_FLAG)
#define SCREENSAVER_RESET  (GLOBAL_FLAGS &= ~SCREENSAVER_FLAG)
#define ISNT_HWS_MORE_SET  ((GLOBAL_FLAGS & HWS_MORE_SET_FLAG) == 0)
#define IS_HWS_MORE_SET    ((GLOBAL_FLAGS & HWS_MORE_SET_FLAG) == HWS_MORE_SET_FLAG)
#define HWS_MORE_SET       (GLOBAL_FLAGS |= HWS_MORE_SET_FLAG)
#define HWS_LESS_SET       (GLOBAL_FLAGS &= ~HWS_MORE_SET_FLAG)

// Дрова
#define ISNT_FIREWOOD      ((GLOBAL_FLAGS & FIREWOOD_FLAG) == 0)
#define IS_FIREWOOD        ((GLOBAL_FLAGS & FIREWOOD_FLAG) == FIREWOOD_FLAG)
#define FIREWOOD_ON        (GLOBAL_FLAGS |= FIREWOOD_FLAG)
#define FIREWOOD_OFF       (GLOBAL_FLAGS &= ~FIREWOOD_FLAG)

// Приоритет бойлера
#define ISNT_BOILER_PRIORITY (sBoilerPriority.Point == OFF)
#define IS_BOILER_PRIORITY   (sBoilerPriority.Point == ON)

// Термостат
#define ISNT_THERMO        ((GLOBAL_FLAGS & THERMO_FLAG) == 0)
#define IS_THERMO          ((GLOBAL_FLAGS & THERMO_FLAG) == THERMO_FLAG)
#define THERMO_ON          (GLOBAL_FLAGS |= THERMO_FLAG)
#define THERMO_OFF         (GLOBAL_FLAGS &= ~THERMO_FLAG)
#define IS_THERMO_OPEN     ((GLOBAL_FLAGS & THERMO_STATE) == 0)
#define IS_THERMO_CLOSE    ((GLOBAL_FLAGS & THERMO_STATE) == THERMO_STATE)
#define THERMO_CLOSE       (GLOBAL_FLAGS |= THERMO_STATE)
#define THERMO_OPEN        (GLOBAL_FLAGS &= ~THERMO_STATE)

// Авария
#define ISNT_ALARM_FLAG    ((GLOBAL_FLAGS & ALARM_FLAG) == 0)
#define IS_ALARM_FLAG      ((GLOBAL_FLAGS & ALARM_FLAG) == ALARM_FLAG)
#define ALARM_ON           (GLOBAL_FLAGS |= ALARM_FLAG); \
                            ALARM_PIN_ON;
#define ALARM_OFF          (GLOBAL_FLAGS &= ~ALARM_FLAG); \
                            ALARM_PIN_OFF;
// Вентилятор
#define ISNT_FAN           ((GLOBAL_FLAGS & FAN_FLAG) == 0)
#define IS_FAN             ((GLOBAL_FLAGS & FAN_FLAG) == FAN_FLAG)
#define FAN_ON             (GLOBAL_FLAGS |= FAN_FLAG); \
                            LED_Fan_ON
#define FAN_OFF            (GLOBAL_FLAGS &= ~FAN_FLAG); \
                            LED_Fan_OFF
// Шнек
#define ISNT_SCREW         ((GLOBAL_FLAGS & SCREW_FLAG) == 0)
#define IS_SCREW           ((GLOBAL_FLAGS & SCREW_FLAG) == SCREW_FLAG)
#define SCREW_ON           (GLOBAL_FLAGS |= SCREW_FLAG); 
#define SCREW_OFF          (GLOBAL_FLAGS &= ~SCREW_FLAG);
                              
// Насос циркуляции
#define ISNT_CIRPUMP       ((GLOBAL_FLAGS & CIRPUMP_FLAG) == 0)
#define IS_CIRPUMP         ((GLOBAL_FLAGS & CIRPUMP_FLAG) == CIRPUMP_FLAG)
#define CIRPUMP_ON         (GLOBAL_FLAGS |= CIRPUMP_FLAG); \
                            LED_Pump_ON; \
                            KV_CirPump_ON;
#define CIRPUMP_OFF        (GLOBAL_FLAGS &= ~ CIRPUMP_FLAG); \
                            LED_Pump_OFF; \
                            KV_CirPump_OFF;
// Насос ГВС
#define ISNT_HWSPUMP       ((GLOBAL_FLAGS & HWSPUMP_FLAG) == 0)
#define IS_HWSPUMP         ((GLOBAL_FLAGS & HWSPUMP_FLAG) == HWSPUMP_FLAG)
#define HWSPUMP_ON         (GLOBAL_FLAGS |= HWSPUMP_FLAG);
#define HWSPUMP_OFF        (GLOBAL_FLAGS &= ~HWSPUMP_FLAG);
                            
// Теплый пол
#define ISNT_HEATFLOOR     ((GLOBAL_FLAGS & HEATFLOOR_FLAG) == 0)
#define IS_HEATFLOOR       ((GLOBAL_FLAGS & HEATFLOOR_FLAG) == HEATFLOOR_FLAG)
#define HEATFLOOR_ON       (GLOBAL_FLAGS |= HEATFLOOR_FLAG)
#define HEATFLOOR_OFF      (GLOBAL_FLAGS &= ~HEATFLOOR_FLAG)
// Насос центрального отопления
#define ISNT_CHPUMP        ((GLOBAL_FLAGS & CHPUMP_FLAG) == 0)
#define IS_CHPUMP          ((GLOBAL_FLAGS & CHPUMP_FLAG) == CHPUMP_FLAG)
#define CHPUMP_ON          (GLOBAL_FLAGS |= CHPUMP_FLAG)
#define CHPUMP_OFF         (GLOBAL_FLAGS &= ~CHPUMP_FLAG)
// Пищалка
#define ISNT_BUZZER        ((GLOBAL_FLAGS & BUZZER_FLAG) == 0)
#define IS_BUZZER          ((GLOBAL_FLAGS & BUZZER_FLAG) == BUZZER_FLAG)
#define BUZZER_ON          (GLOBAL_FLAGS |= BUZZER_FLAG); \
                            Buzzer_TIM = (uint32_t) (Buzzer_TIM_Period*50)
#define BUZZER_OFF         (GLOBAL_FLAGS &= ~BUZZER_FLAG); \
                            Buzzer_TIM = (uint32_t) (Buzzer_TIM_Period*0)

// Коды кнопок
extern uint32_t BUTTONS;
#define But_START       0x00000001 // Start
#define But_STOP        0x00000002 // Stop
#define But_MORE        0x00000004 // Up
#define But_LESS        0x00000008 // Down

#define But_EXT_START   0x00000010 // External Start
#define But_EXT_STOP    0x00000020 // External Stop
#define But_SW          0x00000040 // Discret Input

// Коды предупреждений
extern uint32_t WARNING_FLAGS;
#define ALARM_THERMOSTAT_FLAG  0x00000001 // Аварийный термостат
#define CRIT_TEMP_FLAG         0x00000002 // Критическая температура котла
#define SCREW_TEMP_FLAG        0x00000004 // Перегрев шнека
#define HIGH_TEMP_FLAG         0x00000008 // Высокая температура котла
#define LOW_TEMP_FLAG          0x00000010 // Низкая температура
#define ALARM_THERMO_TRIG_FLAG 0x00000020 // Сработка аварийного термостата
#define HWS_TEMP_FLAG          0x00000040 // Высокая температура ГВС
#define TEMP_SENSOR_FLAG       0x00010000 // Повреждение датчика котла
#define HWS_SENSOR_FLAG        0x00020000 // Повреждение датчика ГВС
#define SCREW_SENSOR_FLAG      0x00040000 // Повреждение датчика шнека
#define HWS_SENSOR_DEAD        0x00100000 // Датчик ГВС не работает 7 дней
#define SCREW_SENSOR_DEAD      0x00200000 // Датчик шнека не работает 7 дней

#define ISNT_ALARM_THERMOSTAT  ((WARNING_FLAGS & ALARM_THERMOSTAT_FLAG) == 0)
#define IS_ALARM_THERMOSTAT    ((WARNING_FLAGS & ALARM_THERMOSTAT_FLAG) == ALARM_THERMOSTAT_FLAG)
#define ALARM_THERMOSTAT_ON    (WARNING_FLAGS |= ALARM_THERMOSTAT_FLAG)
#define ALARM_THERMOSTAT_OFF   (WARNING_FLAGS &= ~ALARM_THERMOSTAT_FLAG)
#define ISNT_ALARM_THERMO_TRIG ((WARNING_FLAGS & ALARM_THERMO_TRIG_FLAG) == 0)
#define IS_ALARM_THERMO_TRIG   ((WARNING_FLAGS & ALARM_THERMO_TRIG_FLAG) == ALARM_THERMO_TRIG_FLAG)
#define ALARM_THERMO_TRIG_SET  (WARNING_FLAGS |= ALARM_THERMO_TRIG_FLAG)
#define ALARM_THERMO_TRIG_RESET (WARNING_FLAGS &= ~ALARM_THERMO_TRIG_FLAG)
#define ISNT_CRIT_TEMP         ((WARNING_FLAGS & CRIT_TEMP_FLAG) == 0)
#define IS_CRIT_TEMP           ((WARNING_FLAGS & CRIT_TEMP_FLAG) == CRIT_TEMP_FLAG)
#define CRIT_TEMP_ON           (WARNING_FLAGS |= CRIT_TEMP_FLAG)
#define CRIT_TEMP_OFF          (WARNING_FLAGS &= ~CRIT_TEMP_FLAG)
#define ISNT_SCREW_TEMP        ((WARNING_FLAGS & SCREW_TEMP_FLAG) == 0)
#define IS_SCREW_TEMP          ((WARNING_FLAGS & SCREW_TEMP_FLAG) == SCREW_TEMP_FLAG)
#define SCREW_TEMP_ON          (WARNING_FLAGS |= SCREW_TEMP_FLAG)
#define SCREW_TEMP_OFF         (WARNING_FLAGS &= ~SCREW_TEMP_FLAG)
#define ISNT_HIGH_TEMP         ((WARNING_FLAGS & HIGH_TEMP_FLAG) == 0)
#define IS_HIGH_TEMP           ((WARNING_FLAGS & HIGH_TEMP_FLAG) == HIGH_TEMP_FLAG)
#define HIGH_TEMP_ON           (WARNING_FLAGS |= HIGH_TEMP_FLAG)
#define HIGH_TEMP_OFF          (WARNING_FLAGS &= ~HIGH_TEMP_FLAG)
#define ISNT_LOW_TEMP          ((WARNING_FLAGS & LOW_TEMP_FLAG) == 0)
#define IS_LOW_TEMP            ((WARNING_FLAGS & LOW_TEMP_FLAG) == LOW_TEMP_FLAG)
#define LOW_TEMP_ON            (WARNING_FLAGS |= LOW_TEMP_FLAG)
#define LOW_TEMP_OFF           (WARNING_FLAGS &= ~LOW_TEMP_FLAG)
#define ISNT_HWS_TEMP          ((WARNING_FLAGS & HWS_TEMP_FLAG) == 0)
#define IS_HWS_TEMP            ((WARNING_FLAGS & HWS_TEMP_FLAG) == HWS_TEMP_FLAG)
#define HWS_TEMP_ON            (WARNING_FLAGS |= HWS_TEMP_FLAG)
#define HWS_TEMP_OFF           (WARNING_FLAGS &= ~HWS_TEMP_FLAG)

#define ISNT_TEMP_SENSOR       ((WARNING_FLAGS & TEMP_SENSOR_FLAG) == 0)
#define IS_TEMP_SENSOR         ((WARNING_FLAGS & TEMP_SENSOR_FLAG) == TEMP_SENSOR_FLAG)
#define TEMP_SENSOR_SET        (WARNING_FLAGS |= TEMP_SENSOR_FLAG)
#define TEMP_SENSOR_RESET      (WARNING_FLAGS &= ~TEMP_SENSOR_FLAG)
#define ISNT_HWS_SENSOR        ((WARNING_FLAGS & HWS_SENSOR_FLAG) == 0)
#define IS_HWS_SENSOR          ((WARNING_FLAGS & HWS_SENSOR_FLAG) == HWS_SENSOR_FLAG)
#define HWS_SENSOR_SET         (WARNING_FLAGS |= HWS_SENSOR_FLAG)
#define HWS_SENSOR_RESET       (WARNING_FLAGS &= ~HWS_SENSOR_FLAG)
#define ISNT_SCREW_SENSOR      ((WARNING_FLAGS & SCREW_SENSOR_FLAG) == 0)
#define IS_SCREW_SENSOR        ((WARNING_FLAGS & SCREW_SENSOR_FLAG) == SCREW_SENSOR_FLAG)
#define SCREW_SENSOR_SET       (WARNING_FLAGS |= SCREW_SENSOR_FLAG)
#define SCREW_SENSOR_RESET     (WARNING_FLAGS &= ~SCREW_SENSOR_FLAG)
#define ISNT_HWS_SENSOR_DEAD   ((WARNING_FLAGS & HWS_SENSOR_DEAD) == 0)
#define IS_HWS_SENSOR_DEAD     ((WARNING_FLAGS & HWS_SENSOR_DEAD) == HWS_SENSOR_DEAD)
#define HWS_SENSOR_DEAD_SET    (WARNING_FLAGS |= HWS_SENSOR_DEAD)
#define HWS_SENSOR_DEAD_RESET  (WARNING_FLAGS &= ~HWS_SENSOR_DEAD)
#define ISNT_SCREW_SENSOR_DEAD ((WARNING_FLAGS & SCREW_SENSOR_DEAD) == 0)
#define IS_SCREW_SENSOR_DEAD   ((WARNING_FLAGS & SCREW_SENSOR_DEAD) == SCREW_SENSOR_DEAD)
#define SCREW_SENSOR_DEAD_SET  (WARNING_FLAGS |= SCREW_SENSOR_DEAD)
#define SCREW_SENSOR_DEAD_RESET (WARNING_FLAGS &= ~SCREW_SENSOR_DEAD)


// Коды графический флагов
extern uint32_t GRAPHIC_FLAGS;
#define BLINK              0x0001 // Флаг мигания
#define NEED_UPDATE        0x0002 // Флаг необходимости обновить пункты меню
#define NEED_UPDATE_HEAD   0x0004 // Флаг необходимости обновить заголовок
#define NEED_UPDATE_SCROLL 0x0008 // Флаг необходимости обновить скролл
#define NEED_UPDATE_MENU   0x0010 // Флаг необходимости обновить после перехода в дочернее меню
#define NEED_UPDATE_LANG   0x0020 // Флаг необходимости обновить после изменения языка
#define NEED_UPDATE_BAT    0x0040 // Флаг необходимости обновления батареи
#define BLINK_BATTERY      0x0080 // Флаг мигания батареей
#define ADC_FLAG           0x0100 // Флаг вывода АЦП
#define MENU_EXIT_FLAG     0x1000 // Флаг выхода из меню

#define ISNT_BLINK               ((GRAPHIC_FLAGS & BLINK) == 0)
#define IS_BLINK                 ((GRAPHIC_FLAGS & BLINK) == BLINK)
#define BLINK_SET                (GRAPHIC_FLAGS |= BLINK)
#define BLINK_RESET              (GRAPHIC_FLAGS &= ~BLINK)
#define ISNT_BLINK_BATTERY       ((GRAPHIC_FLAGS & BLINK_BATTERY) == 0)
#define IS_BLINK_BATTERY         ((GRAPHIC_FLAGS & BLINK_BATTERY) == BLINK_BATTERY)
#define BLINK_BATTERY_SET        (GRAPHIC_FLAGS |= BLINK_BATTERY)
#define BLINK_BATTERY_RESET      (GRAPHIC_FLAGS &= ~BLINK_BATTERY)

#define ISNT_NEED_UPDATE         ((GRAPHIC_FLAGS & NEED_UPDATE) == 0)
#define IS_NEED_UPDATE           ((GRAPHIC_FLAGS & NEED_UPDATE) == NEED_UPDATE)
#define NEED_UPDATE_SET          (GRAPHIC_FLAGS |= NEED_UPDATE)
#define NEED_UPDATE_RESET        (GRAPHIC_FLAGS &= ~NEED_UPDATE)
#define ISNT_NEED_UPDATE_HEAD    ((GRAPHIC_FLAGS & NEED_UPDATE_HEAD) == 0)
#define IS_NEED_UPDATE_HEAD      ((GRAPHIC_FLAGS & NEED_UPDATE_HEAD) == NEED_UPDATE_HEAD)
#define NEED_UPDATE_HEAD_SET     (GRAPHIC_FLAGS |= NEED_UPDATE_HEAD)
#define NEED_UPDATE_HEAD_RESET   (GRAPHIC_FLAGS &= ~NEED_UPDATE_HEAD)
#define ISNT_NEED_UPDATE_SCROLL  ((GRAPHIC_FLAGS & NEED_UPDATE_SCROLL) == 0)
#define IS_NEED_UPDATE_SCROLL    ((GRAPHIC_FLAGS & NEED_UPDATE_SCROLL) == NEED_UPDATE_SCROLL)
#define NEED_UPDATE_SCROLL_SET   (GRAPHIC_FLAGS |= NEED_UPDATE_SCROLL)
#define NEED_UPDATE_SCROLL_RESET (GRAPHIC_FLAGS &= ~NEED_UPDATE_SCROLL)
#define ISNT_NEED_UPDATE_MENU    ((GRAPHIC_FLAGS & NEED_UPDATE_MENU) == 0)
#define IS_NEED_UPDATE_MENU      ((GRAPHIC_FLAGS & NEED_UPDATE_MENU) == NEED_UPDATE_MENU)
#define NEED_UPDATE_MENU_SET     (GRAPHIC_FLAGS |= NEED_UPDATE_MENU)
#define NEED_UPDATE_MENU_RESET   (GRAPHIC_FLAGS &= ~NEED_UPDATE_MENU)
#define ISNT_NEED_UPDATE_LANG    ((GRAPHIC_FLAGS & NEED_UPDATE_LANG) == 0)
#define IS_NEED_UPDATE_LANG      ((GRAPHIC_FLAGS & NEED_UPDATE_LANG) == NEED_UPDATE_LANG)
#define NEED_UPDATE_LANG_SET     (GRAPHIC_FLAGS |= NEED_UPDATE_LANG)
#define NEED_UPDATE_LANG_RESET   (GRAPHIC_FLAGS &= ~NEED_UPDATE_LANG)
#define ISNT_NEED_UPDATE_BAT     ((GRAPHIC_FLAGS & NEED_UPDATE_BAT) == 0)
#define IS_NEED_UPDATE_BAT       ((GRAPHIC_FLAGS & NEED_UPDATE_BAT) == NEED_UPDATE_BAT)
#define NEED_UPDATE_BAT_SET      (GRAPHIC_FLAGS |= NEED_UPDATE_BAT)
#define NEED_UPDATE_BAT_RESET    (GRAPHIC_FLAGS &= ~NEED_UPDATE_BAT)
#define ISNT_ADC_FLAG            ((GRAPHIC_FLAGS & ADC_FLAG) == 0)
#define IS_ADC_FLAG              ((GRAPHIC_FLAGS & ADC_FLAG) == ADC_FLAG)
#define ADC_FLAG_SET             (GRAPHIC_FLAGS |= ADC_FLAG)
#define ADC_FLAG_RESET           (GRAPHIC_FLAGS &= ~ADC_FLAG)
#define ISNT_MENU_EXIT           ((GRAPHIC_FLAGS & MENU_EXIT_FLAG) == 0)
#define IS_MENU_EXIT             ((GRAPHIC_FLAGS & MENU_EXIT_FLAG) == MENU_EXIT_FLAG)
#define MENU_EXIT_SET            (GRAPHIC_FLAGS |= MENU_EXIT_FLAG)
#define MENU_EXIT_RESET          (GRAPHIC_FLAGS &= ~MENU_EXIT_FLAG)

// Режим День/ночь
#define NORMAL    0 // Нормальный режим
#define DAYNIGHT  1 // Режим день/ночь

#define IS_NORMAL   (sDailyMode.Point == NORMAL)   // Нормальный режим
#define IS_DAYNIGHT (sDailyMode.Point == DAYNIGHT) // Режим День/Ночь
#define ISNT_NIGHT  ((IS_NORMAL) || (IS_DAY))

// Термостат
#define IS_THERMO_OFF (sThermostat.Point == OFF) // Термостат выключен
#define IS_THERMO_ON  (sThermostat.Point == ON)  // Термостат включен

// Язык интерфейса
extern uint8_t Language;
#define RUS 0 // русский
#define ENG 1 // английский
#define SER 2 // сербский

// Тип данных времени и даты
#define TIME      0x01
#define DATE      0x02
#define WEEKDAY   0x03

void Error_Handler(void);
void FLASH_BLOCK (void);

void PWR_ProduceSysCounters(void);

void Work_ModeHandler(void);
void Work_FanPowerSet (uint16_t Power);
void Work_SupplyPowerSet (uint16_t Power);
void Work_HWSPumpPowerSet (uint16_t Power);
uint8_t Work_TempRegulationAchieve (void);
uint8_t Work_TempSetAchieve (void);
uint8_t Work_TempFallHyster (void);
void Work_TempOffAchieve (void);
void Work_TempOnAchieve (void);
void Work_FanCycle (void);
void Work_SupplyCycle (void);
void Work_BuzzerShort (void);
void Work_ThermostatCycle (void);
void Work_ExtinguishingEnd (void);
void Work_DayNightCycle(void);
void Work_HWSPumpCycle (void);
void Work_CirPumpCycle (void);
void Work_CirPumpCircThermo (void);
void Work_TempAlarmCycle (void);
void Work_StopEXT(void);

uint32_t FLASH_OB_GetRDP(void);
HAL_StatusTypeDef FLASH_OB_RDP_LevelConfig(uint8_t ReadProtectLevel);
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
