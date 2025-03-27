/**
  ******************************************************************************
  * @file           : USBInterface.h
  * @author         : ben
  * @date           : Dec 5, 2024
  ******************************************************************************
  */
#ifndef USBINTERFACE_H_
#define USBINTERFACE_H_

/* Includes ------------------------------------------------------------------*/
#include "stdbool.h"
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void USBINT_milli(void);
bool USBINT_TransmitPacket(uint8_t *data, uint32_t length);

#endif /* USBINTERFACE_H_ */
