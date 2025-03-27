/**
  ********************************************************************************************************************
  * @file           : espThroughHelper.c
  * @author         : Mar 25, 2025
  * @date           : ben
  *
  ********************************************************************************************************************
  */

/* Includes --------------------------------------------------------------------------------------------------------*/
#include "espCore.h"
#include "esp.h"
#include "stdbool.h"
#include "stdint.h"
#include "utils.h"

/* Private includes ------------------------------------------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------------------------------------------*/
/* Private define --------------------------------------------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------------------------------------------*/
bool ESP_PGM_RxHandler(ESP_td *esp, uint8_t *data, uint32_t length);

/* Private functions -----------------------------------------------------------------------------------------------*/


/**
 * @brief Handle pass through of data
 * @param esp: pointer to esp system to use
 */
void ESP_PGM_milli(ESP_td *esp)
{
	static uint8_t tmr = 0;
	if(tmr)
		tmr--;

	//Retry system
	static uint8_t data[150];
	static uint8_t dataLength = 0;
	if(dataLength > 0)
	{
		if(ESP_PGM_RxHandler(esp, data, dataLength))
		{
			tmr = 10;
			dataLength = 0;
		}
		else
			return;	//Prevent data from being overwritten
	}

	//Periodic Receive
	uint32_t available = ESP_GetReceivedCount(esp);
	if((tmr == 0) || (available > 100))
	{
		uint8_t length = utlMin(available, sizeof(data));
		if(ESP_Receive(esp, data, length) == ESP_OK)
		{
			dataLength = length;
			if(ESP_PGM_RxHandler(esp, data, dataLength))
			{
				tmr = 10;
				dataLength = 0;
			}
		}
	}
}

/* -----------------------------------------------------------------------------------------------------------------*/
/**
 * @brief Weak call to esp USART received handler
 * @param esp: pointer to the esp system
 * @param data: pointer to the data to transmit
 * @param length: amount of data to transmit
 * @return Transmitted
 */
__attribute__ ((weak)) bool ESP_PGM_RxHandler(ESP_td *esp, uint8_t *data, uint32_t length)
{
	return true;
}
