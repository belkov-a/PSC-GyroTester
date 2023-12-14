
#include "main.h"
#include "ILI9341.h"
#include "Init.h"
#include "Menu.h"
#include "adc.h"

/*******************************************************************************
  *
  * Отрисовка символа нужного размера, цвета, на выбранном фоне, из указанной 
  * таблицы
  *
*******************************************************************************/

uint16_t TFT_WriteChar(uint16_t x, uint16_t width, // координаты символа (верхний левый угол)
                       uint32_t color,         // цвет символа
                       uint32_t background,    // цвет фона
                       char* str,              // указатель на символ
                       sFont font)             // ссылка на структуру шрифта
{
  // Ждем, пока DMA закончит передачу
  while(IS_SPI_DMA_BUSY) {}
  
  // Проверка алфавита и определение номера символа в массиве
  // с 0x20      - символы и английский алфавит
  // с 0xC0-0x60 - русский алфавит
  uint8_t snum = (*str < 0xC0 ? *str-0x20 : *str-0x60);
  
  // Указатель на символ
  const uint8_t* symbol = &font.FontPointer[(1+font.Width*font.Bytes)*snum];
  
  // Ширина символа ограничивается первым числом в таблице шрифтов + пробел
  // между символами
  for (uint8_t i = 0; i < symbol[0]; i++)
  {
    for(uint8_t j = 0; j < font.Height; j++)
    {
      // Закрашивание цветом символ и фон, пробел закрашиваем фоном
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

  // Возвращаем ширину символа
  return symbol[0]+font.Space;
}

/*******************************************************************************
  *
  * Отрисовка строки символами из указанной таблицы в буфере
  *
*******************************************************************************/

uint16_t TFT_PutString(uint16_t x, uint16_t y, // координаты строки (верхний левый угол)
                       uint8_t align,          // выравнивание строки в буфере
                       uint32_t color,         // цвет символов
                       uint32_t background,    // цвет фона                                             
                       char *string,           // указатель на строку
                       sFont font)             // ссылка на структуру шрифта
{ 
  // вычисление ширины строки
  uint16_t string_width = TFT_CalcStringWidth(string,font) + font.Space;
  
  // Если указано выравнивание строки, то заполняем буфер шириной TFT_STR_WIDTH
  // иначе буфер шириной string_width
  uint16_t buf_width;
  
  if (align == S_ALIGN)        buf_width = TFT_SIGN_WIDTH;
  else if (align == B_ALIGN)   buf_width = TFT_BLOCK_WIDTH;
  else if (align == NO_ALIGN)  buf_width = string_width;
  else if (align == RIS_ALIGN) buf_width = string_width + x;
  else if (align == MAN_ALIGN) buf_width = y;
  else                         buf_width = TFT_STR_WIDTH;
  
  // Ждем, пока DMA закончит передачу
  while(IS_SPI_DMA_BUSY) {}
  
  // Добавляем отступ между символами перед строкой
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
  // Если строка не кончилась
  while(*string)
  {      
    // Изменяем координату для отрисовки следующего символа и выводим текущий
    x += TFT_WriteChar(x,buf_width,color,background,string,font);  
    // Увеличиваем значение указателя на следующий символ
    *string++;           
  }
  
  return string_width;
}

/*******************************************************************************
  *
  * Вывод строки, возвращает текущую координату Х
  *
*******************************************************************************/

uint16_t TFT_WriteString(uint16_t x, uint16_t y,
                         uint8_t align,
                         uint32_t color,         // цвет символов
                         uint32_t background,    // цвет фона                                             
                         char *string,           // указатель на строку
                         sFont font)             // ссылка на структуру шрифта
{
  // Ждем, пока DMA закончит передачу
  while(IS_SPI_DMA_BUSY) {}
        
  // Заполняем строку в начале буфера
  uint16_t string_width = TFT_PutString(0,0,NO_ALIGN,color,background,string,font);
    
  // Если указано выравнивание строки, то меняем координату начала
  // строки, иначе оставляем указанную
  if (align != NO_ALIGN)
  {
    // выравнивание координаты Х по ширине дисплея
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
  
  // Устанавливаем поле вывода
  TFT_ILI9341_SetAddrWindow(x,y,x+string_width-1,y+font.Height);
  
  // Передаем буффер
  TFT_SendBuffer(TFT_BUF,(string_width)*2*font.Height);
  
  return string_width;
}

/*******************************************************************************
  *
  * Вычисление ширины строки в пикселах
  *
  * Функция возвращает ширину строки в пикселах
  *
*******************************************************************************/

uint16_t TFT_CalcStringWidth(char *string, // строка
                             sFont font)   // указатель на структуру шрифта
{
  uint16_t string_width = 0;
    
  // Если строка не кончилась
  while (*string)
  {
    // Проверка алфавита и определение номера символа в массиве
    // с 0x20      - символы и английский алфавит
    // с 0xC0-0x60 - русский алфавит
    uint8_t snum = (*string < 0xC0 ? *string-0x20 : *string-0x60);  
    
    // Указатель на символ
    const uint8_t* symbol = &font.FontPointer[(1+font.Width*font.Bytes)*snum];  
    
    // Прибавление ширины символа
    string_width += symbol[0] + font.Space;
    
    // Увеличиваем значение указателя на следующий символ
    *string++;
  }
  
  return string_width;
}

/*******************************************************************************
  *
  * Получение символа из числа c Dec кодировкой
  *
  * Функция возвращает символ
  *
*******************************************************************************/

char TFT_GetDecimalChar(uint8_t x) // число от 0 до 9
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
  * Получение строки единицы измерения из числа Unit
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
  * Вставка параметра в десятичном формате со знаком, запятой, мантиссой в буфер
  *
  * Возвращает количество пикселов, занимаемые параметром
  *
*******************************************************************************/

uint16_t TFT_PutFloatValue(uint16_t x, uint16_t y, // координата (верхний левый угол)
                           uint8_t align,        // выравнивание
                           int Value,            // значение числа 
                           uint8_t Digit,        // количество целых знаков
                           uint8_t Mantissa,     // количество дробных знаков
                           uint8_t Sign,         // знак
                           char* Unit,           // единица измерения (не выводится, если пустая)
                           uint16_t color,       // цвет символов
                           uint32_t background,  // цвет фона
                           sFont font)           // ссылка на структуру шрифта
{
  uint8_t DigWidth = 0;
  uint8_t Dig = 0;
  char Num_String[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  
  // Если есть знак
  if (Sign == SIGNED)
  {
    if (Value < 0) 
    {
      Num_String[DigWidth] = '-';
      Value *= -1;
    }
    else           Num_String[DigWidth] = '+';
    DigWidth += 1;
  }
  
  // Добавляем запятую и мантиссу, если нужно
  if (Mantissa > 0) 
  {
    Num_String[DigWidth + Digit] = '.';
    
    // Формирование цифр мантиссы
    for (uint8_t i = DigWidth + Mantissa + Digit; i > DigWidth + Digit; i--)
    {
      Dig = Value % 10;
      Value /= 10;
      Num_String[i] = TFT_GetDecimalChar(Dig);
    }
  }
  else
  // Если мантиссы нет, то корректируем число знаков от значения
  {
    if (Value < 10) Digit = 1;
    else if ((Value >= 10) && (Value < 100)) Digit = 2;
    else if ((Value >= 100) && (Value < 1000)) Digit = 3;
    else if (Value >= 1000) Digit = 4;
  }
  
  // Добавляем целые знаки
  for (int i = DigWidth + Digit - 1; i >= DigWidth; i--)
  {
    Dig = Value % 10;
    Value /= 10;
    Num_String[i] = TFT_GetDecimalChar(Dig);
  }
  
  // Формирование единицы измерения
  if (Unit[0] > 0) 
  {
    Num_String[DigWidth + Mantissa + Digit+1] = ' '; // пробел
    
    uint8_t i = 1;
    
    // Вывод единицы измерения
    while(*Unit) 
    {
      Num_String[DigWidth + Mantissa + Digit + i+1] = *Unit;
      
      *Unit++;
      i++;
    }
  }
  
  if (align == S_ALIGN) x = (TFT_SIGN_WIDTH - TFT_CalcStringWidth(Num_String,font))/2;
  
  // Выводим строку на дисплей
  return TFT_PutString(x,y,align,color,background,Num_String,font);
}

/*******************************************************************************
  *
  * Отрисовка параметра в десятичном формате со знаком, запятой, мантиссой 
  *
  * Возвращает количество пикселов, занимаемые параметром
  *
*******************************************************************************/

uint16_t TFT_WriteFloatValue(uint16_t x, uint16_t y, // координата (верхний левый угол)
                             uint8_t align,          // выравнивание
                             int Value,            // значение числа 
                             uint8_t Digit,        // количество целых знаков
                             uint8_t Mantissa,     // количество дробных знаков
                             uint8_t Sign,         // знак
                             char* Unit,           // единица измерения (не выводится, если пустая)
                             uint16_t color,       // цвет символов
                             uint32_t background,  // цвет фона
                             sFont font)           // ссылка на структуру шрифта
{
  // Заполняем параметр в начале буфера
  // Заполняем строку в начале буфера
  uint16_t string_width = TFT_PutFloatValue(0,0,NO_ALIGN,Value,Digit,Mantissa,Sign,Unit,color,background,font);
  
  // Если указано выравнивание строки, то меняем координату начала
  // строки, иначе оставляем указанную
  if (align != NO_ALIGN)
  {
    // выравнивание координаты Х по ширине дисплея
    switch (align)
    {
      case L_ALIGN:  x = 0; break;
      case R_ALIGN:  x = TFT_WIDTH - string_width; break;
      case C_ALIGN:  x = (TFT_WIDTH - string_width)/2; break;
      case RI_ALIGN: x = (TFT_WIDTH - R_INDENT) - string_width; break;
      case CI_ALIGN: x = (TFT_WIDTH - string_width)/2 - R_INDENT; break;
    }
  }

  // Устанавливаем поле вывода
  TFT_ILI9341_SetAddrWindow(x,y,x+string_width-1,y+font.Height);
  
  // Передаем буффер
  TFT_SendBuffer(TFT_BUF,(string_width)*2*font.Height);  
    
  return string_width;
}

/*******************************************************************************
  *
  * Отрисовка имени параметра
  *
*******************************************************************************/

uint16_t TFT_WriteParamName (uint16_t x, uint16_t y,   // координаты параметра
                             uint8_t align,            // выравнивание 
                             sFloatSigned *Element,    // ссылка на структуру элемента
                             uint16_t color,           // цвет символов строки
                             uint32_t background,      // цвет фона
                             sFont font)
{
  char*   Name = (char*) (*Element).Name[Language];
  uint16_t BlockWidth;
  uint16_t x0;
  
  // Выравнивание и ширина поля
  switch(align)
  {
    // Выравниваем по ширине блока TFT_SIGN_WIDTH
    case S_ALIGN: 
      BlockWidth = TFT_SIGN_WIDTH; 
      x0 = (BlockWidth - TFT_CalcStringWidth(Name,font))/2;
      break;
    case B_ALIGN: 
      BlockWidth = TFT_BLOCK_WIDTH; 
      break;
  }  
  
  // Ждем, пока DMA закончит передачу
  while(IS_SPI_DMA_BUSY) {}
  
  // Помещаем в буфер наименование
  uint16_t string_width = TFT_PutString(x0,0,align,color,background,Name,font);
  
  // Устанавливаем поле вывода
  TFT_ILI9341_SetAddrWindow(x,y,x+BlockWidth-1,y+font.Height);
  
  // Передаем буффер
  TFT_SendBuffer(TFT_BUF,(BlockWidth)*2*font.Height);  
  
  return string_width;
}

/*******************************************************************************
  *
  * Отрисовка значения параметра
  *
*******************************************************************************/

uint16_t TFT_WriteParamSign (uint16_t x, uint16_t y,   // координаты параметра
                             uint8_t align,            // выравнивание 
                             sFloatSigned *Element,    // ссылка на структуру элемента
                             uint16_t color,           // цвет символов строки
                             uint32_t background,      // цвет фона
                             sFont font)
{
  int value = (*Element).Value;
  // Если температура ГВС, подачи или шнека, то делим на 10 для вывода
  if (((Element == (sFloatSigned*) &sTempHWS) || 
       (Element == (sFloatSigned*) &sTempIn) ||
       (Element == (sFloatSigned*) &sTempScrew)) &&
       (value != TEMP_SENSOR_BREAK) &&
       (value != TEMP_SENSOR_CLOSURE)) value /= 10;
  char*   Unit = TFT_GetUnitChar((*Element).Unit);
  uint16_t BlockWidth;  
  uint16_t x0;
  uint16_t string_width = 0;
  
  // Выравнивание и ширина поля
  switch(align)
  {
    // Выравниваем по ширине блока TFT_SIGN_WIDTH
    case S_ALIGN: 
      BlockWidth = TFT_SIGN_WIDTH; break;
    case B_ALIGN: 
      BlockWidth = TFT_BLOCK_WIDTH; break;
  }  
  
  // Ждем, пока DMA закончит передачу
  while(IS_SPI_DMA_BUSY) {}

  // Если разрыв, выводим символ 
  if (value == TEMP_SENSOR_BREAK)
  {
    if (align == S_ALIGN) x0 = (BlockWidth - TFT_CalcStringWidth(" ",CircuitBreak))/2;
    string_width += TFT_PutString(x0,0,align,HEAD_COLOR,background," ",CircuitBreak); 
  }
  else
  // Если замыкание, выводим символ
  if (value == TEMP_SENSOR_CLOSURE)
  {
    if (align == S_ALIGN) x0 = (BlockWidth - TFT_CalcStringWidth(" ",CircuitClosure))/2;
    string_width += TFT_PutString(x0,0,align,HEAD_COLOR,background," ",CircuitClosure); 
  }
  // Иначе значение
  else
  {
    uint8_t Sign = 0;
    if (value < 0) Sign = SIGNED;
    else           Sign = NON_SIGNED;
    string_width += TFT_PutFloatValue(0,0,align,value,1,0,Sign,Unit,color,background,font);
  }  
  
    // Устанавливаем поле вывода
  TFT_ILI9341_SetAddrWindow(x,y,x+BlockWidth-1,y+font.Height-1);
  
  // Передаем буффер
  TFT_SendBuffer(TFT_BUF,(BlockWidth)*2*(font.Height));  
  
  return string_width;
}

/*******************************************************************************
  *
  * Отрисовка строки параметра с запятой, мантиссой, размерностью и курсором
  *
  * В структуре элемента редактирования sFloat прописана ссылка на отображения
  * элемента sFloatView и ссылка на данные редактирования sFloatControl
  * ViewCursor > 0 - курсор отображается, отсчет от самого младшего разряда
  *
*******************************************************************************/

uint16_t TFT_FormatFloat (uint16_t x, uint16_t y, // координаты строки (верхний левый угол)
                          uint8_t align,          // выравнивание по ширине
                          sFloat *Element,        // ссылка на структуру элемента редактирования
                          uint8_t ViewCursor,     // индикатор отображения курсора
                          uint16_t color,         // цвет символов строки
                          uint32_t background,    // цвет фона
                          sFont font)             // ссылка на структуру шрифта
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
  
  // Определяем знаки мантиссы
  if ((*Element).Mantissa > 0)
  {
    for (uint8_t i = 0; i < Mantissa; i++) Value /= 10;
    
    if (Value < 10) Digits = 1;
    else if ((Value >= 10) && (Value < 100)) Digits = 2;
    else if ((Value >= 100) && (Value < 1000)) Digits = 3;
    else if (Value >= 1000) Digits = 4;
    
    Num_String[DigNum + Digits] = '.';
    
    // Формирование цифр мантиссы
    for (uint8_t i = DigNum + Mantissa + Digits; i > DigNum + Digits; i--)
    {
      Dig = ValueDig % 10;
      ValueDig /= 10;
      Num_String[i] = TFT_GetDecimalChar(Dig);
    }
  }
  else
  // Если мантиссы нет, то корректируем число знаков от значения
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
  
  // Добавляем целые знаки
  for (int i = DigNum + Digits - 1; i >= DigNum; i--)
  {
    Dig = Value % 10;
    Value /= 10;
    Num_String[i] = TFT_GetDecimalChar(Dig);
  }  
  
  // Прибавляем ширину символов для отрисовки блока
  int ValueWidth = TFT_CalcStringWidth(Num_String,font);
  BlockWidth += ValueWidth;
    
  // Определение единицы измерения
  if (Unit[0] > 0)
  {
    BlockWidth += TFT_CalcStringWidth(" ",font);
    BlockWidth += UnitWidth - font.Space - 1;
  }
  
  BlockWidth += font.Space;
  
  dot_width = TFT_WIDTH - R_INDENT - BlockWidth - x;
  buf_size = dot_width + BlockWidth;
  // Вставляем точки пункта меню в буфер
  TFT_HDotLine(0,buf_size,dot_width,2,color,background);
    
  uint32_t num_color = 0;
  uint32_t num_back_color = 0; 
  
  // Если вышли за диапазон, то параметр отображаем красным цветом
  if (Digit_Alarm > 0) 
  {
    num_color = ALARM_COLOR;
    num_back_color = background;
  }
  else
  {
    // Отображаем курсор и выводим число
    if ((ViewCursor > 0) & (IS_BLINK))
    {
      // Если отображение курсора включено и активирован флаг мигания BLINK,
      // то меняем местами цвет символа и цвет фона
      num_color = background;
      num_back_color = color;
    }
    else
    // Вывод цифры без курсора
    {
      num_color = color;
      num_back_color = background;
    } 
  }
  
  string_width += TFT_PutString(dot_width,dot_width+BlockWidth,MAN_ALIGN,num_color,num_back_color,Num_String,font);
  
  uint16_t unit_width = 0;
  
  // Прибавляем ширину символов для отрисовки единицы измерения 
  if (Unit[0] > 0)
  {
    char Unit_String[5] = {0,0,0,0,0};
    
    uint8_t i = 0;
    
    // Вывод единицы измерения
    while(Unit[i]) 
    {
      Unit_String[i] = Unit[i];
      i++;
    } 
    
    unit_width = TFT_PutString(dot_width+string_width,0,RIS_ALIGN,color,background,Unit_String,font); 
  }
  
    // Устанавливаем поле вывода
  TFT_ILI9341_SetAddrWindow(x,y,TFT_WIDTH-R_INDENT-1,y+font.Height);
  
  // Передаем буффер
  TFT_SendBuffer(TFT_BUF,buf_size*2*font.Height); 
  
  return string_width + unit_width + dot_width;
}

/*******************************************************************************
  *
  * Отрисовка меню редактирования числового параметра 
  *
  * Рисует и редактирует параметр *Element
  * В структуре элемента редактирования прописана ссылка на отображения элемента
  * и на данные редактирования
  *
*******************************************************************************/

int8_t Digit_Alarm = 0;

uint16_t TFT_WriteMenuDigitalControl (uint16_t x, uint16_t y, // координаты элемента
                                      uint8_t align,          // выравнивание по ширине
                                      sMenu *Menu,            // ссылка на структуру меню управления
                                      uint8_t MenuItem,       // номер элемента меню, соотв. выбору элемента
                                      sFloat *Element,        // ссылка на структуру элемента редактирования
                                      uint16_t Color,         // цвет символов элемента меню
                                      uint32_t Background,    // цвет фона
                                      uint16_t AlarmColor,    // цвет предупреждений (выход параметра на границу), если 0 не исп-ся
                                      sFont font)             // ссылка на структуру шрифта
{
  uint16_t Off_width = TFT_CalcStringWidth((char*) sOnOffValue[Language][0],font) + font.Space;
  uint16_t dot_width = 0; 
  uint16_t buf_size = 0;

  if ((Element->Off & OFF_STATE) == OFF_STATE)
  {
    dot_width = TFT_WIDTH - R_INDENT - Off_width - x;
    buf_size = dot_width + Off_width;
    
    // Устанавливаем поле вывода
    TFT_ILI9341_SetAddrWindow(x,y,TFT_WIDTH-R_INDENT-1,y+font.Height);
  }
  
  // Если выбран другой элемент, просто рисуем число и размерность либо "Откл"
  if (((*Menu).Item & ~mActive) != MenuItem) 
  {
    if (((((*Menu).Item & mActive) != mActive) && IS_NEED_UPDATE) || IS_NEED_UPDATE_LANG)
    {
      // Если элемент не выключен, выводим значение
      if ((Element->Off & OFF_STATE) == 0)
      {
        if (Element->Value > Element->Max) Element->Value = Element->Max;
        if (Element->Value < Element->Min) Element->Value = Element->Min;

        TFT_FormatFloat (x,y,align,Element,0,Color,Background,font);
      }
      // Если элемент выключен, то выводим "Откл"
      else
      {
        // Вставляем точки пункта меню в буфер
        TFT_HDotLine(0,buf_size,dot_width,2,Color,NO_BACKGROUND);
        // Вставляем параметр в буфер
        TFT_PutString(dot_width,y,RIS_ALIGN,Color,NO_BACKGROUND,(char*) sOnOffValue[Language][0],font);
        // Передаем буффер
        TFT_SendBuffer(TFT_BUF,buf_size*2*font.Height); 
      }
    }
  }
  else
  //элемент выбран, но не активен (замена цветов)
  if ((*Menu).Item == MenuItem) 
  {
    // Если элемент не выключен, выводим значение
    if ((Element->Off & OFF_STATE) == 0) 
    {
      TFT_FormatFloat (x,y,align,Element,0,Background,Color,font);
    }
    // Если элемент выключен, то выводим "Откл"
    else 
    {
      // Вставляем точки пункта меню в буфер
      TFT_HDotLine(0,buf_size,dot_width,2,Background,Color);
      // Вставляем параметр в буфер
      TFT_PutString(dot_width,y,RIS_ALIGN,Background,Color,(char*) sOnOffValue[Language][0],font);
      // Передаем буффер
      TFT_SendBuffer(TFT_BUF,buf_size*2*font.Height); 
    }
  }
  else
  if ((*Menu).Item == MenuItem + mActive) //элемент активен. Редактирование по позициям курсора
  {
    // Если параметр вышел за границы, отрисовываем его цветом AlarmColor
    if ((AlarmColor != 0) & (Digit_Alarm > 0))
    {
      Digit_Alarm--;
      // Если элемент не выключен, выводим значение
      if ((Element->Off & OFF_STATE) == 0) 
      {
        TFT_FormatFloat (x,y,align,Element,0,Color,Background,font);
      }
      // Если элемент выключен, то выводим "Откл"
      else 
      {
        // Вставляем точки пункта меню в буфер
        TFT_HDotLine(0,buf_size,dot_width,2,Color,NO_BACKGROUND);
        // Вставляем параметр в буфер
        TFT_PutString(dot_width,y,RIS_ALIGN,AlarmColor,Background,(char*) sOnOffValue[Language][0],font);
        // Передаем буффер
        TFT_SendBuffer(TFT_BUF,buf_size*2*font.Height); 
      }
    }
    else 
    {
      // Если элемент не выключен, выводим значение
      if ((Element->Off & OFF_STATE) == 0) 
      {
        TFT_FormatFloat (x,y,align,Element,1,Color,Background,font);
      }
      // Если элемент выключен, то выводим "Откл"
      else 
      {
        // Вставляем точки пункта меню в буфер
        TFT_HDotLine(0,buf_size,dot_width,2,Color,NO_BACKGROUND);
        
        if (IS_BLINK) 
          TFT_PutString(dot_width,y,RIS_ALIGN,Background,Color,(char*) sOnOffValue[Language][0],font);
        else          
          TFT_PutString(dot_width,y,RIS_ALIGN,Color,NO_BACKGROUND,(char*) sOnOffValue[Language][0],font);
      
        // Передаем буффер
        TFT_SendBuffer(TFT_BUF,buf_size*2*font.Height); 
      }
      
      // Корректировка параметра поразрядно
      Digit_Alarm = Menu_CorrectFloatUpDown(Element,Menu);
    }
  }
  
  return 0;
}

/*******************************************************************************
  *
  * Отрисовка меню редактирования параметра типа перечисления
  *
  * Рисует и редактирует параметр *Element
  *
*******************************************************************************/

int Enum_Alarm = 0;

uint16_t TFT_WriteMenuEnumControl (uint8_t x, uint8_t y, // координаты элемента
                                   uint8_t align,        // выравнивание по ширине
                                   sMenu *Menu,          // ссылка на структуру меню управления
                                   uint8_t MenuItem,     // номер элемента меню, соотв. выбору элемента
                                   sEnum *Element,       // ссылка на структуру элемента редактирования
                                   uint16_t Color,       // цвет символов 
                                   uint32_t Background,  // цвет фона 
                                   uint16_t AlarmColor,  // цвет предупреждений (выход параметра за границу), если 0 не исп-ся
                                   sFont font)           // ссылка на структуру шрифта
{
  char *str = (char*) Element->Value[Language][Element->Point];
  uint16_t str_width = TFT_CalcStringWidth(str,font) + font.Space;
  uint16_t dot_width = TFT_WIDTH - R_INDENT - str_width - x;
  uint16_t buf_size = str_width + dot_width;
  
  // Устанавливаем поле вывода
  TFT_ILI9341_SetAddrWindow(x,y,TFT_WIDTH-R_INDENT-1,y+font.Height);
  
  // Выбран другой элемент, просто рисуем элемент
  if (((*Menu).Item & ~mActive) != MenuItem) // выбран другой элемент, просто рисуем наш элемент
  {
    if (((((*Menu).Item & mActive) != mActive) && IS_NEED_UPDATE) || IS_NEED_UPDATE_LANG)
    {
      // Вставляем точки пункта меню в буфер
      TFT_HDotLine(0,buf_size,dot_width,2,Color,NO_BACKGROUND);
      // Вставляем параметр в буфер
      TFT_PutString(dot_width,y,RIS_ALIGN,Color,NO_BACKGROUND,str,font);   
      // Передаем буффер
      TFT_SendBuffer(TFT_BUF,buf_size*2*font.Height); 
    }
  }
  else
  // Элемент выбран, но не активен (замена цветов)
  if ((*Menu).Item == MenuItem)
  {
    // Вставляем точки пункта меню в буфер
    TFT_HDotLine(0,buf_size,dot_width,2,Background,Color);
    // Вставляем параметр в буфер
    TFT_PutString(dot_width,y,RIS_ALIGN,Background,Color,str,font);
    // Передаем буффер
    TFT_SendBuffer(TFT_BUF,buf_size*2*font.Height); 
  }
  else
  // Элемент активен, редактирование
  if ((*Menu).Item == MenuItem + mActive)
  {
    TFT_HDotLine(0,buf_size,dot_width,2,Color,Background);
    // Если элемент имеет только состояние вкл/откл, то не переходим в редактирование,
    // а только меняем значение, иначе переходим
    if (NON_ONOFF)
    {
      if (AlarmColor != 0 & Enum_Alarm > 0) // рисуем параметр с предупреждением цветом
      {
        Enum_Alarm--;
        TFT_PutString(dot_width,y,RIS_ALIGN,AlarmColor,Background,str,font);
      } 
      else // рисуем параметр с курсором
      if (IS_BLINK) TFT_PutString(dot_width,y,RIS_ALIGN,Background,Color,str,font);
      else          TFT_PutString(dot_width,y,RIS_ALIGN,Color,Background,str,font);
    }
    else TFT_PutString(dot_width,y,RIS_ALIGN,Background,Color,str,font);
  
    // Редактирование в соответствии с заданным режимом
    Enum_Alarm += Menu_CorrectEnumUpDown(Element,Menu);
    
    // Передаем буффер
    TFT_SendBuffer(TFT_BUF,buf_size*2*font.Height); 
  }
  
  return 0;
}