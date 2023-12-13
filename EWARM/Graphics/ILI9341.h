
#include "main.h"

extern int8_t Digit_Alarm;

#define TFT_HEIGHT      240          
#define TFT_WIDTH       320
#define TFT_STR_WIDTH   240   //256
#define TFT_SIGN_WIDTH  98
#define TFT_BLOCK_WIDTH 120

#define TFT9341_MADCTL_MY  0x80
#define TFT9341_MADCTL_MX  0x40
#define TFT9341_MADCTL_MV  0x20
#define TFT9341_MADCTL_ML  0x10
#define TFT9341_MADCTL_RGB 0x00
#define TFT9341_MADCTL_BGR 0x08
#define TFT9341_MADCTL_MH  0x04
//#define TFT9341_ROTATION (TFT9341_MADCTL_MX | TFT9341_MADCTL_BGR)
#define TFT9341_ROTATION (TFT9341_MADCTL_MV | TFT9341_MADCTL_BGR)
//#define TFT9341_ROTATION (TFT9341_MADCTL_MV | TFT9341_MADCTL_MY | TFT9341_MADCTL_MX | TFT9341_MADCTL_BGR)

// Палитра цветов
#define	BLACK           0x0000
#define	BLUE            0x001F
#define DARK_BLUE       0x1111
#define	RED             0xE0E0 //0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define YELLOW_ORANGE   0xF7C0
#define ORANGE          0xFBC4
#define LIGHT_ORANGE    0xE5E5
#define WHITE           0xFFFF
#define NO_BACKGROUND   0xFFFFF

// Палитра используемых цветов
#define DEFAULT_COLOR   BLACK
#define TEXT_COLOR      WHITE
//#define BACK_COLOR      BLACK
#define ALARM_COLOR     RED
#define HEAD_COLOR      YELLOW
#define SUBHEAD_COLOR   YELLOW
extern uint32_t BACK_COLOR;

// Ориентация текста 
#define NO_ALIGN        0x00
#define R_ALIGN         0x01
#define L_ALIGN         0x02
#define C_ALIGN         0x03
#define RI_ALIGN        0x04
#define CI_ALIGN        0x05
#define RIS_ALIGN       0x06
#define S_ALIGN         0x07
#define B_ALIGN         0x08
#define MAN_ALIGN       0x09

#define R_INDENT        15

#define swap(a,b) {int16_t t=a;a=b;b=t;}

extern uint8_t *TFT_BUF; 
