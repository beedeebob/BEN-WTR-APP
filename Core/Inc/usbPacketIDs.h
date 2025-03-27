/**
  ******************************************************************************
  * @file     	usbPacketIDs.h
  * @author		beede
  * @version	1V0
  * @date		Jul 20, 2024
  * @brief
  */


#ifndef INC_USBPACKETIDS_H_
#define INC_USBPACKETIDS_H_

/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
enum
{
    pktACK = 0,
    pktNACK = 1,

    pktDeviceInfo = 2,

	pktFileHeaderRequest = 12,
	pktFileHeader = 13,

	pktUSRDataRequest = 14,
	pktUSRData = 15,
	pktUSRClose = 16,
	pktUSRAlive = 17,

	pktESPLockToUSB = 20,
	/*
	 * Command to lock ESP to USB control
	 * Byte			Bit			Value			Description
	 * 0						20				pktESPLockToUSB
	 * 1			0x01		0x01			Lock
	 * 							0x00			Unlock
	 *
	 * RETURNS
	 * pktACK
	 * 0						0				pktACK
	 * 1						20				pktESPLockToUSB
	 */
	pktESPModeReq = 21,
	/*
	 * Request for ESP mode
	 * Byte			Bit			Value			Description
	 * 0						21				pktESPModeReq
	 *
	 * RETURNS
	 * pktESPModeRet
	 */
	pktESPModeRet = 22,
	/*
	 * Return of ESP mode
	 * Byte			Bit			Value			Description
	 * 0						22				pktESPModeRet
	 * 1										@ref ESP_BootModeEnum
	 */
	pktESPModeSet = 25,
	/*
	 * Return of ESP mode
	 * Byte			Bit			Value			Description
	 * 0						25				pktESPModeSet
	 * 1										@ref ESP_BootModeEnum
	 *
	 * RETURNS
	 * pktACK
	 * 0						0				pktACK
	 * 1						20				pktESPModeSet
	 */
	pktESPTransmitData = 23,
	/*
	 * Transmit data to ESP
	 * Byte			Bit			Value			Description
	 * 0						23				pktESPTransmitData
	 * 1 - N									Data
	 */
	pktESPReceiveData = 24,
	/*
	 * Receive data from ESP
	 * Byte			Bit			Value			Description
	 * 0						23				pktESPTransmitData
	 * 1 - N									Data
	 */
};

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* INC_USBPACKETIDS_H_ */
