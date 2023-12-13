
#include "ILI9341.h"
#include "Init.h"
#include "Menu.h"
#include "main.h"

/*******************************************************************************
  *
  * ��������� �������
  *
*******************************************************************************/

void TFT_DrawPixel(uint16_t x,
                   uint16_t y,
                   uint16_t color)
{
  // ��������� ������� �������
  TFT_ILI9341_SetAddrWindow(x,y,x,y);
  
  // ��������� � ���������� ������
  TFT_DC_DATA();
  uint8_t data[] = {color >> 8, color & 0xFF};
  HAL_SPI_Transmit(&hspi2, data, 2, 0);
}

/*******************************************************************************
  *
  * ��������� �������������� ���������� ����� � ������
  *
*******************************************************************************/
                  
void TFT_HDotLine(uint16_t x,          // ��������� ���������� ����� �� �����������
                  uint16_t buf_size,   // ������ ������
                  uint16_t w,          // ������ �����
                  uint8_t size,        // ������� ����� � ��������
                  uint16_t color,      // ���� �����
                  uint32_t background) // ���� ����
{ 
  // ����, ���� DMA �������� ��������
  while(IS_SPI_DMA_BUSY) {}
  
  uint8_t k = 0;
  
  // ���������� ������ �����
  if (background != NO_BACKGROUND)
  {
    // ���������� ������ �����
    for (int j = 0; j < 18; j++)
    {
      for (int i = 0; i < w; i++)
      {
        TFT_BUF[2*((x+i) + buf_size*j)] = background >> 8;
        TFT_BUF[2*((x+i) + buf_size*j)+1] = background & 0xFF;
      }
    }
  }
  
  // ���������� ������ �������
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
  * ������� �������������� ������
  *
*******************************************************************************/

void TFT_FillRect(uint16_t x, 
                  uint16_t y, 
                  uint16_t w, 
                  uint16_t h, 
                  uint16_t color)
{
  // ����, ���� DMA �������� ��������
  while(IS_SPI_DMA_BUSY) {}
  
  if (w > 1)
  {
    // ������������� ���� ��� ������
    TFT_ILI9341_SetAddrWindow(x, y, x+w-1, y+h-1);                
    
    // ������������ �������� ������
    if (w*h < TFT_STR_WIDTH*18*2)
    {     
      // ��������� �����
      for (uint16_t i = 0; i < w*h; i++)
      {
        TFT_BUF[2*i] = color >> 8;
        TFT_BUF[2*i+1] = color & 0xFF;
      }
      
      // �������� ������
      TFT_SendBuffer(TFT_BUF,w*2*h);
    }
  }
}

/*******************************************************************************
  *                                                                                     
  * ��������� �������� �������������� � ������ � ������������ �� ����� �������
  *
*******************************************************************************/

void TFT_FillString(uint16_t color)   // ���� �������
{
  // ����, ���� DMA �������� ��������
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
  * ������� ������ ������
  *
  * ��������� ����� ������� ������ �������� 320 � 20, ����� �������� ������
  * ����� ~7 ��
  *
*******************************************************************************/

void TFT_FillScreen(uint16_t color)
{
  // ����, ���� DMA �������� ��������
  while(IS_SPI_DMA_BUSY) {}
  
  // ������������� ���� ��� ������ - ���� �������
  TFT_ILI9341_SetAddrWindow(0, 0, TFT_WIDTH-1, TFT_HEIGHT-1);
  
  for (uint16_t i = 0; i < TFT_STR_WIDTH*18; i++)
  {
    TFT_BUF[2*i]   = color >> 8;
    TFT_BUF[2*i+1] = color & 0xFF;
  }
  
  for (int i = 0; i < 18; i++)
  {
    // �������� ������
    TFT_SendBuffer(TFT_BUF,TFT_STR_WIDTH*2*18);
  }
}

/*******************************************************************************
  *
  * ������� ����
  *
  * ���� �������� 180 � (320 - RI_INDENT)
  * ������ MENU_BASE_Y + MENU_SHIFT_Y
  *
*******************************************************************************/

void TFT_ClearMenu(uint16_t color)
{
  // ������������� ���� ��� ������ - ���� �������
  TFT_ILI9341_SetAddrWindow(0, MENU_BASE_Y + MENU_SHIFT_Y, 
                            TFT_WIDTH - R_INDENT - 1, MENU_BASE_Y + 11*MENU_SHIFT_Y - 1);  
  
  // ��������� ������ ������ ������
  for (uint16_t i = 0; i < TFT_STR_WIDTH*15; i++)
  {
    TFT_BUF[2*i]   = color >> 8;
    TFT_BUF[2*i+1] = color & 0xFF;
  }
  
  for (int i = 0; i < 15; i++)
  {
    // �������� ������
    TFT_SendBuffer(TFT_BUF,TFT_STR_WIDTH*2*15);
  }
}

/*******************************************************************************
  *
  * ��������� ����� �����������
  *
*******************************************************************************/

void TFT_HLineDivider (uint8_t y,      // ���������� ����� �� ���������
                       uint8_t size,   // ������ �����
                       uint16_t color) // ���� �������
{
  // ����, ���� DMA �������� ��������
  while(IS_SPI_DMA_BUSY) {}
  
  // ���������� ������
  for (uint8_t j = 0; j < size; j++)
  {
    for (uint16_t i = 0; i < TFT_WIDTH; i++)
    {
      TFT_BUF[2*(TFT_WIDTH*j+i)] = color >> 8;
      TFT_BUF[2*(TFT_WIDTH*j+i)+1] = color & 0xFF;
    }
  }
  
  // ������������� ���� ������
  TFT_ILI9341_SetAddrWindow(0,y,TFT_WIDTH-1,y+size-1);
  
  // �������� ������
  TFT_SendBuffer(TFT_BUF,TFT_WIDTH*2*size);  
}

/*******************************************************************************
  *
  * ��������� �������������� �����
  *
*******************************************************************************/

void TFT_HLine(uint16_t x,      // ��������� ���������� ����� �� �����������
               uint8_t y,       // ���������� ����� �� ���������
               uint16_t w,      // ������ �����
               uint8_t size,    // ������� ����� � ��������
               uint16_t color)  // ���� �����
{
  // ����, ���� DMA �������� ��������
  while(IS_SPI_DMA_BUSY) {}
  
  // ���������� ������ 
  for (uint8_t j = 0; j < size; j++)
  {
    for(uint16_t i = 0; i < w; i++)
    {
      TFT_BUF[2*(w*j+i)] = color >> 8;
      TFT_BUF[2*(w*j+i)+1] = color & 0xFF;
    }
  }
  
  // ������������� ���� ������
  TFT_ILI9341_SetAddrWindow(x,y,x+w-1,y+size-1);
  
  // �������� ������
  TFT_SendBuffer(TFT_BUF,w*2*size);  
}

/*******************************************************************************
  *
  * ��������� ������������ �����
  *
*******************************************************************************/

void TFT_VLine(uint16_t x,      // ���������� ����� �� �����������
               uint8_t y,       // ��������� ���������� ����� �� ���������
               uint8_t h,       // ������ �����
               uint8_t size,    // ������� ����� � ��������
               uint16_t color)  // ���� �����
{
  // ����, ���� DMA �������� ��������
  while(IS_SPI_DMA_BUSY) {}
  
  // ���������� ������
  for(uint8_t i = 0; i < size; i++)
  {
    for(uint8_t j = 0; j < h; j++)
    {
      TFT_BUF[2*(j+h*i)] = color >> 8;
      TFT_BUF[2*(j+h*i)+1] = color & 0xFF;
    }
  }
  
  // ������������� ���� ������
  TFT_ILI9341_SetAddrWindow(x,y,x+size-1,y+h-1);
  
  // �������� ������
  TFT_SendBuffer(TFT_BUF,h*2*size);    
}

/*******************************************************************************
  *
  * ��������� �������������� � ������ � ������������ �� ����� �������
  *
*******************************************************************************/

void TFT_Rectangle(uint16_t x,      // ���������� �������� ������ ���� �� �����������
                   uint8_t y,       // ���������� �������� ������ ���� �� ���������
                   uint16_t w,      // ������ ��������������
                   uint8_t h,       // ������ ��������������
                   uint8_t size,    // ������� ����� �������
                   uint16_t color)  // ���� �����
{
  TFT_HLine(x, y, w, size, color);
  TFT_HLine(x, y + h, w, size, color);
  TFT_VLine(x, y, h, size, color);
  TFT_VLine(x + w - size, y, h, size, color);
}

/*******************************************************************************
  *
  * ����� ����������� �� �������
  *
  * ���� ��� ����������� img.ImageType - ������� (0x00), ��������� �������� 
  * �������, ���� ��� - ���� (0x01), �������� 0x01 ���������� �� img.Color, � 
  * �������� 0x00 - �� img.Background
  *
  * ����������� ������� ������ ������, ��������� �� �������
  * ����� 3840 ���� uint16_t
  *
*******************************************************************************/

void TFT_DrawImage(uint16_t x,     // ���������� �� �����������
                   uint16_t y,     // ���������� �� ���������
                   uint8_t  retry, // ���� ������� ������ �����������
                   sImage *img)    // ������ �� ��������� �����������
{ 
  if ((IS_NEED_UPDATE) || (retry != 0))
  {
    uint32_t CountImg = (*img).Width*(*img).Height; // ������ �����������
    uint32_t CountBuf = TFT_STR_WIDTH*18;           // ������ ������
    uint16_t BufHeight = CountBuf / (*img).Width;   // ������ �����������, ����������� �� ������
    uint16_t CountBufImg = BufHeight * (*img).Width; // ������ ������, ����������� �� ������ �����������
    uint8_t  ImgBuf   = ((CountImg % CountBufImg) > 0) ? (CountImg/CountBufImg + 1) : (CountImg/CountBufImg); // ����� ������� � �����������
    uint8_t  ShiftImg = 0;
    
    // ������� ����������� �������� ������ ������� ������, ���� �� �������� �����������
    while (ImgBuf > 1)
    {
      // ����, ���� DMA �������� ��������
      while(IS_SPI_DMA_BUSY) {}
    
      // ������������� ���� ������
      TFT_ILI9341_SetAddrWindow(x,y + ShiftImg*BufHeight,
                                x+(*img).Width-1,y+BufHeight*(ShiftImg+1)-1);
    
      // ��������� ������� ��� ����������� �����������
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
      
      // �������� ��� ���������� ���������� ������
      ShiftImg++;
      ImgBuf--;
      CountImg -= CountBufImg;
      
      // �������� ������
      TFT_SendBuffer(TFT_BUF,CountBufImg*2);
    }
    
    // ����, ���� DMA �������� ��������
    while(IS_SPI_DMA_BUSY) {}
    
    // ������������� ���� ������
    TFT_ILI9341_SetAddrWindow(x,y + ShiftImg*BufHeight,
                              x+(*img).Width-1,y+BufHeight*(ShiftImg+1)-1);
    
    // ��������� ������� �������� ��� ������������ �����������
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
    
    // �������� ������
    TFT_SendBuffer(TFT_BUF,CountImg*2);
  }
}