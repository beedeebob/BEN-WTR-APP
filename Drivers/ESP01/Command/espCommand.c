/**
  ********************************************************************************************************************
  * @file           : espCommand.c
  * @author         : Mar 21, 2025
  * @date           : ben
  *
  ********************************************************************************************************************
  */

/* Includes --------------------------------------------------------------------------------------------------------*/
#include "espCommand.h"
#include "bQueue.h"
#include "stddef.h"
#include "assert.h"
#include "esp.h"
#include "utils.h"

/* Private includes ------------------------------------------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------------------------------------------*/
/* Private define --------------------------------------------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------------------------------------------*/
static void ESP_CMD_PacketHandle(ESP_CMD_td *espCmd, ESPPKT_RxPacket_TD *packet);

/* Private functions -----------------------------------------------------------------------------------------------*/

/**
 * @brief Start the command system.
 * @param espCmd: pointer to the command system struct
 * @param esp: pointer to the esp system on which the commands run
 * @return ESP_Result
 */

ESP_Result ESP_CMD_Start(ESP_CMD_td *espCmd, ESP_td *esp)
{
	assert(espCmd != NULL);
	assert(esp != NULL);

	if(espCmd->isRunning)
		return ESP_OK;

	QUEUE_Initialize(&espCmd->rx, espCmd->rxBuff, sizeof(espCmd->rxBuff));
	espCmd->isRunning = true;
	espCmd->esp = esp;
	return ESP_OK;
}

/*------------------------------------------------------------------------------------------------------------------*/
/**
 * @brief Stop the command system
 * @param espCmd: pointer to the command system struct
 */
void ESP_CMD_Stop(ESP_CMD_td *espCmd)
{
	assert(espCmd != NULL);

	espCmd->isRunning = false;
}

/*------------------------------------------------------------------------------------------------------------------*/
/**
 * @brief Get whether the command system is running
 * @param espCmd: pointer to the command system struct
 * @return Is command system running
 */
bool ESP_CMD_IsRunning(ESP_CMD_td *espCmd)
{
	assert(espCmd != NULL);

	return espCmd->isRunning;
}

/*------------------------------------------------------------------------------------------------------------------*/
/**
 * @brief Tick control routine to run system
 * @param espCmd: pointer to the command system struct
 */
void ESP_CMD_Tick(ESP_CMD_td *espCmd)
{
	assert(espCmd != NULL);

	//Check if running
	if(!espCmd->isRunning)
		return;

	//Suck all the data from esp system
	assert(espCmd->esp != NULL);
	while((ESP_GetReceivedCount(espCmd->esp) > 0) && (QUEUE_SPACE(&espCmd->rx) > 0))
	{
		uint32_t lngth = utlMin(espCmd->rx.size - espCmd->rx.in, QUEUE_SPACE(&espCmd->rx));
		lngth = utlMin(lngth, ESP_GetReceivedCount(espCmd->esp));
		ESP_Receive(espCmd->esp, &espCmd->rx.pBuff[espCmd->rx.in], lngth);
		espCmd->rx.in = QUEUE_PTRLOOP(&espCmd->rx, (espCmd->rx.in + lngth));
	}

	//Parse received data for packets
	ESPPKT_RxPacket_TD receivedPacket = {0};
	while(QUEUE_COUNT(&espCmd->rx) > 0)
	{
		ESPPKT_DECODEEnum result = ESPPKT_Decode(&espCmd->rx, &receivedPacket);
		if(result == ESPPKT_OK)
		{
			ESP_CMD_PacketHandle(espCmd, &receivedPacket);
			QUEUE_Remove(&espCmd->rx, ESPPKT_PACKETSIZE(receivedPacket.length));
		}
		else if(result == ESPPKT_NOTENOUGHDATA)
			break;
		else
			QUEUE_Remove(&espCmd->rx, 1);
	}
}

/*------------------------------------------------------------------------------------------------------------------*/
/**
 * @brief Handle the packet
 * @param espCmd: pointer to the command system struct
 * @param packet: pointer to the packet
 */
static void ESP_CMD_PacketHandle(ESP_CMD_td *espCmd, ESPPKT_RxPacket_TD *packet)
{
	assert(espCmd != NULL);
	assert(packet != NULL);

	ESP_CMDInterface_td *espCmdInterface = espCmd->connectedProtocolChain;
	while(espCmdInterface != NULL)
	{
		espCmdInterface->packetReceived(espCmd, packet);
		espCmdInterface = espCmdInterface->next;
	}
}

/*------------------------------------------------------------------------------------------------------------------*/
/**
 * @brief Subscribe to receive packets from the command system
 * @param espCmd: pointer to the command system struct
 * @param espCmdInterface: pointer to the subscription system
 * @return ESP_Result
 */
ESP_Result ESP_CMD_SubscribeToPackets(ESP_CMD_td *espCmd, ESP_CMDInterface_td *espCmdInterface)
{
	assert(espCmd != NULL);
	assert(espCmdInterface != NULL);

	ESP_CMDInterface_td *srchEnd = espCmd->connectedProtocolChain;
	while((srchEnd != NULL) && (srchEnd->next != NULL) && (srchEnd != espCmdInterface))
		srchEnd = srchEnd->next;

	if(srchEnd == espCmdInterface)	//Already in list
		return ESP_OK;

	espCmdInterface->next = NULL;
	if(srchEnd == NULL)
		espCmd->connectedProtocolChain = espCmdInterface;
	else
		srchEnd->next = espCmdInterface;
	return ESP_OK;
}

/*------------------------------------------------------------------------------------------------------------------*/
/**
 * @brief Unsubscribe from receiving command packets
 * @param espCmd: pointer to the command system struct
 * @param espCmdInterface: pointer to the subscription system
 * @return ESP_Result
 */
ESP_Result ESP_CMD_UnsubscribeFromPackets(ESP_CMD_td *espCmd, ESP_CMDInterface_td *espCmdInterface)
{
	assert(espCmd != NULL);

	ESP_CMDInterface_td *srch = espCmd->connectedProtocolChain;
	ESP_CMDInterface_td *previous = espCmd->connectedProtocolChain;

	while((srch != NULL) && (srch != espCmdInterface))
	{
		previous = srch;
		srch = srch->next;
	}

	if(srch == NULL)	//Not in list
		return ESP_OK;

	if(previous == NULL)
		espCmd->connectedProtocolChain = espCmdInterface->next;
	else
		previous->next = espCmdInterface->next;

	espCmdInterface->next = NULL;
	return ESP_OK;
}

/*------------------------------------------------------------------------------------------------------------------*/
/**
 * @brief Send a command to the esp
 * @param espCmd: pointer to the command system struct
 * @param command
 * @param parameters
 * @param parameterSize: size of the parameter list
 * @return ESP_Result
 */
ESP_Result ESP_CMD_SendCommand(ESP_CMD_td *espCmd, uint16_t command, uint8_t *parameters, uint16_t parameterSize)
{
	assert(espCmd != NULL);
	assert(!((parameters == NULL) && (parameterSize != 0)));

	if(!espCmd->isRunning)
		return ESP_NOTRUNNING;

	if(ESP_GetTransmitSpace(espCmd->esp) < ESPPKT_PACKETSIZE(parameterSize + 1))
		return ESP_NOSPACE;

	//Encode command
	uint8_t txData[ESPPKT_PACKETSIZE(ESPPKT_MAXDATALENGTH)];
	uint8_t *pTxData = txData;
	uint8_t data[2];
	data[0] = (uint8_t)command;
	uint32_t crc = 0;
	ESP_Result result = ESPPKT_EncodeStartArray(&pTxData, 1 + parameterSize, data, 1, &crc);
	if(result != ESP_OK)
		return result;
	result = ESPPKT_EncodeEndArray(&pTxData, parameters, parameterSize, &crc);
	if(result != ESP_OK)
		return result;

	//Send command
	return ESP_Transmit(espCmd->esp, txData, (pTxData - txData));
}
