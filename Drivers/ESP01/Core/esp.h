
/**
  ******************************************************************************
  * @file     	esp.h
  * @author		ben
  * @version	1V0
  * @date		Mar 5, 2024
  * @brief
  */

#ifndef ESP_H_
#define ESP_H_

/* Includes ------------------------------------------------------------------*/
#include "espCore.h"
#include "stdint.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void ESP_Initialize(ESP_td *esp);
void ESP_tick(ESP_td *esp);

void ESP_SetMode(ESP_td *esp, ESP_BootModeEnum mode);
ESP_BootModeEnum ESP_GetMode(ESP_td *esp);

uint32_t ESP_GetTransmitSpace(ESP_td *esp);
ESP_Result ESP_Transmit(ESP_td *esp, uint8_t *data, uint32_t length);

uint32_t ESP_GetReceivedCount(ESP_td *esp);
ESP_Result ESP_Receive(ESP_td *esp, uint8_t *data, uint32_t length);

#endif /* ESP_H_ */
