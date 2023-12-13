
/* Includes ------------------------------------------------------------------*/
#include "main.h"

extern uint8_t RepeatButtons20;
extern uint16_t RepeatButtons;
extern uint16_t MoreLessCounter;
extern uint8_t Check_Stage;

/*******************************************************************************
  *
  * ��������� ������ � ����
  *
*******************************************************************************/

// ��������� ������ ���� 
typedef struct PROGMEM{
  void *Parent;         // ��������� �� ������������ ����
  char *MenuNameRus; // ������������ ���� �� �������
  char *MenuNameEng; // ������������ ���� �� ����������
  uint32_t Item;        // ����� ������� ���� � ����� ����������
  uint32_t ItemCount;   // ���������� �������� � ����
  uint32_t Page;        // ����� �������� ����
  uint32_t ItemPerPage; // ���������� ��������� �� ��������
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
#define MENU_BASE_Y    52  // ���������� ������� �������� ����
#define MENU_SHIFT_Y   21  // ����� ����� �������� ����
#define MENU_MAX_POINT 9  // ������������ ����� ������� �� ��������

#define MENU_SCROLL_FULL (TFT_HEIGHT - MENU_BASE_Y - 8) // ������������ ������ ������� � �������� ����
#define MENU_SCROLL_START (MENU_BASE_Y + 3)         // ��������� ���������� ������� � �������� ����
#define MENU_SCROLL_FULL_CHILD (MENU_SCROLL_FULL - MENU_SHIFT_Y)   // ������������ ������ ������� � �������� ����
#define MENU_SCROLL_START_CHILD (MENU_SCROLL_START + MENU_SHIFT_Y) // ��������� ���������� ������� � �������� ����

#define MENU_POSITION(x) MENU_BASE_Y + (x-Menu_SelectedItem->Page)*MENU_SHIFT_Y

#define ISNT_MANUAL_MENU (Menu_SelectedItem != (sMenu*) &ManualControlMenu)

#define NON_ONOFF ((*Element).Value[0][0] != (char*) sOnOffValue[0][0])

// ��� ���������� �������� ���� ��� ����� ����
#define  mActive   (0x4000) // ���� �������, ������ ������� ����
#define  mPassive  (0x0000) // ���� ��������, ������� ���� �� ������

/*******************************************************************************
  *
  * ��������� ������ � ��������� �����������
  *
*******************************************************************************/

// ��������� ��������� ��������� 
typedef struct {
  const char* Name[2];    // ������������ ������ �� ������� � ����������
  int16_t Value;          // �������� �����
  int16_t Min;            // ����������� ��������
  int16_t Max;            // ������������ ��������
  uint8_t Mantissa;       // ����� ������� ��������
  uint8_t Off;            // ���� ���������� ���������
  uint8_t Unit;           // ������� ���������
} sFloat;

#define OFF_STATE    0x01
#define OFF_SUPPORT  0x02
#define IS_OFF(x) ((x.Off & OFF_STATE) == OFF_STATE)
#define ISNT_OFF(x) ((x.Off & OFF_STATE) == 0)
// �������������� � ������ �� �������
#define ALARM_TIME      7

extern sFloat sTemperatureSetting;          // �������� ����������� �����
extern sFloat sSupplyTime;                  // ����� ������ �������
extern sFloat sSupplyPeriod;                // ������ ������ �������
extern sFloat sFanPower;                    // �������� ��������
extern sFloat sSupplyRepetition;            // ���������� �����
extern sFloat sBlowingTime;                 // ����� �������
extern sFloat sBlowingPeriod;               // ������ �������
extern sFloat sFanPowerBlowing;             // �������� ����������� � ��������
extern sFloat sFanPowerFirewood;            // �������� ����������� � ������
extern sFloat sNightCorrection;             // ������ ���������
extern sFloat sTemperatureHWS;              // ����������� ���
extern sFloat sTemperatureSettingMin;       // ����������� ����������� �����
extern sFloat sTemperatureSettingMax;       // ������������ ����������� �����
extern sFloat sHysteresisBoilerSensor;      // ���������� ������� �����
extern sFloat sHysteresisHWSSensor;         // ���������� ������� ���
extern sFloat sFanPowerMin;                 // ����������� �������� �����������
extern sFloat sFanPowerMax;                 // ������������ �������� �����������
extern sFloat sFuelLackTime;                // ����� ����������� �������� �������
extern sFloat sTemperatureHeatingPumpStart; // ����������� ��������� ������ ���������
extern sFloat sTimePumpOff;                 // ����� ���������� ������
extern sFloat sTimePumpOn;                  // ����� ��������� ������
extern sFloat sTemperatureOff;              // ����������� ���������� �����
extern sFloat sTemperatureScrewAlarm;       // ����������� ������� �����
extern sFloat sTimeTransfer;                // ����� �������
extern sFloat sHours;                       // ����
extern sFloat sMinutes;                     // ������
extern sFloat sNightPeriodStartTime;        // ������ ������� �������
extern sFloat sDayPeriodStartTime;          // ������ �������� �������
extern sFloat sEnergySaving;                // ����������������
extern sFloat sScreenSaver;                 // ��������� ������
extern sFloat sBattery;                     // ���������� �������
extern sFloat sFrequency;                   // ������� ����

// ��������� ��������� ��������� �� ������
typedef struct {
  const char* Name[2];       // ������������ ������ �� ������� � ����������
  int16_t     Value;         // �������� �����
  uint8_t     Unit;          // ������� ���������
} sFloatSigned;

extern sFloatSigned sTempHWS;
extern sFloatSigned sTempIn;
extern sFloatSigned sTempScrew;
extern sFloatSigned sTemperatureSet;
extern sFloatSigned sTempNight;
extern sFloatSigned sFanCurrent;
extern sFloatSigned sSupplyCurrent;

// �������� ����� ����������� ����������� Def
#define NON_SIGNED    0    // ��� �����
#define SIGNED        1    // �� ������

// ������� ���������
extern const char UnitSecName[2][4];  // �������
extern const char UnitMinName[2][4];  // ������
extern const char UnitHourName[2][3]; // ����
extern const char UnitPercName[2][2]; // ��������
extern const char UnitDegName[2][2];  // ������� �������
extern const char UnitkHzName[2][4];  // ���������
extern const char UnitHzName[2][3];   // �����
extern const char UnitVoltName[2][2]; // ������
extern const char UnitNullName[2][2]; // ������� ������� ���������

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
  * ��������� ������ � ����������� ������������
  *
*******************************************************************************/

// ��������� ������ ������������
typedef struct {
  const char* Name[2];      // ������������ ������ � ������ �����
  uint8_t Point;            // ��� �������
  uint8_t MaxPoints;        // ���������� ���������
  const char* Value[2][7];  // 7 ����� - ������������ ��������� � ������ �����
} sEnum;

extern const char sOnOffValue[2][2][5];

extern sEnum sLanguage;              // ����
extern sEnum sBrightness;            // �������
extern sEnum sMode;                  // ����� ������
#define IS_AUTO_MODE (sMode.Point == 0)
#define IS_WOOD_MODE (sMode.Point == 1)
extern sEnum sDailyMode;             // ������� �����
extern sEnum sBoilerPriority;        // ��������� �������
#define IS_BOILER_PRIORITY (sBoilerPriority.Point == ON)
#define ISNT_BOILER_PRIORITY (sBoilerPriority.Point == OFF)
extern sEnum sThermostat;            // ��������� ���������
extern sEnum sFanManual;             // ���������� � ������ ����������
extern sEnum sScrewManual;           // ���� � ������ ����������
extern sEnum sCirPumpManual;         // �������������� ����� ��������� � ������ ������

/*******************************************************************************
  *
  * ��������� ������ �� ��������
  *
*******************************************************************************/

typedef struct {
  uint8_t* FontPointer;  // ��������� �� �����
  uint8_t  Bytes;        // ���������� ���� � ������ �������
  uint8_t  Height;       // ������ ������ � ��������
  uint8_t  Width;        // ������ ������ � ��������
  uint8_t  Space;        // ������ ����� ��������� � ��������
} sFont;

extern sFont Calibri;
extern sFont CalibriLarge;
extern sFont Battery_Symbol; 
extern sFont CircuitBreak;
extern sFont CircuitClosure;

/*******************************************************************************
  *
  * ��������� ������ � �������������
  *
*******************************************************************************/

typedef struct {
  uint8_t*  MonoPointer;  // ��������� �� �����-����� �����������
  uint16_t* ImagePointer; // ��������� �� ������� �����������
  uint8_t  Height;        // ������ �����������
  uint8_t  Width;         // ������ �����������
  uint8_t  ImageType;     // ��� �����������:
                          // 0x00 - ������� �����������
                          // 0x01 - ���� �����������
  uint16_t  Color;        // ���� �������� ������� 0x01
  uint16_t  Background;   // ���� �������� ������� 0x00
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

// ��� �����������
#define COLOR_IMG       0x00
#define MONO_IMG        0x01

// Menu.c
void Menu_ProduceParameters (void);
void Menu_ProduceButtonFlags(void);
uint8_t Menu_CorrectFloatUpDown(sFloat *Element, // ������ �� ��������� ��������
                                sMenu *Menu);    // ������ �� ��������� ����
uint8_t Menu_CorrectEnumUpDown(sEnum *Element, // ������ �� ��������� ��������
                               sMenu *Menu);   // ������ �� ��������� ����
uint8_t Menu_CorrectEnumBinUpDown(sEnum *Element, // ������ �� ��������� ��������
                                  sMenu *Menu);      // ������ �� ��������� ����
void Menu_StartStopMoreLess(sMenu* Menu);
void TFT_WriteMenuControl (uint16_t x, uint16_t y,  // ���������� ������ ����
                           sMenu *Menu,             // ������ �� ��������� ���� ����������
                           uint8_t MenuItem,        // ����� �������� ����, �����. ������ ��������
                           sFloat *Dig,             // ������ �� ��������� ��������� �������� (0 - ���� �� ���.)
                           sEnum  *Enum,            // ������ �� ��������� �������� ������������ (0 - ���� �� ���.)
                           uint16_t Color,          // ���� �������� ������ ����
                           uint32_t Background,     // ���� ���� ������ ����
                           uint16_t AlarmColor,     // ���� �������������� (����� ��������� �� �������)
                           sFont font);             // ������ �� ��������� ������
void TFT_WriteMenuString (uint16_t x, int y,       // ���������� ������ ����
                          sMenu *Menu,            // ������ �� ��������� �������� ���� ����������
                          sMenu *ChildMenu,       // ������ �� �������� ����
                          uint8_t MenuItem,       // ����� �������� ����, �����. ������ ��������
                          char *Str,              // ������ �� ��������� ���������� ��������
                          void ConfirmVoid(void), // �������, ���������� �� �������������
                          uint16_t Color,         // ���� �������� ������ ����
                          uint32_t Background,    // ���� ���� ������ ����
                          sFont font);            // ������ �� ��������� ������
void TFT_WriteMenuStringNested (uint16_t x, uint16_t y, // ���������� ������ ����
                                sMenu *Menu,            // ������ �� ��������� ����
                                sMenu *ChildMenu,       // ������ �� ��������� ��������� ����
                                uint8_t MenuItem,       // ����� �������� ����, �����. ������ ��������
                                sEnum *Enum,            // ������ �� ��������� �������� ������������ 
                                char  *str,             // ��������� �� ����� ������ 
                                uint16_t color,         // ���� �������� ������ ����
                                uint32_t background,    // ���� ����
                                sFont font);            // ������ �� ����� 
void TFT_WriteMenuNullText (uint8_t x, uint8_t y,       // ���������� ������
                            sMenu *Menu,                // ������ �� ��������� �������� ����
                            uint8_t MenuItem,           // ����� �������� ����, �����. ������ ��������
                            char *TextName,             // ��������� �� ������������ ������
                            char *Text,                 // ��������� �� ������
                            uint16_t Color,             // ���� �������� ������ ����
                            uint32_t Background,        // ���� ���� ������ ����
                            sFont font);                // ������ �� ��������� ������
void TFT_WriteMenuImage (uint8_t x, uint8_t y,  // ���������� �����������
                         uint8_t align,         // ������������ �� ������
                         sMenu *Menu,           // ������ �� ��������� ����
                         uint8_t MenuItem,      // ����� �������� ����, �����. ������ ��������
                         sImage *img);          // ������ �� ��������� �����������
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
void Menu_FillBatteryLevel (uint16_t x, uint16_t y,  // ���������� ������
                            uint16_t Level,          // �������� ������ ������
                            uint16_t color);         // ���� �������

// Power.c
void RTC_Write(uint8_t DataType,    // ��� ������
               uint8_t x,           // ���������� �� �����������
               uint8_t y,           // ���������� �� ���������
               uint8_t align,       // ������������ �� ������
               uint16_t color,      // ���� ������
               uint32_t background, // ���� ����
               sFont font);         // ������ �� �����

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
void TFT_HDotLine(uint16_t x,          // ��������� ���������� ����� �� �����������
                  uint16_t buf_size,   // ������ ������
                  uint16_t w,          // ������ �����
                  uint8_t size,        // ������� ����� � ��������
                  uint16_t color,      // ���� �����
                  uint32_t background);// ���� ����
void TFT_FillRect(uint16_t x1, 
                  uint16_t y1, 
                  uint16_t x2, 
                  uint16_t y2, 
                  uint16_t color);
void TFT_FillScreen(uint16_t color);
void TFT_ClearMenu(uint16_t color);
void TFT_FillString(uint16_t color);  // ���� �������
void TFT_HLineDivider (uint8_t y,      // ���������� ����� �� ���������
                       uint8_t size,   // ������ �����
                       uint16_t color);// ���� �������
void TFT_HLine(uint16_t x,      // ��������� ���������� ����� �� �����������
               uint8_t y,       // ���������� ����� �� ���������
               uint16_t w,      // ������ �����
               uint8_t size,    // ������� ����� � ��������
               uint16_t color); // ���� �����
void TFT_VLine(uint16_t x,      // ���������� ����� �� �����������
               uint8_t y,       // ��������� ���������� ����� �� ���������
               uint8_t h,       // ������ �����
               uint8_t size,    // ������� ����� � ��������
               uint16_t color); // ���� �����
void TFT_Rectangle(uint16_t x,      // ���������� �������� ������ ���� �� �����������
                   uint8_t y,       // ���������� �������� ������ ���� �� ���������
                   uint16_t w,      // ������ ��������������
                   uint8_t h,       // ������ ��������������
                   uint8_t size,    // ������� ����� �������
                   uint16_t color); // ���� �����
void TFT_DrawImage(uint16_t x,     // ���������� �� �����������
                   uint16_t y,     // ���������� �� ���������
                   uint8_t  retry, // ���� ������� ������ �����������
                   sImage *img);   // ������ �� ��������� �����������

// TFT_String.c
uint16_t TFT_WriteChar(uint16_t x, uint16_t width, // ���������� ������� (������� ����� ����)
                       uint32_t color,       // ���� �������
                       uint32_t background,  // ���� ����
                       char* str,            // ��������� �� ������
                       sFont font);          // ������ �� ��������� ������
uint16_t TFT_PutString(uint16_t x, uint16_t y, // ���������� ������ (������� ����� ����)
                       uint8_t align,          // ������������ ������
                       uint32_t color,         // ���� ��������
                       uint32_t background,    // ���� ����                                             
                       char *string,           // ��������� �� ������
                       sFont font);             // ������ �� ��������� ������
uint16_t TFT_WriteString(uint16_t x, uint16_t y,
                         uint8_t align,
                         uint32_t color,         // ���� ��������
                         uint32_t background,    // ���� ����                                             
                         char *string,           // ��������� �� ������
                         sFont font);            // ������ �� ��������� ������
uint16_t TFT_CalcStringWidth(char *string,   // ������
                             sFont font);    // ��������� �� ��������� ������
char TFT_GetDecimalChar(uint8_t x); // ����� �� 0 �� 9
char* TFT_GetUnitChar (uint8_t UnitType);
uint16_t TFT_PutFloatValue(uint16_t x, uint16_t y, // ���������� (������� ����� ����)
                           uint8_t align,        // ������������
                           int Value,            // �������� ����� 
                           uint8_t Digit,        // ���������� ����� ������
                           uint8_t Mantissa,     // ���������� ������� ������
                           uint8_t Sign,         // ����
                           char* Unit,           // ������� ��������� (�� ���������, ���� ������)
                           uint16_t color,       // ���� ��������
                           uint32_t background,  // ���� ����
                           sFont font);          // ������ �� ��������� ������
uint16_t TFT_WriteFloatValue(uint16_t x, uint16_t y, // ���������� (������� ����� ����)
                             uint8_t align,        // ������������
                             int Value,            // �������� ����� 
                             uint8_t Digit,        // ���������� ����� ������
                             uint8_t Mantissa,     // ���������� ������� ������
                             uint8_t Sign,         // ����
                             char* Unit,           // ������� ��������� (�� ���������, ���� ������)
                             uint16_t color,       // ���� ��������
                             uint32_t background,  // ���� ����
                             sFont font);          // ������ �� ��������� ������
uint16_t TFT_WriteParamName (uint16_t x, uint16_t y,   // ���������� ���������
                             uint8_t align,            // ������������ 
                             sFloatSigned *Element,    // ������ �� ��������� ��������
                             uint16_t color,           // ���� �������� ������
                             uint32_t background,      // ���� ����
                             sFont font);
uint16_t TFT_WriteParamSign (uint16_t x, uint16_t y,   // ���������� ���������
                             uint8_t align,            // ������������ 
                             sFloatSigned *Element,    // ������ �� ��������� ��������
                             uint16_t color,           // ���� �������� ������
                             uint32_t background,      // ���� ����
                             sFont font);
uint16_t TFT_FormatFloat (uint16_t x, uint16_t y, // ���������� ������ (������� ����� ����)
                      uint8_t align,        // ������������ �� ������
                      sFloat *Element,      // ������ �� ��������� �������� ��������������
                      uint8_t ViewCursor,   // ��������� ����������� �������
                      uint16_t color,       // ���� �������� ������
                      uint32_t background,  // ���� ����
                      sFont font);           // ������ �� ��������� ������
uint16_t TFT_WriteMenuDigitalControl (uint16_t x, uint16_t y, // ���������� ��������
                                      uint8_t align,          // ������������ �� ������
                                      sMenu *Menu,            // ������ �� ��������� ���� ����������
                                      uint8_t MenuItem,       // ����� �������� ����, �����. ������ ��������
                                      sFloat *Element,        // ������ �� ��������� �������� ��������������
                                      uint16_t Color,         // ���� �������� �������� ����
                                      uint32_t Background,    // ���� ����
                                      uint16_t AlarmColor,    // ���� �������������� (����� ��������� �� �������), ���� 0 �� ���-��
                                      sFont font);            // ������ �� ��������� ������
uint16_t TFT_WriteMenuEnumControl (uint8_t x, uint8_t y, // ���������� ��������
                                   uint8_t align,        // ������������ �� ������
                                   sMenu *Menu,          // ������ �� ��������� ���� ����������
                                   uint8_t MenuItem,     // ����� �������� ����, �����. ������ ��������
                                   sEnum *Element,       // ������ �� ��������� �������� ��������������
                                   uint16_t Color,       // ���� �������� 
                                   uint32_t Background,  // ���� ���� 
                                   uint16_t AlarmColor,  // ���� �������������� (����� ��������� �� �������), ���� 0 �� ���-��
                                   sFont font);          // ������ �� ��������� ������

// Others
void EEPROM_ParamToBuf (sFloat *Dig,       // ��������� �� �������� ��������
                        sEnum *Enum,       // ��������� �� �������� ������������
                        uint8_t NumOnPage);// ����� ��������� �� �������� 
                                           // �� 0 �� 15 - ��� sFloat
                                           // �� 0 �� 31 - ��� Enum
void EEPROM_ParamFromBuf (sFloat *Dig,       // ��������� �� �������� ��������
                          sEnum *Enum,       // ��������� �� �������� ������������
                          uint8_t NumOnPage);// ����� ��������� �� �������� 
                                             // �� 0 �� 15 - ��� sFloat
                                             // �� 0 �� 31 - ��� Enum