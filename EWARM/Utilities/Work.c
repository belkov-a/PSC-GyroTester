#include "main.h"
#include "Init.h"
#include "Menu.h"
#include "adc.h"
#include "24C_EEPROM.h"

/*******************************************************************************
  *
  * ���������� ������� ������
  *
*******************************************************************************/

uint8_t MODE_FLAG = SWITCHOFF_MODE;

void Work_ModeHandler(void)
{
  if (Menu_SelectedItem != (sMenu*) &CheckWindow)
  {
    switch(MODE_FLAG)
    {
      // ����� ��������
      case SWITCHOFF_MODE:
        // ����� ��������� 
        FeedTime_Counter = 0;
        FeedTime_Num = 0;
        BlowingTime_Counter = 0;

        break;
              
      // ����� ����������
      case IGNITION_MODE:
        // �������� ����������� �� ���������� ��������
        Work_TempSetAchieve();
        // �������� ����������� �� ������� � ���������
        Work_TempRegulationAchieve();
        break;
      
      // ����� ��������
      case CONTROL_MODE:
        // �������� ����������� �� ������ �����������
        Work_TempFallHyster();
        // �������� �� ������� ����������� �� ����������� ����������
        Work_TempOffAchieve();
        break;
        
      // ����� ���������
      case REGULATION_MODE:
        // �������� �� ���������� �������� �����������
        Work_TempSetAchieve();
        // �������� �� ������� ����������� �� ����������� ����������
        Work_TempOffAchieve();
        break;
        
      // ����� �������
      case EXTINGUISHING_MODE:      
        // �������� �� ����� ����������� ���� ����������� ����������
        Work_TempOnAchieve();          
        // �������� �� ������� � �������� ����� ����� �������
        Work_ExtinguishingEnd();
        break;
        
      // ����� ��������
      case WAITING_MODE:
    
        break;
    }
    // ���� �����������
    Work_FanCycle();
    
    // ���� ������
    Work_SupplyCycle();
    
    // ���� ����������
    Work_ThermostatCycle();
    
    // ���� ����/����
    Work_DayNightCycle();
    
    // ���� ������ ���
    Work_HWSPumpCycle();
    
    // ���� ��������������� ������ ���������
    Work_CirPumpCycle();
    
    // ���� ��������� ��������������
    Work_TempAlarmCycle();
    
    // �������� ���� �������
    Work_BuzzerShort(); 
    
    // ������� ����
    Work_StopEXT();
  }
}
  
/*******************************************************************************
  *
  * ������������� �������� �����������
  *
*******************************************************************************/

float Power_Fan_mdf = 0;

void Work_FanPowerSet (uint16_t Power)
{
  // ������ �������� �������� ��������
  // Power �������� � ��������� �� 10 �� 100, ������� ���������� � ��������� 
  // ����� ������������ � ����������� ���������:
  // Power = 100% -> Fan = Wmax
  // Power = 10% - > Fan = Wmin + 10% 
  
  if (Power > 0)
  {
    float k = sFanPowerMax.Value-sFanPowerMin.Value;
    k /= sFanPower.Max;
    uint16_t b = sFanPowerMin.Value;
    
    Power_Fan_mdf = Power * k + b;
  }
  else Power_Fan_mdf = 0;
  
  if (Power == 0) {FAN_OFF;}
  else            {FAN_ON;}

  sFanCurrent.Value = Power;
}

/*******************************************************************************
  *
  * ���� ������ �����������
  *
  * � ���� ������� ���������� �� ��������������
  *
*******************************************************************************/

void Work_FanCycle (void)
{
  // ���� �� � ������ ������ � �� ����������� �����������
  if (ISNT_MANUAL_MENU && ISNT_CRIT_TEMP)
  {
    // ���� ��������� ���������
    if (ISNT_OVERFLOW)
    {
      switch (MODE_FLAG)
      {
        // � ������ �������� � �������� ��������� ����������
        case SWITCHOFF_MODE:
        case WAITING_MODE:
          Work_FanPowerSet(0);
          break;
        // � ������ ����������, ��������� ��� �������
        case IGNITION_MODE:
        case REGULATION_MODE:
        case EXTINGUISHING_MODE:
          // �������� ���������� �� �������� "�������� �� ������"
          if (IS_FIREWOOD)        Work_FanPowerSet(sFanPowerFirewood.Value);
          // �������� ���������� �� �������� "�������� �����������"
          else if (ISNT_FIREWOOD) Work_FanPowerSet(sFanPower.Value);
          break;  
        // � ������ ��������
        case CONTROL_MODE:
          // �������� ����������� �� ����� ������� � "�������� � ��������"
          if (BlowingTime_Counter < sBlowingTime.Value)
          {
            Work_FanPowerSet(sFanPowerBlowing.Value);
          }
          // ����� ���������
          else
          {
            Work_FanPowerSet(0);
          }
          break;
      }
    }
    else
    {
      // ��������� ���������� ��� ���������
      Work_FanPowerSet(0);
    }
  }
}

/*******************************************************************************
  *
  * ������������� �������� �����
  *
*******************************************************************************/

uint8_t Power_Screw = 0;

void Work_SupplyPowerSet (uint16_t Power)
{   
  if (Power == 0) 
  {
    SCREW_OFF;
  }
  else            
  {
    SCREW_ON;
    // ������ ������� �� Period c ��������� Power*Perc
    // ��� ���� Power, ��� ������ ������ �������� �������
    Power_Screw = Power;
  }
}

/*******************************************************************************
  *
  * ���� ������ �����
  *
  * � ������ ������ �� ��������������
  *
*******************************************************************************/

void Work_SupplyCycle (void)
{
  // ���� ������ �� ��������� 7 ����
  if (ISNT_SCREW_SENSOR_DEAD)
  {
    // ���� ��������� ���������
    if (ISNT_OVERFLOW)
    {
      // ���� �� � ������ ������ � �� ����������� �����������
      if (ISNT_MANUAL_MENU && ISNT_CRIT_TEMP)
      {
        switch (MODE_FLAG)
        {
          // � ������ �������� � �������� ��������� ����
          case SWITCHOFF_MODE:
          case WAITING_MODE:
            Work_SupplyPowerSet(0);
            break;
          // � ������ ����������, ��������� ��� �������
          case IGNITION_MODE:
          case REGULATION_MODE:
          case EXTINGUISHING_MODE:
            // ���� ���� ������ � �� �� ������, �������� ����
            if ((FeedTime_Counter < sSupplyTime.Value) && ISNT_FIREWOOD)
            {
              sSupplyCurrent.Value = sSupplyTime.Value - FeedTime_Counter;
              Work_SupplyPowerSet(100);
            }
            else
            // ����� �������� ��� �����
            {
              Work_SupplyPowerSet(0);
            }
            break;
          
          // � ������ ��������
          case CONTROL_MODE:
            // �������� ������ ����� ���������� �����, ���� �� �� ������
            if ((FeedTime_Counter < sSupplyTime.Value) &&
                (FeedTime_Num == 0) &&
                ISNT_FIREWOOD)
            {
              sSupplyCurrent.Value = sSupplyTime.Value - FeedTime_Counter;
              Work_SupplyPowerSet(100);
            }
            else
            // ����� �������� ��� �����
            {
              Work_SupplyPowerSet(0);
            } 
            break;
        }
      }
    }
    else
    // ���� �������� ���������
    {
      Work_SupplyPowerSet(100);
      sSupplyCurrent.Value = sTimeTransfer.Value*60 - Overflow_Counter;
    }
  }
}

/*******************************************************************************
  *
  * �������� ������ �������
  *
*******************************************************************************/

void Work_BuzzerShort (void)
{
    // �������� �� ����� ������ 250 ��
  if (Buzzer_Counter > 0) {BUZZER_ON;}
  else                    {BUZZER_OFF;}
}

/*******************************************************************************
  *
  * ���� ������ ����������
  *
*******************************************************************************/

uint8_t SW2_P = 0; // ��������� ��� ��������

void Work_ThermostatCycle (void)
{
  // ��������� ��������� ����������, ���� �������
  if (IS_THERMO)
  {
    if (SW2_State) THERMO_CLOSE;
    else           THERMO_OPEN;
  }
}

/*******************************************************************************
  *
  * �������� �������� � �������� ����� ����� �������
  *
*******************************************************************************/

void Work_ExtinguishingEnd (void)
{
  // ���� ����� ������� �����������
  if (IS_EXTINGUISHING_MODE && (Extinguishing_Counter == 0))
  {
    // ������� � ��������
    MODE_FLAG = WAITING_MODE;
    
    ADC_FLAG_SET;
    // ����� ��������� 
    FeedTime_Counter = 0;
    FeedTime_Num = 0;
    BlowingTime_Counter = 0;
  }
}

/*******************************************************************************
  *
  * ���� ������ ������ ����/����
  *
*******************************************************************************/

void Work_DayNightCycle(void)
{
  // ���� ����� ����/����
  if (IS_DAYNIGHT)
  {
    // ���� ������ �����
    if ((sTimeCurrent.Hours >= sNightPeriodStartTime.Value) ||
        (sTimeCurrent.Hours < sDayPeriodStartTime.Value))
      NIGHT_SET;
    else
    // ���� ������� �����
    if ((sTimeCurrent.Hours >= sDayPeriodStartTime.Value) &&
        (sTimeCurrent.Hours < sNightPeriodStartTime.Value))
      DAY_SET;
  }
}

/*******************************************************************************
  *
  * ������������� �������� ������ ���
  *
*******************************************************************************/

uint8_t Power_HWS = 0;

void Work_HWSPumpPowerSet (uint16_t Power)
{ 
  if (Power == 0) 
  {
    HWSPUMP_OFF;
  }
  else            
  {
    HWSPUMP_ON;
    // ������ ������� �� Period c ��������� Power*Period/100
    // ��� ���� Power, ��� ������ ������ �������� �������
    Power_HWS = Power;
  }
}

/*******************************************************************************
  *
  * ���� ������ ������ ���
  *
*******************************************************************************/

void Work_HWSPumpCycle (void)
{
  // ���� �������� ��������� ���������, �� �������� �����
  if (IS_ALARM_THERMO_TRIG)
  {
    Work_HWSPumpPowerSet(100);
  }
  else
  // ����� ��� ���������� ��� ����������� ����� ���� 40 � �� ��������
  if ((sTempIn.Value >= 400) && ISNT_OFF(sTemperatureHWS) && ISNT_HWS_SENSOR_DEAD)
  {
    // ���� ��������, �� �������� ����� ���
    if (sTempIn.Value >= 800)
    {
      Work_HWSPumpPowerSet(100);
    }
    else
    {
      // ���� ��������� ������� ��������
      if (ISNT_BOILER_PRIORITY)
      {
        // �� �������� ����� ���, ���� ����������� ��� ���� ����������� �����
        if (sTempHWS.Value > sTempIn.Value)
        {
          Work_HWSPumpPowerSet(0);
          HWS_MORE_SET; 
        }
        else
        // ���� ����������� ��� ������ ���� ����� ����� ���������� ��� ������ ��
        // ������, �� ���� �� ���������
        if (((sTempHWS.Value < sTempIn.Value - 20) && IS_HWS_MORE_SET) ||
            ((sTempHWS.Value <= sTempIn.Value) && ISNT_HWS_MORE_SET))
        {
          HWS_LESS_SET;
          // ���� ����������� ��� ����� ���� �������� ��� ����� ����������,
          // �� �������� ����� ���
          if (sTempHWS.Value < ((sTemperatureHWS.Value - sHysteresisHWSSensor.Value)*10))
          {
            Work_HWSPumpPowerSet(100);
          } 
          else
          // ���� ����������� ��� �������� �������� ���, �� ��������� ����� ���
          if (sTempHWS.Value >= sTemperatureHWS.Value*10)
          {
            Work_HWSPumpPowerSet(0);
          }
        }
      }
      else
      // ���� ��������� ������� �������
      if (IS_BOILER_PRIORITY)
      {
        // ���� ��������� ������� � ����������� ��� ���� ����������� �����, ��
        // ��������� ����� ���
        if ((sTempHWS.Value > sTempIn.Value) && IS_THERMO_ON)
        {
          Work_HWSPumpPowerSet(0);
          HWS_MORE_SET;
        }
        else
        // ���� ����������� ��� ������ ���� ����� ����� ���������� ��� ������ ��
        // ������, �� ���� �� ���������
        if (((((sTempHWS.Value < sTempIn.Value - 20) && IS_HWS_MORE_SET) ||
              ((sTempHWS.Value <= sTempIn.Value) && ISNT_HWS_MORE_SET)) && IS_THERMO_ON) ||
            IS_THERMO_OFF)
        {
          HWS_LESS_SET;
          // ���� ����������� ��� ����� ���� �������� ��� ����� ����������
          if (sTempHWS.Value < ((sTemperatureHWS.Value - sHysteresisHWSSensor.Value)*10))
          {
            Work_HWSPumpPowerSet(100);
          } 
          else
          // ���� ����������� ��� �������� �������� ���, �� ��������� ����� ���
          if (sTempHWS.Value >= sTemperatureHWS.Value*10)
          {
            Work_HWSPumpPowerSet(0);
          }
        }
      }
    }
  }
  else
  // ���� ����������� ���������� ���� 5 ��������, �� �������� ����� ��� ����������
  // ���������� ������� ���������
  if ((sTempIn.Value <= 50) && (sTempIn.Value > -200) && 
      ISNT_OFF(sTemperatureHWS) && ISNT_HWS_SENSOR_DEAD)
  {
    Work_HWSPumpPowerSet(100);
  }
  else
  // ���� ����������� ����� ����� ���� 36, �� ��������� ����� ���
  if ((sTempIn.Value <= 360) && (sTempIn.Value >= 70) && 
      ISNT_OFF(sTemperatureHWS) && ISNT_HWS_SENSOR_DEAD)
  {
    Work_HWSPumpPowerSet(0);
  }
  else
  // ���� ����������� ��� - ����, �� ����� ��� ��������
  if (IS_OFF(sTemperatureHWS) || IS_HWS_SENSOR_DEAD)
  {
    Work_HWSPumpPowerSet(0);
  }
}

/*******************************************************************************
  *
  * ���� ������ ��������������� ������ ���������
  *
*******************************************************************************/

void Work_CirPumpCycle (void)
{
  // �� �������������� � ������ ������
  if (ISNT_MANUAL_MENU)
  {
    // ���� ����������� ����� ������ 80 ��� ������ 5, ��� ������ �������,
    // ��� ����������� ��������� ���������, �� ����� �������� ���������
    if ((sTempIn.Value >= 800) || 
        (sTempIn.Value <= 50) || 
        (sTempIn.Value == TEMP_SENSOR_CLOSURE) ||
        (sTempIn.Value == TEMP_SENSOR_BREAK) ||
        IS_ALARM_THERMO_TRIG) 
    {
      CIRPUMP_ON;
      CirPumpStart_Counter = 0;
      CirPump_Counter = 0;
    }
    else
    // � ��������� ������� �������� �� ���������
    {
      // �������� ����-���� ��� � ������ �� 1 ������
      if (IS_SWITCHOFF_MODE && (AntiSTOP_Counter == 10079))
      { 
        CIRPUMP_ON;
      }
      else
      // ���� ����������� ����� ���� ����������� ��������� ������ ���������, �� 
      // �������� ����� �� ���������
      if ((sTempIn.Value >= (sTemperatureHeatingPumpStart.Value * 10)) && 
          ISNT_OFF(sTemperatureHeatingPumpStart))
      { 
        // ���� ��������� ������� ��������
        if (ISNT_BOILER_PRIORITY) 
        {
          // ���� ��������� ��������, �� �������� ����� ���������
          if (IS_THERMO_OFF)
          {
            CIRPUMP_ON;
            CirPumpStart_Counter = 0;
            CirPump_Counter = 0;
          }
          else
          // ���� ��������� �������
          if (IS_THERMO_ON)
          {
            // ���� ��������� ���������
            if (IS_THERMO_OPEN)
            {
              CIRPUMP_ON;
              CirPumpStart_Counter = 0;
              CirPump_Counter = 0;              
            }
            else
            // ���� ��������� �������
            if (IS_THERMO_CLOSE)
            {
              Work_CirPumpCircThermo();            
            }
          }
        }
        else
        // ���� ��������� ������� �������
        if (IS_BOILER_PRIORITY)
        {
          // ���� ��������� ��������
          if (IS_THERMO_OFF)
          {
            // ���� ����������� ��� ����� ���� �������� ��� ����� ����������,
            // �� ��������� ����� ���������
            if (sTempHWS.Value < ((sTemperatureHWS.Value - sHysteresisHWSSensor.Value)*10))
            {
              CIRPUMP_OFF;
              CirPumpStart_Counter = 0;
              CirPump_Counter = 0; 
            } 
            // ���� ����������� ��� �������� �������� ���, �� �������� ����� ���������
            else 
            if (sTempHWS.Value >= sTemperatureHWS.Value*10)
            {
              CIRPUMP_ON;
              CirPumpStart_Counter = 0;
              CirPump_Counter = 0;               
            }
          }
          // ���� ��������� �������
          else
          if (IS_THERMO_ON)
          {
            // �������� ����� ���������, ���� ����������� ��� ���� ����������� �����
            if (sTempHWS.Value > sTempIn.Value)
            {
              HWS_MORE_SET;
              // ���� ��������� �������, �� �������� ����� ��������� � ���������
              // ������
              if (IS_THERMO_CLOSE)
              {
                Work_CirPumpCircThermo();
              }
              // ����� ������ ��������
              else
              {
                CIRPUMP_ON;
                CirPumpStart_Counter = 0;
                CirPump_Counter = 0;  
              }
            }
            else
            // ���� ����������� ��� ������ ���� ����� ����� ���������� ��� ������ ��
            // ������, �� ���� �� ���������
            if (((sTempHWS.Value < sTempIn.Value - 20) && (IS_HWS_MORE_SET)) ||
                ((sTempHWS.Value <= sTempIn.Value) && (ISNT_HWS_MORE_SET)))  
            {
              HWS_LESS_SET;
              // ���� ����������� ��� ����� ���� �������� ��� ����� ����������,
              // �� ��������� ����� ���������
              if (sTempHWS.Value < ((sTemperatureHWS.Value - sHysteresisHWSSensor.Value)*10))
              {
                CIRPUMP_OFF;
                CirPumpStart_Counter = 0;
                CirPump_Counter = 0; 
              }
              else
              // ���� ����������� ��� �������� �������� ���, �� �������� ����� ���������
              if (sTempHWS.Value >= sTemperatureHWS.Value*10)
              {   
                // ���� ����� �������� � ��������� �������, �� �������� �����
                // ��������� � ��������� ������
                if (IS_THERMO_CLOSE && IS_CONTROL_MODE)
                {
                  Work_CirPumpCircThermo();                  
                }       
                // ����� ������ ��������
                else
                {
                  CIRPUMP_ON;
                  CirPumpStart_Counter = 0;
                  CirPump_Counter = 0; 
                }                  
              }
              // ���� � ���������� ����������
              else
              {
                // ���� ��������� ������� � ����� ��������, �� ������������
                // ����������� ����� ������ ������ ���������
                if (IS_THERMO_CLOSE && IS_CONTROL_MODE)
                {
                  Work_CirPumpCircThermo();
                }      
              }
            }
          }
        }
      }
      else
      // ���� ����������� ����� ����� ���� ����������� ��������� ������ ���������
      // ����� 4 �������, �� ��������� �����
      if (((sTempIn.Value <= ((sTemperatureHeatingPumpStart.Value - 4) * 10)) &&
           (sTempIn.Value >= 70)) ||
          IS_OFF(sTemperatureHeatingPumpStart))
      {
        CirPumpStart_Counter = 0;
        CirPump_Counter = 0;
        CIRPUMP_OFF;
      }
    }
  }
}

/*******************************************************************************
  *
  * ���������� ������������ ������ ������ ������ ���������
  *
  * ������������ 
  * - ��� ����������� � ���������� ���������� � ������ �������� ��� 
  *   ���������� ���������� �������
  * - ��� ����������� � ���������� ���������� ��� ����������� ���������� �������
  *
*******************************************************************************/

void Work_CirPumpCircThermo (void)
{
  // �������� ����� �� 25 ������ ��� ��������� ���������
  if (CirPumpStart_Counter < 25) {CIRPUMP_ON;}
  else
  {
    // �������� ����� �� ����� ��������� ������
    if (CirPump_Counter < sTimePumpOn.Value) {CIRPUMP_ON;}
    else                                     {CIRPUMP_OFF;}
  }                    
}

/*******************************************************************************
  *
  * �������������� �� �����������
  *
  * ����������:
  * 1. ������ ���� ��������� ���������� ���������� ��� ���������� 85-90 �
  * 2. ����������� ����������� ����� ����� 90 �
  * 3. �������� ����� ������ 
  * 4. ������� ����������� ����� �� 80 �� 90 �
  * 5. ������� ����������� ���� 5 �
  *
*******************************************************************************/

uint8_t FF_Byte[1] = {0xFF};

void Work_TempAlarmCycle (void)
{
  // ���� �������� ��������� ���������, �� ��������� ���� � ����������, ��������� 
  // � ����� ��������. �������� ������� ����� ������, ����� ����������� ������ ���� 50�
  if (SW1_State == GPIO_PIN_RESET)
  {
    if (ISNT_ALARM_THERMOSTAT)
    {
      MODE_FLAG = SWITCHOFF_MODE;
    }
      
    ALARM_THERMOSTAT_ON;
    ALARM_THERMO_TRIG_SET;
    ALARM_ON;
    Buzzer_Counter = 4; // �� ������������, ����� ���������
  }
  else
  // ���� ����������� ���� 90 ��������, �� ����������� �������� �����
  // ���������� �������� ������������, ���� � ���������� �����������,
  // ����� ��������� �������� � ���������� ������
  if (sTempIn.Value >= 900) 
  {
    ALARM_ON;
    CRIT_TEMP_ON;
    // ���� �� � ������ ����������
    if (ISNT_MANUAL_MENU)
    {
      CIRPUMP_ON;
      Work_FanPowerSet(0);
      SCREW_OFF;
    }
    Buzzer_Counter = 4; // �� ������������, ����� ���������
  }
  else
  // ���� ����������� ����� ��������� ����������� ������� ����� � �� ��������, �������� ���������
  if ((sTempScrew.Value >= sTemperatureScrewAlarm.Value * 10) && ISNT_OFF(sTemperatureScrewAlarm))
  {
    ALARM_ON;
    SCREW_TEMP_ON;
    if (ISNT_OVERFLOW_START) {OVERFLOW_START_ON; OVERFLOW_ON; OVERFLOW_BUZZER_ON;}
    if (IS_OVERFLOW_BUZZER) Buzzer_Counter = 4; // ����� ���������, ����������� ��� ������� ����� ������
  }
  else
  // ���� ����������� ����� �� 80 �� 90, ������������ ���������� �������� ������, 
  // ���������� ��������� ������, �� ����� ��������
  if ((sTempIn.Value >= 800) && (sTempIn.Value < 900))
  {
    ALARM_ON;
    HIGH_TEMP_ON; 
    
    // ����� 1 ������� ��� � 30 ���
    if (Buzzer_Counter == 0) 
    {
      if (Buzzer_Period_Counter == 0) 
      {
        Buzzer_Period_Counter = 30;
        Buzzer_Counter = 4;
      }
    }
  }
  else
  // ���� ����������� ��� ���� 80, ������������ ���������� �������� ������,
  // ���������� ��������� ������, �� ����� ��������
  if ((sTempHWS.Value >= 800))
  {
    ALARM_ON;
    HWS_TEMP_ON;
    
    // ����� 1 ������� ��� � 30 ���
    if (Buzzer_Counter == 0) 
    {
      if (Buzzer_Period_Counter == 0) 
      {
        Buzzer_Period_Counter = 30;
        Buzzer_Counter = 4;
      }
    }
  }
  else
  // ���� ����������� ����� ���� 5 �, �������� �������� ������ � ��������� ������,
  // ������ �������� � ���������� ������ 
  if ((sTempIn.Value < 50) && (sTempIn.Value > -200))
  {
    ALARM_ON;
    LOW_TEMP_ON;
  }
  
  // ���� ��������� ��������� ���������, � ����������� ����� ���� 50�, �� ��������� ���� ���������� ����������
  if(SW1_State)
  {
    if (IS_ALARM_THERMOSTAT) {ALARM_THERMOSTAT_OFF;}
    if (IS_ALARM_THERMO_TRIG && (sTempIn.Value < 500)) {ALARM_THERMO_TRIG_RESET;}
  }
    
  // ���� ����������� ����� ���� 89 �������� ��� ������ ���� �����������, �� ��������� ���� ����������� ����
  if ((sTempIn.Value < 890) && IS_CRIT_TEMP) {CRIT_TEMP_OFF;}
  
  // ���� ����������� ����� ���� 80 �������� ��� ������ ���� �����������, ��������� ���� ���������
  if ((sTempIn.Value < 800) && IS_HIGH_TEMP) {HIGH_TEMP_OFF;}
  
  // ���� ����������� ��� ����� ���� 80 �������� ��� ������ ���� �����������, ��������� ���� ��������� ���
  if ((sTempHWS.Value < 800) && IS_HWS_TEMP) {HWS_TEMP_OFF;}
  
  // ���� ����������� ��������� ���� 5 �������� ��� ���������� ������, ��������� ���� ������ �����������
  if (((sTempIn.Value > 50) || (sTempIn.Value == TEMP_SENSOR_CLOSURE) || (sTempIn.Value == TEMP_SENSOR_BREAK)) && 
      IS_LOW_TEMP) {LOW_TEMP_OFF;};
  
  // ���� ����������� ����� ����� ���� ����������� ������� ����� ��� ���������� ������, 
  // ��������� ���� ������� �����
  if ((sTempScrew.Value < (sTemperatureScrewAlarm.Value - 2)*10) && IS_SCREW_TEMP) 
  {
    SCREW_TEMP_OFF;
    OVERFLOW_START_OFF;
    //OVERFLOW_OFF;
    OVERFLOW_BUZZER_OFF;
  }
  
  // ���� ��������� ��� ����� ������ �����, �� �����, ��������� � ����� ��������, 
  // �������� ������ ������, �������� ����� ��������� 
  if ((sTempIn.Value == TEMP_SENSOR_CLOSURE) || (sTempIn.Value == TEMP_SENSOR_BREAK))
  {
    TEMP_SENSOR_SET;
    ALARM_ON;
    MODE_FLAG = SWITCHOFF_MODE;
    Buzzer_Counter = 4;
  }
  else
  // ���� ��������� ��� ����� ������� �����, �� ����� ��� ������ �����, �������� ������,
  // ������� �� ���������� ��� ����������� ������� ����� - ����.
  // ���� ������ 7 ����, �� ��������� � ����� ��������. ������ �������� ������ �� ������
  if ((sTempScrew.Value == TEMP_SENSOR_CLOSURE) || (sTempScrew.Value == TEMP_SENSOR_BREAK))
  {
    SCREW_SENSOR_SET;
    ALARM_ON;
    if (ISNT_OFF(sTemperatureScrewAlarm) && IS_SCREW) Buzzer_Counter = 4;
    
    if (IS_SCREW_SENSOR_DEAD && IS_AUTO_MODE && ISNT_OFF(sTemperatureScrewAlarm)) 
      MODE_FLAG = SWITCHOFF_MODE;
  }
  else
  // ���� ��������� ��� ����� ������� ���, �� ����� ��� ������ ������ ���, �������� ������,
  // ������� �� ���������� ��� ����������� ��� - ���� (�� 7 ����) ��� ����
  // ���� ������ 7 ����, �� ��������� ����������� ��� - ���� �� �����������, � ����� ���������
  // � ����� �������� � ������ �� ����������
  if ((sTempHWS.Value == TEMP_SENSOR_CLOSURE) || (sTempHWS.Value == TEMP_SENSOR_BREAK))
  {
    HWS_SENSOR_SET;
    ALARM_ON;
    if (ISNT_OFF(sTemperatureHWS) && 
        ((IS_HWS_SENSOR_DEAD 
          || 
          (ISNT_HWS_SENSOR_DEAD && (sTemperatureHWS.Value != sTemperatureHWS.Max * 10))) 
         &&
         IS_HWSPUMP))
      Buzzer_Counter = 4;
    
    if (IS_HWS_SENSOR_DEAD && IS_AUTO_MODE && ISNT_OFF(sTemperatureHWS))
      MODE_FLAG = SWITCHOFF_MODE;
  }
   
  // ���� ��������� ��� ����� ������� ����� �����������, � ���� �������, �� ����������
  if ((sTempIn.Value != TEMP_SENSOR_CLOSURE) && (sTempIn.Value != TEMP_SENSOR_BREAK))
  {
    if (IS_TEMP_SENSOR) {TEMP_SENSOR_RESET;}
  }
  
  // ���� ��������� ��� ����� ������� ����� �����������, � ���� �������, �� ����������
  if ((sTempScrew.Value != TEMP_SENSOR_CLOSURE) && (sTempScrew.Value != TEMP_SENSOR_BREAK))
  {
    if (Screw_Sensor_Counter[0] != 0)
    {
      Screw_Sensor_Counter[0] = 0;
      EEPROM_WriteByte ((uint8_t*) FF_Byte,SCREW_SENS_ADDR);  
    }
    if (IS_SCREW_SENSOR || IS_SCREW_SENSOR_DEAD)
    {
      SCREW_SENSOR_DEAD_RESET;
      SCREW_SENSOR_RESET;
    }
  }
  
  // ���� ��������� ��� ����� ������� ��� �����������, � ���� �������, �� ����������
  if ((sTempHWS.Value != TEMP_SENSOR_CLOSURE) && (sTempHWS.Value != TEMP_SENSOR_BREAK))
  {
    if (HWS_Sensor_Counter[0] != 0)
    {
      HWS_Sensor_Counter[0] = 0;
      EEPROM_WriteByte ((uint8_t*) FF_Byte,HWS_SENS_ADDR);
    }
    if (IS_HWS_SENSOR || IS_HWS_SENSOR_DEAD)
    {
      HWS_SENSOR_RESET;
      HWS_SENSOR_DEAD_RESET;
    }
  }
  
  // ���� ��� ��������������, �� ��������� ������
  if (WARNING_FLAGS == 0x00000000) { ALARM_OFF;}
  
}

/*******************************************************************************
  *
  * �������� ����������� ��� �������� � ��������� �� �������
  *
  * ����������� �������� - ����������� ���������� ����� + 15 �
  *
*******************************************************************************/

uint8_t Work_TempRegulationAchieve (void)
{
  if (sTempIn.Value >= ((sTemperatureOff.Value * 10) + 150))
  {
    // ���� 2 ������� ��� ���������� ������� ������������
    if (ModeChange_Counter == 2) ModeChange_Counter = 0;
    if (ModeChange_Counter == 1)
    {    
      // ��������� � ���������
      MODE_FLAG = REGULATION_MODE;
      ADC_FLAG_SET;
      MODE_Buf[0] = MODE_FLAG;
      EEPROM_WriteByte ((uint8_t*) MODE_Buf,MODE_FLAG_ADDR);
      return 1;
    }
  }
  return 0;
}

/*******************************************************************************
  *
  * �������� ����������� �� ���������� ��������
  *
*******************************************************************************/

uint8_t Work_TempSetAchieve (void)
{
  // ���� ��������� ������� ��������
  if (ISNT_BOILER_PRIORITY)
  {
    // ���� ��������� ��������
    if (IS_THERMO_OFF)
    {
      // ���� ����������� ����� �������� ��������, �� ��������� � ����� ��������
      if (((sTempIn.Value >= (sTemperatureSetting.Value*10)) && ISNT_NIGHT) ||
          ((sTempIn.Value >= (sTempNight.Value*10)) && IS_NIGHT)) {}
      else return 0;
    }
    else
    // ���� ��������� �������
    if (IS_THERMO_ON)
    {
      // ���� ����������� ����� �������� �������� ��� ���������� �������� ����������
      // ��� ����������� ����� ���� �����������, �� ��������� � ����� ��������
      if ((((sTempIn.Value >= (sTemperatureSetting.Value*10)) && ISNT_NIGHT) ||
           ((sTempIn.Value >= (sTempNight.Value*10)) && IS_NIGHT))
          ||
            (IS_THERMO_CLOSE && (sTempIn.Value >= (sTemperatureSetting.Min*10)))) {}
      else return 0;
    }
    else return 0;
  }
  else
  // ���� ��������� ������� �������
  if (IS_BOILER_PRIORITY)
  {
    // ���� ��������� ��������
    if (IS_THERMO_OFF)
    {
      // ���� ����������� ����� ������ ������������ ��� ����������� ��� ������
      // �������� ��� ��� ����������� ����� ���� ��������, �� ��������� � ��������
      if ((sTempIn.Value >= (sTemperatureSetting.Max*10)) ||
           ((((sTempIn.Value >= (sTemperatureSetting.Value*10)) && ISNT_NIGHT) ||
             ((sTempIn.Value >= (sTempNight.Value*10)) && IS_NIGHT))
             && 
            (sTempHWS.Value >= (sTemperatureHWS.Value*10)))) {}
      else return 0;
    }
    else
    // ���� ��������� �������
    if (IS_THERMO_ON)
    {
      // ���� ����������� ����� ������ ������������ ��� ����������� ��� ������
      // �������� ��� ��� ����������� ����� ���� ��������, ���� ��� �����������
      // ����� ���� ����������� � ��������� �������, �� ��������� � ��������
      if ((sTempIn.Value >= (sTemperatureSetting.Max*10)) ||
          ((sTempHWS.Value >= (sTemperatureHWS.Value*10)) &&
           ((((sTempIn.Value >= (sTemperatureSetting.Value*10)) && ISNT_NIGHT) ||
             ((sTempIn.Value >= (sTempNight.Value*10)) && IS_NIGHT)) ||
             ((sTempIn.Value >= sTemperatureSetting.Min*10) && IS_THERMO_CLOSE)))) {}
      else return 0;      
    }
    else return 0; 
  }
  else return 0;
  
  // ���� 2 ������� ��� ���������� ������� ������������
  if (ModeChange_Counter == 2) ModeChange_Counter = 0;
  if (ModeChange_Counter == 1)
  {
    // ��������� � ��������, ���������� ����� � ������, ���� ������� �� �������
    if (IS_IGNITION_MODE)
    {
      MODE_FLAG = CONTROL_MODE;
      MODE_Buf[0] = MODE_FLAG;
      EEPROM_WriteByte ((uint8_t*) MODE_Buf,MODE_FLAG_ADDR);
    }
    else MODE_FLAG = CONTROL_MODE;
    
    ADC_FLAG_SET;
    FeedTime_Counter = sBlowingTime.Value; // ����� ��������� 1 ������ � ��������
    FeedTime_Num = sSupplyRepetition.Value + 1; // �� ������ ������
    BlowingTime_Counter = sBlowingTime.Value;
  }
  return 1;
}

/*******************************************************************************
  *
  * �������� �� ������� ����������� �� �������� �����������
  *
*******************************************************************************/

uint8_t Work_TempFallHyster (void)
{
  // ���� ��������� ������� ��������
  if (ISNT_BOILER_PRIORITY)
  {
    // ���� ��������� ��������
    if (IS_THERMO_OFF)
    {
      // ���� ����������� ����� ����� ���� �������� ����� ����������
      // � ������ ����/����, �� ��������� � ����� ���������
      if (((sTempIn.Value < ((sTemperatureSetting.Value - sHysteresisBoilerSensor.Value)*10)) && ISNT_NIGHT) ||
          ((sTempIn.Value < ((sTempNight.Value - sHysteresisBoilerSensor.Value)*10)) && IS_NIGHT)) {}
      else return 0;
    }
    else
    // ���� ��������� �������
    if (IS_THERMO_ON)
    {
      // ���� ����������� ����� ����� ���� �������� ����� ���������� � ������ 
      // ����/���� � ��������� ��������� ��� ����������� ����� ����� ����
      // �����������, �� ��������� � ����� ���������
      if (((((sTempIn.Value < ((sTemperatureSetting.Value - sHysteresisBoilerSensor.Value)*10)) && ISNT_NIGHT) ||
           ((sTempIn.Value < ((sTempNight.Value - sHysteresisBoilerSensor.Value)*10)) && IS_NIGHT)) &&
           IS_THERMO_OPEN) 
          ||
           (sTempIn.Value < ((sTemperatureSetting.Min - sHysteresisBoilerSensor.Value) * 10))) {}
      else return 0;
    }
    else return 0;
  }
  else
  // ���� ��������� ������� �������
  if (IS_BOILER_PRIORITY)
  {
    // ���� ��������� ��������
    if (IS_THERMO_OFF)
    {
      // ���� ����������� ����� ����� ���� �������� ����� ���������� ��� �����������
      // ����� ���� ������������ � ����������� ��� ����� ���� �������� ��� ����� 
      // ���������� � ������ ����/����, �� ��������� � ����� ���������
      if ((((sTempIn.Value < ((sTemperatureSetting.Value - sHysteresisBoilerSensor.Value)*10)) && ISNT_NIGHT) ||
           ((sTempIn.Value < ((sTempNight.Value - sHysteresisBoilerSensor.Value)*10)) && IS_NIGHT))
           ||
          ((sTempIn.Value < ((sTemperatureSetting.Max - sHysteresisBoilerSensor.Value)*10)) && 
           (sTempHWS.Value < ((sTemperatureHWS.Value - sHysteresisHWSSensor.Value)*10)))) {}
      else return 0;
    }
    else
    // ���� ��������� �������
    if (IS_THERMO_ON)
    {
      // ���� ����������� ����� ����� ���� ������������ � ����������� ��� ����� 
      // ���� �������� ��� ����� ���������� ��� ����������� ����� ����� ����
      // �������� ����� ���������� ��� ����������� ���������� ��� ����������� 
      // ����� ����� ���� ����������� � ������ ����/����, �� ��������� � �����
      // ���������
      if (((sTempIn.Value < ((sTemperatureSetting.Max - sHysteresisBoilerSensor.Value) * 10)) &&
          (sTempHWS.Value < ((sTemperatureHWS.Value - sHysteresisHWSSensor.Value)*10))) 
          ||
          ((((sTempIn.Value < ((sTemperatureSetting.Value - sHysteresisBoilerSensor.Value)*10)) && ISNT_NIGHT) ||
            ((sTempIn.Value < ((sTempNight.Value - sHysteresisBoilerSensor.Value)*10)) && IS_NIGHT)) &&
            IS_THERMO_OPEN)
          ||
          (sTempIn.Value < ((sTemperatureSetting.Min - sHysteresisBoilerSensor.Value) * 10))) {}
      else return 0; 
    }
    else return 0; 
  }
  else return 0;
  
  // ���� 2 ������� ��� ���������� ������� ������������
  if (ModeChange_Counter == 2) ModeChange_Counter = 0;
  if ((ModeChange_Counter == 1) && (sTempIn.Value > -200))
  { 
    // ��������� � ���������
    MODE_FLAG = REGULATION_MODE;
    ADC_FLAG_SET;
    FeedTime_Counter = 0;
    FeedTime_Num = 0;
    BlowingTime_Counter = 0;
  }
  return 1;
}

/*******************************************************************************
  *
  * �������� �� ������� ����������� �� ����������� ���������� �����
  *
*******************************************************************************/

void Work_TempOffAchieve (void)
{
  if ((sTempIn.Value < sTemperatureOff.Value*10) && (sTempIn.Value > -200))
  {
    // ���� 2 ������� ��� ���������� ������� ������������
    if (ModeChange_Counter == 2) ModeChange_Counter = 0;
    if (ModeChange_Counter == 1)
    {
      // ��������� � �������
      MODE_FLAG = EXTINGUISHING_MODE;
      ADC_FLAG_SET;
      FeedTime_Counter = 0;
      FeedTime_Num = 0;
      BlowingTime_Counter = 0;    
      Buzzer_Counter = 2; // 0.5 ���
      
      // ���������� ����� �������
      Extinguishing_Counter = sFuelLackTime.Value*60;
    }
  }
}

/*******************************************************************************
  *
  * �������� �� ����� �� �������
  *
*******************************************************************************/

void Work_TempOnAchieve (void)
{
  if (sTempIn.Value >= (sTemperatureOff.Value + sHysteresisBoilerSensor.Value)*10)
  {
    // ���� 2 ������� ��� ���������� ������� ������������
    if (ModeChange_Counter == 2) ModeChange_Counter = 0;
    if (ModeChange_Counter == 1)
    {
      // ��������� � ���������
      MODE_FLAG = REGULATION_MODE;
      ADC_FLAG_SET;
      FeedTime_Counter = 0;
      FeedTime_Num = 0;
      BlowingTime_Counter = 0;    
      Buzzer_Counter = 0;
    }
  }
}

/*******************************************************************************
  *
  * �������� �� ������������ �������� �����
  *
*******************************************************************************/

void Work_StopEXT(void)
{
  // ���� �������� ������� ���� � ����� �� ��������, �� ���������
  if (Stop_EXT_State && ISNT_SWITCHOFF_MODE)
  {
    MODE_FLAG = SWITCHOFF_MODE;
    MODE_Buf[0] = MODE_FLAG;
    EEPROM_WriteByte ((uint8_t*) MODE_Buf,MODE_FLAG_ADDR);
  }
}