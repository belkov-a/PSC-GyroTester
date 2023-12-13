
/* Includes ------------------------------------------------------------------*/
#include "main.h"

extern uint8_t RepeatButtons20;
extern uint16_t RepeatButtons;
extern uint16_t MoreLessCounter;
extern uint8_t Check_Stage;

/*******************************************************************************
  *
  * Структуры работы с меню
  *
*******************************************************************************/

// Структура данных меню 
typedef struct PROGMEM{
  void *Parent;         // указатель на родительское меню
  char *MenuNameRus; // наименование меню на русском
  char *MenuNameEng; // наименование меню на английском
  uint32_t Item;        // номер элемета меню с битом активности
  uint32_t ItemCount;   // количество элеметов в меню
  uint32_t Page;        // номер страницы меню
  uint32_t ItemPerPage; // количество элементов на страницу
} sMenu;

extern const char MainMenuName[2][19];
extern const char ManualControlMenuName[2][18];
extern const char ServiceMenuName[2][20];
extern const char TimeMenuName[2][14];
extern const char ManufacturerMenuName[2][22];
extern const char DesignerMenuName[2][20];
extern const char DeviceMenuName[2][14];
extern const char ResetSettingsMenuName[2][15];

extern sMenu* Menu_SelectedItem;
extern sMenu  NullMenu;
extern sMenu  CheckWindow;
extern sMenu  LicenseWindow;
extern sMenu  StartWindow;
extern sMenu  MainWindow;
extern sMenu  MainMenu;
extern sMenu  ManualControlMenu;
extern sMenu  ServiceMenu;
extern sMenu  TimeMenu;
extern sMenu  ManufacturerMenu;
extern sMenu  DesignerMenu;
extern sMenu  DeviceMenu;
extern sMenu  ResetSettingsMenu;

#define NULL_ENTRY NullMenu
#define MENU_BASE_Y    52  // координата первого элемента меню
#define MENU_SHIFT_Y   21  // сдвиг между пунктами меню
#define MENU_MAX_POINT 9  // максимальное число пунктов на странице

#define MENU_SCROLL_FULL (TFT_HEIGHT - MENU_BASE_Y - 8) // максимальная высота скролла в основном меню
#define MENU_SCROLL_START (MENU_BASE_Y + 3)         // начальная координата скролла в основном меню
#define MENU_SCROLL_FULL_CHILD (MENU_SCROLL_FULL - MENU_SHIFT_Y)   // максимальная высота скролла в дочернем меню
#define MENU_SCROLL_START_CHILD (MENU_SCROLL_START + MENU_SHIFT_Y) // начальная координата скролла в дочернем меню

#define MENU_POSITION(x) MENU_BASE_Y + (x-Menu_SelectedItem->Page)*MENU_SHIFT_Y

#define ISNT_MANUAL_MENU (Menu_SelectedItem != (sMenu*) &ManualControlMenu)

#define NON_ONOFF ((*Element).Value[0][0] != (char*) sOnOffValue[0][0])

// Бит активности элемента меню или всего окна
#define  mActive   (0x4000) // окно активно, выбран элемент меню
#define  mPassive  (0x0000) // окно пассивно, элемент меню не выбран

/*******************************************************************************
  *
  * Структуры работы с числовыми параметрами
  *
*******************************************************************************/

// Структура числового параметра 
typedef struct {
  const char* Name[2];    // наименование пункта на русском и английском
  int16_t Value;          // значение числа
  int16_t Min;            // минимальное значение
  int16_t Max;            // максимальное значение
  uint8_t Mantissa;       // число дробных разрядов
  uint8_t Off;            // флаг выключения параметра
  uint8_t Unit;           // единица измерения
} sFloat;

#define OFF_STATE    0x01
#define OFF_SUPPORT  0x02
#define IS_OFF(x) ((x.Off & OFF_STATE) == OFF_STATE)
#define ISNT_OFF(x) ((x.Off & OFF_STATE) == 0)
// Предупреждение о выходе за пределы
#define ALARM_TIME      7

extern sFloat sTemperatureSetting;          // Заданная температура котла
extern sFloat sSupplyTime;                  // Время подачи топлива
extern sFloat sSupplyPeriod;                // Период подачи топлива
extern sFloat sFanPower;                    // Мощность продувов
extern sFloat sSupplyRepetition;            // Повторение подач
extern sFloat sBlowingTime;                 // Время продува
extern sFloat sBlowingPeriod;               // Период продува
extern sFloat sFanPowerBlowing;             // Мощность вентилятора в продувах
extern sFloat sFanPowerFirewood;            // Мощность вентилятора в дровах
extern sFloat sNightCorrection;             // Ночная коррекция
extern sFloat sTemperatureHWS;              // Температура ГВС
extern sFloat sTemperatureSettingMin;       // Минимальная температура котла
extern sFloat sTemperatureSettingMax;       // Максимальная температура котла
extern sFloat sHysteresisBoilerSensor;      // Гистерезис датчика котла
extern sFloat sHysteresisHWSSensor;         // Гистерезис датчика ГВС
extern sFloat sFanPowerMin;                 // Минимальная мощность вентилятора
extern sFloat sFanPowerMax;                 // Максимальная мощность вентилятора
extern sFloat sFuelLackTime;                // Время обнаружения нехватки пламени
extern sFloat sTemperatureHeatingPumpStart; // Температура включения насоса отопления
extern sFloat sTimePumpOff;                 // Время отключения насоса
extern sFloat sTimePumpOn;                  // Время включения насоса
extern sFloat sTemperatureOff;              // Температура выключения блока
extern sFloat sTemperatureScrewAlarm;       // Температура тревоги шнека
extern sFloat sTimeTransfer;                // Время засыпки
extern sFloat sHours;                       // Часы
extern sFloat sMinutes;                     // Минуты
extern sFloat sNightPeriodStartTime;        // Начало ночного периода
extern sFloat sDayPeriodStartTime;          // Начало дневного периода
extern sFloat sEnergySaving;                // Энергосбережение
extern sFloat sScreenSaver;                 // Хранитель экрана
extern sFloat sBattery;                     // Напряжение батареи
extern sFloat sFrequency;                   // Частота сети

// Структура числового параметра со знаком
typedef struct {
  const char* Name[2];       // наименование пункта на русском и английском
  int16_t     Value;         // значение числа
  uint8_t     Unit;          // единица измерения
} sFloatSigned;

extern sFloatSigned sTempHWS;
extern sFloatSigned sTempIn;
extern sFloatSigned sTempScrew;
extern sFloatSigned sTemperatureSet;
extern sFloatSigned sTempNight;
extern sFloatSigned sFanCurrent;
extern sFloatSigned sSupplyCurrent;

// Значения флага определения подключения Def
#define NON_SIGNED    0    // без знака
#define SIGNED        1    // со знаком

// Единицы измерения
extern const char UnitSecName[2][4];  // секунды
extern const char UnitMinName[2][4];  // минуты
extern const char UnitHourName[2][3]; // часы
extern const char UnitPercName[2][2]; // проценты
extern const char UnitDegName[2][2];  // градусы цельсия
extern const char UnitkHzName[2][4];  // килогерцы
extern const char UnitHzName[2][3];   // герцы
extern const char UnitVoltName[2][2]; // вольты
extern const char UnitNullName[2][2]; // нулевая единица измерения

#define UNIT_NULL 0
#define UNIT_SEC  1
#define UNIT_MIN  2
#define UNIT_HOUR 3
#define UNIT_PERC 4
#define UNIT_DEG  5
#define UNIT_KHZ  6
#define UNIT_HZ   7
#define UNIT_VOLT 8

/*******************************************************************************
  *
  * Структуры работы с параметрами перечисления
  *
*******************************************************************************/

// Структура данных перечисления
typedef struct {
  const char* Name[2];      // наименование пункта с учетом языка
  uint8_t Point;            // сам элемент
  uint8_t MaxPoints;        // количество элементов
  const char* Value[2][7];  // 7 строк - наименования элементов с учетом языка
} sEnum;

extern const char sOnOffValue[2][2][5];

extern sEnum sLanguage;              // Язык
extern sEnum sBrightness;            // Яркость
extern sEnum sMode;                  // Режим работы
#define IS_AUTO_MODE (sMode.Point == 0)
#define IS_WOOD_MODE (sMode.Point == 1)
extern sEnum sDailyMode;             // Сутоный режим
extern sEnum sBoilerPriority;        // Приоритет бойлера
#define IS_BOILER_PRIORITY (sBoilerPriority.Point == ON)
#define ISNT_BOILER_PRIORITY (sBoilerPriority.Point == OFF)
extern sEnum sThermostat;            // Комнатный термостат
extern sEnum sFanManual;             // Вентилятор в ручном управлении
extern sEnum sScrewManual;           // Шнек в ручном управлении
extern sEnum sCirPumpManual;         // Циркуляционный насос отопления в ручном режиме

/*******************************************************************************
  *
  * Структуры работы со шрифтами
  *
*******************************************************************************/

typedef struct {
  uint8_t* FontPointer;  // указатель на шрифт
  uint8_t  Bytes;        // количество байт в высоте символа
  uint8_t  Height;       // высота шрифта в пикселах
  uint8_t  Width;        // ширина шрифта в пикселах
  uint8_t  Space;        // отступ между символами в пикселах
} sFont;

extern sFont Calibri;
extern sFont CalibriLarge;
extern sFont Battery_Symbol; 
extern sFont CircuitBreak;
extern sFont CircuitClosure;

/*******************************************************************************
  *
  * Структуры работы с изображениями
  *
*******************************************************************************/

typedef struct {
  uint8_t*  MonoPointer;  // указатель на черно-белое изображение
  uint16_t* ImagePointer; // указатель на цветное изображение
  uint8_t  Height;        // высота изображения
  uint8_t  Width;         // ширина изображения
  uint8_t  ImageType;     // Тип изображения:
                          // 0x00 - Цветное изображение
                          // 0x01 - Моно изображение
  uint16_t  Color;        // Цвет элемента массива 0x01
  uint16_t  Background;   // Цвет элемента массива 0x00
} sImage;

extern sImage logo;
extern sImage logo_pelletor;
extern sImage AlarmImg;
extern sImage CircularPumpImg;
extern sImage FanImg;
extern sImage ScrewImg;
extern sImage HWSpumpImg;
extern sImage ThermostatOpenImg;
extern sImage ThermostatCloseImg;
extern sImage FirewoodImg;
extern sImage BoilerImg;
extern sImage AlarmThermostatImg;
extern sImage DayImg;
extern sImage NightImg;

// Тип изображения
#define COLOR_IMG       0x00
#define MONO_IMG        0x01

// Menu.c
void Menu_ProduceParameters (void);
void Menu_ProduceButtonFlags(void);
uint8_t Menu_CorrectFloatUpDown(sFloat *Element, // ссылка на структуру элемента
                                sMenu *Menu);    // ссылка на структуру меню
uint8_t Menu_CorrectEnumUpDown(sEnum *Element, // ссылка на структуру элемента
                               sMenu *Menu);   // ссылка на структуру меню
uint8_t Menu_CorrectEnumBinUpDown(sEnum *Element, // ссылка на структуру элемента
                                  sMenu *Menu);      // ссылка на структуру меню
void Menu_StartStopMoreLess(sMenu* Menu);
void TFT_WriteMenuControl (uint16_t x, uint16_t y,  // координаты пункта меню
                           sMenu *Menu,             // ссылка на структуру меню управления
                           uint8_t MenuItem,        // номер элемента меню, соотв. выбору элемента
                           sFloat *Dig,             // ссылка на структуру числового элемента (0 - если не исп.)
                           sEnum  *Enum,            // ссылка на структуру элемента перечисления (0 - если не исп.)
                           uint16_t Color,          // цвет символов пункта меню
                           uint32_t Background,     // цвет фона пункта меню
                           uint16_t AlarmColor,     // цвет предупреждений (выход параметра за границу)
                           sFont font);             // ссылка на структуру шрифта
void TFT_WriteMenuString (uint16_t x, int y,       // координаты пункта меню
                          sMenu *Menu,            // ссылка на структуру текущего меню управления
                          sMenu *ChildMenu,       // ссылка на дочернее меню
                          uint8_t MenuItem,       // номер элемента меню, соотв. выбору элемента
                          char *Str,              // ссылка на структуру строкового элемента
                          void ConfirmVoid(void), // функция, вызываемая по подтверждению
                          uint16_t Color,         // цвет символов пункта меню
                          uint32_t Background,    // цвет фона пункта меню
                          sFont font);            // ссылка на структуру шрифта
void TFT_WriteMenuStringNested (uint16_t x, uint16_t y, // координаты пункта меню
                                sMenu *Menu,            // ссылка на структуру меню
                                sMenu *ChildMenu,       // ссылка на структуру дочернего меню
                                uint8_t MenuItem,       // номер элемента меню, соотв. выбору элемента
                                sEnum *Enum,            // ссылка на структуру элемента перечисления 
                                char  *str,             // указатель на текст пункта 
                                uint16_t color,         // цвет символов пункта меню
                                uint32_t background,    // цвет фона
                                sFont font);            // ссылка на шрифт 
void TFT_WriteMenuNullText (uint8_t x, uint8_t y,       // координаты текста
                            sMenu *Menu,                // ссылка на структуру текущего меню
                            uint8_t MenuItem,           // номер элемента меню, соотв. выбору элемента
                            char *TextName,             // указатель на наименование пункта
                            char *Text,                 // указатель на строку
                            uint16_t Color,             // цвет символов пункта меню
                            uint32_t Background,        // цвет фона пункта меню
                            sFont font);                // ссылка на структуру шрифта
void TFT_WriteMenuImage (uint8_t x, uint8_t y,  // координаты изображения
                         uint8_t align,         // выравнивание по ширине
                         sMenu *Menu,           // ссылка на структуру меню
                         uint8_t MenuItem,      // номер элемента меню, соотв. выбору элемента
                         sImage *img);          // ссылка на структуру изображения
void Menu_ScreenSaver(void);
void Menu_ResetSettings(void);
void Menu_DrawLoadBar (void);
void Menu_AlarmBackground (void);

// Windows.c
void Menu_DrawHeaderPanel(void);
void Window_EnterEscapeUpDown (sMenu *Menu);
uint8_t Menu_DrawWindows(sMenu* Item);
void Menu_DrawLicenseWindow(void);
void Menu_DrawStartWindow(void);
void Menu_DrawMainWindow(void);
void Menu_DrawTemplate(sMenu* Item);
void Menu_DrawHead(sMenu* Item);
uint8_t Menu_DrawSettings(sMenu* Item);
void Menu_DrawScroll(sMenu *Menu);
void Menu_DrawBattery (uint16_t x, uint16_t y);
void Menu_FillBatteryLevel (uint16_t x, uint16_t y,  // координаты уровня
                            uint16_t Level,          // значение уровня заряда
                            uint16_t color);         // цвет заливки

// Power.c
void RTC_Write(uint8_t DataType,    // тип данных
               uint8_t x,           // координата по горизонтали
               uint8_t y,           // координата по вертикали
               uint8_t align,       // выравнивание по ширине
               uint16_t color,      // цвет шрифта
               uint32_t background, // цвет фона
               sFont font);         // ссылка на шрифт

// ILI9341_StartUp.c
void TFT_ILI9341_SendCommand(uint8_t cmd);
void TFT_ILI9341_SendData(uint8_t data);
void TFT_WriteData(uint8_t* buff,
                   size_t buff_size);
void TFT_SendBuffer(uint8_t* data,
                    uint32_t size);
void TFT_ClearBuffer(uint8_t* data,
                     uint32_t Size);
void TFT_HardReset(void);
void TFT_ILI9341_Init(void);
void TFT_ILI9341_SetAddrWindow(uint16_t x0, 
                               uint16_t y0, 
                               uint16_t x1, 
                               uint16_t y1);
void TFT_LED_Set_Brightness (uint16_t Brightness);

// TFT_Drawing.c
void TFT_DrawPixel(uint16_t x,
                   uint16_t y,
                   uint16_t color);
void TFT_HDotLine(uint16_t x,          // начальная координата линии по горизонтали
                  uint16_t buf_size,   // ширина буфера
                  uint16_t w,          // ширина линии
                  uint8_t size,        // толщина линии в пикселах
                  uint16_t color,      // цвет линии
                  uint32_t background);// цвет фона
void TFT_FillRect(uint16_t x1, 
                  uint16_t y1, 
                  uint16_t x2, 
                  uint16_t y2, 
                  uint16_t color);
void TFT_FillScreen(uint16_t color);
void TFT_ClearMenu(uint16_t color);
void TFT_FillString(uint16_t color);  // цвет заливки
void TFT_HLineDivider (uint8_t y,      // координата линии по вертикали
                       uint8_t size,   // ширина линии
                       uint16_t color);// цвет заливки
void TFT_HLine(uint16_t x,      // начальная координата линии по горизонтали
               uint8_t y,       // координата линии по вертикали
               uint16_t w,      // ширина линии
               uint8_t size,    // толщина линии в пикселах
               uint16_t color); // цвет линии
void TFT_VLine(uint16_t x,      // координата линии по горизонтали
               uint8_t y,       // начальная координата линии по вертикали
               uint8_t h,       // высота линии
               uint8_t size,    // толщина линии в пикселах
               uint16_t color); // цвет линии
void TFT_Rectangle(uint16_t x,      // координата верхнего левого угла по горизонтали
                   uint8_t y,       // координата верхнего левого угла по вертикали
                   uint16_t w,      // ширина прямоугольника
                   uint8_t h,       // высота прямоугольника
                   uint8_t size,    // толщина линии стороны
                   uint16_t color); // цвет линий
void TFT_DrawImage(uint16_t x,     // координата по горизонтали
                   uint16_t y,     // координата по вертикали
                   uint8_t  retry, // флаг повтора вывода изображения
                   sImage *img);   // ссылка на структуру изображения

// TFT_String.c
uint16_t TFT_WriteChar(uint16_t x, uint16_t width, // координаты символа (верхний левый угол)
                       uint32_t color,       // цвет символа
                       uint32_t background,  // цвет фона
                       char* str,            // указатель на символ
                       sFont font);          // ссылка на структуру шрифта
uint16_t TFT_PutString(uint16_t x, uint16_t y, // координаты строки (верхний левый угол)
                       uint8_t align,          // выравнивание строки
                       uint32_t color,         // цвет символов
                       uint32_t background,    // цвет фона                                             
                       char *string,           // указатель на строку
                       sFont font);             // ссылка на структуру шрифта
uint16_t TFT_WriteString(uint16_t x, uint16_t y,
                         uint8_t align,
                         uint32_t color,         // цвет символов
                         uint32_t background,    // цвет фона                                             
                         char *string,           // указатель на строку
                         sFont font);            // ссылка на структуру шрифта
uint16_t TFT_CalcStringWidth(char *string,   // строка
                             sFont font);    // указатель на структуру шрифта
char TFT_GetDecimalChar(uint8_t x); // число от 0 до 9
char* TFT_GetUnitChar (uint8_t UnitType);
uint16_t TFT_PutFloatValue(uint16_t x, uint16_t y, // координата (верхний левый угол)
                           uint8_t align,        // выравнивание
                           int Value,            // значение числа 
                           uint8_t Digit,        // количество целых знаков
                           uint8_t Mantissa,     // количество дробных знаков
                           uint8_t Sign,         // знак
                           char* Unit,           // единица измерения (не выводится, если пустая)
                           uint16_t color,       // цвет символов
                           uint32_t background,  // цвет фона
                           sFont font);          // ссылка на структуру шрифта
uint16_t TFT_WriteFloatValue(uint16_t x, uint16_t y, // координата (верхний левый угол)
                             uint8_t align,        // выравнивание
                             int Value,            // значение числа 
                             uint8_t Digit,        // количество целых знаков
                             uint8_t Mantissa,     // количество дробных знаков
                             uint8_t Sign,         // знак
                             char* Unit,           // единица измерения (не выводится, если пустая)
                             uint16_t color,       // цвет символов
                             uint32_t background,  // цвет фона
                             sFont font);          // ссылка на структуру шрифта
uint16_t TFT_WriteParamName (uint16_t x, uint16_t y,   // координаты параметра
                             uint8_t align,            // выравнивание 
                             sFloatSigned *Element,    // ссылка на структуру элемента
                             uint16_t color,           // цвет символов строки
                             uint32_t background,      // цвет фона
                             sFont font);
uint16_t TFT_WriteParamSign (uint16_t x, uint16_t y,   // координаты параметра
                             uint8_t align,            // выравнивание 
                             sFloatSigned *Element,    // ссылка на структуру элемента
                             uint16_t color,           // цвет символов строки
                             uint32_t background,      // цвет фона
                             sFont font);
uint16_t TFT_FormatFloat (uint16_t x, uint16_t y, // координаты строки (верхний левый угол)
                      uint8_t align,        // выравнивание по ширине
                      sFloat *Element,      // ссылка на структуру элемента редактирования
                      uint8_t ViewCursor,   // индикатор отображения курсора
                      uint16_t color,       // цвет символов строки
                      uint32_t background,  // цвет фона
                      sFont font);           // ссылка на структуру шрифта
uint16_t TFT_WriteMenuDigitalControl (uint16_t x, uint16_t y, // координаты элемента
                                      uint8_t align,          // выравнивание по ширине
                                      sMenu *Menu,            // ссылка на структуру меню управления
                                      uint8_t MenuItem,       // номер элемента меню, соотв. выбору элемента
                                      sFloat *Element,        // ссылка на структуру элемента редактирования
                                      uint16_t Color,         // цвет символов элемента меню
                                      uint32_t Background,    // цвет фона
                                      uint16_t AlarmColor,    // цвет предупреждений (выход параметра на границу), если 0 не исп-ся
                                      sFont font);            // ссылка на структуру шрифта
uint16_t TFT_WriteMenuEnumControl (uint8_t x, uint8_t y, // координаты элемента
                                   uint8_t align,        // выравнивание по ширине
                                   sMenu *Menu,          // ссылка на структуру меню управления
                                   uint8_t MenuItem,     // номер элемента меню, соотв. выбору элемента
                                   sEnum *Element,       // ссылка на структуру элемента редактирования
                                   uint16_t Color,       // цвет символов 
                                   uint32_t Background,  // цвет фона 
                                   uint16_t AlarmColor,  // цвет предупреждений (выход параметра за границу), если 0 не исп-ся
                                   sFont font);          // ссылка на структуру шрифта

// Others
void EEPROM_ParamToBuf (sFloat *Dig,       // указатель на числовой параметр
                        sEnum *Enum,       // указатель на параметр перечисления
                        uint8_t NumOnPage);// номер параметра на странице 
                                           // от 0 до 15 - для sFloat
                                           // от 0 до 31 - для Enum
void EEPROM_ParamFromBuf (sFloat *Dig,       // указатель на числовой параметр
                          sEnum *Enum,       // указатель на параметр перечисления
                          uint8_t NumOnPage);// номер параметра на странице 
                                             // от 0 до 15 - для sFloat
                                             // от 0 до 31 - для Enum