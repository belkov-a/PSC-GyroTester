
#include "main.h"
#include "Menu.h"
#include "ILI9341.h"
#include "Init.h"
#include "adc.h"
#include "24C_EEPROM.h"

/*******************************************************************************
  *
  * ��������� ���� ����
  *
*******************************************************************************/

uint8_t Menu_DrawWindows (sMenu* Item)
{
  // �������� ��������������� ��
  if (Item == (sMenu*) &LicenseWindow)
  {
    // ��������� �������� ��������������� ��
    Menu_DrawLicenseWindow();
  }
  else
  // ��������� ��������
  if (Item == (sMenu*) &StartWindow)
  {
    // ��������� ��������� ��������
    Menu_DrawStartWindow();
    // ����� ������ ����������
    NEED_UPDATE_RESET;
    NEED_UPDATE_HEAD_RESET;
    // ������� �� ������
    Window_EnterEscapeUpDown (Item);
  }
  else
  if (Item == (sMenu*) &MainWindow)
  {
    // ������� ���� ��� ���������������
    Menu_AlarmBackground();
    
    // ��������� ������ ��������
    Menu_DrawLoadBar();

    // ��������� �������
    Menu_DrawMainWindow();
    
    // ������� �� ������
    Window_EnterEscapeUpDown (Item);
    // ����� ������ ���������� 
    NEED_UPDATE_HEAD_RESET;
  }
  else
  // ���� ��������
  { 
    if (IS_NEED_UPDATE_MENU)
    {
      NEED_UPDATE_SET;
      NEED_UPDATE_HEAD_SET;
      NEED_UPDATE_SCROLL_SET;
      NEED_UPDATE_MENU_RESET;
    }
    // ��������� ��������� ����������
    Menu_ProduceParameters();
    // ��������� ������ � ����
    Menu_StartStopMoreLess(Item);
    if (Menu_SelectedItem == (sMenu*) &StartWindow) return 1;
    // ��������� ������� ����
    Menu_DrawTemplate(Menu_SelectedItem);
    // ��������� ��������� ��������
    Menu_DrawHead(Menu_SelectedItem);
    // ��������� ������
    Menu_DrawScroll(Menu_SelectedItem); 
    // ��������� ����
    Menu_DrawSettings(Menu_SelectedItem);
    // ����� ������ ����������
    NEED_UPDATE_RESET;
    NEED_UPDATE_HEAD_RESET;
    NEED_UPDATE_SCROLL_RESET;
    NEED_UPDATE_LANG_RESET;
  }
  
  return 0;
}

/*******************************************************************************
  *
  * �������� ����� ������
  *
*******************************************************************************/
uint16_t MoreLessCounter;

void Window_EnterEscapeUpDown (sMenu *Menu)
{
  int Act,Res;
  Act  = (*Menu).Item & mActive;
  Res  = (*Menu).Item & ~mActive;
  
  // ���� �� ������� ���� ����������������
  if (ISNT_ENERGY && ISNT_SCREENSAVER)
  {
    if (BUTTONS > 0) {EnergySaving_Counter = 0;ScreenSaver_Counter = 0;ScreenSaverChange_Counter = 0;}
    
    // ���� �������� ������� ����� � �������, ����� ������� ��������� �������
    if ((BUTTONS > 0) && (IS_OVERFLOW_BUZZER)) OVERFLOW_BUZZER_OFF;
    
    // ���� �� ������ ������, �� ���������� ���� ������ �� ����
    // ������������ ��� ������� ������ �� ������
    if ((BUTTONS == 0) && (RepeatButtons == 0)) MENU_EXIT_RESET;
    
    // ���� �������� ����
    if (Menu == (sMenu*)&MainWindow)
    {
      switch (BUTTONS)
      {
        // ����, �������� � ������� ����� ��������
        case (But_STOP):
          if (ISNT_SWITCHOFF_MODE)
          {
            // ���� ������������ � �� ����� �� ����, ��������� � ��������
            if ((RepeatButtons >= 1000) && ISNT_MENU_EXIT)
            {
              MoreLessCounter = 0;
              MODE_FLAG = SWITCHOFF_MODE;
              MODE_Buf[0] = MODE_FLAG;
              EEPROM_WriteByte ((uint8_t*) MODE_Buf,MODE_FLAG_ADDR);
              ADC_FLAG_SET;
              NEED_UPDATE_BAT_SET;  
              BUTTONS = 0;
            }
          }

          // ������ ���������
          if (IS_OVERFLOW)
          {
            Overflow_Counter = 0;
            OVERFLOW_OFF;
          }
          break;
        // ������, �������� ������ � ������ ��������
        case (But_LESS):
            // ���� ������������, ��������� �����
            if (RepeatButtons >= 1000) MoreLessCounter += 15;
            // ���� ����� ����������� ��������� � ��������� ��������� 
            if (MoreLessCounter > 314) 
            {
              //MoreLessCounter = 0;
              if (IS_SWITCHOFF_MODE || IS_WAITING_MODE)
                Menu_SelectedItem = (sMenu*)&ServiceMenu;
              else
                Menu_SelectedItem = (sMenu*)&MainMenu;   
              BACK_COLOR = BLACK;
              TFT_FillScreen(BACK_COLOR);
              NEED_UPDATE_MENU_SET;
              NEED_UPDATE_BAT_SET;
            }
            break;        
        // ������ 
        case (But_MORE):
          // ���� ������������, ��������� �����
          if (RepeatButtons >= 1000) MoreLessCounter += 15;
          // ���� ����� ����������� ��������� � �������� ���������
          if (MoreLessCounter > 314) 
          {
            //MoreLessCounter = 0;
            Menu_SelectedItem = (sMenu*)&MainMenu;
            BACK_COLOR = BLACK;
            TFT_FillScreen(BACK_COLOR);
            NEED_UPDATE_MENU_SET;
            NEED_UPDATE_BAT_SET;
          }
          break;
        // �����
        case (But_START):
          // ���� �������� ��� ��������, ��������� � ������, ���� �����������
          // ���� ��������, ����� ��������
          // �� ���������, ���� �������� ��������� ��������� � ����������� 
          // �� ���������, ���� ��������� ������ �����
          // �� ���������, ���� ������ 7 ���� ����� ����������� ������� �����
          // �� ���������, ���� ������� ������� ����
          if (ISNT_ALARM_THERMO_TRIG)
          {
            if (Stop_EXT_State == GPIO_PIN_RESET)
            {
              if ((IS_SWITCHOFF_MODE || IS_WAITING_MODE || IS_EXTINGUISHING_MODE) &&
                   ISNT_TEMP_SENSOR)
              {
                if (((IS_SCREW_SENSOR_DEAD && ISNT_OFF(sTemperatureScrewAlarm))
                     || (IS_HWS_SENSOR_DEAD && ISNT_OFF(sTemperatureHWS))) && 
                    IS_AUTO_MODE) {}
                else 
                {
                  if (sTempIn.Value >= sTemperatureSetting.Value * 10) 
                  {
                    MODE_FLAG = CONTROL_MODE;
                    FeedTime_Counter = sBlowingTime.Value; // ����� ��������� 1 ������ � ��������
                    FeedTime_Num = sSupplyRepetition.Value + 1; // �� ������ ������
                    BlowingTime_Counter = sBlowingTime.Value;
                    CirPumpStart_Counter = 25; // �������� ����� �� 25 ��� 
                    CirPump_Counter = 0;
                  }
                  else MODE_FLAG = IGNITION_MODE;
                  
                  MODE_Buf[0] = MODE_FLAG;
                  EEPROM_WriteByte ((uint8_t*) MODE_Buf,MODE_FLAG_ADDR);
                }
              }
            }
          }
          break;
      }
      (*Menu).Item = Res | Act;
    }
    
    // ���� ��������� ��������, �� ������� ��������� � �������� ������
    if ((Menu == (sMenu*)&StartWindow) && (BUTTONS > 0))
    {
      //Act &= ~mActive;
      Menu_SelectedItem = (sMenu*)&MainWindow;
      TFT_FillScreen(BACK_COLOR);
      NEED_UPDATE_HEAD_SET;
      NEED_UPDATE_BAT_SET;
      BUTTONS = 0;
      (*Menu).Item = Res | Act;
    }
  }
  else
  // ���� � ���������������� ��� � ���������, �� ������� ������ �������� ���������
  // ��� ���������� ���������� ������ �� ������� ������ ���� ��������������
  {
    // ��������� ���������
    if (IS_ENERGY && (BUTTONS > 0))
    {
      TFT_LED_Set_Brightness(100);      
      ENERGY_RESET;
      
      BUTTONS = 0;
      EnergySaving_Counter = 0;
    }
    
    // ����� �� ���������
    if (IS_SCREENSAVER && ((BUTTONS > 0) || (WARNING_FLAGS > 0)))
    {
      // ������� �������
      TFT_FillScreen(BACK_COLOR);
      // ������� ����� ������������� ����������
      NEED_UPDATE_MENU_SET;
      NEED_UPDATE_SET;
      NEED_UPDATE_LANG_SET;
      NEED_UPDATE_BAT_SET;
      
      SCREENSAVER_RESET;
      
      BUTTONS = 0;
      ScreenSaver_Counter = 0;
      ScreenSaverChange_Counter = 0;
    }  
  }
}

/*******************************************************************************
  *
  * ��������� ������� ������
  *
*******************************************************************************/

void Menu_DrawHeaderPanel(void)
{
  // ���� ��������� ��������
  if (ISNT_SCREENSAVER)
  {  
    // ��������� �������� ������� �� ������
    RTC_Write(TIME,0,2,C_ALIGN,TEXT_COLOR,NO_BACKGROUND,Calibri);
    
    // ��������� �������
    Menu_DrawBattery (289, 2);
       
    // ������ ������
    if      (IS_ALARM_FLAG)   TFT_DrawImage(3, 2, 1, (sImage*) &AlarmImg);
    else if (ISNT_ALARM_FLAG) TFT_FillRect (3, 2, AlarmImg.Width, AlarmImg.Height, BACK_COLOR);

    // ������ �����������
    if      (IS_FAN)          TFT_DrawImage(22, 2, 1, (sImage*) &FanImg);
    else if (ISNT_FAN)        TFT_FillRect (22, 2, FanImg.Width, FanImg.Height, BACK_COLOR);
    
    // ������ �����
    if      (IS_SCREW)        TFT_DrawImage(41, 2, 1, (sImage*) &ScrewImg);
    else if (ISNT_SCREW)      TFT_FillRect (41, 2, ScrewImg.Width, ScrewImg.Height, BACK_COLOR);
     
    // ������ ������ ���
    if      (IS_HWSPUMP)      TFT_DrawImage(60, 2, 1, (sImage*) &HWSpumpImg);
    else if (ISNT_HWSPUMP)    TFT_FillRect (60, 2, HWSpumpImg.Width, HWSpumpImg.Height, BACK_COLOR);
    
    // ������ ��������������� ������ ���������
    if      (IS_CIRPUMP)      TFT_DrawImage(79, 2, 1, (sImage*) &CircularPumpImg);
    else if (ISNT_CIRPUMP)    TFT_FillRect (79, 2, CircularPumpImg.Width, CircularPumpImg.Height, BACK_COLOR);
    
    // ������ ���������� ����������
    if      (IS_ALARM_THERMO_TRIG)   TFT_DrawImage(98, 2, 1, (sImage*) &AlarmThermostatImg);
    else if (ISNT_ALARM_THERMO_TRIG) TFT_FillRect (98, 2, AlarmThermostatImg.Width, AlarmThermostatImg.Height, BACK_COLOR);
    
    // ������ ������ �� ������
    if      (IS_FIREWOOD)     TFT_DrawImage(210, 2, 1, (sImage*) &FirewoodImg);
    else if (ISNT_FIREWOOD)   TFT_FillRect (210, 2, FirewoodImg.Width, FirewoodImg.Height, BACK_COLOR);
    
    // ������ ����������
    if (IS_THERMO)
    {
      if (IS_THERMO_OPEN)       TFT_DrawImage(229, 2, 1, (sImage*) &ThermostatOpenImg);
      else if (IS_THERMO_CLOSE) TFT_DrawImage(229, 2, 1, (sImage*) &ThermostatCloseImg);
    }
    else if (ISNT_THERMO)       TFT_FillRect (229, 2, ThermostatOpenImg.Width, ThermostatOpenImg.Height, BACK_COLOR);
    
    // ������ �������
    if (IS_BOILER_PRIORITY)        TFT_DrawImage(248, 2, 1, (sImage*) &BoilerImg);
    else if (ISNT_BOILER_PRIORITY) TFT_FillRect (248, 2, BoilerImg.Width, BoilerImg.Height, BACK_COLOR);
    
    // ������ ���/����
    if (IS_DAYNIGHT)
    {
      if      (IS_DAY)   TFT_DrawImage(267, 2, 1, (sImage*) &DayImg);
      else if (IS_NIGHT) TFT_DrawImage(267, 2, 1, (sImage*) &NightImg);
    }
    else TFT_FillRect (267, 2, DayImg.Width, DayImg.Height, BACK_COLOR);
  }
}

/*******************************************************************************
  *
  * ��������� �������� ��������������� ��
  *
*******************************************************************************/

const char LicenseTitle[2][15] = {"��������������","UNLICENSED"};
const char LicenseTitle1[2][9] = {"��","SOFTWARE"};

void Menu_DrawLicenseWindow(void)
{  
  // �������������� ����������� �����������
  TFT_WriteString(0,95, C_ALIGN,HEAD_COLOR,BACK_COLOR,(char*) LicenseTitle[Language],Calibri); 
  TFT_WriteString(0,135,C_ALIGN,HEAD_COLOR,BACK_COLOR,(char*) LicenseTitle1[Language],Calibri);
}

/*******************************************************************************
  *
  * ��������� ��������� ��������
  *
  * ������ �������, ������������ ����� ����������, ������ � ���� ��, ����
  *
*******************************************************************************/

#ifdef PSC_START_DEVICE
const char DeviceTitleStart[2][25] = {"���������� ����������","Pellet boiler controller"};
const char DeviceTitleStart1[2][16] = {"����� ''�����''","''START''"};
#endif
#ifdef PSC_PELLETOR_DEVICE
const char DeviceTitlePelletor[2][16] = {"��������� �����","Pellet boiler"};
const char DeviceTitlePelletor1[2][9] = {"PELLETOR","PELLETOR"};
#endif
const char SoftVerStart[2][6]  = {"���. ","Ver. "};
const char SoftDateStart[2][7] = {" �� "," from "};
const char WebsiteTermokub[2][16] = {"www.termokub.ru","www.termokub.ru"};

void Menu_DrawStartWindow(void)
{  
  // ��������� ������� ��������, ���� �� ��������� ������
  if (ISNT_SCREENSAVER)
  {
    sImage *DeviceLogo;
    char *Title1;
    char *Title2;
    #ifdef PSC_PELLETOR_DEVICE
    DeviceLogo = &logo_pelletor;
    Title1 = (char*) DeviceTitlePelletor[Language];
    Title2 = (char*) DeviceTitlePelletor1[Language];
    #endif
    #ifdef PSC_START_DEVICE
    DeviceLogo = &logo;
    Title1 = (char*) DeviceTitleStart[Language];
    Title2 = (char*) DeviceTitleStart1[Language];
    #endif
    
    // ������� ��������
    TFT_WriteMenuImage(0, 30, C_ALIGN, Menu_SelectedItem, 1,DeviceLogo);
    
    if (IS_NEED_UPDATE)
    {      
      // ������������ ����� ����������
      TFT_WriteString(0,122,C_ALIGN,TEXT_COLOR,BACK_COLOR,Title1,Calibri);
      TFT_WriteString(0,145,C_ALIGN,TEXT_COLOR,BACK_COLOR,Title2,Calibri);
      
      // ����� ������������ ����������
      TFT_WriteString(0,173,C_ALIGN,TEXT_COLOR,BACK_COLOR,DeviceNamePoint,Calibri);
      
      // ������ � ���� �����
      char SoftText[24] ;
      for (int i = 0; i < 32; i++)
      {
        if (i < 5)                   SoftText[i] = SoftVerStart[Language][i];
        if ((i >= 5) && (i < 8))     SoftText[i] = SoftVersion[i-5];
        if (Language == RUS) 
        {
          if ((i >= 8) && (i < 12))  SoftText[i] = SoftDateStart[Language][i-8];
          if ((i >= 12) && (i < 20)) SoftText[i] = SoftDate[i-12];
          if (i == 20) SoftText[i] = 0;
        }
        else
        if (Language == ENG) 
        {
          if ((i >= 8) && (i < 14))  SoftText[i] = SoftDateStart[Language][i-8];
          if ((i >= 14) && (i < 22)) SoftText[i] = SoftDate[i-14];
          if (i == 22) SoftText[i] = 0;
        }
        
      }
      TFT_WriteString(0,198,C_ALIGN,TEXT_COLOR,BACK_COLOR,(char*) SoftText,Calibri);
      
      // ���� ��������
      TFT_WriteString(0,220,C_ALIGN,TEXT_COLOR,BACK_COLOR,(char*) WebsiteTermokub,Calibri);  

    }
  }
}

/*******************************************************************************
  *
  * ��������� ��������� ����
  *
*******************************************************************************/

const char AutoModeName[2][10]          = {"���������","AUTOMODE"};
const char FirewoodModeName[2][9]      = {"�����","FIREWOOD"};
const char ThermostatModeName[2][11]    = {"���������","THERMOSTAT"};

const char SwitchOffModeName[2][15]     = {"   ��������   "," SWITCHED OFF "};
const char IgnitionModeName[2][21]      = {"     ������     ","      IGNITION      "};
const char ControlModeName[2][18]       = {"   ��������   ","     CONTROL     "};
const char RegulationModeName[2][17]    = {"  ���������  ", "   REGULATION   "};
const char ExtinguishingModeName[2][14] = {"   �������   ", "EXTINGUISHING"};
const char WaitingModeName[2][20]       = {"   ��������   ","      WAITING      "};
const char ExtrusionModeName[2][17]     = {"  ���������  ", "    OVERFLOW    "};

void Menu_DrawMainWindow(void)
{
  char* ModeName;
  
  // ���� ��������� ������ ��������
  if (ISNT_SCREENSAVER)
  {
    switch(MODE_FLAG)
    {
      // ����� ��������
      case SWITCHOFF_MODE:
        ModeName = (char*) SwitchOffModeName[Language];
        if (ISNT_OVERFLOW && ISNT_CRIT_TEMP && ISNT_ALARM_THERMOSTAT)
        {
          TFT_FillRect(111,130,TFT_SIGN_WIDTH,18,BACK_COLOR);
          TFT_FillRect(111,155,TFT_SIGN_WIDTH,18,BACK_COLOR);
        }
        break;      
        
      // ����� ����������
      case IGNITION_MODE:
        ModeName = (char*) IgnitionModeName[Language];
        // ������� "������ �� ���", ���� �� �����
        if (ISNT_OVERFLOW && ISNT_CRIT_TEMP && ISNT_ALARM_THERMOSTAT)
        {
          if ((FeedTime_Counter < sSupplyTime.Value) && ISNT_FIREWOOD)
          {
            TFT_WriteParamName (111,130,S_ALIGN,&sSupplyCurrent,TEXT_COLOR,BACK_COLOR,Calibri);
            TFT_WriteParamSign (111,155,S_ALIGN,&sSupplyCurrent,TEXT_COLOR,BACK_COLOR,Calibri);
          }
          // ����� ������ ����
          else
          {
            TFT_FillRect(111,130,TFT_SIGN_WIDTH,18,BACK_COLOR);
            TFT_FillRect(111,155,TFT_SIGN_WIDTH,18,BACK_COLOR);
          }
        }
        break;

      // ����� ��������
      case CONTROL_MODE:
        ModeName = (char*) ControlModeName[Language];
        if (ISNT_OVERFLOW && ISNT_CRIT_TEMP && ISNT_ALARM_THERMOSTAT)
        {
          // ������� "������ �� ���" ����� ���������� �����, ���� �� �����
          if ((FeedTime_Counter < sSupplyTime.Value) &&
              (FeedTime_Num == 0) && ISNT_FIREWOOD)
          {
            TFT_WriteParamName (111,130,S_ALIGN,&sSupplyCurrent,TEXT_COLOR,BACK_COLOR,Calibri);
            TFT_WriteParamSign (111,155,S_ALIGN,&sSupplyCurrent,TEXT_COLOR,BACK_COLOR,Calibri);
          }
          // ����� ������ ����
          else
          {
            TFT_FillRect(111,135,TFT_SIGN_WIDTH,18,BACK_COLOR);
            TFT_FillRect(111,155,TFT_SIGN_WIDTH,18,BACK_COLOR);
          }
        }
        break;
        
      // ����� ���������
      case REGULATION_MODE:
        ModeName = (char*) RegulationModeName[Language];
        if (ISNT_OVERFLOW && ISNT_CRIT_TEMP && ISNT_ALARM_THERMOSTAT)
        {
          // ������� "������ �� ���", ���� �� �����
          if ((FeedTime_Counter < sSupplyTime.Value) && ISNT_FIREWOOD)
          {
            TFT_WriteParamName (111,130,S_ALIGN,&sSupplyCurrent,TEXT_COLOR,BACK_COLOR,Calibri);
            TFT_WriteParamSign (111,155,S_ALIGN,&sSupplyCurrent,TEXT_COLOR,BACK_COLOR,Calibri);
          }
          // ����� ������ ����
          else
          {
            TFT_FillRect(111,130,TFT_SIGN_WIDTH,18,BACK_COLOR);
            TFT_FillRect(111,155,TFT_SIGN_WIDTH,18,BACK_COLOR);
          }
        }
        break;
      
      // ����� �������
      case EXTINGUISHING_MODE:
        ModeName = (char*) ExtinguishingModeName[Language];
        if (ISNT_OVERFLOW && ISNT_CRIT_TEMP && ISNT_ALARM_THERMOSTAT)
        {
          // ������� "������ �� ���", ���� �� �����
          if ((FeedTime_Counter < sSupplyTime.Value) && ISNT_FIREWOOD)
          {
            TFT_WriteParamName (111,130,S_ALIGN,&sSupplyCurrent,TEXT_COLOR,BACK_COLOR,Calibri);
            TFT_WriteParamSign (111,155,S_ALIGN,&sSupplyCurrent,TEXT_COLOR,BACK_COLOR,Calibri);
          }
          // ����� ������ ����
          else
          {
            TFT_FillRect(111,130,TFT_SIGN_WIDTH,18,BACK_COLOR);
            TFT_FillRect(111,155,TFT_SIGN_WIDTH,18,BACK_COLOR);
          }
        }
        break;
        
      // ����� ��������
      case WAITING_MODE:
        ModeName = (char*) WaitingModeName[Language];
        if (ISNT_OVERFLOW && ISNT_CRIT_TEMP && ISNT_ALARM_THERMOSTAT)
        {
          TFT_FillRect(111,130,TFT_SIGN_WIDTH,18,BACK_COLOR);
          TFT_FillRect(111,155,TFT_SIGN_WIDTH,18,BACK_COLOR);
        }
        break;
    }
    
    // ���� ���� ����������� ��� ��������� ���������, �� ������
    if (IS_CRIT_TEMP && IS_ALARM_THERMOSTAT)
    {
      TFT_FillRect(111,130,TFT_SIGN_WIDTH,18,BACK_COLOR);
      TFT_FillRect(111,155,TFT_SIGN_WIDTH,18,BACK_COLOR);    
    }
    else
    // ���� ���������
    if (IS_OVERFLOW)
    {
      TFT_WriteParamName (111,130,S_ALIGN,&sSupplyCurrent,TEXT_COLOR,BACK_COLOR,Calibri);
      TFT_WriteParamSign (111,155,S_ALIGN,&sSupplyCurrent,TEXT_COLOR,BACK_COLOR,Calibri);    
    }
    
    // ������� �����, ���� �� ���������
    if (ISNT_OVERFLOW) TFT_WriteString(0,100,C_ALIGN,HEAD_COLOR,BACK_COLOR,ModeName,Calibri);
    // ����� ������� ������������ �������
    else TFT_WriteString(0,100,C_ALIGN,HEAD_COLOR,BACK_COLOR,(char*) ExtrusionModeName[Language],Calibri);
    
    // ��� � ������� ������� ���������
    if (IS_ADC_FLAG)
    {
      // ���� ����� ���������� ��� ����, ������� ����������� ��������
      if ((sDailyMode.Point == NORMAL) || (IS_DAY))
      {
        TFT_WriteParamName (5,130,S_ALIGN,&sTemperatureSet,ORANGE,BACK_COLOR,Calibri);
        TFT_WriteParamSign (5,155,S_ALIGN,&sTemperatureSet,TEXT_COLOR,BACK_COLOR,Calibri);
      }
      else
      // ���� ����� ����/���� � ����, ������� ������
      if ((sDailyMode.Point == DAYNIGHT) && (IS_NIGHT))
      {
        TFT_WriteParamName (5,130,S_ALIGN,&sTempNight,ORANGE,BACK_COLOR,Calibri);
        TFT_WriteParamSign (5,155,S_ALIGN,&sTempNight,TEXT_COLOR,BACK_COLOR,Calibri);
      }
      
      // ����������� ���
      TFT_WriteParamName (5,180,S_ALIGN,&sTempHWS,ORANGE,BACK_COLOR,Calibri);
      TFT_WriteParamSign (5,205,S_ALIGN,&sTempHWS,TEXT_COLOR,BACK_COLOR,Calibri);    

      // ����������� �����
      TFT_WriteParamSign (107,186,S_ALIGN,&sTempIn,TEXT_COLOR,BACK_COLOR,CalibriLarge);   
      
      // �������� �����������
      TFT_WriteParamName (210,130,S_ALIGN,&sFanCurrent,ORANGE,BACK_COLOR,Calibri);
      TFT_WriteParamSign (210,155,S_ALIGN,&sFanCurrent,TEXT_COLOR,BACK_COLOR,Calibri);
      
      // ����������� �����
      TFT_WriteParamName (210,180,S_ALIGN,&sTempScrew,ORANGE,BACK_COLOR,Calibri);
      TFT_WriteParamSign (210,205,S_ALIGN,&sTempScrew,TEXT_COLOR,BACK_COLOR,Calibri);       
      
      ADC_FLAG_RESET;
    }
  }
}

/*******************************************************************************
  *
  * ��������� ������� ����
  *
  * ������ ����� ���������� � ������ ������
  *
*******************************************************************************/
    
void Menu_DrawTemplate(sMenu* Item)
{
  if (IS_NEED_UPDATE_HEAD)
  {    
    // ������ ��� ��������� ���� � ���������� ����
    if ((Item == (sMenu*) &MainMenu) || 
        (Item == (sMenu*) &ServiceMenu) ||
        (Item == (sMenu*) &CheckWindow))
    {
      // ��������� ����� �����������
      TFT_HLineDivider(22,1,TEXT_COLOR);
      TFT_HLineDivider(44,1,TEXT_COLOR);
      
      // ��������� ����� �������
      // �������� ������ �� ������� ����, 
      // ��������� ����� �� ��������� MENU_BASE_Y, 
      // ������ ����� HEIGHT - MENU_BASE_Y - 3
      // ������ ����� 9 ��������, ������� ����� 1
      // ���� TEXT_COLOR (������������� ����� ������)
      // ������, ���� �� ����� ��������
      if (Item != (sMenu*) &CheckWindow)
        TFT_Rectangle(TFT_WIDTH - R_INDENT + 5,
                      MENU_BASE_Y,
                      9,
                      TFT_HEIGHT - MENU_BASE_Y - 3,
                      1,
                      TEXT_COLOR);  
    }
    // ������ �������
    else
    {      
      // ��������� ����� �����������
      TFT_HLineDivider(22,2,TEXT_COLOR);
      TFT_HLineDivider(44,2,TEXT_COLOR);  
      TFT_HLineDivider(67,1,TEXT_COLOR);  
      
      // ��������� ����� �������
      // �������� ������ �� ������� ����, 
      // ��������� ����� �� ��������� MENU_BASE_Y + MENU_SHIFT_Y, 
      // ������ ����� HEIGHT - MENU_BASE_Y - MENU_SHIFT_Y - 3
      // ������ ����� 9 ��������, ������� ����� 1
      // ���� TEXT_COLOR (������������� ����� ������)
      TFT_Rectangle(TFT_WIDTH - R_INDENT + 5,
                    MENU_BASE_Y + MENU_SHIFT_Y,
                    9,
                    TFT_HEIGHT - MENU_BASE_Y - MENU_SHIFT_Y - 3,
                    1,
                    TEXT_COLOR);   
    }
  }
}

/*******************************************************************************
  *
  * ��������� ���������� ����
  *
*******************************************************************************/

void Menu_DrawHead (sMenu* Item)
{
  // ���� ��������� ���������� � ��������� ��������
  if (IS_NEED_UPDATE_HEAD && ISNT_SCREENSAVER)
  {
    // ��������� ��� ��������� ���� � ���������� ����
    if ((Item == (sMenu*) &MainMenu) || 
        (Item == (sMenu*) &ServiceMenu) || 
        (Item == (sMenu*) &CheckWindow))
    {
      // ������� ���������
      switch (Language)
      {
        case RUS: TFT_WriteString(0,24,C_ALIGN,HEAD_COLOR,NO_BACKGROUND,(*Item).MenuNameRus,Calibri); break;
        case ENG: TFT_WriteString(0,24,C_ALIGN,HEAD_COLOR,NO_BACKGROUND,(*Item).MenuNameEng,Calibri); break;
      }
      
    }
    // ��������� ��� ����
    else
    {
      sMenu* ParentMenu = (sMenu*) (*Item).Parent;
      
      // ������� ���������
      switch (Language)
      {
        case RUS: 
          TFT_WriteString(0,24,C_ALIGN,HEAD_COLOR,NO_BACKGROUND,(*ParentMenu).MenuNameRus,Calibri); 
          TFT_WriteString(0,47,C_ALIGN,HEAD_COLOR,NO_BACKGROUND,(*Item).MenuNameRus,Calibri); 
          break;
        case ENG: 
          TFT_WriteString(0,24,C_ALIGN,HEAD_COLOR,NO_BACKGROUND,(*ParentMenu).MenuNameEng,Calibri); 
          TFT_WriteString(0,47,C_ALIGN,HEAD_COLOR,NO_BACKGROUND,(*Item).MenuNameEng,Calibri);
          break;
      }
    }
  }
}

/*******************************************************************************
  *
  * ��������� �������
  *
*******************************************************************************/

void Menu_DrawScroll(sMenu *Menu)
{
  // ���� ���������� �������� � ��������� ��������
  if (IS_NEED_UPDATE_SCROLL && ISNT_SCREENSAVER && (Menu != (sMenu*)&CheckWindow))
  {
    
    uint8_t PageCount = 0;
    uint16_t Scroll_Size = 0;
    uint16_t Scroll_Start = 0;
    
    // ���� ������� ���� ��� ��������� ���� (11 ������� �� ��������)
    if ((Menu == (sMenu*) &MainMenu) || (Menu == (sMenu*) &ServiceMenu))
    {
      // ������ ������
      TFT_VLine(TFT_WIDTH - R_INDENT + 8,
                MENU_SCROLL_START,
                MENU_SCROLL_FULL,
                3,
                BACK_COLOR);
      
      // ������ ������ ������� � ����������� �� ���������� ������� ���� (*Menu).ItemCount
      // � ����������� ������������ ��������� MENU_SCROLL_FULL
      Scroll_Size = (MENU_SCROLL_FULL * MENU_MAX_POINT) / (*Menu).ItemCount;
      if (Scroll_Size > MENU_SCROLL_FULL) Scroll_Size = MENU_SCROLL_FULL;
      
      // ������ ������� ������� �� ��������� � ����������� �� ������� �������� (*Menu).Page
      // � ����� ������� PageCount
      if (((*Menu).ItemCount - MENU_MAX_POINT) < 1) PageCount = 1;
      else PageCount = (*Menu).ItemCount - MENU_MAX_POINT;
      Scroll_Start = MENU_SCROLL_START + (*Menu).Page*(MENU_SCROLL_FULL - Scroll_Size)/(PageCount);    
    }
    // ���� �������� ���� (10 ������� �� ��������)
    else
    {
      // ������ ������
      TFT_VLine(TFT_WIDTH - R_INDENT + 8,
                MENU_SCROLL_START_CHILD,
                MENU_SCROLL_FULL_CHILD,
                3,
                BACK_COLOR);
      
      // ������ ������ ������� � ����������� �� ���������� ������� ���� (*Menu).ItemCount
      // � ����������� ������������ ��������� MENU_SCROLL_FULL_CHILD
      Scroll_Size = (MENU_SCROLL_FULL_CHILD * ((*Menu).ItemPerPage)) / (*Menu).ItemCount;
      if (Scroll_Size > MENU_SCROLL_FULL_CHILD) Scroll_Size = MENU_SCROLL_FULL_CHILD;
      
      // ������ ������� ������� �� ��������� � ����������� �� ������� �������� (*Menu).Page
      // � ����� ������� PageCount
      PageCount = (*Menu).ItemCount - (*Menu).ItemPerPage;
      Scroll_Start = MENU_SCROLL_START_CHILD + (*Menu).Page*(MENU_SCROLL_FULL_CHILD - Scroll_Size)/(PageCount); 
    }
    
    // ��������� �������
    // �������� ������ �� ������� ����, 
    // ��������� ������� �� ��������� Scroll_Start, ������ ������� Scroll_Size
    // ������ ������� 3 �������, ������ ���������� 1
    // ���� TEXT_COLOR (������������� ����� ������)
    TFT_VLine(TFT_WIDTH - R_INDENT + 8,
              Scroll_Start,
              Scroll_Size,
              3,
              TEXT_COLOR);
  }
}

/*******************************************************************************
  *
  * ��������� �������
  *
  * �������������� ������� ������ � ���������
  *
*******************************************************************************/

uint16_t BatteryVoltage = 0;
int BatteryLevel = 0;

void Menu_DrawBattery (uint16_t x, uint16_t y) // ���������� ������������ �������
{   
  int BatteryLevel_new = 0;
  // ������ ������ ������ � ���������
  // �������
  // �����������  - 2.20 �
  // ������������ - 2.75 �
  BatteryLevel_new = (int) (1.82 * BatteryVoltage - 400);
  // ����������� ������ ������
  if (BatteryLevel_new < 0)   BatteryLevel_new = 0;
  if (BatteryLevel_new > 100) BatteryLevel_new = 100;
  
  // ��������� �������, ���� ��������� ������� ������ ��� �� ������ 15%
  if ((BatteryLevel_new != BatteryLevel) || 
      (BatteryLevel_new < 15) ||
      IS_NEED_UPDATE_BAT)
  {
    BatteryLevel = BatteryLevel_new;
    
    uint16_t BatteryFillColor = 0;
    uint16_t BatterySymbolColor = 0;
      
    // ���� ����� ������ 15% � �� ��������, �� ������ ������� � ������ �����
    if (BatteryLevel < 15)
    {
      if (IS_BLINK_BATTERY) {BatterySymbolColor = TEXT_COLOR; BatteryFillColor = RED;}
      else
      if (ISNT_BLINK_BATTERY) {BatterySymbolColor = BACK_COLOR; BatteryFillColor = BACK_COLOR;}
    }
    else
    { 
      BatterySymbolColor = TEXT_COLOR;
      
      // ���� ����� ������ 15% � ������ 25%, �� ������� ������� �������
      if ((BatteryLevel >= 15) && (BatteryLevel < 25)) {BatteryFillColor = RED;}
      else
      // ���� ����� ������ 25% � ������ 40%, �� ������� ������� �����-���������
      if ((BatteryLevel >= 25) && (BatteryLevel < 40)) {BatteryFillColor = YELLOW_ORANGE;}
      else
      // ���� ����� ������ 40%, �� ������� ������� �������
      if (BatteryLevel >= 40) {BatteryFillColor = GREEN;}
    }
    
    // ��������� ������� �������� ������� Battery_Symbol
    TFT_WriteString(x,y,NO_ALIGN,BatterySymbolColor,NO_BACKGROUND," ",Battery_Symbol); 
    // ��������� ������ ������ �������
    Menu_FillBatteryLevel (x+6,y+4,BatteryLevel, BatteryFillColor);  
    
    NEED_UPDATE_BAT_RESET;
  }
}

/*******************************************************************************
  *
  * ������� ������ ������
  *
*******************************************************************************/

void Menu_FillBatteryLevel (uint16_t x, uint16_t y,  // ���������� ������
                            uint16_t Level,          // �������� ������ ������
                            uint16_t color)          // ���� �������
{
  uint16_t BatteryLevelPoint = (uint16_t) (Level * 0.18);
  // ��������� ������ ������
  TFT_FillRect(x + (18-BatteryLevelPoint),y,BatteryLevelPoint,10,color);
}