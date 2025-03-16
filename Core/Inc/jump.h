/**
  ******************************************************************************
  * @file           : jump.h
  * @author         : ben
  * @date           : Mar 16, 2025
  ******************************************************************************
  */
#ifndef JUMP_H_
#define JUMP_H_

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported definitions ------------------------------------------------------*/
enum JUMP_FLAGs
{
	JUMP_FLAG_STAYINBOOT = 0x01,
};

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void JUMP_SetAddress(uint32_t address, uint8_t flags);
void JUMP_CheckJump(uint8_t *flags);

#endif /* JUMP_H_ */
