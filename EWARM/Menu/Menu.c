
#include "main.h"
#include "Menu.h"
#include "ILI9341.h"
#include "Init.h"
#include "24C_EEPROM.h"
#include "adc.h"

/*******************************************************************************
  *
  * ��������� ��������� ����������
  *
*******************************************************************************/

void Menu_ProduceParameters (void)
{
  // ����������� ������������ ����������
  DeviceNamePoint = (char*) DeviceNameSTART[Language];
  
  // �����
  if ((TimeMenu.Item & mActive) != mActive)
  {
    // ���� �� �������� ����� � ���� � ������ ������, �� ��������� ����
    sHours.Value   = sTimeCurrent.Hours;
    sMinutes.Value = sTimeCurrent.Minutes;
  }
  else
  {
    // ���� �������� ���� � �����, �� ���������� � ����������
    switch (TimeMenu.Item & ~mActive)
    {
      // ����
      case 0: 
        sTimeCurrent.Hours = sHours.Value;
        HAL_RTC_SetTime(&hrtc, &sTimeCurrent, RTC_FORMAT_BIN);
        break;
      // ������
      case 1:
        sTimeCurrent.Minutes = sMinutes.Value;
        HAL_RTC_SetTime(&hrtc, &sTimeCurrent, RTC_FORMAT_BIN);
        break;
    }
  }
  
  // ��� � ���� �������� �� ��������� ��������
  // - �������� ����������� �����
  // - �������� �����������
  // - �������� ����������� � ��������
  // - ���������� � ������ ����������
  if (sTemperatureSetting.Min != sTemperatureSettingMin.Value)
    sTemperatureSetting.Min = sTemperatureSettingMin.Value;
  if (sTemperatureSetting.Max != sTemperatureSettingMax.Value)
    sTemperatureSetting.Max = sTemperatureSettingMax.Value;
  
  // �������� ���������
  if (sTemperatureSetting.Value != sTemperatureSet.Value)
    sTemperatureSet.Value = sTemperatureSetting.Value;
  
  // ���������� �������
  // ���������, ���� � ���� "�� ����������" � �������� ����������
  if ((sBattery.Value != BatteryVoltage) & 
      (Menu_SelectedItem == (sMenu*) &DeviceMenu))
  {
    sBattery.Value = BatteryVoltage; 
    //NEED_UPDATE_SET;
  }
  
  // ���� ������� ����������
  if (Menu_SelectedItem == (sMenu*) &ManualControlMenu)
  {
    // ����������
    if (sFanManual.Point == 0) {Work_FanPowerSet(0);}
    else                       {Work_FanPowerSet(sFanPower.Value);}
    // ����
    if (sScrewManual.Point == 0) {Work_SupplyPowerSet(0);}
    else                         {Work_SupplyPowerSet(100);}
    // �������������� ����� ���������
    if (sCirPumpManual.Point == 0) {CIRPUMP_OFF;}
    else                           {CIRPUMP_ON;}
    
    // ����� ��� ������������� ���������
    HWSPUMP_OFF;
  }
  
  // ���������
  if (IS_AUTO_MODE)           {FIREWOOD_OFF;}
  else                        {FIREWOOD_ON;}
  if (sThermostat.Point == 0) {THERMO_OFF;}
  else                        {THERMO_ON;}
  
  // ������ �����������
  sTempNight.Value = sTemperatureSetting.Value + sNightCorrection.Value;
  if (sTempNight.Value < sTemperatureSettingMin.Value) 
    sTempNight.Value = sTemperatureSettingMin.Value;
  if (sTempNight.Value > sTemperatureSettingMax.Value)
    sTempNight.Value = sTemperatureSettingMax.Value;
  
  // ���� ����� ���������� � ���� ����/���� �������, �� �������� ����
  if ((IS_NIGHT) && (sDailyMode.Point == NORMAL)) DAY_SET;
  
  // ����������� ����������� ������� ���, ������������ �������� ������ ����
  // ������ ����������� ����� �������� �� 3 ������� � ���������� �����
  //
  // � ���������� ������� ����������� ����������� ������� ��� - 75 ��������
  
  if (ISNT_BOILER_PRIORITY)
  {
    sTemperatureHWS.Max = sTemperatureSetting.Value - 3;
    
    if (sTemperatureHWS.Value > sTemperatureHWS.Max) 
    {
      sTemperatureHWS.Value = sTemperatureHWS.Max;
    }
  }
  else
  if (IS_BOILER_PRIORITY)
  {
    sTemperatureHWS.Max = 75;
  }
  
  // ���� ����������� �������� ��� ���������, �� ��������� ��������� ������� � 
  // �� ���� ����������
  if (IS_OFF(sTemperatureHWS) && IS_BOILER_PRIORITY &&
      (((*Menu_SelectedItem).Item & mActive) != mActive))
  {
    sBoilerPriority.Point = OFF;
    //NEED_UPDATE_MENU_SET;
    NEED_UPDATE_SET;
  }
}

/*******************************************************************************
  *
  * ������� ��������� ������ ������
  *
*******************************************************************************/

uint16_t InButt[4] = {0,0,0,0};
uint16_t AndInButt = 0;
uint16_t OrInButt = 0;
uint16_t OldButtState, NewButtState, RepeatButtons;
uint8_t  RepeatButtons20;

void Menu_ProduceButtonFlags(void)
{
  for (uint8_t i = 0; i < 4; i++)
  {
    // ��������� ��������� ������ � ������
    uint16_t ButtReadFlag = 0;
    ButtReadFlag |= But_START_State * But_START;
    ButtReadFlag |= But_STOP_State  * But_STOP;
    ButtReadFlag |= But_MORE_State  * But_MORE;
    ButtReadFlag |= But_LESS_State  * But_LESS;
    
    // ������ ��� ������� ������ � ������ �� �������
    InButt[3] = InButt[2];
    InButt[2] = InButt[1];
    InButt[1] = InButt[0]; 
    InButt[0] = ButtReadFlag;
    
    // ���������� � ��� ��������
    AndInButt  = InButt[0];
    AndInButt &= InButt[1]; 
    AndInButt &= InButt[2];
    AndInButt &= InButt[3];
    
    // ���������� ��� ��� ��������
    OrInButt  = InButt[0];  
    OrInButt |= InButt[1];
    OrInButt |= InButt[2];
    OrInButt |= InButt[3];
    
    // ����������� ������ � ������� ��������� ������  
    OldButtState = NewButtState;
    NewButtState |= AndInButt;
    NewButtState &= OrInButt;
   
    // ����������� ���������� �������� 0->1  � ��������� ������
    BUTTONS = (OldButtState ^ 0x0FFFF) & NewButtState;
  }
  
  // �������� ������������ ��� ����������� ������,
  // ���� ��� ���� BUTTONS = 0, ������� RepeatButtons,
  // ���� ������ ������, �� ��������� RepeatBottons
  if (NewButtState == 0x0000) 
  {
    RepeatButtons = 0; 
    RepeatButtons20 = 0;
  }
  else RepeatButtons++;
  
  // �������� ���� ������� ������� ������ ����� � ����, ���� ���� �� ������ ����������
  if ((((NewButtState == But_MORE) || (NewButtState == But_LESS)) ||
       ((NewButtState == But_STOP) && (Menu_SelectedItem == (sMenu*) &MainWindow))) &&
       (Menu_SelectedItem != (sMenu*) &ManualControlMenu))
  {
    if (RepeatButtons == 0x0008)
    {
      RepeatButtons = 0x1000; //������� ��� ��������������� 
      BUTTONS = NewButtState; //������ ������
    }
  
    // ��������� �������
    if (RepeatButtons == 0x1001)
    { 
      RepeatButtons = 0x1000; //������� ��� ��������������� 
      BUTTONS = NewButtState; //������ ������
      RepeatButtons20++;
      if (RepeatButtons20 > 20) RepeatButtons20 = 20;
    }
  }
}

/*******************************************************************************
  *
  * ��������� �������� ������ (������� �������)
  *
  * ������ ����� � ���� ������ �������� ��������
  * ������ ����� � ���� ��������� ��������
  *
*******************************************************************************/

uint8_t Menu_CorrectFloatUpDown(sFloat *Element, // ������ �� ��������� ��������
                                sMenu *Menu)     // ������ �� ��������� ����
{
  uint8_t Alarm = 0;
  int Res = (*Element).Value;
  int Act = (*Menu).Item & mActive;
  int Rac = (*Menu).Item & ~mActive;
  
  switch (BUTTONS)
  {
    case But_START: // �������������
    case But_STOP:
      Act &= ~mActive;
      (*Menu).Item = Rac | Act;
      BUTTONS &= ~(But_START | But_STOP);
      break;
      
    case But_MORE: // ������� �����
      // ���� ������� ������� ������ 20, �� ���������� 10, ����� 1
      if (RepeatButtons20 >= 20) Res = (int) (Res + 10);
      else                       
      {
        if ((Element->Off & OFF_STATE) == OFF_STATE) Element->Off = OFF_SUPPORT;
        else                                         Res = (int) (Res + 1);
      }
      BUTTONS = 0;
      break;
      
    case But_LESS: // ������� ����
      // ���� ������� ������� ������ 20, �� �������� 10, ����� 1
      if (RepeatButtons20 >= 20) Res = (int) (Res - 10);
      else                       Res = (int) (Res - 1);
      BUTTONS = 0;
      break;
  }
  
  if (Res > (*Element).Max) {Res = (*Element).Max; Alarm = ALARM_TIME;}
  if (Res < (*Element).Min) {
    Res = (*Element).Min; 
    // ���� ���� ������� ����������, �� ���������� �
    if ((Element->Off & OFF_SUPPORT) == OFF_SUPPORT) 
    {
      if ((Element->Off & OFF_STATE) == OFF_STATE) Alarm = ALARM_TIME;
      Element->Off = OFF_STATE + OFF_SUPPORT;
    }
    else Alarm = ALARM_TIME; 
  }
  (*Element).Value = Res;
  return Alarm;
}

/*******************************************************************************
  *
  * ��������� �������������� ��������� � ������������ ���������� (������� �������)
  *
  * ������ ����� � ���� ������ �������� ��������
  *
*******************************************************************************/

uint8_t Menu_CorrectEnumUpDown(sEnum *Element, // ������ �� ��������� ��������
                               sMenu *Menu)    // ������ �� ��������� ����
{
  uint8_t Alarm = 0;
  uint8_t Res = (*Element).Point;
  int Act = (*Menu).Item & ~mActive;
  
  // ���� ������� ����� ������ ��������� ���/����, �� �� ��������� � ��������������,
  // � ������ ������ ��������, ����� ���������
  if (NON_ONOFF)
  {
    switch (BUTTONS)
    {
      // ������������� ������
      case But_START:
      case But_STOP:
        (*Menu).Item = Act;
        BUTTONS = 0;
        break;
      
      // ������� �����
      case But_MORE:
        Res++;
        if (Res > (*Element).MaxPoints - 1) {Res = 0; Alarm = ALARM_TIME;}
        BUTTONS = 0;
        break;
        
      // ������� ����
      case But_LESS:
        if (Res > 0) Res--;
        else {Res = (*Element).MaxPoints - 1; Alarm = ALARM_TIME;}
        BUTTONS = 0;
        break;
    }
  }
  else
  {
    (*Menu).Item = Act;
    if (Res == 1) Res = 0;
    else          Res = 1;    
  }
  
  (*Element).Point = Res;
  return Alarm;
}

/*******************************************************************************
  *
  * ������� �� ������ � ���� ��������
  *
*******************************************************************************/

void Menu_StartStopMoreLess(sMenu* Menu)
{ 
  if ((ISNT_ENERGY) && (ISNT_SCREENSAVER)) 
  {
    if (BUTTONS > 0) {EnergySaving_Counter = 0;ScreenSaver_Counter = 0;ScreenSaverChange_Counter = 0;}
    
    // ���� �������������� �� �������
    if(((*Menu).Item & mActive) != mActive)
    {
      int Act,Res,Max,Page;
      Act  = (*Menu).Item & mActive;
      Res  = (*Menu).Item & ~mActive;
      Max  = (*Menu).ItemCount;
      Page = (*Menu).Page;  
      
      // ���� �������� ������� ����� � �������, ����� ������� ��������� �������
      if ((BUTTONS > 0) && (IS_OVERFLOW_BUZZER)) OVERFLOW_BUZZER_OFF;
      
      // ���� ������ ������� � ����, �� ������ �� ���������
      if (MoreLessCounter > 314)
      {
        if (BUTTONS == 0) MoreLessCounter = 0;
      }
      else
      {
        switch (BUTTONS)
        {
          // ������ �����
          // ���������� ����� ����
          case But_START:
            // ��� ������� � ���� ������������ � �������������
            if ((Menu_SelectedItem != (sMenu*) &DesignerMenu) &&
                (Menu_SelectedItem != (sMenu*) &ManufacturerMenu) &&
                (Menu_SelectedItem != (sMenu*) &CheckWindow))
            {
              Act ^= mActive;
              Act &= mActive;
              NEED_UPDATE_SET;
            }
            if (Check_Stage != 1) Check_Stage++;
            // � ���� �������� ������������ ��� ������������ ������
            if (Menu_SelectedItem == (sMenu*) &CheckWindow)
            {
              if ((Check_Stage == 8)  || 
                  (Check_Stage == 16) || 
                  (Check_Stage == 24) ||
                  (Check_Stage == 27))
              {
                // ������� �������
                TFT_FillScreen(BACK_COLOR);
                // ������� ����� ������������� ����������
                NEED_UPDATE_MENU_SET;
                NEED_UPDATE_HEAD_SET;
                NEED_UPDATE_LANG_SET;
                NEED_UPDATE_BAT_SET;
                
                if (Check_Stage == 27)
                {
                  Menu_SelectedItem = (sMenu*)((*Menu_SelectedItem).Parent);
                  NEED_UPDATE_SET;
                }
              }
            }
            
            BUTTONS = 0;
            // � ������ ������ ��������/��������� ����
            if (Menu_SelectedItem == (sMenu*) &ManualControlMenu)
            {
              Act &= ~mActive;
              if (ISNT_SCREW)    sScrewManual.Point = 1;
              else if (IS_SCREW) sScrewManual.Point = 0;
            }
            break;
          
          // ������ ����
          // ������� � ������������ ����
          case But_STOP:
            Act &= ~mActive;
            // ���� ������� �� ���� ������� ����������, �� ��������� ��� ��������
            if (Menu_SelectedItem == (sMenu*) &ManualControlMenu)
            {
              // ��������� ��������
              CIRPUMP_OFF;
              SCREW_OFF;
              FAN_OFF;
              // �������� ����� 0%
              Work_SupplyPowerSet(0);
              // �������� ����������� 0%
              Work_FanPowerSet(0);
              // ���������� ���� � ��������� ����
              sFanManual.Point = 0;
              sScrewManual.Point = 0;
              sCirPumpManual.Point = 0;
            }

            // ���� ������� �� �������� 
            if (Menu_SelectedItem != (sMenu*) &CheckWindow)
            {
              // ������� �������
              TFT_FillScreen(BACK_COLOR);
              
              Menu_SelectedItem = (sMenu*)((*Menu_SelectedItem).Parent);
                
              if (Menu_SelectedItem != (sMenu*) &MainWindow)
              {
                // ������� ����� ����������
                NEED_UPDATE_SET;
                NEED_UPDATE_HEAD_SET;
                NEED_UPDATE_SCROLL_SET;
                NEED_UPDATE_BAT_SET;
              }
              else
              {
                NEED_UPDATE_BAT_SET;
                MENU_EXIT_SET;
                // ��������� ��������� � ������
                EEPROM_WriteSettings();          
              }
            }
            else
            // � ���� ��������
            {
              if (Check_Stage == 1) Check_Stage++;
            }
            BUTTONS = 0;
            break;
            
          // ������ ������
          case But_MORE:
            // ���� ���� ������� ����������, �� ��������/��������� ����������
            if (Menu_SelectedItem == (sMenu*) &ManualControlMenu)
            {
              if (ISNT_FAN)    sFanManual.Point = 1;
              else if (IS_FAN) sFanManual.Point = 0;
            }
            else
            // � ���� �������� ����������� ������ �����
            if ((Menu_SelectedItem == (sMenu*) &CheckWindow) && (Check_Stage == 2)) Check_Stage++;
            else
            // � ��������� ������� - �������� �� ����
            {
              // ���� ����� �� ������
              if (Res > 0)
              {
                // ���� ������ ������ ����� �� ������� �������� �� ��������� �����
                // � ������� ��������, ����� ������ �����
                if (Res == Page) 
                {
                  Page--; Res--;
                  NEED_UPDATE_SCROLL_SET;
                }
                else Res--;
              }
              // ���� ����� ������ 
              else
              {
                // ������ �� ��������� �����
                Res = Max - 1;
                if (Page == 0)
                {
                  // ���� ����� ������� �� �������� ������ ������
                  if ((*Menu).ItemPerPage > 1)
                  {
                    Page = Max - (*Menu).ItemPerPage;
                    NEED_UPDATE_SCROLL_SET;
                  }
                  // ���� ���� ����� �� ��������
                  else
                  {
                    Page = Max - (*Menu).ItemPerPage;
                    NEED_UPDATE_SCROLL_SET;
                  }
                }
                if (Page < 0) 
                {
                  Page = 0;
                  NEED_UPDATE_SCROLL_SET;
                }
              }
            }
            BUTTONS = 0;
            NEED_UPDATE_SET;
            break;
            
          // ������ ������
          case But_LESS:
            // ���� ���� ������� ����������, �� ��������/��������� ����� ���������
            if (Menu_SelectedItem == (sMenu*) &ManualControlMenu)
            {
              if (ISNT_CIRPUMP)    sCirPumpManual.Point = 1;
              else if (IS_CIRPUMP) sCirPumpManual.Point = 0;
            }
            // � ���� �������� ��������� ������
            if ((Menu_SelectedItem == (sMenu*) &CheckWindow) && (Check_Stage == 3)) Check_Stage++;
            else
            // � ��������� ������� - �������� �� ����
            {
              // ���� ����� �� ���������
              if (Res < Max - 1)
              {
                // ���� ����� ������� �� �������� ������ ������
                if ((*Menu).ItemPerPage > 1)
                {
                  // ���� ��������� ����� �� ������� �������� �� ��������� ���� 
                  // � ������� ��������, ����� ������ �����
                  if (Res >= (*Menu).ItemPerPage - 1 + Page) 
                  {
                    Page++; Res++;
                    NEED_UPDATE_SCROLL_SET;
                  }
                  else Res++;
                }
                // ���� ���� ����� �� ��������
                else 
                {
                  Page++; Res++;
                  NEED_UPDATE_SCROLL_SET;
                }
              }
              // ���� ����� ���������
              else
              {
                Res = 0; Page = 0;
                NEED_UPDATE_SCROLL_SET;
              }
            }
            BUTTONS = 0;
            NEED_UPDATE_SET;
            break;
        }
      }
      
      (*Menu).Page = Page;
      (*Menu).Item = Res|Act;
    }
  }
  else
  {
    // ��������� ���������
    if ((IS_ENERGY) && (BUTTONS > 0))
    {
      TFT_LED_Set_Brightness(100);      
      ENERGY_RESET;
      
      BUTTONS = 0;
      EnergySaving_Counter = 0;
    }
    
    // ����� �� ���������
    if ((IS_SCREENSAVER) && ((BUTTONS > 0) || (WARNING_FLAGS > 0)))
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
  * ��������� ���� ��������� ��������� ��� ��������� ������������
  *
  * ������ � ����������� �������� 
  * *Dig  - �������� �������� (0 - ���� �� ������������)
  * *Enum - �������� ������������ (0 - ���� �� ������������)
  *
*******************************************************************************/

void TFT_WriteMenuControl (uint16_t x, uint16_t y,  // ���������� ������ ����
                           sMenu *Menu,             // ������ �� ��������� ���� ����������
                           uint8_t MenuItem,        // ����� �������� ����, �����. ������ ��������
                           sFloat *Dig,             // ������ �� ��������� ��������� �������� (0 - ���� �� ���.)
                           sEnum  *Enum,            // ������ �� ��������� �������� ������������ (0 - ���� �� ���.)
                           uint16_t Color,          // ���� �������� ������ ����
                           uint32_t Background,     // ���� ���� ������ ����
                           uint16_t AlarmColor,     // ���� �������������� (����� ��������� �� �������)
                           sFont font)              // ������ �� ��������� ������
{
  // ������������, ���� ��������� ������� ���� �������� �� ��������
  uint8_t Shift;
  if ((Menu == (sMenu*)&MainMenu) ||
      (Menu == (sMenu*)&ServiceMenu)) Shift = MENU_BASE_Y + 0*MENU_SHIFT_Y;
  else                                Shift = MENU_BASE_Y + 1*MENU_SHIFT_Y;
  
  if ((y >= Shift) && (y <= MENU_BASE_Y + MENU_MAX_POINT*MENU_SHIFT_Y - 10))
  {
    while(IS_SPI_DMA_BUSY) {}
    
    char *str;
    uint16_t x0 = 0;
    
    // ����� ������������ ��������� �������� ��� �������� ������������ � ������ �����
    if (Enum != 0) str = (char*) Enum->Name[Language];
    else
    if (Dig != 0) str = (char*) Dig->Name[Language];
    
    // ������� ���� � ����� ������������ ���������
    if (((*Menu).Item & ~mActive) != MenuItem) // ������ ������ �������, ������� ������� �������
    {
      if (((((*Menu).Item & mActive) != mActive) && IS_NEED_UPDATE) || IS_NEED_UPDATE_LANG)
      {
        // ����� ������������
        x0 += TFT_WriteString(0,y,L_ALIGN,Color,NO_BACKGROUND,str,font);
      }
    }
    else
    if ((*Menu).Item == MenuItem) // ������� ������, �� �� ������� (������ ������)
    {
      // ����� ������������
      x0 += TFT_WriteString(x,y,L_ALIGN,Background,Color,str,font);
    }
    else
    if ((*Menu).Item == MenuItem + mActive) //������� �������. �������������� 
    {
      // ����� ������������
      x0 += TFT_WriteString(x,y,L_ALIGN,Color,NO_BACKGROUND,str,font);
    }
    
    // ����� ���������

    if (Enum != 0) TFT_WriteMenuEnumControl (x0,y,RIS_ALIGN,Menu,MenuItem,Enum,Color,Background,AlarmColor,font);
    else
    if (Dig != 0) TFT_WriteMenuDigitalControl (x0,y,RI_ALIGN,Menu,MenuItem,Dig,Color,Background,AlarmColor,font);
  }
}


/*******************************************************************************
  *
  * ��������� ������ ���� �� ���������
  *
  * ������ �������� ���������� ���� *Str
  *
*******************************************************************************/

void TFT_WriteMenuString (uint16_t x, int y,       // ���������� ������ ����
                          sMenu *Menu,            // ������ �� ��������� �������� ���� ����������
                          sMenu *ChildMenu,       // ������ �� �������� ����
                          uint8_t MenuItem,       // ����� �������� ����, �����. ������ ��������
                          char *Str,              // ������ �� ��������� ���������� ��������
                          void ConfirmVoid(void), // �������, ���������� �� �������������
                          uint16_t Color,         // ���� �������� ������ ����
                          uint32_t Background,    // ���� ���� ������ ����
                          sFont font)             // ������ �� ��������� ������
{
  // ������������, ���� ��������� ������� ���� �������� �� ��������
  uint8_t Shift;
  if ((Menu == (sMenu*)&MainMenu) ||
      (Menu == (sMenu*)&ServiceMenu) ||
      (Menu == (sMenu*)&CheckWindow)) Shift = MENU_BASE_Y + 0*MENU_SHIFT_Y;
  else                                Shift = MENU_BASE_Y + 1*MENU_SHIFT_Y;
  if (y < 0) y = 0;
  
  // ������������, ���� ��������� ������� ���� �������� �� ��������
  if ((y >= Shift) && (y <= MENU_BASE_Y + MENU_MAX_POINT*MENU_SHIFT_Y - 10))
  {
    while(IS_SPI_DMA_BUSY) {}
    
    uint8_t x0 = 0;
    uint16_t dot_width = TFT_WIDTH - R_INDENT - TFT_CalcStringWidth(Str,font) - font.Space;
    // ������� ���� � ����� ������������ ���������
    if (((*Menu).Item & ~mActive) != MenuItem) // ������ ������ �������, ������� ������� �������
    {
      if (((((*Menu).Item & mActive) != mActive) && IS_NEED_UPDATE) || IS_NEED_UPDATE_LANG)
      {      
        // ����� ������������
        x0 += TFT_WriteString(x,y,L_ALIGN,Color,NO_BACKGROUND,Str,font);
        // ����� �������� �� ����� ������
        TFT_HDotLine(0,dot_width,dot_width,2,Color,Background);
        // ������������� ���� ������
        TFT_ILI9341_SetAddrWindow(x0,y,TFT_WIDTH-R_INDENT-1,y+font.Height);
        // �������� ������
        TFT_SendBuffer(TFT_BUF,dot_width*2*font.Height);        
      }
    }
    else
    if ((*Menu).Item == MenuItem) // ������� ������, �� �� ������� (������ ������)
    {
      // ����� ������������
      x0 += TFT_WriteString(x,y,L_ALIGN,Background,Color,Str,font);
      // ����� �������� �� ����� ������
      TFT_HDotLine(0,dot_width,dot_width,2,Background,Color);
      // ������������� ���� ������
      TFT_ILI9341_SetAddrWindow(x0,y,TFT_WIDTH-R_INDENT-1,y+font.Height);
      // �������� ������
      TFT_SendBuffer(TFT_BUF,dot_width*2*font.Height);
    }
    else
    if ((*Menu).Item == MenuItem + mActive) //������� �������. �������������� 
    {
      (*Menu).Item &= ~mActive; // ������ ������
      // ���� �������, ���������� �� �������������, �� ������
      if (ConfirmVoid)
      {
        ConfirmVoid();
      }
      // ������� � �������� ���� � ���������� ������, ���� �� ������
      if (ChildMenu != &NULL_ENTRY) 
      {
        Menu_SelectedItem = (sMenu*) ChildMenu;
        TFT_FillScreen(Background);
        NEED_UPDATE_MENU_SET;
        NEED_UPDATE_BAT_SET;
      }
    }
  }  
}

/*******************************************************************************
  *
  * ��������� ������ ���� �� ��������� � ������������ �������� ��������� 
  * ���������
  *
  * ������ �������� ���������� ���� � ������� ��������� ���������
  *
*******************************************************************************/

void TFT_WriteMenuStringNested (uint16_t x, uint16_t y, // ���������� ������ ����
                                sMenu *Menu,            // ������ �� ��������� ����
                                sMenu *ChildMenu,       // ������ �� ��������� ��������� ����
                                uint8_t MenuItem,       // ����� �������� ����, �����. ������ ��������
                                sEnum *Enum,            // ������ �� ��������� �������� ������������ 
                                char  *str,             // ��������� �� ����� ������ 
                                uint16_t color,         // ���� �������� ������ ����
                                uint32_t background,    // ���� ����
                                sFont font)             // ������ �� ����� 
{
  // ������������, ���� ��������� ������� ���� �������� �� ��������
  uint8_t Shift;
  if (Menu == (sMenu*)&MainMenu) Shift = MENU_BASE_Y + 0*MENU_SHIFT_Y;
  else                           Shift = MENU_BASE_Y + 1*MENU_SHIFT_Y;
  
  // ������������, ���� ��������� ������� ���� �������� �� ��������
  if ((y >= Shift) && (y <= MENU_BASE_Y + MENU_MAX_POINT*MENU_SHIFT_Y))
  {
    uint8_t x0 = 0;
    uint16_t x_indent = 0;
    
    // ������� ���� � ����� ������������ ���������
    if (((*Menu).Item & ~mActive) != MenuItem) // ������ ������ �������, ������� ������� �������
    {
      if ((((*Menu).Item & mActive) != mActive) && (IS_NEED_UPDATE)) 
      {   
        // ����� ���������
        x_indent = TFT_WriteString(x+font.Space,y,RIS_ALIGN,color,background,(char*) (*Enum).Value[Language][(*Enum).Point],font);
        // ����� ������������
        x0 += TFT_WriteString(x,y,NO_ALIGN,color,NO_BACKGROUND,str,font);
        // ����� �������� �� ����� ������
        TFT_HDotLine(x0,y,TFT_WIDTH-R_INDENT-x0-x_indent+2,2,color,background);   
      }
    }
    else
    if ((*Menu).Item == MenuItem) // ������� ������, �� �� ������� (������ ������)
    {
      // ����� ���������
      x_indent = TFT_WriteString(x+font.Space,y,RIS_ALIGN,background,color,(char*) (*Enum).Value[Language][(*Enum).Point],font);
      // ����� ������������
      x0 += TFT_WriteString(x,y,NO_ALIGN,background,color,str,font);
      // ����� �������� �� ����� ������
      TFT_HDotLine(x0,y,TFT_WIDTH-R_INDENT-x0-x_indent+2,2,background,color);
    }
    else
    if ((*Menu).Item == MenuItem + mActive) //������� �������. �������������� 
    {
      (*Menu).Item &= ~mActive; // ������ ������
      // ������� � �������� ����
      Menu_SelectedItem = (sMenu*) ChildMenu;
      // ������� �������
      TFT_FillScreen(BACK_COLOR);
      // ������� ���� ������������� ����������
      NEED_UPDATE_MENU_SET;
    }    
  }
}

/*******************************************************************************
  *
  * ��������� ������ ���� � ��������� ���������� ��� �������� �� �������
  *
*******************************************************************************/

void TFT_WriteMenuNullText (uint8_t x, uint8_t y,       // ���������� ������
                            sMenu *Menu,                // ������ �� ��������� �������� ����
                            uint8_t MenuItem,           // ����� �������� ����, �����. ������ ��������
                            char *TextName,             // ��������� �� ������������ ������
                            char *Text,                 // ��������� �� ������
                            uint16_t Color,             // ���� �������� ������ ����
                            uint32_t Background,        // ���� ���� ������ ����
                            sFont font)                 // ������ �� ��������� ������
{
  // ������������, ���� ��������� ������� ���� �������� �� ��������
  uint8_t Shift;
  if ((Menu == (sMenu*)&MainMenu) ||
      (Menu == (sMenu*)&ServiceMenu) ||
      (Menu == (sMenu*)&CheckWindow))   Shift = MENU_BASE_Y + 0*MENU_SHIFT_Y;
  else                                  Shift = MENU_BASE_Y + 1*MENU_SHIFT_Y;
  
  if ((y >= Shift) && (y <= MENU_BASE_Y + MENU_MAX_POINT*MENU_SHIFT_Y - 10))
  {
    if (IS_NEED_UPDATE)
    {
      while(IS_SPI_DMA_BUSY) {}
      
      uint8_t x0 = 0;
      
      // ����� ������������
      x0 += TFT_WriteString(x,y,L_ALIGN,Color,NO_BACKGROUND,TextName,font);
      
      uint16_t str_width = TFT_CalcStringWidth(Text,font) + font.Space;
      uint16_t dot_width = TFT_WIDTH - R_INDENT - str_width - x0;
      uint16_t buf_size = str_width + dot_width;
      
      // ����� �������� �� ����� ������
      TFT_HDotLine(0,buf_size,dot_width,2,Color,NO_BACKGROUND);   
      
      // ����� ������
      if (Text != 0) TFT_PutString(dot_width,y,RIS_ALIGN,Color,NO_BACKGROUND,Text,font);    
      
      // ������������� ���� ������
      TFT_ILI9341_SetAddrWindow(x0,y,TFT_WIDTH-R_INDENT-1,y+font.Height);

      // �������� ������
      TFT_SendBuffer(TFT_BUF,buf_size*2*font.Height);
    }
    
    // ���� ������� �������, �� ������� ������
    if ((*Menu).Item == MenuItem + mActive) (*Menu).Item &= ~mActive;
  }
}

/*******************************************************************************
  *
  * ��������� ����������� � �������������
  *
*******************************************************************************/

void TFT_WriteMenuImage (uint8_t x, uint8_t y,  // ���������� �����������
                         uint8_t align,         // ������������ �� ������
                         sMenu *Menu,           // ������ �� ��������� ����
                         uint8_t MenuItem,      // ����� �������� ����, �����. ������ ��������
                         sImage *img)           // ������ �� ��������� �����������
{
  // ����, ���� DMA �������� ��������
  while(IS_SPI_DMA_BUSY) {}
  
  uint16_t img_width = (*img).Width;
  // ������������ ����������� �� ������
  switch (align)
  {
    case L_ALIGN:  x = 0; break;
    case R_ALIGN:  x = TFT_WIDTH - img_width; break;
    case C_ALIGN:  x = (TFT_WIDTH - img_width)/2; break;
    case RI_ALIGN: x = (TFT_WIDTH - R_INDENT) - img_width; break;
    case CI_ALIGN: x = (TFT_WIDTH - img_width)/2 - R_INDENT; break;
  }
  // ����� �����������
  TFT_DrawImage(x, y, 0, img);
  
  // ���� ������� �������, �� ������� ������
  if ((*Menu).Item == MenuItem + mActive) (*Menu).Item &= ~mActive;
}

/*******************************************************************************
  *
  * ��������� ����������� �������
  *
*******************************************************************************/

void Menu_DrawLoadBar (void)
{
  // ���� �� ��������� ������
  if (ISNT_SCREENSAVER)
  {
    // ����, ���� DMA �������� ��������
    while(IS_SPI_DMA_BUSY) {}
    
    if (Menu_SelectedItem == (sMenu*) &MainWindow)
    {
      switch (BUTTONS)
      {
        // ���� ������������ ������� � ������, �� ������ ����� �������� ���
        // �������� � ���� ��������
        case (But_LESS + But_MORE):
        case (But_LESS):
        case (But_MORE):
          if (MoreLessCounter > 0)
          {
            TFT_Rectangle(1,225,318,14,1,TEXT_COLOR);
            TFT_FillRect(3,227,MoreLessCounter,11,GREEN);
          }
          break;
        // � ������ ������� ������� ����� ��������, ���� ��� ������������
        default:
          if (MoreLessCounter > 0) 
          {
            TFT_Rectangle(1,225,318,14,1,BACK_COLOR);
            TFT_FillRect(3,227,MoreLessCounter,11,BACK_COLOR);
            MoreLessCounter = 0;
          }
          break;
      }
    }
  }
}

/*******************************************************************************
  *
  * ������� ����� � ��������������� � ����� ��������������
  *
*******************************************************************************/

const char AlarmThermostatTitle[2][25] = {"         ������         ","         DANGER         "};
const char CritTempTitle[2][20] =  {" ����. ����������� ", " CRIT. TEMPERATURE "};
const char ScrewTempTitle[2][19] = {" ����������� �����", " SCREW TEMPERATURE"};
const char HighTempTitle[2][30] =  {"          ��������           ","         OVERHEAT          "};
const char HWSTempTitle[2][30] =   {"        �������� ���         ","       HWS OVERHEAT        "};
const char LowTempTitle[2][21] =   {" ������ ����������� ","  LOW TEMPERATURE  "}; 

const char TempSensorTitle[2][22]  = {"������. ����. �����","BOILER SENS IS BROKEN"};
const char ScrewSensorTitle[2][21] = {"������. ����. �����","SCREW SENS IS BROKEN"};
const char HWSSensorTitle[2][21]   = {" ������. ����. ��� "," HWS SENS IS BROKEN "};

void Menu_AlarmBackground (void)
{
  // ���� ��������� ������ ��������
  if (ISNT_SCREENSAVER)
  {
    // ���� �������� ��������� ���������, �� ����� ������
    if (IS_ALARM_THERMOSTAT)
    {
      // ���� ��� �� �������, ��������
      if (BACK_COLOR != RED)
      {
        BACK_COLOR = RED;
        TFT_FillScreen(BACK_COLOR);
        NEED_UPDATE_BAT_SET;
        ADC_FLAG_SET;
      }   
      
      TFT_WriteString(0,72,C_ALIGN,TEXT_COLOR,BACK_COLOR,(char*) AlarmThermostatTitle[Language],Calibri);
    }
    else
    // ���� ����������� ���� 90 ��������, �������� ��� �������
    if (IS_CRIT_TEMP)
    {
      // ���� ��� �� �������, ��������
      if (BACK_COLOR != RED)
      {
        BACK_COLOR = RED;
        TFT_FillScreen(BACK_COLOR);
        NEED_UPDATE_BAT_SET;
        ADC_FLAG_SET;
      }
      
      TFT_WriteString(0,72,C_ALIGN,TEXT_COLOR,BACK_COLOR,(char*) CritTempTitle[Language],Calibri);
    }
    else
    // ���� ����������� ����� ������ ����������� ������� ����� ��� ����������� �����
    // �� 80 �� 90 �������� ��� ����������� ��� ���� 80, �������� ������
    if ((IS_SCREW_TEMP) || (IS_HIGH_TEMP) || (IS_HWS_TEMP))
    {
      if (BACK_COLOR != LIGHT_ORANGE)
      {
        BACK_COLOR = LIGHT_ORANGE;
        TFT_FillScreen(BACK_COLOR);
        NEED_UPDATE_BAT_SET;
        ADC_FLAG_SET;
      }
      
      if (IS_SCREW_TEMP) TFT_WriteString(0,72,C_ALIGN,RED,BACK_COLOR,(char*) ScrewTempTitle[Language],Calibri);
      else
      if (IS_HIGH_TEMP) TFT_WriteString(0,72,C_ALIGN,RED,BACK_COLOR,(char*) HighTempTitle[Language],Calibri);
      else
      if (IS_HWS_TEMP) TFT_WriteString(0,72,C_ALIGN,RED,BACK_COLOR,(char*) HWSTempTitle[Language],Calibri);
    }
    else
    // ���� ����������� ����� ������ 5 ��������, �������� �����
    if (IS_LOW_TEMP)
    {
      if (BACK_COLOR != DARK_BLUE)
      {
        BACK_COLOR = DARK_BLUE;
        TFT_FillScreen(BACK_COLOR);
        NEED_UPDATE_BAT_SET;
        ADC_FLAG_SET;
        
        TFT_WriteString(0,72,C_ALIGN,RED,BACK_COLOR,(char*) LowTempTitle[Language],Calibri);
      }
    }
    else
    // � ��������� ������� ��� ������ � ������� ��������������
    {
      if (BACK_COLOR != BLACK)
      {
        BACK_COLOR = BLACK;
        TFT_FillScreen(BACK_COLOR);
        NEED_UPDATE_BAT_SET;
        ADC_FLAG_SET;
      }
      TFT_FillRect(100,72,120,18,BACK_COLOR);
    }
    
    // ���� ������ ����� ���������
    if (IS_TEMP_SENSOR)
    {
      TFT_WriteString(0,30,C_ALIGN,ORANGE,BACK_COLOR,(char*) TempSensorTitle[Language],Calibri);
    }
    else
    // ���� ������ ����� ���������
    if (IS_SCREW_SENSOR)
    { 
      TFT_WriteString(0,30,C_ALIGN,ORANGE,BACK_COLOR,(char*) ScrewSensorTitle[Language],Calibri);
    }
    else
    // ���� ������ ��� ���������
    if (IS_HWS_SENSOR)
    {   
      TFT_WriteString(0,30,C_ALIGN,ORANGE,BACK_COLOR,(char*) HWSSensorTitle[Language],Calibri);    
    }
    else
    // ����� �������
    {
      TFT_FillRect(45,30,240,18,BACK_COLOR);
    }
  }
}

/*******************************************************************************
  *
  * ��������� ��������� ������
  *
*******************************************************************************/

uint8_t ScreenSaverChanged = 0;

void Menu_ScreenSaver(void)
{
  if (IS_SCREENSAVER && ((ScreenSaverChange_Counter % 5) == 0) && (WARNING_FLAGS == 0))
  {
    if (ScreenSaverChanged == 0)
    {
      TFT_FillScreen(BACK_COLOR);
      
      uint16_t x0 = 0;
      uint16_t y0 = 0;
      switch (ScreenSaverChange_Counter)
      {
        case 0:  x0 = 5;   y0 = 20;  break;
        case 5:  x0 = 107; y0 = 180; break; 
        case 10: x0 = 205; y0 = 70;  break; 
        case 15: x0 = 5;   y0 = 120; break;
        case 20: x0 = 107; y0 = 20;  break; 
        case 25: x0 = 205; y0 = 180; break; 
        case 30: x0 = 5;   y0 = 70;  break; 
        case 35: x0 = 107; y0 = 120; break; 
        case 40: x0 = 205; y0 = 20;  break; 
        case 45: x0 = 7;   y0 = 180; break;
        case 50: x0 = 107; y0 = 70;  break; 
        case 55: x0 = 205; y0 = 120; break; 
        case 60: x0 = 7;   y0 = 20;  break; 
      }
      TFT_WriteParamSign (x0, y0, S_ALIGN,&sTempIn,TEXT_COLOR,BACK_COLOR,CalibriLarge);
      
      ScreenSaverChanged = 1;
    }
  }
  else ScreenSaverChanged = 0;
}

/*******************************************************************************
  *
  * ����� ��������
  *
*******************************************************************************/

void Menu_ResetSettings(void)
{
  // �������� ���������
  sMode.Point                           = 0;   // ����� - ��������������
  sDailyMode.Point                      = 0;   // �������� ����� - ����������
  sTemperatureSetting.Value             = 60;  // �������� ����������� �����
  sTemperatureSetting.Off               = 0;
  sSupplyTime.Value                     = 10;  // ����� ������
  sSupplyTime.Off                       = 0;
  sSupplyPeriod.Value                   = 15;  // ������ �����
  sSupplyPeriod.Off                     = 0;
  sFanPower.Value                       = 40;  // �������� �����������
  sFanPower.Off                         = 0;
  sSupplyRepetition.Value               = 2;   // ���������� �����
  sSupplyRepetition.Off                 = 0;
  sBlowingTime.Value                    = 15;  // ����� �������
  sBlowingTime.Off                      = OFF_SUPPORT;    
  sBlowingPeriod.Value                  = 2;   // ������ ��������
  sBlowingPeriod.Off                    = 0;
  sFanPowerBlowing.Value                = 50;  // �������� ����������� � ��������
  sFanPowerBlowing.Off                  = 0;
  sFanPowerFirewood.Value               = 40;  // �������� ����������� �� ������
  sFanPowerFirewood.Off                 = 0;
  sTemperatureHWS.Value = sTemperatureHWS.Min; // �������� ����������� ���
  sTemperatureHWS.Off                   = OFF_SUPPORT + OFF_STATE;
  sBoilerPriority.Point                 = 0;   // ��������� ������� - ����
  sNightCorrection.Value                = 5;   // ������ ���������
  sNightCorrection.Off                  = 0;
  sThermostat.Point                     = 0;   // ��������� ��������� - ����
  
  // ��������� ���������
  sTemperatureSettingMin.Value          = 40;  // ���. ����������� �����
  sTemperatureSettingMin.Off            = 0;
  sTemperatureSettingMax.Value          = 80;  // ����. ����������� �����
  sTemperatureSettingMax.Off            = 0;
  sHysteresisBoilerSensor.Value         = 1;   // ���������� ������� �����
  sHysteresisBoilerSensor.Off           = 0;
  sHysteresisHWSSensor.Value            = 3;   // ���������� ������� ���
  sHysteresisHWSSensor.Off              = 0;
  sFanPowerMin.Value                    = 25;  // ����������� �������� �����������
  sFanPowerMin.Off                      = 0;
  sFanPowerMax.Value                    = 55;  // ������������ �������� �����������
  sFanPowerMax.Off                      = 0;
  sFuelLackTime.Value                   = 30;  // ����� ����������� �������� �������
  sFuelLackTime.Off                     = OFF_SUPPORT;   
  sTemperatureHeatingPumpStart.Value    = 40;  // ����������� ��������� ������ ���������
  sTemperatureHeatingPumpStart.Off      = OFF_SUPPORT;
  sTimePumpOff.Value                    = 3;   // ����� ���������� ������
  sTimePumpOff.Off                      = 0;
  sTimePumpOn.Value                     = 30;  // ����� ��������� ������
  sTimePumpOn.Off                       = 0;
  sTemperatureOff.Value                 = 30;  // ����������� ���������� �����
  sTemperatureOff.Off                   = 0;
  sTemperatureScrewAlarm.Value          = 70;  // ����������� ������� �����
  sTemperatureScrewAlarm.Off            = OFF_SUPPORT;  
  sTimeTransfer.Value                   = 10;  // ����� ���������
  sTimeTransfer.Off                     = 0;
  sNightPeriodStartTime.Value           = 22;  // ������ ������� �������
  sNightPeriodStartTime.Off             = 0;   
  sDayPeriodStartTime.Value             = 6;   // ������ �������� �������
  sDayPeriodStartTime.Off               = 0;
  sEnergySaving.Value                   = sEnergySaving.Min;   // ����������������
  sEnergySaving.Off                     = OFF_SUPPORT + OFF_STATE;
  sScreenSaver.Value                    = sScreenSaver.Min;    // ��������� ������
  sScreenSaver.Off                      = OFF_SUPPORT + OFF_STATE;
}

/*******************************************************************************
  *
  * ��������� ������ � ��������� �����������
  *
*******************************************************************************/

const char UnitSecName[2][4]  = {"���","sec"}; // �������
const char UnitMinName[2][4]  = {"���","min"}; // ������
const char UnitHourName[2][3] = {"�","hr"};    // ����
const char UnitPercName[2][2] = {"%",  "%"};   // ��������
const char UnitDegName[2][2]  = {"C",  "C"};   // ������� �������
const char UnitkHzName[2][4]  = {"���","kHz"}; // ���������
const char UnitHzName[2][3]   = {"��","Hz"};   // �����
const char UnitVoltName[2][2] = {"�",  "V"};   // ������
const char UnitNullName[2][2] = {0,    0};     // ������� ������� ���������