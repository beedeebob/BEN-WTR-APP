/**
  ********************************************************************************************************************
  * @file           : USBInterface.c
  * @author         : Dec 5, 2024
  * @date           : ben
  *
  ********************************************************************************************************************
  */

/* Includes --------------------------------------------------------------------------------------------------------*/
#include "USBInterface.h"
#include "usbd_cdc_if.h"
#include "bQueue.h"
#include "bPacket.h"
#include "stddef.h"
#include "stdbool.h"
#include "appInfo.h"
#include "string.h"
#include "utils.h"
#include "espCore.h"
#include "esp.h"
#include "usbPacketIDs.h"

/* Private typedef -------------------------------------------------------------------------------------------------*/
/* Private define --------------------------------------------------------------------------------------------------*/
#define USBINT_RXBUFFERSIZE								512
#define USBINT_TXBUFFERSIZE								512

/* Private macro ---------------------------------------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------------------------------*/
extern ESP_td esp;

static uint8_t rxBuff[USBINT_RXBUFFERSIZE];
static QUEUE_Typedef rxQueue = {rxBuff, USBINT_RXBUFFERSIZE, 0, 0};
static uint8_t txBuff[USBINT_TXBUFFERSIZE];
static QUEUE_Typedef txQueue = {txBuff, USBINT_TXBUFFERSIZE, 0, 0};

static uint8_t *pendingRXBuffer = NULL;
static uint32_t pendingRXLength;

/* Private function prototypes -------------------------------------------------------------------------------------*/
static bool USBINT_PacketHandler(BPKT_Packet_TD *packet);
USBD_StatusTypeDef USB_ReceiveHandler(uint8_t *data, uint32_t length);

/* Private functions -----------------------------------------------------------------------------------------------*/

/**
  * @brief	None
  * @param	None
  * @retval	None
  */
void USBINT_milli(void)
{
	//Parse Received
	while(1)
	{
		BPKT_Packet_TD rxPacket = {0};
		BPKT_STATUS_ENUM result = PKT_Decode(&rxQueue, &rxPacket);
		if(result == BPKT_OK)
		{
			if(USBINT_PacketHandler(&rxPacket) == true)
			{
				QUEUE_Remove(&rxQueue, BPKT_PACKETSIZE(rxPacket.length));
				continue;
			}
			break;
		}
		else if (result != BPKT_NOTENOUGHDATA)
		{
			QUEUE_Remove(&rxQueue, 1);
			continue;
		}
		break;
	}

	//Restart USB
	if(pendingRXLength > 0)
	{
		uint32_t length = QUEUE_SPACE(&rxQueue);
		if(length > pendingRXLength)
			length = pendingRXLength;

		if(USB_ReceiveHandler(pendingRXBuffer, length) == USBD_OK)
		{
			pendingRXBuffer += length;
			pendingRXLength -= length;
		}

		if(pendingRXLength == 0)
			CDC_ReceiveRestart();
	}

	//Transmit
	if(QUEUE_COUNT(&txQueue) > 0)
	{
		uint32_t length  = QUEUE_COUNT(&txQueue);
		if(length > (txQueue.size - txQueue.out))
			length = txQueue.size - txQueue.out;
		USBD_StatusTypeDef result = CDC_Transmit_FS(&txQueue.pBuff[txQueue.out], length);
		if(result == USBD_OK)
			QUEUE_Remove(&txQueue, length);
	}
}

/*------------------------------------------------------------------------------------------------------------------*/
/**
  * @brief  USB Received handler
  * @param  data: Pointer to the received data
  * @param  length: Amount of data received
  * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
USBD_StatusTypeDef USB_ReceiveHandler(uint8_t *data, uint32_t length)
{
	if(QUEUE_SPACE(&rxQueue) < length)
	{
		pendingRXBuffer = data;
		pendingRXLength = length;
		return USBD_BUSY;
	}

	QUEUE_AddArray(&rxQueue, data, length);
	return USBD_OK;
}

/*------------------------------------------------------------------------------------------------------------------*/
/**
  * @brief  Received packet handler
  * @param  packet: pointer to the received packet
  * @retval True - Handled; False - Not Handled
  */
static uint32_t lostESPPackets = 0;
static bool USBINT_PacketHandler(BPKT_Packet_TD *packet)
{
	switch(packet->data[0])
	{
/*	case BPKT_ID_INFOREQ:
	{
		APP_ResetUSBTimeout();
		uint8_t *toSend = packet->data;	//Cuz why not
		uint16_t idx = 0;
		toSend[idx++] = BPKT_ID_INFORET;
		toSend[idx++] = 1;	//Version

		toSend[idx++] = (uint8_t)(bootInfo.appCode);
		toSend[idx++] = (uint8_t)(bootInfo.appCode >> 8);
		toSend[idx++] = bootInfo.vMinor;
		toSend[idx++] = bootInfo.vMajor;
		memcpy(&toSend[idx], bootInfo.buildDate, sizeof(bootInfo.buildDate));
		idx += sizeof(bootInfo.buildDate);
		memcpy(&toSend[idx], bootInfo.buildTime, sizeof(bootInfo.buildTime));
		idx += sizeof(bootInfo.buildTime);

		toSend[idx++] = (uint8_t)(0xffff);
		toSend[idx++] = (uint8_t)(0xffff >> 8);
		toSend[idx++] = 0xff;
		toSend[idx++] = 0xff;
		memset(&toSend[idx], 0xff, sizeof(bootInfo.buildDate));
		idx += sizeof(bootInfo.buildDate);
		memset(&toSend[idx], 0xff, sizeof(bootInfo.buildTime));
		idx += sizeof(bootInfo.buildTime);

		memcpy(&toSend[idx], (uint8_t*)(UID_BASE), 12);
		idx += 12;
		USBINT_TransmitPacket(toSend, idx);
		break;
	}*/
	case pktESPLockToUSB:
	{
		isESPReservedForUSB = (packet->data[1] & 0x01) ? true : false;

		uint8_t *toSend = packet->data;	//Cuz why not
		uint16_t idx = 0;
		toSend[idx++] = pktACK;
		toSend[idx++] = pktESPLockToUSB;	//Packet
		USBINT_TransmitPacket(toSend, idx);
		break;
	}
	case pktESPModeReq:
	{
		uint8_t *toSend = packet->data;	//Cuz why not
		uint16_t idx = 0;
		toSend[idx++] = pktESPModeRet;
		toSend[idx++] = (uint8_t)ESP_GetMode(&esp);	//Packet
		USBINT_TransmitPacket(toSend, idx);
		break;
	}
	case pktESPModeSet:
	{
		ESP_SetMode(&esp, packet->data[1]);

		uint8_t *toSend = packet->data;	//Cuz why not
		uint16_t idx = 0;
		toSend[idx++] = pktACK;
		toSend[idx++] = pktESPModeSet;
		USBINT_TransmitPacket(toSend, idx);
		break;
	}
	case pktESPTransmitData:
	{
		if(ESP_Transmit(&esp, &packet->data[1], packet->length - 1) != ESP_OK)
			lostESPPackets++;

		uint8_t *toSend = packet->data;	//Cuz why not
		uint16_t idx = 0;
		toSend[idx++] = pktACK;
		toSend[idx++] = pktESPModeSet;
		USBINT_TransmitPacket(toSend, idx);
		break;
	}
	}
	return true;
}

/*------------------------------------------------------------------------------------------------------------------*/
/**
  * @brief  Transmit a packet
  * @param  data: pointer to the data to wrap and transmit
  * @param  length: amount of data to wrap and transmit
  * @retval True - Transmitted; False - Not Transmitted
  */
bool USBINT_TransmitPacket(uint8_t *data, uint32_t length)
{
	if(PKT_Encode(data, length, &txQueue) == BPKT_OK)
		return true;
	return false;
}

/* -----------------------------------------------------------------------------------------------------------------*/
/**
 * @brief Weak call to esp USART received handler
 * @param esp: pointer to the esp system
 * @param data: pointer to the data to transmit
 * @param length: amount of data to transmit
 */
bool ESP_PGM_RxHandler(ESP_td *esp, uint8_t *data, uint32_t length)
{
	return USBINT_TransmitPacket(data, length);
}
