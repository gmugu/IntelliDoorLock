#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x_conf.h"

void FlashReadID(u8 *Data);


void SPI_Flash_Init(void);
u8 SPI_FLASH_ReadByte(void);
u8 SPI_FLASH_SendByte(u8 byte);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_SectorErase(u32 SectorAddr);
void SPI_FLASH_BulkErase(void);

void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);

void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);

void SPI_FLASH_WaitForWriteEnd(void);
void SPI_FLASH_StartReadSequence(u32 ReadAddr);


#endif 

