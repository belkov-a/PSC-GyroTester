
#include "ILI9341.h"
#include "Init.h"
#include "Menu.h"
#include "main.h"

/*******************************************************************************
  *
  * Отрисовка пиксела
  *
*******************************************************************************/

void TFT_DrawPixel(uint16_t x,
                   uint16_t y,
                   uint16_t color)
{
  // Установка позиции пиксела
  TFT_ILI9341_SetAddrWindow(x,y,x,y);
  
  // Формируем и отправляем пиксел
  TFT_DC_DATA();
  uint8_t data[] = {color >> 8, color & 0xFF};
  HAL_SPI_Transmit(&hspi2, data, 2, 0);
}

/*******************************************************************************
  *
  * Отрисовка горизонтальной пунктирной линии в буфере
  *
*******************************************************************************/
                  
void TFT_HDotLine(uint16_t x,          // начальная координата линии по горизонтали
                  uint16_t buf_size,   // ширина буфера
                  uint16_t w,          // ширина линии
                  uint8_t size,        // толщина линии в пикселах
                  uint16_t color,      // цвет линии
                  uint32_t background) // цвет фона
{ 
  // Ждем, пока DMA закончит передачу
  while(IS_SPI_DMA_BUSY) {}
  
  uint8_t k = 0;
  
  // Заполнения буфера фоном
  if (background != NO_BACKGROUND)
  {
    // Заполнения буфера фоном
    for (int j = 0; j < 18; j++)
    {
      for (int i = 0; i < w; i++)
      {
        TFT_BUF[2*((x+i) + buf_size*j)] = background >> 8;
        TFT_BUF[2*((x+i) + buf_size*j)+1] = background & 0xFF;
      }
    }
  }
  
  // Заполнение буфера точками
  for (int j = 14; j < 14 + size; j++)
  {
    k = 0;
    for (int i = 0; i < w; i++)
    {
      k++;
      if (k < size+1) 
      {
        TFT_BUF[2*((x+i) + buf_size*j)] = color >> 8;
        TFT_BUF[2*((x+i) + buf_size*j)+1] = color & 0xFF;
      }
      else 
      if (k == 2*size+1) k = 0;
    }
  }
}

/*******************************************************************************
  *
  * Заливка прямоугольника цветом
  *
*******************************************************************************/

void TFT_FillRect(uint16_t x, 
                  uint16_t y, 
                  uint16_t w, 
                  uint16_t h, 
                  uint16_t color)
{
  // Ждем, пока DMA закончит передачу
  while(IS_SPI_DMA_BUSY) {}
  
  if (w > 1)
  {
    // Устанавливаем поле для вывода
    TFT_ILI9341_SetAddrWindow(x, y, x+w-1, y+h-1);                
    
    // Ограничиваем размером буфера
    if (w*h < TFT_STR_WIDTH*18*2)
    {     
      // Заполняем буфер
      for (uint16_t i = 0; i < w*h; i++)
      {
        TFT_BUF[2*i] = color >> 8;
        TFT_BUF[2*i+1] = color & 0xFF;
      }
      
      // Передаем буффер
      TFT_SendBuffer(TFT_BUF,w*2*h);
    }
  }
}

/*******************************************************************************
  *                                                                                     
  * Отрисовка залитого прямоугольника в буфере с ограничением по краям дисплея
  *
*******************************************************************************/

void TFT_FillString(uint16_t color)   // цвет заливки
{
  // Ждем, пока DMA закончит передачу
  while(IS_SPI_DMA_BUSY) {}
  
  for (int j = 0; j < 15; j++)
  {
    for (int i = 0; i < TFT_STR_WIDTH; i++)
    {
      TFT_BUF[2*(TFT_STR_WIDTH*j+i)] = color >> 8;
      TFT_BUF[2*(TFT_STR_WIDTH*j+i)+1] = color & 0xFF;
    }
  }
}

/*******************************************************************************
  *
  * Заливка экрана цветом
  *
  * Выполнено через заливку блоков размером 320 х 20, время передачи одного
  * блока ~7 мс
  *
*******************************************************************************/

void TFT_FillScreen(uint16_t color)
{
  // Ждем, пока DMA закончит передачу
  while(IS_SPI_DMA_BUSY) {}
  
  // Устанавливаем поле для вывода - весь дисплей
  TFT_ILI9341_SetAddrWindow(0, 0, TFT_WIDTH-1, TFT_HEIGHT-1);
  
  for (uint16_t i = 0; i < TFT_STR_WIDTH*18; i++)
  {
    TFT_BUF[2*i]   = color >> 8;
    TFT_BUF[2*i+1] = color & 0xFF;
  }
  
  for (int i = 0; i < 18; i++)
  {
    // Передаем буффер
    TFT_SendBuffer(TFT_BUF,TFT_STR_WIDTH*2*18);
  }
}

/*******************************************************************************
  *
  * Очистка меню
  *
  * Поле размером 180 х (320 - RI_INDENT)
  * Начало MENU_BASE_Y + MENU_SHIFT_Y
  *
*******************************************************************************/

void TFT_ClearMenu(uint16_t color)
{
  // Устанавливаем поле для вывода - весь дисплей
  TFT_ILI9341_SetAddrWindow(0, MENU_BASE_Y + MENU_SHIFT_Y, 
                            TFT_WIDTH - R_INDENT - 1, MENU_BASE_Y + 11*MENU_SHIFT_Y - 1);  
  
  // Заполняем буффер строки цветом
  for (uint16_t i = 0; i < TFT_STR_WIDTH*15; i++)
  {
    TFT_BUF[2*i]   = color >> 8;
    TFT_BUF[2*i+1] = color & 0xFF;
  }
  
  for (int i = 0; i < 15; i++)
  {
    // Передаем буффер
    TFT_SendBuffer(TFT_BUF,TFT_STR_WIDTH*2*15);
  }
}

/*******************************************************************************
  *
  * Отрисовка линии разделителя
  *
*******************************************************************************/

void TFT_HLineDivider (uint8_t y,      // координата линии по вертикали
                       uint8_t size,   // ширина линии
                       uint16_t color) // цвет заливки
{
  // Ждем, пока DMA закончит передачу
  while(IS_SPI_DMA_BUSY) {}
  
  // Заполнение буфера
  for (uint8_t j = 0; j < size; j++)
  {
    for (uint16_t i = 0; i < TFT_WIDTH; i++)
    {
      TFT_BUF[2*(TFT_WIDTH*j+i)] = color >> 8;
      TFT_BUF[2*(TFT_WIDTH*j+i)+1] = color & 0xFF;
    }
  }
  
  // Устанавливаем поле вывода
  TFT_ILI9341_SetAddrWindow(0,y,TFT_WIDTH-1,y+size-1);
  
  // Передаем буффер
  TFT_SendBuffer(TFT_BUF,TFT_WIDTH*2*size);  
}

/*******************************************************************************
  *
  * Отрисовка горизонтальной линии
  *
*******************************************************************************/

void TFT_HLine(uint16_t x,      // начальная координата линии по горизонтали
               uint8_t y,       // координата линии по вертикали
               uint16_t w,      // ширина линии
               uint8_t size,    // толщина линии в пикселах
               uint16_t color)  // цвет линии
{
  // Ждем, пока DMA закончит передачу
  while(IS_SPI_DMA_BUSY) {}
  
  // Заполнение буфера 
  for (uint8_t j = 0; j < size; j++)
  {
    for(uint16_t i = 0; i < w; i++)
    {
      TFT_BUF[2*(w*j+i)] = color >> 8;
      TFT_BUF[2*(w*j+i)+1] = color & 0xFF;
    }
  }
  
  // Устанавливаем поле вывода
  TFT_ILI9341_SetAddrWindow(x,y,x+w-1,y+size-1);
  
  // Передаем буффер
  TFT_SendBuffer(TFT_BUF,w*2*size);  
}

/*******************************************************************************
  *
  * Отрисовка вертикальной линии
  *
*******************************************************************************/

void TFT_VLine(uint16_t x,      // координата линии по горизонтали
               uint8_t y,       // начальная координата линии по вертикали
               uint8_t h,       // высота линии
               uint8_t size,    // толщина линии в пикселах
               uint16_t color)  // цвет линии
{
  // Ждем, пока DMA закончит передачу
  while(IS_SPI_DMA_BUSY) {}
  
  // Заполнение буфера
  for(uint8_t i = 0; i < size; i++)
  {
    for(uint8_t j = 0; j < h; j++)
    {
      TFT_BUF[2*(j+h*i)] = color >> 8;
      TFT_BUF[2*(j+h*i)+1] = color & 0xFF;
    }
  }
  
  // Устанавливаем поле вывода
  TFT_ILI9341_SetAddrWindow(x,y,x+size-1,y+h-1);
  
  // Передаем буффер
  TFT_SendBuffer(TFT_BUF,h*2*size);    
}

/*******************************************************************************
  *
  * Отрисовка прямоугольника в буфере с ограничением по краям дисплея
  *
*******************************************************************************/

void TFT_Rectangle(uint16_t x,      // координата верхнего левого угла по горизонтали
                   uint8_t y,       // координата верхнего левого угла по вертикали
                   uint16_t w,      // ширина прямоугольника
                   uint8_t h,       // высота прямоугольника
                   uint8_t size,    // толщина линии стороны
                   uint16_t color)  // цвет линий
{
  TFT_HLine(x, y, w, size, color);
  TFT_HLine(x, y + h, w, size, color);
  TFT_VLine(x, y, h, size, color);
  TFT_VLine(x + w - size, y, h, size, color);
}

/*******************************************************************************
  *
  * Вывод изображения на дисплей
  *
  * Если тип изображения img.ImageType - цветной (0x00), выводятся элементы 
  * массива, если тип - моно (0x01), элементы 0x01 заменяются на img.Color, а 
  * элементы 0x00 - на img.Background
  *
  * Изображение делится кратно буферу, выводится по очереди
  * Буфер 3840 слов uint16_t
  *
*******************************************************************************/

void TFT_DrawImage(uint16_t x,     // координата по горизонтали
                   uint16_t y,     // координата по вертикали
                   uint8_t  retry, // флаг повтора вывода изображения
                   sImage *img)    // ссылка на структуру изображения
{ 
  if ((IS_NEED_UPDATE) || (retry != 0))
  {
    uint32_t CountImg = (*img).Width*(*img).Height; // размер изображения
    uint32_t CountBuf = TFT_STR_WIDTH*18;           // размер буфера
    uint16_t BufHeight = CountBuf / (*img).Width;   // высота изображения, приведенная по буферу
    uint16_t CountBufImg = BufHeight * (*img).Width; // размер буфера, приведенный по ширине изображения
    uint8_t  ImgBuf   = ((CountImg % CountBufImg) > 0) ? (CountImg/CountBufImg + 1) : (CountImg/CountBufImg); // число буферов в изображении
    uint8_t  ShiftImg = 0;
    
    // Выводим изображение циклично кратно размеру буфера, пока не кончится изображение
    while (ImgBuf > 1)
    {
      // Ждем, пока DMA закончит передачу
      while(IS_SPI_DMA_BUSY) {}
    
      // Устанавливаем поле вывода
      TFT_ILI9341_SetAddrWindow(x,y + ShiftImg*BufHeight,
                                x+(*img).Width-1,y+BufHeight*(ShiftImg+1)-1);
    
      // Заполняем цветное или двухцветное изображение
      if ((*img).ImageType == COLOR_IMG) 
        for (int i = 0; i < CountBufImg; i++)
        {
          TFT_BUF[2*i]   = (*img).ImagePointer[i + ShiftImg*CountBufImg] >> 8;
          TFT_BUF[2*i+1] = (*img).ImagePointer[i + ShiftImg*CountBufImg] & 0xFF;
        }
      else
      if ((*img).ImageType == MONO_IMG)
        for (int i = 0; i < CountBufImg; i++)
        {
          TFT_BUF[2*i]   = (*img).MonoPointer[i + ShiftImg*CountBufImg] == 0x00 ? ((*img).Background >> 8) : ((*img).Color >> 8);
          TFT_BUF[2*i+1] = (*img).MonoPointer[i + ShiftImg*CountBufImg] == 0x00 ? ((*img).Background & 0xFF) : ((*img).Color & 0xFF);
        }
      
      // Сдвигаем для заполнения следующего буфера
      ShiftImg++;
      ImgBuf--;
      CountImg -= CountBufImg;
      
      // Передаем буффер
      TFT_SendBuffer(TFT_BUF,CountBufImg*2);
    }
    
    // Ждем, пока DMA закончит передачу
    while(IS_SPI_DMA_BUSY) {}
    
    // Устанавливаем поле вывода
    TFT_ILI9341_SetAddrWindow(x,y + ShiftImg*BufHeight,
                              x+(*img).Width-1,y+BufHeight*(ShiftImg+1)-1);
    
    // Заполняем остаток цветного или двухцветного изображения
    if ((*img).ImageType == COLOR_IMG) 
      for (int i = 0; i < CountImg; i++)
      {
        TFT_BUF[2*i]   = (*img).ImagePointer[i + ShiftImg*CountBufImg] >> 8;
        TFT_BUF[2*i+1] = (*img).ImagePointer[i + ShiftImg*CountBufImg] & 0xFF;      
      }
    else
    if ((*img).ImageType == MONO_IMG)
    for (int i = 0; i < CountImg; i++)
    {
      TFT_BUF[2*i]   = (*img).MonoPointer[i + ShiftImg*CountBufImg] == 0x00 ? ((*img).Background >> 8) : ((*img).Color >> 8);
      TFT_BUF[2*i+1] = (*img).MonoPointer[i + ShiftImg*CountBufImg] == 0x00 ? ((*img).Background & 0xFF) : ((*img).Color & 0xFF);
    }
    
    // Передаем буффер
    TFT_SendBuffer(TFT_BUF,CountImg*2);
  }
}