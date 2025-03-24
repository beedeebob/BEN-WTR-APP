/**
  ******************************************************************************
  * @file     	esp.c
  * @author		ben
  * @version	1V0
  * @date		Mar 5, 2024
  * @brief
  */


/* Includes ------------------------------------------------------------------*/
#include "esp.h"
#include "stddef.h"
#include "bQueue.h"
#include "string.h"

/* Private define ------------------------------------------------------------*/
typedef enum
{
	ESP_MODESTATE_UNKOWN = 0,
	ESP_MODESTATE_AWAITRESET,
	ESP_MODESTATE_RESET,
	ESP_MODESTATE_AWAITBOOTFLASH,
	ESP_MODESTATE_AWAITBOOTENABLE,
	ESP_MODESTATE_BOOT,
	ESP_MODESTATE_AWAITAPP,
	ESP_MODESTATE_APP
}ESP_ModeStateEnum;

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void ESP_commsTick(ESP_td *esp);
static void ESP_ModeTick(ESP_td *esp);
uint32_t ESP_USARTTransmit(ESP_td *esp, QUEUE_Typedef *queue);
uint32_t ESP_USARTReceive(ESP_td *esp, QUEUE_Typedef *queue);
void ESP_GPIORESET(ESP_td *esp, uint8_t pinState);
void ESP_GPIOEN(ESP_td *esp, uint8_t pinState);
void ESP_GPIO0(ESP_td *esp, uint8_t pinState);
void ESP_GPIO2(ESP_td *esp, uint8_t pinState);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief	ESP initialize
  * @param	esp: pointer to the esp control system
  */
void ESP_Initialize(ESP_td *esp)
{
	memset(esp, 0, sizeof(ESP_td));

	esp->comms.toEspQ.pBuff = esp->comms.toEspBuffer;
	esp->comms.toEspQ.size =  ESP_BUFFERSIZE;
	esp->comms.toEspQ.in =  0;
	esp->comms.toEspQ.out =  0;

	esp->comms.fromEspQ.pBuff = esp->comms.fromEspBuffer;
	esp->comms.fromEspQ.size = ESP_BUFFERSIZE;
	esp->comms.fromEspQ.in = 0;
	esp->comms.fromEspQ.out = 0;
}

/*----------------------------------------------------------------------------*/
/**
  * @brief	ESP control millisecond tick
  * @param	esp: pointer to the esp control system
  * @retval	None
  */
void ESP_tick(ESP_td *esp)
{
	ESP_ModeTick(esp);
	ESP_commsTick(esp);
}

/*----------------------------------------------------------------------------*/
/**
  * @brief	Set the boot mode
  * @param	esp: pointer to the esp control system
  * @param	mode: desired boot mode of the ESP
  */
void ESP_SetMode(ESP_td *esp, ESP_BootModeEnum mode)
{
	esp->bootMode = mode;
}

/*----------------------------------------------------------------------------*/
/**
  * @brief	Get the boot mode
  * @param	esp: pointer to the esp control system
  * @return	Current boot mode or ESP_BOOTMODE_Unknown if undefined
 */
ESP_BootModeEnum ESP_GetMode(ESP_td *esp)
{
	if(esp->bootState == ESP_MODESTATE_RESET)
		return ESP_BOOTMODE_Disabled;
	else if (esp->bootState == ESP_MODESTATE_BOOT)
		return ESP_BOOTMODE_Boot;
	else if (esp->bootState == ESP_MODESTATE_APP)
		return ESP_BOOTMODE_Application;
	else
		return ESP_BOOTMODE_Unknown;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Tick routine for the mode control
 */
static void ESP_ModeTick(ESP_td *esp)
{
	if(esp->tmrBoot)
		esp->tmrBoot--;

	switch(esp->bootState)
	{
	case ESP_MODESTATE_UNKOWN:
		esp->tmrBoot = 5;
		ESP_GPIORESET(esp, 1);
		ESP_GPIOEN(esp, 0);
		ESP_GPIO0(esp, 0);	//FLASH
		esp->bootState = ESP_MODESTATE_AWAITRESET;
		break;

	case ESP_MODESTATE_AWAITRESET:
		if(esp->tmrBoot > 0)
			break;
		esp->bootState = ESP_MODESTATE_RESET;
	case ESP_MODESTATE_RESET:
		if(esp->bootMode == ESP_BOOTMODE_Boot)
		{
			esp->tmrBoot = 5;
			ESP_GPIO0(esp, 1);	//FLASH
			esp->bootState = ESP_MODESTATE_AWAITBOOTFLASH;
		}
		else if (esp->bootMode == ESP_BOOTMODE_Application)
		{
			esp->tmrBoot = 5;
			ESP_GPIOEN(esp, 1);
			esp->bootState = ESP_MODESTATE_AWAITAPP;
		}
		break;

	case ESP_MODESTATE_AWAITBOOTFLASH:
		if(esp->tmrBoot > 0)
			break;
		esp->tmrBoot = 5;
		ESP_GPIOEN(esp, 1);
		ESP_GPIORESET(esp, 0);
		esp->bootState = ESP_MODESTATE_AWAITBOOTENABLE;
	case ESP_MODESTATE_AWAITBOOTENABLE:
		if(esp->tmrBoot > 0)
			break;
		esp->bootState = ESP_MODESTATE_BOOT;
	case ESP_MODESTATE_BOOT:
		if((esp->bootMode == ESP_BOOTMODE_Disabled) || (esp->bootMode == ESP_BOOTMODE_Application))
		{
			esp->tmrBoot = 5;
			ESP_GPIOEN(esp, 0);
			ESP_GPIORESET(esp, 1);
			ESP_GPIO0(esp, 0);	//FLASH
			esp->bootState = ESP_MODESTATE_AWAITRESET;
		}
		break;

	case ESP_MODESTATE_AWAITAPP:
		if(esp->tmrBoot > 0)
			break;
		esp->bootState = ESP_MODESTATE_APP;
	case ESP_MODESTATE_APP:
		if((esp->bootMode == ESP_BOOTMODE_Disabled) || (esp->bootMode == ESP_BOOTMODE_Boot))
		{
			esp->tmrBoot = 5;
			ESP_GPIOEN(esp, 0);
			ESP_GPIORESET(esp, 1);
			ESP_GPIO0(esp, 0);	//FLASH
			esp->bootState = ESP_MODESTATE_AWAITRESET;
		}
		break;
	}
}

/*----------------------------------------------------------------------------*/
/**
 * @brief	Get amount of space in the transmit buffer
  * @param	esp: pointer to the esp control system
 * @return	Amount of space
 */
inline uint32_t ESP_GetTransmitSpace(ESP_td *esp)
{
	return QUEUE_SPACE(&esp->comms.toEspQ);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief	Add an amount of data to the transmit queue
  * @param	esp: pointer to the esp control system
  * @param	data: pointer to the data to be transmitted
  * @param	length: pointer to the amount of data to be transmitted
 * @return	ESP_Result
 */
inline ESP_Result ESP_Transmit(ESP_td *esp, uint8_t *data, uint32_t length)
{
	return ((QUEUE_AddArray(&esp->comms.toEspQ, data, length) == QUEUE_OK) ? ESP_OK : ESP_NOSPACE);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief	Get amount of data in the receive buffer
  * @param	esp: pointer to the esp control system
 * @return	Amount of data
 */
inline uint32_t ESP_GetReceivedCount(ESP_td *esp)
{
	return QUEUE_COUNT(&esp->comms.fromEspQ);
}

/*----------------------------------------------------------------------------*/
/**
 * @brief	Retrieve an amount of data from the transmit queue
  * @param	esp: pointer to the esp control system
  * @param[out]	data: pointer to the data to be transmitted
  * @param	length: pointer to the amount of data to be received
 * @return	ESP_Result
 */
inline ESP_Result ESP_Receive(ESP_td *esp, uint8_t *data, uint32_t length)
{
	return ((QUEUE_ReadOutArray(&esp->comms.fromEspQ, data, length) == QUEUE_OK) ? ESP_OK : ESP_NOSPACE);
}

/* ---------------------------------------------------------------------------*/
/**
  * @brief	Task tick routine
  * @param	commsStruct: pointer to structure
  * @retval	None
  */
static void ESP_commsTick(ESP_td *esp)
{
	ESP_Communications_td *commsStruct = &esp->comms;

	//Parse received data for packets
	while(QUEUE_SPACE(&commsStruct->fromEspQ) > 0)
	{
		uint32_t receivedCount = ESP_USARTReceive(esp, &commsStruct->fromEspQ);
		if(receivedCount == 0)
			break;
	}

	//Transfer out to USART
	if(QUEUE_COUNT(&commsStruct->toEspQ) > 0)
	{
		uint32_t toSend = commsStruct->toEspQ.size - commsStruct->toEspQ.out;
		if(toSend > QUEUE_COUNT(&commsStruct->toEspQ))
			toSend = QUEUE_COUNT(&commsStruct->toEspQ);

		uint32_t sent = ESP_USARTTransmit(esp, &commsStruct->toEspQ);
		QUEUE_Remove(&commsStruct->toEspQ, sent);
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief	Weak call to transmit data
 * @param 	esp: pointer to the ESP struct
 * @param 	queue: pointer to the queue from which to transmit
 * @return	Amount of data transmitted
 */
__attribute__ ((weak)) uint32_t ESP_USARTTransmit(ESP_td *esp, QUEUE_Typedef *queue)
{
	return 0;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief	Weak call to receive data
 * @param 	esp: pointer to the ESP struct
 * @param 	queue: pointer to the queue into which to receive
 * @return	Amount of data received
 */
__attribute__ ((weak)) uint32_t ESP_USARTReceive(ESP_td *esp, QUEUE_Typedef *queue)
{
	return 0;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief	Weak call to GPIO change
 * @param 	esp: pointer to the ESP struct
 * @param 	pinState: desired state of ESP reset pin
 */
__attribute__ ((weak)) void ESP_GPIORESET(ESP_td *esp, uint8_t pinState)
{

}

/* ---------------------------------------------------------------------------*/
/**
 * @brief	Weak call to GPIO change
 * @param 	esp: pointer to the ESP struct
 * @param 	pinState: desired state of ESP enable
 */
__attribute__ ((weak)) void ESP_GPIOEN(ESP_td *esp, uint8_t pinState)
{

}

/* ---------------------------------------------------------------------------*/
/**
 * @brief	Weak call to GPIO change
 * @param 	esp: pointer to the ESP struct
 * @param 	pinState: desired state of ESP GPIO0
 */
__attribute__ ((weak)) void ESP_GPIO0(ESP_td *esp, uint8_t pinState)
{

}

/* ---------------------------------------------------------------------------*/
/**
 * @brief	Weak call to GPIO change
 * @param 	esp: pointer to the ESP struct
 * @param 	pinState: desired state of ESP GPIO2
 */
__attribute__ ((weak)) void ESP_GPIO2(ESP_td *esp, uint8_t pinState)
{

}
