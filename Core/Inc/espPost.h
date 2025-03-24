/**
  ******************************************************************************
  * @file     	espPost.h
  * @author		beede
  * @version	1V0
  * @date		Sep 2, 2024
  * @brief
  */


#ifndef SRC_ESPPOST_H_
#define SRC_ESPPOST_H_

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdbool.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void WTRPST_tick(void);
bool WTRPST_PostWeatherUpdate(uint32_t pressure, int32_t temperature, uint32_t humidity);
bool WTRPST_GetHasPosted(void);

#endif /* SRC_ESPPOST_H_ */
