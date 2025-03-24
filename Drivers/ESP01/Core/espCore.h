/**
  ******************************************************************************
  * @file     	espCore.h
  * @author		ben
  * @version	1V0
  * @date		Aug 31, 2024
  * @brief
  */


#ifndef ESP01_ESPCORE_H_
#define ESP01_ESPCORE_H_

/* Includes ------------------------------------------------------------------*/
#include "bQueue.h"
#include "stdint.h"
#include "espDefs.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef enum
{
	ESP_OK = 0,
	ESP_BUSY = 1,
	ESP_ALREADYEXISTS = 2,
	ESP_NOSPACE = 3,
	ESP_LIMITSEXCEEDED = 4,
	ESP_ALREADYINUSE = 5,
	ESP_NOTOWNER = 6,
	ESP_NOTRUNNING = 7,
}ESP_Result;

typedef struct
{
	uint8_t toEspBuffer[ESP_BUFFERSIZE];
	QUEUE_Typedef toEspQ;
	uint8_t fromEspBuffer[ESP_BUFFERSIZE];
	QUEUE_Typedef fromEspQ;
}ESP_Communications_td;

typedef enum
{
	ESP_BOOTMODE_Unknown = 0,
	ESP_BOOTMODE_Disabled,
	ESP_BOOTMODE_Boot,
	ESP_BOOTMODE_Application,
}ESP_BootModeEnum;

typedef struct ESP_td
{
	ESP_Communications_td comms;		//Communications control
	ESP_BootModeEnum bootMode;			//Boot mode
	uint8_t bootState;					//Boot state
	uint8_t tmrBoot;
}ESP_td;

/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* ESP01_ESPCORE_H_ */
