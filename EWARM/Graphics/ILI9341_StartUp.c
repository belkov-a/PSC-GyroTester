
#include "main.h"
#include "ILI9341.h"
#include "Menu.h"
#include "Init.h"

uint32_t BACK_COLOR = BLACK;

/*******************************************************************************
  *
  * �������� ������� � ������� �� ���� SPI
  *
*******************************************************************************/

void TFT_ILI9341_SendCommand(uint8_t cmd)
{
  TFT_DC_COMMAND();
  
  HAL_SPI_Transmit(&hspi2, &cmd, 1, 5000);
}

/*******************************************************************************
  *
  * �������� ������ � ������� �� ���� SPI
  *
*******************************************************************************/

void TFT_ILI9341_SendData(uint8_t data)
{
  TFT_DC_DATA();
  
  HAL_SPI_Transmit(&hspi2, &data, 1, 5000);
}

/*******************************************************************************
  *
  * �������� ������ ������ � ������� 
  *
*******************************************************************************/

void TFT_WriteData(uint8_t* buff,
                   size_t buff_size)
{
  TFT_DC_DATA();
  
  while(buff_size > 0) 
  {
    uint16_t chunk_size = buff_size > 32768 ? 32768 : buff_size;
    HAL_SPI_Transmit(&hspi2, buff, chunk_size, 0);
    buff += chunk_size;
    buff_size -= chunk_size;
  }
}

/*******************************************************************************
  *
  * ������� �������� ������ � �������
  *
*******************************************************************************/

uint8_t TFT_BUFFER[TFT_STR_WIDTH*2*18] = {0};

uint8_t *TFT_BUF = TFT_BUFFER;

uint16_t TFT_BUF_SIZE = 0;

void TFT_SendBuffer(uint8_t* data,
                    uint32_t size)
{
  // ����, ���� DMA �������� ��������
  while(IS_SPI_DMA_BUSY) {}
    
  TFT_DC_DATA();
  
  TFT_CS_ACTIVE();
  
  // ������� ���� ���������
  SPI_DMA_BUSY_SET;
  
  // ��������� ������ ������
  TFT_BUF_SIZE = size;
  
  // �������� ������ 8 ��� � �������
  HAL_SPI_Transmit_DMA(&hspi2, data, TFT_BUF_SIZE);
}

/*******************************************************************************
  *
  * ���������� ���������� ������ �������� ������ SPI
  *
  * ������ ������ �������� ������, ���� ���������
  *
*******************************************************************************/
  
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  if (hspi->Instance == SPI2)
  {
    // ������ ������ �������� ������ ������� � ������� ���� �������
    CLEAR_TFT_BUF_SET;
    TFT_ClearBuffer(TFT_BUF+TFT_BUF_SIZE/2,TFT_BUF_SIZE/2);
    
    // ���������� Chip Select
    TFT_CS_IDLE();
  }
}

/*******************************************************************************
  *
  * ���������� ���������� �������� �������� ������ SPI
  *
  * ������ ������ �������� ������, ���� ���������
  *
*******************************************************************************/

void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
  if (hspi->Instance == SPI2)
  {
    // ������ ������ �������� ������ �������, ���� ��� ����������
    TFT_ClearBuffer(TFT_BUF,TFT_BUF_SIZE/2);
  }
}

/*******************************************************************************
  *
  * ������� ������� ������� ������ ����� DMA
  *
*******************************************************************************/

void TFT_ClearBuffer(uint8_t* data, // ��������� �� ����� �������
                     uint32_t Size) // ������ ������
{ 
  uint8_t  TFT_BACKGROUND_COLOR[1] = {BACK_COLOR >> 8};
  uint8_t *TFT_BACK = TFT_BACKGROUND_COLOR;

  // �������� ����� ����� ������
  HAL_DMA_Start_IT(&hdma_memtomem_fill, (uint32_t) TFT_BACK, (uint32_t) data,Size);
}

/*******************************************************************************
  *
  * ���������� ����� �������
  *
*******************************************************************************/

void TFT_HardReset(void)
{
  // ������ ������� ��������� � �����
  TFT_RST_ACTIVE();
  // ���� 5 ��
  HAL_Delay(10);
  // �������� ������� ������� �������
  TFT_RST_IDLE();
}

/*******************************************************************************
  *
  * ������������� �������
  *
*******************************************************************************/

void TFT_ILI9341_Init(void)
{   
  // ����, ���� DMA �������� ��������
  while(IS_SPI_DMA_BUSY) {}
  
  // ���������� �����
  TFT_HardReset();
  
  // ����������� ������������
  TFT_ILI9341_SendCommand(0x01);
  // ���� 1 ���
  HAL_Delay(100);
  
  // ���������� Chip Select 
  TFT_CS_IDLE();
  HAL_Delay(1);
  TFT_CS_ACTIVE();
  
  // Power Control A  
  TFT_ILI9341_SendCommand(0xCB);
  TFT_ILI9341_SendData(0x39);
  TFT_ILI9341_SendData(0x2C);
  TFT_ILI9341_SendData(0x00);
  TFT_ILI9341_SendData(0x34); // Vcore = 1.6 V
  TFT_ILI9341_SendData(0x02); // VddVDH = 5.6 V

  // Power Control B
  TFT_ILI9341_SendCommand(0xCF);
  TFT_ILI9341_SendData(0x00);
  TFT_ILI9341_SendData(0x81); // VGH and VGL voltage level
  TFT_ILI9341_SendData(0x30); // ESD protection

  // Driver timing control A
  TFT_ILI9341_SendCommand(0xE8);
  TFT_ILI9341_SendData(0x84); // gate driver non-overlap timing control
  TFT_ILI9341_SendData(0x11); // EQ timing control
  TFT_ILI9341_SendData(0x7A); // pre-charge timing control

  // Driver timing control B
  TFT_ILI9341_SendCommand(0xEA);
  TFT_ILI9341_SendData(0x66); // gate driver timing control
  TFT_ILI9341_SendData(0x00);

  // Power on Sequence control
  TFT_ILI9341_SendCommand(0xED);
  TFT_ILI9341_SendData(0x55); // soft start control
  TFT_ILI9341_SendData(0x01); // power on sequence control
  TFT_ILI9341_SendData(0x23); // power on sequence control
  TFT_ILI9341_SendData(0x01); // DDVDH enhance mode
  
  // Enable 3G (���� �� ���� ��� ��� �� �����)
  TFT_ILI9341_SendCommand(0xF2);
  TFT_ILI9341_SendData(0x02); // �� ��������

  // Pump ratio control
  TFT_ILI9341_SendCommand(0xF7);
  TFT_ILI9341_SendData(0x10); // DDVDH = 2xVCI

  // Power Control,VRH[5:0]
  TFT_ILI9341_SendCommand(0xC0);
  TFT_ILI9341_SendData(0x10); // GVDD = 3.65 V //0x10

  // Power Control,SAP[2:0];BT[3:0]
  TFT_ILI9341_SendCommand(0xC1);
  TFT_ILI9341_SendData(0x10); 

  // VCOM Control 1
  TFT_ILI9341_SendCommand(0xC5);
  TFT_ILI9341_SendData(0x31); // VCOMH = 3.45 V
  TFT_ILI9341_SendData(0x3C); // VCOML = -1.5 V

  // VCOM Control 2
  TFT_ILI9341_SendCommand(0xC7);
  TFT_ILI9341_SendData(0xC0); // VCOMH offset = VMH-58; VCOML offset = VML-58

  // Memory Acsess Control
  TFT_ILI9341_SendCommand(0x36);
  TFT_ILI9341_SendData(TFT9341_ROTATION); // Column Address Order; BGR // 0x48

  // Pixel Format Set
  TFT_ILI9341_SendCommand(0x3A);
  TFT_ILI9341_SendData(0x55); // 16 bits / pixel

  // Frame Rratio Control, Standard RGB Color
  TFT_ILI9341_SendCommand(0xB1);
  TFT_ILI9341_SendData(0x00); // Division Ratio = fosc
  TFT_ILI9341_SendData(0x1B); // Frame Rate = 79 Hz

  // Display Function Control
  TFT_ILI9341_SendCommand(0xB6);
  TFT_ILI9341_SendData(0x08); // Interval scan mode in non-display area
  TFT_ILI9341_SendData(0x82); // Normally white LC type; Scan cycle = 5 frames
  TFT_ILI9341_SendData(0x27); // 320 LCD drive lines
  
  // Gamma set
  TFT_ILI9341_SendCommand(0x26);
  TFT_ILI9341_SendData(0x01); // Gamma Curve (G2.2)
  
  // Display Inversion ON
  // ������� 2.4 ������-�� ����������� �����
  TFT_ILI9341_SendCommand(0x21);
  
  //TFT_ILI9341_SendCommand(0x39); //��������� �����
  
  // ������ �� ������� ������
  TFT_ILI9341_SendCommand(0x11);
  // ���� 120 ��
  HAL_Delay(120);
  
  // �������� �������
  TFT_ILI9341_SendCommand(0x29); 
  
  TFT_CS_IDLE();
}

/*******************************************************************************
  *
  * ��������� ������ ����
  *
  * x0, y0 - ���������� ������ �������� ����
  * x1, y1 - ���������� ������� ������� ����
  *
*******************************************************************************/

void TFT_ILI9341_SetAddrWindow(uint16_t x0, 
                               uint16_t y0, 
                               uint16_t x1, 
                               uint16_t y1)
{
  // ����, ���� DMA �������� ��������
  while(IS_SPI_DMA_BUSY) {}
  
  TFT_CS_ACTIVE();
  
  // ��������� ��������� �� �����������
  TFT_ILI9341_SendCommand(0x2A); // CASET
  {
    uint8_t data[] = { (x0 >> 8) & 0xFF, x0 & 0xFF, (x1 >> 8) & 0xFF, x1 & 0xFF };
    TFT_WriteData(data, sizeof(data));
  }
 
  // ��������� ��������� �� ���������
  TFT_ILI9341_SendCommand(0x2B); // RASET
  {
    uint8_t data[] = { (y0 >> 8) & 0xFF, y0 & 0xFF, (y1 >> 8) & 0xFF, y1 & 0xFF };
    TFT_WriteData(data, sizeof(data));
  }
 
  // ���������� � ������
  TFT_ILI9341_SendCommand(0x2C); // RAMWR
  
  TFT_CS_IDLE();
}

/*******************************************************************************
  *
  * ��������� ������� �������
  *
  * ������� ������������� ������� ������� ������� �������� ������������ 
  * ��������� � ������� CCR1 ������� ��������� ������� TIM14_CH1
  *
  * ����������� ������������  - 0   ���
  * ������������ ������������ - 100 ���
  *
*******************************************************************************/

void TFT_LED_Set_Brightness (uint16_t Brightness) // ������� ��������� ������� � ���������
{
  if (Brightness <= 100)
  TIM14->CCR1 = 360*Brightness;
}