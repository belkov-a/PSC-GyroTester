#include "main.h"
#include "Menu.h"
#include "ILI9341.h"
#include "Init.h"
#include "adc.h"

/*******************************************************************************
  *
  * ��������� ������ � ����
  *
*******************************************************************************/

// ������ ����
const char NullMenuName[2] = " ";
sMenu NullMenu = {(void*) 0,(char*) NullMenuName,(char*) NullMenuName, 0, 0, 0, 0};

// ������� ����� ����
sMenu* Menu_SelectedItem;

#define MENU_MAKE(Name, Parent, MenuNameRus, MenuNameEng, Item, ItemCount, Page, ItemPerPage) \
	extern sMenu Parent;   \
          sMenu Name = {(void*)&Parent, (char*) MenuNameRus, (char*) MenuNameEng, \
                        (uint32_t)Item, (uint32_t)ItemCount, (uint32_t)Page,(uint32_t)ItemPerPage};

const char CheckWindowName[2][20] = {"�������� ����������","Device check"};
const char MainMenuName[2][19] = {"�������� ���������","Main settings"};
const char ManualControlMenuName[2][18] = {"������ ����������","Manual control"};
const char ServiceMenuName[2][20] = {"��������� ���������","Service settings"};
const char TimeMenuName[2][14] = {"������� �����","Current time"};
const char ManufacturerMenuName[2][22] = {"� ������������� �����","Bolier manufacturer"};
const char DesignerMenuName[2][20] = {"� ������������","Controller designer"};
const char DeviceMenuName[2][14] = {"�� ����������","Device data"};
const char ResetSettingsMenuName[2][15] = {"����� ��������","Reset settings"};
                        
MENU_MAKE(CheckWindow,StartWindow,(char*) CheckWindowName[0], (char*)CheckWindowName[1],0,1,0,1);
MENU_MAKE(LicenseWindow,LicenseWindow,(char*) NullMenuName,(char*) NullMenuName,0,0,0,0);
MENU_MAKE(StartWindow,NullMenu,(char*) NullMenuName,(char*) NullMenuName,0,0,0,0);
MENU_MAKE(MainWindow,MainMenu,(char*) NullMenuName,(char*) NullMenuName,0,0,0,0);
MENU_MAKE(MainMenu,MainWindow,(char*) MainMenuName[0],(char*) MainMenuName[1],0,19,0,9);
MENU_MAKE(ManualControlMenu,MainMenu,(char*) ManualControlMenuName[0],(char*) ManualControlMenuName[1],0,3,0,3);
MENU_MAKE(ServiceMenu,MainWindow,(char*) ServiceMenuName[0],(char*) ServiceMenuName[1],0,20,0,9);
MENU_MAKE(TimeMenu,ServiceMenu,(char*) TimeMenuName[0],(char*) TimeMenuName[1],0,2,0,2);
MENU_MAKE(ManufacturerMenu,MainMenu,(char*) ManufacturerMenuName[0],(char*) ManufacturerMenuName[1],0,1,0,1);
MENU_MAKE(DesignerMenu,MainMenu,(char*) DesignerMenuName[0],(char*) DesignerMenuName[1],0,1,0,1);
MENU_MAKE(DeviceMenu,MainMenu,(char*) DeviceMenuName[0],(char*) DeviceMenuName[1],0,1,0,1);
MENU_MAKE(ResetSettingsMenu,ServiceMenu,(char*) ResetSettingsMenuName[0],(char*) ResetSettingsMenuName[1],0,2,0,2);

/*******************************************************************************
  *
  * ��������� ���� ��������
  *
*******************************************************************************/

const char DevInfoText1[2][21] = {"''�����-���������''","Aktiv-Komponent ltd."};
const char DevInfoText2[2][16] = {"620039, ������,","620039, Russia,"};
const char DevInfoText3[2][17] = {"�. ������������,","Ekaterinburg,"};
const char DevInfoText4[2][24] = {"��. ����������, 24, 4","Donbasskaya str., 24, 4"};
const char DevInfoText5[2][22] = {"Email: psc@olympus.ru","Email: psc@olympus.ru"};
const char DevInfoText6[2][22] = {"���: +7(902)444-59-90","tel: +7(902)444-59-90"};
const char DevInfoText7[2][16] = {"www.termokub.ru","www.termokub.ru"};

#ifdef PSC_START_DEVICE
const char ManInfoText1[2][19] = {"������������ �����","Heating boliers"};
const char ManInfoText2[2][10] = {"''�����''","''START''"};
const char ManInfoText3[2][21] = {"���: 8 800 550-09-22","tel: 8 800 550-09-22"};
const char ManInfoText4[2][15] = {"www.kotel74.ru","www.kotel74.ru"};
#endif

#ifdef PSC_PELLETOR_DEVICE
const char ManInfoPelletorText1[2][16] = {"��������� �����","Pellet boilers"};
const char ManInfoPelletorText2[2][9] = {"PELLETOR","PELLETOR"};
const char ManInfoPelletorText3[2][21] = {"���: 8 800 700-62-21","tel: 8 800 700-62-21"};
const char ManInfoPelletorText4[2][16] = {"www.pelletor.ru","www.pelletor.ru"};
#endif

const char PSC_ABPName[2][11] = {"����������","Controller"};
const char SoftVerName[2][17] = {"������ ��","Software version"}; 
const char SoftDateName[2][18] = {"���� ��","Software date"};
const char SerialNumName[2][15] = {"�������� �����","Serial number"};
const char ManufactDateName[2][18] = {"���� ������������","Manufacture date"};
const char NoSettings[2][9] = {"��������","Cancel."};
const char YesSettings[2][12] = {"�����������","Confirm."};

uint8_t Check_Stage = 0;

const char OkSettings[2][3] = {"OK","OK"};
const char StartConfirmText[2][26] = {"���� ��� ����������",  "     START to finish     "};
const char StartSkipText[2][19] =    {"���� � ���� ������",   "START to next item"};
const char CheckText1[2][14] = {"������� ����","Press START"};
const char CheckText2[2][13] = {"������� ����","Press STOP"};
const char CheckText3[2][14] = {"������� �����","Press UP"};
const char CheckText4[2][13] = {"������� ����","Press DOWN"};
const char CheckText5[2][14] = {"���� �������","Screw is on"};
const char CheckText6[2][20] = {"���������� �������","Fan is on"};
const char CheckText7[2][20] = {"����� ����� �������","Heat pump is on"};
const char CheckText8[2][19] = {"����� ��� �������","HWS pump is on"};
const char CheckText9[2][17] = {"������ ��������","Alarm is on"};
const char CheckText10[2][20] = {"���� ������ �������","Sound signal is on"};
const char CheckText11[2][23] = {"���� ���� �������","Close alarm thermostat"};
const char CheckText12[2][22] = {"������ ���� �������","Open alarm thermostat"};
const char CheckText13[2][22] = {"���� ���� �������","Close room thermostat"};
const char CheckText14[2][21] = {"������ ���� �������","Open room thermostat"};
const char CheckText15[2][20] = {"���� ������� ����","Close external stop"};
const char CheckText16[2][21] = {"������ ������� ����","Open external stop"};
const char CheckText17[2][22] = {"���� ������ �����","Close boiler sensor"};
const char CheckText18[2][21] = {"������ ������ �����","Discon boiler sensor"};
const char CheckText19[2][20] = {"���� ������ ���","Close HWS sensor"};
const char CheckText20[2][18] = {"������ ������ ���","Discon HWS sensor"};
const char CheckText21[2][22] = {"���� ������ �����","Close screw sensor"};
const char CheckText22[2][20] = {"������ ������ �����","Discon screw sensor"};
const char CheckText23[2][10] = {"�������","Frequency"};
const char CheckText24[2][20] = {"��������� ��������?","Finish checking?"};

uint8_t Menu_DrawSettings(sMenu* Item)
{
  // ���� ��������� ��������
  if (ISNT_SCREENSAVER)
  {
    // ������� ����
    if (Item == (sMenu*) &MainMenu)
    {
      // 0. �������� ����������� �����
      TFT_WriteMenuControl (0, MENU_POSITION(0), 
                            Menu_SelectedItem, 0,
                            &sTemperatureSetting,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri);   
      // 1. ����� ������ �����
      TFT_WriteMenuControl (0, MENU_POSITION(1), 
                            Menu_SelectedItem, 1,
                            0,&sMode,
                            TEXT_COLOR,BACK_COLOR,0,
                            Calibri);   
      // 2. ������ ����������
      TFT_WriteMenuString (0, MENU_POSITION(2),
                           Menu_SelectedItem,
                           &ManualControlMenu, 2,
                           (char*) ManualControlMenuName[Language],
                           0,
                           TEXT_COLOR, BACK_COLOR,
                           Calibri); 
      if (Menu_SelectedItem != (sMenu*) &MainMenu) return 1;
      // 3. �������� �����
      TFT_WriteMenuControl (0, MENU_POSITION(3), 
                            Menu_SelectedItem, 3,
                            0,&sDailyMode,
                            TEXT_COLOR,BACK_COLOR,0,
                            Calibri);      
      // 4. ����� ������
      TFT_WriteMenuControl (0, MENU_POSITION(4), 
                            Menu_SelectedItem, 4,
                            &sSupplyTime,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 5. ����� ����� �������� �������
      TFT_WriteMenuControl (0, MENU_POSITION(5), 
                            Menu_SelectedItem, 5,
                            &sSupplyPeriod,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 6. �������� �����������
      TFT_WriteMenuControl (0, MENU_POSITION(6), 
                            Menu_SelectedItem, 6,
                            &sFanPower,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 7. ���������� �����
      TFT_WriteMenuControl (0, MENU_POSITION(7), 
                            Menu_SelectedItem, 7,
                            &sSupplyRepetition,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 8. ����� �������
      TFT_WriteMenuControl (0, MENU_POSITION(8), 
                            Menu_SelectedItem, 8,
                            &sBlowingTime,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 9. ����� ����� ���������
      TFT_WriteMenuControl (0, MENU_POSITION(9), 
                            Menu_SelectedItem, 9,
                            &sBlowingPeriod,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 10. �������� ����������� ��� �������
      TFT_WriteMenuControl (0, MENU_POSITION(10), 
                            Menu_SelectedItem, 10,
                            &sFanPowerBlowing,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 11. �������� ����������� ��� ������
      TFT_WriteMenuControl (0, MENU_POSITION(11), 
                            Menu_SelectedItem, 11,
                            &sFanPowerFirewood,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri);
      // 12. �������� ����������� ���
      TFT_WriteMenuControl (0, MENU_POSITION(12), 
                            Menu_SelectedItem, 12,
                            &sTemperatureHWS,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 13. ��������� �������
      TFT_WriteMenuControl (0, MENU_POSITION(13), 
                            Menu_SelectedItem, 13,
                            0,&sBoilerPriority,
                            TEXT_COLOR,BACK_COLOR,0,
                            Calibri);   
      // 14. ����������� ������
      TFT_WriteMenuControl (0, MENU_POSITION(14), 
                            Menu_SelectedItem, 14,
                            &sNightCorrection,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 15. ��������� ���������
      TFT_WriteMenuControl (0, MENU_POSITION(15), 
                            Menu_SelectedItem, 15,
                            0,&sThermostat,
                            TEXT_COLOR,BACK_COLOR,0,
                            Calibri);   
      // 16. ���������� � �������������
      TFT_WriteMenuString (0, MENU_POSITION(16),
                           Menu_SelectedItem,
                           &ManufacturerMenu, 16,
                           (char*) ManufacturerMenuName[Language],
                           0,
                           TEXT_COLOR, BACK_COLOR,
                           Calibri); 
      if (Menu_SelectedItem != (sMenu*) &MainMenu) return 1;
      // 17. ���������� � ������������
      TFT_WriteMenuString (0, MENU_POSITION(17),
                           Menu_SelectedItem,
                           &DesignerMenu, 17,
                           (char*) DesignerMenuName[Language],
                           0,
                           TEXT_COLOR, BACK_COLOR,
                           Calibri); 
      if (Menu_SelectedItem != (sMenu*) &MainMenu) return 1;
      // 18. ���������� �� ����������
      TFT_WriteMenuString (0, MENU_POSITION(18),
                           Menu_SelectedItem,
                           &DeviceMenu, 18,
                           (char*) DeviceMenuName[Language],
                           0,
                           TEXT_COLOR, BACK_COLOR,
                           Calibri); 
      if (Menu_SelectedItem != (sMenu*) &MainMenu) return 1;
    }
    else
    // ������ ����������
    if (Item == (sMenu*) &ManualControlMenu)
    {
      // 0. ����
      TFT_WriteMenuControl (0, MENU_POSITION(1), 
                            Menu_SelectedItem, 4,
                            0,&sScrewManual,
                            TEXT_COLOR,BACK_COLOR,0,
                            Calibri);
      // 1. ����������
      TFT_WriteMenuControl (0, MENU_POSITION(2), 
                            Menu_SelectedItem, 4,
                            0,&sFanManual,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 2. ����� ���������
      TFT_WriteMenuControl (0, MENU_POSITION(3), 
                            Menu_SelectedItem, 4,
                            0,&sCirPumpManual,
                            TEXT_COLOR,BACK_COLOR,0,
                            Calibri);
      
      // ��� � ������� ������� ����������� �����
      if (IS_ADC_FLAG) TFT_WriteParamSign (107,183,S_ALIGN,&sTempIn,TEXT_COLOR,BACK_COLOR,CalibriLarge); 
    }
    else
    // ��������� ���������
    if (Item == (sMenu*) &ServiceMenu)
    {
      // ���� ���� ���������, �� ��������� ���������
      if (Language != sLanguage.Point)
      {
        Language = sLanguage.Point;
        // ������� �������
        TFT_FillScreen(BACK_COLOR);
        // ������� ����� ������������� ����������
        NEED_UPDATE_MENU_SET;
        NEED_UPDATE_SET;
        NEED_UPDATE_LANG_SET;
        NEED_UPDATE_BAT_SET;
      }
      
      // 0. ����������� ����������� �����
      TFT_WriteMenuControl (0, MENU_POSITION(0), 
                            Menu_SelectedItem, 0,
                            &sTemperatureSettingMin,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 1. ������������ ����������� �����
      TFT_WriteMenuControl (0, MENU_POSITION(1), 
                            Menu_SelectedItem, 1,
                            &sTemperatureSettingMax,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 2. ���������� ������� �����
      TFT_WriteMenuControl (0, MENU_POSITION(2), 
                            Menu_SelectedItem, 2,
                            &sHysteresisBoilerSensor,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 3. ���������� ������� ���
      TFT_WriteMenuControl (0, MENU_POSITION(3), 
                            Menu_SelectedItem, 3,
                            &sHysteresisHWSSensor,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 4. ����������� �������� �����������
      TFT_WriteMenuControl (0, MENU_POSITION(4), 
                            Menu_SelectedItem, 4,
                            &sFanPowerMin,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 5. ������������ �������� �����������
      TFT_WriteMenuControl (0, MENU_POSITION(5), 
                            Menu_SelectedItem, 5,
                            &sFanPowerMax,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 6. ����� ����������� �������� �������
      TFT_WriteMenuControl (0, MENU_POSITION(6), 
                            Menu_SelectedItem, 6,
                            &sFuelLackTime,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 7. ����������� ��������� ������ ���������
      TFT_WriteMenuControl (0, MENU_POSITION(7), 
                            Menu_SelectedItem, 7,
                            &sTemperatureHeatingPumpStart,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 8. ����� ���������� ������
      TFT_WriteMenuControl (0, MENU_POSITION(8), 
                            Menu_SelectedItem, 8,
                            &sTimePumpOff,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 9. ����� ��������� ������
      TFT_WriteMenuControl (0, MENU_POSITION(9), 
                            Menu_SelectedItem, 9,
                            &sTimePumpOn,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 10. ����������� ���������� �����
      TFT_WriteMenuControl (0, MENU_POSITION(10), 
                            Menu_SelectedItem, 10,
                            &sTemperatureOff,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 11. ����������� ������� �����
      TFT_WriteMenuControl (0, MENU_POSITION(11), 
                            Menu_SelectedItem, 11,
                            &sTemperatureScrewAlarm,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 12. ����� ���������
      TFT_WriteMenuControl (0, MENU_POSITION(12), 
                            Menu_SelectedItem, 12,
                            &sTimeTransfer,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 13. ������� �����
      TFT_WriteMenuString (0, MENU_POSITION(13),
                           Menu_SelectedItem,
                           &TimeMenu, 13,
                           (char*) TimeMenuName[Language],
                           0,
                           TEXT_COLOR, BACK_COLOR,
                           Calibri); 
      if (Menu_SelectedItem != (sMenu*) &ServiceMenu) return 1;
      // 14. ������ ������� �������
      TFT_WriteMenuControl (0, MENU_POSITION(14), 
                            Menu_SelectedItem, 14,
                            &sNightPeriodStartTime,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 15. ������ �������� �������
      TFT_WriteMenuControl (0, MENU_POSITION(15), 
                            Menu_SelectedItem, 15,
                            &sDayPeriodStartTime,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 16. ����
      TFT_WriteMenuControl (0, MENU_POSITION(16), 
                            Menu_SelectedItem, 16,
                            0,&sLanguage,
                            TEXT_COLOR,BACK_COLOR,0,
                            Calibri); 
      // 17. ����������������
      TFT_WriteMenuControl (0, MENU_POSITION(17), 
                            Menu_SelectedItem, 17,
                            &sEnergySaving,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 18. ��������� ������
      TFT_WriteMenuControl (0, MENU_POSITION(18), 
                            Menu_SelectedItem, 18,
                            &sScreenSaver,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 19. ����� ��������
      TFT_WriteMenuString (0, MENU_POSITION(19),
                           Menu_SelectedItem,
                           &ResetSettingsMenu, 19,
                           (char*) ResetSettingsMenuName[Language],
                           0,
                           TEXT_COLOR, BACK_COLOR,
                           Calibri);
      if (Menu_SelectedItem != (sMenu*) &ServiceMenu) return 1;
    }
    else
    // ������� �����
    if (Item == (sMenu*) &TimeMenu)
    {
      // 0. ����
      TFT_WriteMenuControl (0, MENU_POSITION(1), 
                            Menu_SelectedItem, 0,
                            &sHours,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
      // 1. ������
      TFT_WriteMenuControl (0, MENU_POSITION(2), 
                            Menu_SelectedItem, 1,
                            &sMinutes,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
    }
    else
    // � ������������� �����
    if (Item == (sMenu*) &ManufacturerMenu)
    {
      if (IS_NEED_UPDATE)
      {
        // ��������� ����������
        // ������������ �������� 
        // �����
        // �����
        // �������
        // ����
        char *ManInfo1;
        char *ManInfo2;
        char *ManInfo3;
        char *ManInfo4;
        #ifdef PSC_PELLETOR_DEVICE
          ManInfo1 = (char*) ManInfoPelletorText1[Language];
          ManInfo2 = (char*) ManInfoPelletorText2[Language];
          ManInfo3 = (char*) ManInfoPelletorText3[Language];
          ManInfo4 = (char*) ManInfoPelletorText4[Language];
        #endif
        #ifdef PSC_START_DEVICE
          ManInfo1 = (char*) ManInfoText1[Language];
          ManInfo2 = (char*) ManInfoText2[Language];
          ManInfo3 = (char*) ManInfoText3[Language];
          ManInfo4 = (char*) ManInfoText4[Language];
        #endif
        
        TFT_WriteString(5,MENU_POSITION(2),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        ManInfo1,Calibri);
        TFT_WriteString(5,MENU_POSITION(3),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        ManInfo2,Calibri);
        TFT_WriteString(5,MENU_POSITION(5),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        ManInfo3,Calibri);
        TFT_WriteString(5,MENU_POSITION(6),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        ManInfo4,Calibri);
      }
    }
    else
    // � ������������
    if (Item == (sMenu*) &DesignerMenu)
    {
      if (IS_NEED_UPDATE)
      {
        // ��������� ���������� 
        // ������������ �������� 
        // �����
        // �����
        // �������
        // ����
        TFT_WriteString(5,MENU_POSITION(1),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        (char*) DevInfoText1[Language],Calibri);
        TFT_WriteString(5,MENU_POSITION(2),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        (char*) DevInfoText2[Language],Calibri);
        TFT_WriteString(5,MENU_POSITION(3),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        (char*) DevInfoText3[Language],Calibri);
        TFT_WriteString(5,MENU_POSITION(4),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        (char*) DevInfoText4[Language],Calibri);
        TFT_WriteString(5,MENU_POSITION(6),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        (char*) DevInfoText5[Language],Calibri);
        TFT_WriteString(5,MENU_POSITION(7),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        (char*) DevInfoText6[Language],Calibri);
        TFT_WriteString(5,MENU_POSITION(8),
                        C_ALIGN,TEXT_COLOR,BACK_COLOR,
                        (char*) DevInfoText7[Language],Calibri);
      }
    }
    else
    // ���������� �� ����������
    if (Item == (sMenu*) &DeviceMenu)
    {
      // 0. ���� ����������
      TFT_WriteMenuNullText (0,MENU_POSITION(1),
                             Menu_SelectedItem, 0,
                             (char*) PSC_ABPName[Language],
                             DeviceNamePoint,
                             TEXT_COLOR,BACK_COLOR,Calibri);
      // 1. ������ ��
      TFT_WriteMenuNullText (0, MENU_POSITION(2),
                             Menu_SelectedItem, 1,
                             (char*) SoftVerName[Language],
                             (char*) SoftVersion,
                             TEXT_COLOR,BACK_COLOR,Calibri);
      // 2. ���� ��
      TFT_WriteMenuNullText (0, MENU_POSITION(3),
                             Menu_SelectedItem, 2,
                             (char*) SoftDateName[Language],
                             (char*) SoftDate,
                             TEXT_COLOR,BACK_COLOR,Calibri); 
      // 3. �������� �����
      TFT_WriteMenuNullText (0, MENU_POSITION(4),
                             Menu_SelectedItem, 3,
                             (char*) SerialNumName[Language],
                             (char*) SerialNum,
                             TEXT_COLOR,BACK_COLOR,Calibri); 
      // 4. ���� ������������
      TFT_WriteMenuNullText (0, MENU_POSITION(5),
                             Menu_SelectedItem, 4,
                             (char*) ManufactDateName[Language],
                             (char*) ManufactDate,
                             TEXT_COLOR,BACK_COLOR,Calibri); 
      NEED_UPDATE_SET;
      // 5. ���������� �������
      TFT_WriteMenuControl (0, MENU_POSITION(6), 
                            Menu_SelectedItem, 5,
                            &sBattery,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri);   
      // 6. ������� ����
      TFT_WriteMenuControl (0, MENU_POSITION(7),
                            Menu_SelectedItem, 6,
                            &sFrequency,0,
                            TEXT_COLOR,BACK_COLOR,ALARM_COLOR,
                            Calibri); 
    }
    else
    // ���� ��������
    if (Item == (sMenu*) &CheckWindow)
    {
      switch (Check_Stage)
      {
        // ������ �����
        case 0:
          TFT_WriteString(0,MENU_POSITION(0),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText1[Language],Calibri);   
          break;
        // ������ ����
        case 1:
          TFT_WriteString(0,MENU_POSITION(0),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri);  
          TFT_WriteString(0,MENU_POSITION(1),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText2[Language],Calibri);  
          break;
        // ������ �����
        case 2:
          TFT_WriteString(0,MENU_POSITION(1),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri);  
          TFT_WriteString(0,MENU_POSITION(2),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText3[Language],Calibri);
          TFT_WriteString(0,MENU_POSITION(8),
                          C_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) StartSkipText[Language],Calibri);
          break;
        // ������ ����
        case 3:
          TFT_WriteString(0,MENU_POSITION(2),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri);  
          TFT_WriteString(0,MENU_POSITION(3),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText4[Language],Calibri);
          break;
        // ����
        case 4:
          TFT_WriteString(0,MENU_POSITION(3),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri);  
          Work_SupplyPowerSet(100);
          TFT_WriteString(0,MENU_POSITION(4),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText5[Language],Calibri);
          break;
        // ����������
        case 5:
          TFT_WriteString(0,MENU_POSITION(4),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          Work_SupplyPowerSet(0);
          Work_FanPowerSet(50);
          TFT_WriteString(0,MENU_POSITION(5),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText6[Language],Calibri);   
          break;
        // ����� ���������
        case 6:
          TFT_WriteString(0,MENU_POSITION(5),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          Work_FanPowerSet(0);
          CIRPUMP_ON;
          TFT_WriteString(0,MENU_POSITION(6),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText7[Language],Calibri);  
          break;
        // ����� ���
        case 7:
          TFT_WriteString(0,MENU_POSITION(6),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          CIRPUMP_OFF;
          Work_HWSPumpPowerSet(100);
          TFT_WriteString(0,MENU_POSITION(7),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText8[Language],Calibri);  
          break;
        // ������ ����� ��� ����� ��������
        case 8:
          TFT_WriteString(0,MENU_POSITION(0),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText8[Language],Calibri);  
          TFT_WriteString(0,MENU_POSITION(0),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          Work_HWSPumpPowerSet(0);
          Check_Stage++;
        // ������
        case 9:
          TFT_WriteString(0,MENU_POSITION(1),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText9[Language],Calibri);  
          ALARM_ON;
          TFT_WriteString(0,MENU_POSITION(8),
                          C_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) StartSkipText[Language],Calibri);
          break;
        // �������
        case 10:
          TFT_WriteString(0,MENU_POSITION(1),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          ALARM_OFF;
          BUZZER_ON;
          TFT_WriteString(0,MENU_POSITION(2),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText10[Language],Calibri); 
          break;
       // ��������� ���������
       case 11:
          TFT_WriteString(0,MENU_POSITION(2),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          BUZZER_OFF;
          TFT_WriteString(0,MENU_POSITION(3),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText11[Language],Calibri); 
          if (SW1_State == GPIO_PIN_SET) {Check_Stage++; HAL_Delay(500);}
          break;
        // ��������� ���������
        case 12:
          TFT_WriteString(0,MENU_POSITION(3),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(4),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText12[Language],Calibri);  
          if (SW1_State == GPIO_PIN_RESET) {Check_Stage++; HAL_Delay(500);}
          break;
        // ��������� ���������
        case 13:
          TFT_WriteString(0,MENU_POSITION(4),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(5),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText13[Language],Calibri);  
          if (SW2_State == GPIO_PIN_SET) {Check_Stage++; HAL_Delay(500);}
          break;
        // ��������� ���������
        case 14:
          TFT_WriteString(0,MENU_POSITION(5),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(6),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText14[Language],Calibri);  
          if (SW2_State == GPIO_PIN_RESET) {Check_Stage++; HAL_Delay(500);}
          break;
        // ������� ����
        case 15:
          TFT_WriteString(0,MENU_POSITION(6),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(7),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText15[Language],Calibri);  
          if (Stop_EXT_State == GPIO_PIN_SET) 
          {
            Check_Stage++; HAL_Delay(500);
            // ������� �������
            TFT_FillScreen(BACK_COLOR);
            // ������� ����� ������������� ����������
            NEED_UPDATE_MENU_SET;
            NEED_UPDATE_SET;
            NEED_UPDATE_LANG_SET;
            NEED_UPDATE_BAT_SET;
          }
          break;
        // ������ ������� ����
        case 16:
          TFT_WriteString(0,MENU_POSITION(0),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText15[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(0),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          Check_Stage++;
          break;
        // ������� ����
        case 17:
          TFT_WriteString(0,MENU_POSITION(8),
                          C_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) StartSkipText[Language],Calibri);
          TFT_WriteString(0,MENU_POSITION(1),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText16[Language],Calibri);  
          if (Stop_EXT_State == GPIO_PIN_RESET) {Check_Stage++; HAL_Delay(500);}
          break;

        // �������� ������ �����
        case 18:
          TFT_WriteString(0,MENU_POSITION(1),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(2),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText17[Language],Calibri);
          if (sTempIn.Value == TEMP_SENSOR_CLOSURE) {Check_Stage++; HAL_Delay(500);}
          break;
        // ����������� ������ �����
        case 19:
          TFT_WriteString(0,MENU_POSITION(2),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(3),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText18[Language],Calibri);
          if (sTempIn.Value == TEMP_SENSOR_BREAK) {Check_Stage++; HAL_Delay(500);}
          break;
        // �������� ������ ���
        case 20:
          TFT_WriteString(0,MENU_POSITION(3),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(4),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText19[Language],Calibri);
          if (sTempHWS.Value == TEMP_SENSOR_CLOSURE) {Check_Stage++; HAL_Delay(500);}
          break;
        // ����������� ������ ���
        case 21:
          TFT_WriteString(0,MENU_POSITION(4),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(5),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText20[Language],Calibri);
          if (sTempHWS.Value == TEMP_SENSOR_BREAK) {Check_Stage++; HAL_Delay(500);}
          break;
        // �������� ������ �����
        case 22:
          TFT_WriteString(0,MENU_POSITION(5),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(6),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText21[Language],Calibri);
          if (sTempScrew.Value == TEMP_SENSOR_CLOSURE) {Check_Stage++; HAL_Delay(500);}
          break;
        // ����������� ������ �����
        case 23:
          TFT_WriteString(0,MENU_POSITION(6),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          TFT_WriteString(0,MENU_POSITION(7),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText22[Language],Calibri);
          if (sTempScrew.Value == TEMP_SENSOR_BREAK) 
          {
            Check_Stage++; HAL_Delay(500);
            // ������� �������
            TFT_FillScreen(BACK_COLOR);
            // ������� ����� ������������� ����������
            NEED_UPDATE_MENU_SET;
            NEED_UPDATE_SET;
            NEED_UPDATE_LANG_SET;
            NEED_UPDATE_BAT_SET;          
          }
          break;
        // ������ ����������� ������ �����
        case 24:
          TFT_WriteString(0,MENU_POSITION(0),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText22[Language],Calibri);
          TFT_WriteString(0,MENU_POSITION(0),
                          R_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) OkSettings[Language],Calibri); 
          Check_Stage++;
        // �������
        case 25:
          TFT_WriteString(0,MENU_POSITION(1),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText23[Language],Calibri);
          TFT_WriteFloatValue(0, MENU_POSITION(1),
                             R_ALIGN,sFrequency.Value,2,0,0,  
                             (char*) UnitNullName[Language],TEXT_COLOR,BACK_COLOR, 
                             Calibri);
          TFT_WriteString(0,MENU_POSITION(8),
                          C_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) StartSkipText[Language],Calibri);
          break;
        // ��������� ��������
        case 26:
          TFT_WriteString(0,MENU_POSITION(2),
                          NO_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) CheckText24[Language],Calibri);
          TFT_WriteString(0,MENU_POSITION(8),
                          C_ALIGN,TEXT_COLOR,BACK_COLOR,
                          (char*) StartConfirmText[Language],Calibri);
          break;
      }
    }
    else
    // ����� ��������
    if (Item == (sMenu*) &ResetSettingsMenu)
    {
      void (*ConfirmVoidFunc) (void);
      sMenu *MenuChildNo;
      sMenu *MenuChildYes;
      
      if (Item == (sMenu*) &ResetSettingsMenu)
      {
        ConfirmVoidFunc = Menu_ResetSettings;
        MenuChildNo = &ServiceMenu;
        MenuChildYes = &ServiceMenu; 
      }
      
      // 0. ���
      TFT_WriteMenuString (0, MENU_POSITION(1),
                           Menu_SelectedItem,
                           MenuChildNo, 0,
                           (char*) NoSettings[Language],
                           0,
                           TEXT_COLOR, BACK_COLOR,
                           Calibri);    
      // 1. �� 
      TFT_WriteMenuString (0, MENU_POSITION(2),
                           Menu_SelectedItem,
                           MenuChildYes, 1,
                           (char*) YesSettings[Language],
                           ConfirmVoidFunc,
                           TEXT_COLOR, BACK_COLOR,
                           Calibri);    
    }
  }
  return 0;
}