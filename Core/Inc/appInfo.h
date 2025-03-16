/**
  ******************************************************************************
  * @file           : appInfo.h
  * @author         : ben
  * @date           : Dec 6, 2024
  ******************************************************************************
  */
#ifndef APPINFO_H_
#define APPINFO_H_

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdbool.h"
#include "main.h"

/* Exported defines ----------------------------------------------------------*/
#define BOOTSTARTADDRESS					FLASH_BASE
#define BOOTLENGTH							0x8000
#define APPSTARTADDRESS						(BOOTSTARTADDRESS + BOOTLENGTH)
#define APPLENGTH							(0x20000 - (APPSTARTADDRESS - FLASH_BASE))

#define APPCODE_BEN_WTR_APP 				0x0004

/* Exported types ------------------------------------------------------------*/
typedef struct
{
	uint8_t search[8];
	uint8_t blockVersion;
	uint16_t appCode;
	uint8_t vMajor;
	uint8_t vMinor;
	char buildDate[11];	//__DATE__
	char buildTime[8]; //__TIME__
	char progDate[11];
	char progTime[8];
}APPINFO_BootTable;
typedef struct
{
	uint8_t search[8];
	uint8_t blockVersion;
	uint16_t appCode;
	uint8_t vMajor;
	uint8_t vMinor;
	char buildDate[11];	//__DATE__
	char buildTime[8]; //__TIME__
	char progDate[11];
	char progTime[8];
	uint32_t length;	//Application length including CRC checksum
}APPINFO_AppTable;

/* Exported variables --------------------------------------------------------*/
extern APPINFO_BootTable APPINFO_bootInfo;
extern const APPINFO_AppTable APPINFO_appInfo;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
bool APPINFO_ReadBootTable(APPINFO_BootTable *table);
bool APPINFO_ReadAppTable(APPINFO_AppTable *table);

#endif /* APPINFO_H_ */
