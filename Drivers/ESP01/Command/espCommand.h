/**
  ******************************************************************************
  * @file           : espCommand.h
  * @author         : ben
  * @date           : Mar 21, 2025
  ******************************************************************************
  */
#ifndef ESP01_COMMAND_ESPCOMMAND_H_
#define ESP01_COMMAND_ESPCOMMAND_H_

/* Includes ------------------------------------------------------------------*/
#include "espCore.h"
#include "stdbool.h"
#include "espDefs.h"
#include "espUsartEncoding.h"

/* Exported types ------------------------------------------------------------*/
typedef struct ESP_CMDInterface_td
{
	struct ESP_CMDInterface_td *next;
	void (*packetReceived)(void *espCmd, ESPPKT_RxPacket_TD *packet);
}ESP_CMDInterface_td;

typedef struct
{
	ESP_td *esp;				//Pointer to ESP system on which to communicate
	bool isRunning:1;			//True when command system is running
	uint8_t rxBuff[ESP_CMD_BUFFERSIZE];	//Buffer used in the queue
	QUEUE_Typedef rx;			//Receive queue from which the packets are parsed
	ESP_CMDInterface_td *connectedProtocolChain;
}ESP_CMD_td;


/* Exported variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
ESP_Result ESP_CMD_Start(ESP_CMD_td *espCmd, ESP_td *esp);
void ESP_CMD_Stop(ESP_CMD_td *espCmd);
bool ESP_CMD_IsRunning(ESP_CMD_td *espCmd);

void ESP_CMD_Tick(ESP_CMD_td *espCmd);

ESP_Result ESP_CMD_SubscribeToPackets(ESP_CMD_td *espCmd, ESP_CMDInterface_td *espCmdInterface);
ESP_Result ESP_CMD_UnsubscribeFromPackets(ESP_CMD_td *espCmd, ESP_CMDInterface_td *espCmdInterface);

ESP_Result ESP_CMD_SendCommand(ESP_CMD_td *espCmd, uint16_t command, uint8_t *parameters, uint16_t parameterSize);

#endif /* ESP01_COMMAND_ESPCOMMAND_H_ */
