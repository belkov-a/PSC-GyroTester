
#include "main.h"
#include "ILI9341.h"
#include "Init.h"
#include "Menu.h"
#include "adc.h"

/*******************************************************************************
  *
  * ��������� ������� ������� �������, �����, �� ��������� ����, �� ��������� 
  * �������
  *
*******************************************************************************/

uint16_t TFT_WriteChar(uint16_t x, uint16_t width, // ���������� ������� (������� ����� ����)
                       uint32_t color,         // ���� �������
                       uint32_t background,    // ���� ����
                       char* str,              // ��������� �� ������
                       sFont font)             // ������ �� ��������� ������
{
  // ����, ���� DMA �������� ��������
  while(IS_SPI_DMA_BUSY) {}
  
  // �������� �������� � ����������� ������ ������� � �������
  // � 0x20      - ������� � ���������� �������
  // � 0xC0-0x60 - ������� �������
  uint8_t snum = (*str < 0xC0 ? *str-0x20 : *str-0x60);
  
  // ��������� �� ������
  const uint8_t* symbol = &font.FontPointer[(1+font.Width*font.Bytes)*snum];
  
  // ������ ������� �������������� ������ ������ � ������� ������� + ������
  // ����� ���������
  for (uint8_t i = 0; i < symbol[0]; i++)
  {
    for(uint8_t j = 0; j < font.Height; j++)
    {
      // ������������ ������ ������ � ���, ������ ����������� �����
      if (symbol[1+j/8+i*font.Bytes] & (0x01 << (j%8)))
      {
        TFT_BUF[2*((x+i) + width*(j))] = color >> 8;
        TFT_BUF[2*((x+i) + width*(j))+1] = color & 0xFF;
      }
      else
        if (background != NO_BACKGROUND)
        {
          TFT_BUF[2*((x+i) + width*(j))] = background >> 8;
          TFT_BUF[2*((x+i) + width*(j))+1] = background & 0xFF;
        }
    }
  }
  
  if (background != NO_BACKGROUND)
  {
    for (uint8_t i = symbol[0]; i < symbol[0]+font.Space; i++)
    {
      for(uint8_t j = 0; j < font.Height; j++)
      {
        TFT_BUF[2*((x+i) + width*(j))] = background >> 8;
        TFT_BUF[2*((x+i) + width*(j))+1] = background & 0xFF;
      }
    }
  }

  // ���������� ������ �������
  return symbol[0]+font.Space;
}

/*******************************************************************************
  *
  * ��������� ������ ��������� �� ��������� ������� � ������
  *
*******************************************************************************/

uint16_t TFT_PutString(uint16_t x, uint16_t y, // ���������� ������ (������� ����� ����)
                       uint8_t align,          // ������������ ������ � ������
                       uint32_t color,         // ���� ��������
                       uint32_t background,    // ���� ����                                             
                       char *string,           // ��������� �� ������
                       sFont font)             // ������ �� ��������� ������
{ 
  // ���������� ������ ������
  uint16_t string_width = TFT_CalcStringWidth(string,font) + font.Space;
  
  // ���� ������� ������������ ������, �� ��������� ����� ������� TFT_STR_WIDTH
  // ����� ����� ������� string_width
  uint16_t buf_width;
  
  if (align == S_ALIGN)        buf_width = TFT_SIGN_WIDTH;
  else if (align == B_ALIGN)   buf_width = TFT_BLOCK_WIDTH;
  else if (align == NO_ALIGN)  buf_width = string_width;
  else if (align == RIS_ALIGN) buf_width = string_width + x;
  else if (align == MAN_ALIGN) buf_width = y;
  else                         buf_width = TFT_STR_WIDTH;
  
  // ����, ���� DMA �������� ��������
  while(IS_SPI_DMA_BUSY) {}
  
  // ��������� ������ ����� ��������� ����� �������
  if (background != NO_BACKGROUND)
  {
    for (uint8_t i = 0; i < font.Space; i++)
    {
      for(uint8_t j = 0; j < font.Height; j++)
      {
        TFT_BUF[2*((i+x) + buf_width*(j))] = background >> 8;
        TFT_BUF[2*((i+x) + buf_width*(j))+1] = background & 0xFF;
      }
    }
  }
  
  x += font.Space;
  // ���� ������ �� ���������
  while(*string)
  {      
    // �������� ���������� ��� ��������� ���������� ������� � ������� �������
    x += TFT_WriteChar(x,buf_width,color,background,string,font);  
    // ����������� �������� ��������� �� ��������� ������
    *string++;           
  }
  
  return string_width;
}

/*******************************************************************************
  *
  * ����� ������, ���������� ������� ���������� �
  *
*******************************************************************************/

uint16_t TFT_WriteString(uint16_t x, uint16_t y,
                         uint8_t align,
                         uint32_t color,         // ���� ��������
                         uint32_t background,    // ���� ����                                             
                         char *string,           // ��������� �� ������
                         sFont font)             // ������ �� ��������� ������
{
  // ����, ���� DMA �������� ��������
  while(IS_SPI_DMA_BUSY) {}
        
  // ��������� ������ � ������ ������
  uint16_t string_width = TFT_PutString(0,0,NO_ALIGN,color,background,string,font);
    
  // ���� ������� ������������ ������, �� ������ ���������� ������
  // ������, ����� ��������� ���������
  if (align != NO_ALIGN)
  {
    // ������������ ���������� � �� ������ �������
    switch (align)
    {
      case L_ALIGN:   x = 0; break;
      case R_ALIGN:   x = TFT_WIDTH - string_width; break;
      case C_ALIGN:   x = (TFT_WIDTH - string_width)/2; break;
      case RI_ALIGN:  x = (TFT_WIDTH - R_INDENT) - string_width; break;
      case CI_ALIGN:  x = (TFT_WIDTH - string_width)/2 - R_INDENT; break;
      case RIS_ALIGN: x = (TFT_WIDTH - R_INDENT) - string_width + font.Space; break;
    }
  }
  
  // ������������� ���� ������
  TFT_ILI9341_SetAddrWindow(x,y,x+string_width-1,y+font.Height);
  
  // �������� ������
  TFT_SendBuffer(TFT_BUF,(string_width)*2*font.Height);
  
  return string_width;
}

/*******************************************************************************
  *
  * ���������� ������ ������ � ��������
  *
  * ������� ���������� ������ ������ � ��������
  *
*******************************************************************************/

uint16_t TFT_CalcStringWidth(char *string, // ������
                             sFont font)   // ��������� �� ��������� ������
{
  uint16_t string_width = 0;
    
  // ���� ������ �� ���������
  while (*string)
  {
    // �������� �������� � ����������� ������ ������� � �������
    // � 0x20      - ������� � ���������� �������
    // � 0xC0-0x60 - ������� �������
    uint8_t snum = (*string < 0xC0 ? *string-0x20 : *string-0x60);  
    
    // ��������� �� ������
    const uint8_t* symbol = &font.FontPointer[(1+font.Width*font.Bytes)*snum];  
    
    // ����������� ������ �������
    string_width += symbol[0] + font.Space;
    
    // ����������� �������� ��������� �� ��������� ������
    *string++;
  }
  
  return string_width;
}

/*******************************************************************************
  *
  * ��������� ������� �� ����� c Dec ����������
  *
  * ������� ���������� ������
  *
*******************************************************************************/

char TFT_GetDecimalChar(uint8_t x) // ����� �� 0 �� 9
{
  switch (x)
  {
    case 0x0: return '0'; break;
    case 0x1: return '1'; break;
    case 0x2: return '2'; break;
    case 0x3: return '3'; break;
    case 0x4: return '4'; break;
    case 0x5: return '5'; break;
    case 0x6: return '6'; break;
    case 0x7: return '7'; break;  
    case 0x8: return '8'; break;
    case 0x9: return '9'; break;
    default:  return 'x'; break;
  }
}
/*******************************************************************************
  *
  * ��������� ������ ������� ��������� �� ����� Unit
  *
*******************************************************************************/

char* TFT_GetUnitChar (uint8_t UnitType)
{
  char* UnitChar;
  switch (UnitType)
  {
    case UNIT_NULL: UnitChar = (char*) UnitNullName[Language]; break;
    case UNIT_SEC:  UnitChar = (char*) UnitSecName[Language]; break;
    case UNIT_MIN:  UnitChar = (char*) UnitMinName[Language]; break;
    case UNIT_HOUR: UnitChar = (char*) UnitHourName[Language]; break;
    case UNIT_PERC: UnitChar = (char*) UnitPercName[Language]; break;
    case UNIT_DEG:  UnitChar = (char*) UnitDegName[Language]; break;
    case UNIT_KHZ:  UnitChar = (char*) UnitkHzName[Language]; break;
    case UNIT_HZ:   UnitChar = (char*) UnitHzName[Language]; break;
    case UNIT_VOLT: UnitChar = (char*) UnitVoltName[Language]; break;
  }
  return UnitChar;
}

/*******************************************************************************
  *
  * ������� ��������� � ���������� ������� �� ������, �������, ��������� � �����
  *
  * ���������� ���������� ��������, ���������� ����������
  *
*******************************************************************************/

uint16_t TFT_PutFloatValue(uint16_t x, uint16_t y, // ���������� (������� ����� ����)
                           uint8_t align,        // ������������
                           int Value,            // �������� ����� 
                           uint8_t Digit,        // ���������� ����� ������
                           uint8_t Mantissa,     // ���������� ������� ������
                           uint8_t Sign,         // ����
                           char* Unit,           // ������� ��������� (�� ���������, ���� ������)
                           uint16_t color,       // ���� ��������
                           uint32_t background,  // ���� ����
                           sFont font)           // ������ �� ��������� ������
{
  uint8_t DigWidth = 0;
  uint8_t Dig = 0;
  char Num_String[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  
  // ���� ���� ����
  if ((Sign == SIGNED) && (Value < 0))
  {
    Num_String[DigWidth] = '-';
    DigWidth += 1;
    
    Value *= -1;
  }
  
  // ��������� ������� � ��������, ���� �����
  if (Mantissa > 0) 
  {
    Num_String[DigWidth + Digit] = '.';
    
    // ������������ ���� ��������
    for (uint8_t i = DigWidth + Mantissa + Digit; i > DigWidth + Digit; i--)
    {
      Dig = Value % 10;
      Value /= 10;
      Num_String[i] = TFT_GetDecimalChar(Dig);
    }
  }
  else
  // ���� �������� ���, �� ������������ ����� ������ �� ��������
  {
    if (Value < 10) Digit = 1;
    else if ((Value >= 10) && (Value < 100)) Digit = 2;
    else if ((Value >= 100) && (Value < 1000)) Digit = 3;
    else if (Value >= 1000) Digit = 4;
  }
  
  // ��������� ����� �����
  for (int i = DigWidth + Digit - 1; i >= DigWidth; i--)
  {
    Dig = Value % 10;
    Value /= 10;
    Num_String[i] = TFT_GetDecimalChar(Dig);
  }
  
  // ������������ ������� ���������
  if (Unit[0] > 0) 
  {
    Num_String[DigWidth + Mantissa + Digit] = ' '; // ������
    
    uint8_t i = 1;
    
    // ����� ������� ���������
    while(*Unit) 
    {
      Num_String[DigWidth + Mantissa + Digit + i] = *Unit;
      
      *Unit++;
      i++;
    }
  }
  
  if (align == S_ALIGN) x = (TFT_SIGN_WIDTH - TFT_CalcStringWidth(Num_String,font))/2;
  
  // ������� ������ �� �������
  return TFT_PutString(x,y,align,color,background,Num_String,font);
}

/*******************************************************************************
  *
  * ��������� ��������� � ���������� ������� �� ������, �������, ��������� 
  *
  * ���������� ���������� ��������, ���������� ����������
  *
*******************************************************************************/

uint16_t TFT_WriteFloatValue(uint16_t x, uint16_t y, // ���������� (������� ����� ����)
                             uint8_t align,          // ������������
                             int Value,            // �������� ����� 
                             uint8_t Digit,        // ���������� ����� ������
                             uint8_t Mantissa,     // ���������� ������� ������
                             uint8_t Sign,         // ����
                             char* Unit,           // ������� ��������� (�� ���������, ���� ������)
                             uint16_t color,       // ���� ��������
                             uint32_t background,  // ���� ����
                             sFont font)           // ������ �� ��������� ������
{
  // ��������� �������� � ������ ������
  // ��������� ������ � ������ ������
  uint16_t string_width = TFT_PutFloatValue(0,0,NO_ALIGN,Value,Digit,Mantissa,Sign,Unit,color,background,font);
  
  // ���� ������� ������������ ������, �� ������ ���������� ������
  // ������, ����� ��������� ���������
  if (align != NO_ALIGN)
  {
    // ������������ ���������� � �� ������ �������
    switch (align)
    {
      case L_ALIGN:  x = 0; break;
      case R_ALIGN:  x = TFT_WIDTH - string_width; break;
      case C_ALIGN:  x = (TFT_WIDTH - string_width)/2; break;
      case RI_ALIGN: x = (TFT_WIDTH - R_INDENT) - string_width; break;
      case CI_ALIGN: x = (TFT_WIDTH - string_width)/2 - R_INDENT; break;
    }
  }

  // ������������� ���� ������
  TFT_ILI9341_SetAddrWindow(x,y,x+string_width-1,y+font.Height);
  
  // �������� ������
  TFT_SendBuffer(TFT_BUF,(string_width)*2*font.Height);  
    
  return string_width;
}

/*******************************************************************************
  *
  * ��������� ����� ���������
  *
*******************************************************************************/

uint16_t TFT_WriteParamName (uint16_t x, uint16_t y,   // ���������� ���������
                             uint8_t align,            // ������������ 
                             sFloatSigned *Element,    // ������ �� ��������� ��������
                             uint16_t color,           // ���� �������� ������
                             uint32_t background,      // ���� ����
                             sFont font)
{
  char*   Name = (char*) (*Element).Name[Language];
  uint16_t BlockWidth;
  uint16_t x0;
  
  // ������������ � ������ ����
  switch(align)
  {
    // ����������� �� ������ ����� TFT_SIGN_WIDTH
    case S_ALIGN: 
      BlockWidth = TFT_SIGN_WIDTH; 
      x0 = (BlockWidth - TFT_CalcStringWidth(Name,font))/2;
      break;
    case B_ALIGN: 
      BlockWidth = TFT_BLOCK_WIDTH; 
      break;
  }  
  
  // ����, ���� DMA �������� ��������
  while(IS_SPI_DMA_BUSY) {}
  
  // �������� � ����� ������������
  uint16_t string_width = TFT_PutString(x0,0,align,color,background,Name,font);
  
  // ������������� ���� ������
  TFT_ILI9341_SetAddrWindow(x,y,x+BlockWidth-1,y+font.Height);
  
  // �������� ������
  TFT_SendBuffer(TFT_BUF,(BlockWidth)*2*font.Height);  
  
  return string_width;
}

/*******************************************************************************
  *
  * ��������� �������� ���������
  *
*******************************************************************************/

uint16_t TFT_WriteParamSign (uint16_t x, uint16_t y,   // ���������� ���������
                             uint8_t align,            // ������������ 
                             sFloatSigned *Element,    // ������ �� ��������� ��������
                             uint16_t color,           // ���� �������� ������
                             uint32_t background,      // ���� ����
                             sFont font)
{
  int value = (*Element).Value;
  // ���� ����������� ���, ������ ��� �����, �� ����� �� 10 ��� ������
  if (((Element == (sFloatSigned*) &sTempHWS) || 
       (Element == (sFloatSigned*) &sTempIn) ||
       (Element == (sFloatSigned*) &sTempScrew)) &&
       (value != TEMP_SENSOR_BREAK) &&
       (value != TEMP_SENSOR_CLOSURE)) value /= 10;
  char*   Unit = TFT_GetUnitChar((*Element).Unit);
  uint16_t BlockWidth;  
  uint16_t x0;
  uint16_t string_width = 0;
  
  // ������������ � ������ ����
  switch(align)
  {
    // ����������� �� ������ ����� TFT_SIGN_WIDTH
    case S_ALIGN: 
      BlockWidth = TFT_SIGN_WIDTH; break;
    case B_ALIGN: 
      BlockWidth = TFT_BLOCK_WIDTH; break;
  }  
  
  // ����, ���� DMA �������� ��������
  while(IS_SPI_DMA_BUSY) {}

  // ���� ������, ������� ������ 
  if (value == TEMP_SENSOR_BREAK)
  {
    if (align == S_ALIGN) x0 = (BlockWidth - TFT_CalcStringWidth(" ",CircuitBreak))/2;
    string_width += TFT_PutString(x0,0,align,HEAD_COLOR,background," ",CircuitBreak); 
  }
  else
  // ���� ���������, ������� ������
  if (value == TEMP_SENSOR_CLOSURE)
  {
    if (align == S_ALIGN) x0 = (BlockWidth - TFT_CalcStringWidth(" ",CircuitClosure))/2;
    string_width += TFT_PutString(x0,0,align,HEAD_COLOR,background," ",CircuitClosure); 
  }
  // ����� ��������
  else
  {
    uint8_t Sign = 0;
    if (value < 0) Sign = SIGNED;
    else           Sign = NON_SIGNED;
    string_width += TFT_PutFloatValue(0,0,align,value,1,0,Sign,Unit,color,background,font);
  }  
  
    // ������������� ���� ������
  TFT_ILI9341_SetAddrWindow(x,y,x+BlockWidth-1,y+font.Height-1);
  
  // �������� ������
  TFT_SendBuffer(TFT_BUF,(BlockWidth)*2*(font.Height));  
  
  return string_width;
}

/*******************************************************************************
  *
  * ��������� ������ ��������� � �������, ���������, ������������ � ��������
  *
  * � ��������� �������� �������������� sFloat ��������� ������ �� �����������
  * �������� sFloatView � ������ �� ������ �������������� sFloatControl
  * ViewCursor > 0 - ������ ������������, ������ �� ������ �������� �������
  *
*******************************************************************************/

uint16_t TFT_FormatFloat (uint16_t x, uint16_t y, // ���������� ������ (������� ����� ����)
                          uint8_t align,          // ������������ �� ������
                          sFloat *Element,        // ������ �� ��������� �������� ��������������
                          uint8_t ViewCursor,     // ��������� ����������� �������
                          uint16_t color,         // ���� �������� ������
                          uint32_t background,    // ���� ����
                          sFont font)             // ������ �� ��������� ������
{
  uint8_t Dig = 0;
  int BlockWidth = 0;
  uint8_t DigNum = 0;
  uint8_t Sign = 0;
  uint16_t string_width = 0;
  int16_t Value = (*Element).Value;
  int16_t ValueDig = (*Element).Value;
  uint8_t Digits = 0;
  uint8_t Mantissa = (*Element).Mantissa;
  uint16_t dot_width = 0;
  uint16_t buf_size = 0;
  char *Unit = TFT_GetUnitChar((*Element).Unit);
  char Num_String[10] = {0,0,0,0,0,0,0,0,0,0};
  
  int UnitWidth = TFT_CalcStringWidth(Unit,font);
  
  if (Value < 0) 
  {
    Value = -Value;
    ValueDig = -ValueDig;
    DigNum += 1;
    Sign = 1;
  }
  
  // ���������� ����� ��������
  if ((*Element).Mantissa > 0)
  {
    for (uint8_t i = 0; i < Mantissa; i++) Value /= 10;
    
    if (Value < 10) Digits = 1;
    else if ((Value >= 10) && (Value < 100)) Digits = 2;
    else if ((Value >= 100) && (Value < 1000)) Digits = 3;
    else if (Value >= 1000) Digits = 4;
    
    Num_String[DigNum + Digits] = '.';
    
    // ������������ ���� ��������
    for (uint8_t i = DigNum + Mantissa + Digits; i > DigNum + Digits; i--)
    {
      Dig = ValueDig % 10;
      ValueDig /= 10;
      Num_String[i] = TFT_GetDecimalChar(Dig);
    }
  }
  else
  // ���� �������� ���, �� ������������ ����� ������ �� ��������
  {
    if (Value < 10) Digits = 1;
    else if ((Value >= 10) && (Value < 100)) Digits = 2;
    else if ((Value >= 100) && (Value < 1000)) Digits = 3;
    else if (Value >= 1000) Digits = 4;
  }
  
  if (Sign == 1)
  {
    Num_String[0] = '-';
  }
  
  // ��������� ����� �����
  for (int i = DigNum + Digits - 1; i >= DigNum; i--)
  {
    Dig = Value % 10;
    Value /= 10;
    Num_String[i] = TFT_GetDecimalChar(Dig);
  }  
  
  // ���������� ������ �������� ��� ��������� �����
  int ValueWidth = TFT_CalcStringWidth(Num_String,font);
  BlockWidth += ValueWidth;
    
  // ����������� ������� ���������
  if (Unit[0] > 0)
  {
    BlockWidth += TFT_CalcStringWidth(" ",font);
    BlockWidth += UnitWidth - font.Space - 1;
  }
  
  BlockWidth += font.Space;
  
  dot_width = TFT_WIDTH - R_INDENT - BlockWidth - x;
  buf_size = dot_width + BlockWidth;
  // ��������� ����� ������ ���� � �����
  TFT_HDotLine(0,buf_size,dot_width,2,color,background);
    
  uint32_t num_color = 0;
  uint32_t num_back_color = 0; 
  
  // ���� ����� �� ��������, �� �������� ���������� ������� ������
  if (Digit_Alarm > 0) 
  {
    num_color = ALARM_COLOR;
    num_back_color = background;
  }
  else
  {
    // ���������� ������ � ������� �����
    if ((ViewCursor > 0) & (IS_BLINK))
    {
      // ���� ����������� ������� �������� � ����������� ���� ������� BLINK,
      // �� ������ ������� ���� ������� � ���� ����
      num_color = background;
      num_back_color = color;
    }
    else
    // ����� ����� ��� �������
    {
      num_color = color;
      num_back_color = background;
    } 
  }
  
  string_width += TFT_PutString(dot_width,dot_width+BlockWidth,MAN_ALIGN,num_color,num_back_color,Num_String,font);
  
  uint16_t unit_width = 0;
  
  // ���������� ������ �������� ��� ��������� ������� ��������� 
  if (Unit[0] > 0)
  {
    char Unit_String[5] = {0,0,0,0,0};
    
    uint8_t i = 0;
    
    // ����� ������� ���������
    while(Unit[i]) 
    {
      Unit_String[i] = Unit[i];
      i++;
    } 
    
    unit_width = TFT_PutString(dot_width+string_width,0,RIS_ALIGN,color,background,Unit_String,font); 
  }
  
    // ������������� ���� ������
  TFT_ILI9341_SetAddrWindow(x,y,TFT_WIDTH-R_INDENT-1,y+font.Height);
  
  // �������� ������
  TFT_SendBuffer(TFT_BUF,buf_size*2*font.Height); 
  
  return string_width + unit_width + dot_width;
}

/*******************************************************************************
  *
  * ��������� ���� �������������� ��������� ��������� 
  *
  * ������ � ����������� �������� *Element
  * � ��������� �������� �������������� ��������� ������ �� ����������� ��������
  * � �� ������ ��������������
  *
*******************************************************************************/

int8_t Digit_Alarm = 0;

uint16_t TFT_WriteMenuDigitalControl (uint16_t x, uint16_t y, // ���������� ��������
                                      uint8_t align,          // ������������ �� ������
                                      sMenu *Menu,            // ������ �� ��������� ���� ����������
                                      uint8_t MenuItem,       // ����� �������� ����, �����. ������ ��������
                                      sFloat *Element,        // ������ �� ��������� �������� ��������������
                                      uint16_t Color,         // ���� �������� �������� ����
                                      uint32_t Background,    // ���� ����
                                      uint16_t AlarmColor,    // ���� �������������� (����� ��������� �� �������), ���� 0 �� ���-��
                                      sFont font)             // ������ �� ��������� ������
{
  uint16_t Off_width = TFT_CalcStringWidth((char*) sOnOffValue[Language][0],font) + font.Space;
  uint16_t dot_width = 0; 
  uint16_t buf_size = 0;

  if ((Element->Off & OFF_STATE) == OFF_STATE)
  {
    dot_width = TFT_WIDTH - R_INDENT - Off_width - x;
    buf_size = dot_width + Off_width;
    
    // ������������� ���� ������
    TFT_ILI9341_SetAddrWindow(x,y,TFT_WIDTH-R_INDENT-1,y+font.Height);
  }
  
  // ���� ������ ������ �������, ������ ������ ����� � ����������� ���� "����"
  if (((*Menu).Item & ~mActive) != MenuItem) 
  {
    if (((((*Menu).Item & mActive) != mActive) && IS_NEED_UPDATE) || IS_NEED_UPDATE_LANG)
    {
      // ���� ������� �� ��������, ������� ��������
      if ((Element->Off & OFF_STATE) == 0)
      {
        if (Element->Value > Element->Max) Element->Value = Element->Max;
        if (Element->Value < Element->Min) Element->Value = Element->Min;

        TFT_FormatFloat (x,y,align,Element,0,Color,Background,font);
      }
      // ���� ������� ��������, �� ������� "����"
      else
      {
        // ��������� ����� ������ ���� � �����
        TFT_HDotLine(0,buf_size,dot_width,2,Color,NO_BACKGROUND);
        // ��������� �������� � �����
        TFT_PutString(dot_width,y,RIS_ALIGN,Color,NO_BACKGROUND,(char*) sOnOffValue[Language][0],font);
        // �������� ������
        TFT_SendBuffer(TFT_BUF,buf_size*2*font.Height); 
      }
    }
  }
  else
  //������� ������, �� �� ������� (������ ������)
  if ((*Menu).Item == MenuItem) 
  {
    // ���� ������� �� ��������, ������� ��������
    if ((Element->Off & OFF_STATE) == 0) 
    {
      TFT_FormatFloat (x,y,align,Element,0,Background,Color,font);
    }
    // ���� ������� ��������, �� ������� "����"
    else 
    {
      // ��������� ����� ������ ���� � �����
      TFT_HDotLine(0,buf_size,dot_width,2,Background,Color);
      // ��������� �������� � �����
      TFT_PutString(dot_width,y,RIS_ALIGN,Background,Color,(char*) sOnOffValue[Language][0],font);
      // �������� ������
      TFT_SendBuffer(TFT_BUF,buf_size*2*font.Height); 
    }
  }
  else
  if ((*Menu).Item == MenuItem + mActive) //������� �������. �������������� �� �������� �������
  {
    // ���� �������� ����� �� �������, ������������ ��� ������ AlarmColor
    if ((AlarmColor != 0) & (Digit_Alarm > 0))
    {
      Digit_Alarm--;
      // ���� ������� �� ��������, ������� ��������
      if ((Element->Off & OFF_STATE) == 0) 
      {
        TFT_FormatFloat (x,y,align,Element,0,Color,Background,font);
      }
      // ���� ������� ��������, �� ������� "����"
      else 
      {
        // ��������� ����� ������ ���� � �����
        TFT_HDotLine(0,buf_size,dot_width,2,Color,NO_BACKGROUND);
        // ��������� �������� � �����
        TFT_PutString(dot_width,y,RIS_ALIGN,AlarmColor,Background,(char*) sOnOffValue[Language][0],font);
        // �������� ������
        TFT_SendBuffer(TFT_BUF,buf_size*2*font.Height); 
      }
    }
    else 
    {
      // ���� ������� �� ��������, ������� ��������
      if ((Element->Off & OFF_STATE) == 0) 
      {
        TFT_FormatFloat (x,y,align,Element,1,Color,Background,font);
      }
      // ���� ������� ��������, �� ������� "����"
      else 
      {
        // ��������� ����� ������ ���� � �����
        TFT_HDotLine(0,buf_size,dot_width,2,Color,NO_BACKGROUND);
        
        if (IS_BLINK) 
          TFT_PutString(dot_width,y,RIS_ALIGN,Background,Color,(char*) sOnOffValue[Language][0],font);
        else          
          TFT_PutString(dot_width,y,RIS_ALIGN,Color,NO_BACKGROUND,(char*) sOnOffValue[Language][0],font);
      
        // �������� ������
        TFT_SendBuffer(TFT_BUF,buf_size*2*font.Height); 
      }
      
      // ������������� ��������� ����������
      Digit_Alarm = Menu_CorrectFloatUpDown(Element,Menu);
    }
  }
  
  return 0;
}

/*******************************************************************************
  *
  * ��������� ���� �������������� ��������� ���� ������������
  *
  * ������ � ����������� �������� *Element
  *
*******************************************************************************/

int Enum_Alarm = 0;

uint16_t TFT_WriteMenuEnumControl (uint8_t x, uint8_t y, // ���������� ��������
                                   uint8_t align,        // ������������ �� ������
                                   sMenu *Menu,          // ������ �� ��������� ���� ����������
                                   uint8_t MenuItem,     // ����� �������� ����, �����. ������ ��������
                                   sEnum *Element,       // ������ �� ��������� �������� ��������������
                                   uint16_t Color,       // ���� �������� 
                                   uint32_t Background,  // ���� ���� 
                                   uint16_t AlarmColor,  // ���� �������������� (����� ��������� �� �������), ���� 0 �� ���-��
                                   sFont font)           // ������ �� ��������� ������
{
  char *str = (char*) Element->Value[Language][Element->Point];
  uint16_t str_width = TFT_CalcStringWidth(str,font) + font.Space;
  uint16_t dot_width = TFT_WIDTH - R_INDENT - str_width - x;
  uint16_t buf_size = str_width + dot_width;
  
  // ������������� ���� ������
  TFT_ILI9341_SetAddrWindow(x,y,TFT_WIDTH-R_INDENT-1,y+font.Height);
  
  // ������ ������ �������, ������ ������ �������
  if (((*Menu).Item & ~mActive) != MenuItem) // ������ ������ �������, ������ ������ ��� �������
  {
    if (((((*Menu).Item & mActive) != mActive) && IS_NEED_UPDATE) || IS_NEED_UPDATE_LANG)
    {
      // ��������� ����� ������ ���� � �����
      TFT_HDotLine(0,buf_size,dot_width,2,Color,NO_BACKGROUND);
      // ��������� �������� � �����
      TFT_PutString(dot_width,y,RIS_ALIGN,Color,NO_BACKGROUND,str,font);   
      // �������� ������
      TFT_SendBuffer(TFT_BUF,buf_size*2*font.Height); 
    }
  }
  else
  // ������� ������, �� �� ������� (������ ������)
  if ((*Menu).Item == MenuItem)
  {
    // ��������� ����� ������ ���� � �����
    TFT_HDotLine(0,buf_size,dot_width,2,Background,Color);
    // ��������� �������� � �����
    TFT_PutString(dot_width,y,RIS_ALIGN,Background,Color,str,font);
    // �������� ������
    TFT_SendBuffer(TFT_BUF,buf_size*2*font.Height); 
  }
  else
  // ������� �������, ��������������
  if ((*Menu).Item == MenuItem + mActive)
  {
    TFT_HDotLine(0,buf_size,dot_width,2,Color,Background);
    // ���� ������� ����� ������ ��������� ���/����, �� �� ��������� � ��������������,
    // � ������ ������ ��������, ����� ���������
    if (NON_ONOFF)
    {
      if (AlarmColor != 0 & Enum_Alarm > 0) // ������ �������� � ��������������� ������
      {
        Enum_Alarm--;
        TFT_PutString(dot_width,y,RIS_ALIGN,AlarmColor,Background,str,font);
      } 
      else // ������ �������� � ��������
      if (IS_BLINK) TFT_PutString(dot_width,y,RIS_ALIGN,Background,Color,str,font);
      else          TFT_PutString(dot_width,y,RIS_ALIGN,Color,Background,str,font);
    }
    else TFT_PutString(dot_width,y,RIS_ALIGN,Background,Color,str,font);
  
    // �������������� � ������������ � �������� �������
    Enum_Alarm += Menu_CorrectEnumUpDown(Element,Menu);
    
    // �������� ������
    TFT_SendBuffer(TFT_BUF,buf_size*2*font.Height); 
  }
  
  return 0;
}