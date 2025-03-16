/**
  ********************************************************************************************************************
  * @file           : jump.c
  * @author         : Mar 16, 2025
  * @date           : ben
  *
  ********************************************************************************************************************
  */

/* Includes --------------------------------------------------------------------------------------------------------*/
#include "jump.h"
#include "utils.h"
#include "string.h"

/* Private includes ------------------------------------------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------------------------------------------*/
typedef struct
{
	uint32_t key;
	uint32_t antiKey;
	uint32_t address;
	uint8_t flags;
	uint8_t reserved[3];
	uint32_t crc;
}JUMP_Dir_td;

/* Private define --------------------------------------------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------------------------------*/
static uint8_t jumpSpace[sizeof(JUMP_Dir_td)] __attribute__((section(".jump_Space")));

/* Private function prototypes -------------------------------------------------------------------------------------*/
void JUMP_ToAddress(uint32_t address);

/* Private functions -----------------------------------------------------------------------------------------------*/

/**
  * @brief	Set the address
  * @param	address: flash address to which to jump
  * @retval	None
  */
void JUMP_SetAddress(uint32_t address, uint8_t flags)
{
	JUMP_Dir_td *jumpStruct = (JUMP_Dir_td*)jumpSpace;
	jumpStruct->key = 0x4b9e22ca;
	jumpStruct->antiKey = ~jumpStruct->key;
	jumpStruct->address = address;
	jumpStruct->flags = flags;
	jumpStruct->crc = crc32_calculateData(0, jumpSpace, 0, sizeof(JUMP_Dir_td) - sizeof(jumpStruct->crc));
}

/*------------------------------------------------------------------------------------------------------------------*/
/**
  * @brief	Check if jump is valid and jump
  * @param	None
  * @retval	None
  */
void JUMP_CheckJump(uint8_t *flags)
{
	//Verify jump block
	JUMP_Dir_td *jumpStruct = (JUMP_Dir_td*)jumpSpace;
	if((jumpStruct->key != 0x4b9e22ca) || (jumpStruct->antiKey != ~jumpStruct->key))
		return;
	if(jumpStruct->crc != crc32_calculateData(0, jumpSpace, 0, sizeof(JUMP_Dir_td) - sizeof(jumpStruct->crc)))
		return;

	uint32_t address = jumpStruct->address;

	//Destruct jump block
	memset(jumpSpace, 0, sizeof(JUMP_Dir_td));

	//Jump
	JUMP_ToAddress(address);
}

/*------------------------------------------------------------------------------------------------------------------*/
/**
  * @brief	Immediately jump to address
  * @param	None
  * @retval	None
  */
__attribute__ ((weak)) void JUMP_ToAddress(uint32_t address)
{

}
