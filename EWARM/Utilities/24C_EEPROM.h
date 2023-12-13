
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define EEPROM_ADDRESS (0x50 << 1)

#define LICENSE_PAGE 0
#define SYSTEM_PAGE  1
#define FLOAT_PAGE_1 2
#define FLOAT_PAGE_2 3
#define ENUM_PAGE_1  5
#define ENUM_PAGE_2  6

#define MODE_FLAG_ADDR  (SYSTEM_PAGE * sEEPROM.PageSize)
#define SCREW_SENS_ADDR (SYSTEM_PAGE * sEEPROM.PageSize + 1)
#define HWS_SENS_ADDR   (SYSTEM_PAGE * sEEPROM.PageSize + 2)
#define ANTI_STOP_ADDR  (SYSTEM_PAGE * sEEPROM.PageSize + 3)

extern const char DeviceNameSTART[2][13];

typedef struct
{
  uint16_t	PageSize;
  uint32_t	PageCount;
  uint32_t	CapacityInKiloByte;	
  uint8_t	Lock;
} s24C32xx;

extern s24C32xx sEEPROM;

void EEPROM_WriteByte (uint8_t *pByte,     // байт для записи
                       uint16_t ByteAddr); // адрес байта
void EEPROM_WritePage (uint8_t *pBuffer,        // указатель на буфер
                       uint8_t PageNumber,      // номер страницы
                       uint8_t NumByteToWrite); // количество байтов для записи (размер вплоть до страницы)
void EEPROM_ReadByte (uint8_t *pByte,      // байт для записи
                      uint16_t ByteAddr); // адрес байта
void EEPROM_ReadPage (uint8_t *pBuffer,        // указатель на буфер
                      uint8_t PageNumber,      // номер страницы
                      uint8_t NumByteToWrite); // количество байтов для записи (размер вплоть до страницы)
void EEPROM_ErasePage (uint8_t PageNumber); // Номер страницы
void EEPROM_EraseFull (void);
void EEPROM_EraseBuffer (void);
void EEPROM_CheckLicense (void);
void EEPROM_WriteLicence (void);
void EEPROM_WriteSettings (void);
void EEPROM_PageFormation(uint8_t PageNum);
void EEPROM_ReadSettings(void);
void EEPROM_PageExtraction(uint8_t PageNum);