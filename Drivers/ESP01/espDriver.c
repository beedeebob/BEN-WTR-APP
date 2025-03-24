/**
  ******************************************************************************
  * @file     	espDriver.c
  * @author		ben
  * @version	1V0
  * @date		Aug 31, 2024
  * @brief		ST interface driver for the esp01 control
  */


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "esp.h"
#include "espDriver.h"
#include "espPost.h"
#include "stdbool.h"
#include "bQueue.h"
#include "string.h"
#include "espUsartEncoding.h"
#include "assert.h"
#include "utils.h"

/* Private define ------------------------------------------------------------*/
#define ESPDRV_QUEUESIZE							256

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
	USART_TypeDef *usart;
	DMA_TypeDef *dma;
	uint32_t txDMAChan;
	uint32_t rxDMAChan;
	uint32_t lastRxDMAIndex;
	uint8_t txQBuff[ESPDRV_QUEUESIZE];
	uint8_t rxQBuff[ESPDRV_QUEUESIZE];
}ESP_Driver_td;

/* Private variables ---------------------------------------------------------*/
extern ESP_td esp;

static ESP_Driver_td espDriver;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief	Initialize the esp driver
  * @param	data: pointer to the data to transmit
  * @param	size: amount of data to transmit
  * @retval	Number transmitted
  */
void ESPDRV_Init(void)
{
	//Driver initialize
	memset(&espDriver, 0, sizeof(ESP_Driver_td));
	espDriver.usart = USART2;
	espDriver.dma = DMA1;
	espDriver.txDMAChan = LL_DMA_CHANNEL_7;
	espDriver.rxDMAChan = LL_DMA_CHANNEL_6;

	//USART RX DMA initialize
	LL_DMA_SetMemoryAddress(espDriver.dma, espDriver.rxDMAChan, (uint32_t)espDriver.rxQBuff);
	LL_DMA_SetPeriphAddress(espDriver.dma, espDriver.rxDMAChan, (uint32_t)&espDriver.usart->RDR);
	LL_DMA_SetDataLength(espDriver.dma, espDriver.rxDMAChan, sizeof(espDriver.rxQBuff));
	LL_USART_EnableDMAReq_RX(espDriver.usart);
	LL_DMA_EnableChannel(espDriver.dma, espDriver.rxDMAChan);
	espDriver.lastRxDMAIndex = (sizeof(espDriver.rxQBuff) - LL_DMA_GetDataLength(espDriver.dma, espDriver.rxDMAChan));

	//USART TX DMA initialize
	LL_DMA_SetMemoryAddress(espDriver.dma, espDriver.txDMAChan, (uint32_t)espDriver.txQBuff);
	LL_DMA_SetPeriphAddress(espDriver.dma, espDriver.txDMAChan, (uint32_t)&espDriver.usart->TDR);
	LL_USART_EnableDMAReq_TX(espDriver.usart);

	LL_USART_Enable(espDriver.usart);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief	Weak call to receive data
 * @param 	esp: pointer to the ESP struct
 * @param 	queue: pointer to the queue into which to receive
 * @return	Amount of data received
 */
uint32_t ESP_USARTReceive(ESP_td *esp, QUEUE_Typedef *queue)
{
	//Handle the USART RX DMA
	uint32_t newDMAIndex = (sizeof(espDriver.rxQBuff) - LL_DMA_GetDataLength(espDriver.dma, espDriver.rxDMAChan));
	uint32_t count = ((newDMAIndex - espDriver.lastRxDMAIndex) & (sizeof(espDriver.rxQBuff) - 1));

	uint32_t offset = 0;
	while(offset < count)
	{
		uint32_t contiguousDataSize = sizeof(espDriver.rxQBuff) - espDriver.lastRxDMAIndex;
		if(contiguousDataSize > (count - offset))
			contiguousDataSize = (count - offset);

		QUEUE_AddArray(queue, &espDriver.rxQBuff[espDriver.lastRxDMAIndex], contiguousDataSize);
		offset += contiguousDataSize;
		espDriver.lastRxDMAIndex = (espDriver.lastRxDMAIndex + contiguousDataSize) & (sizeof(espDriver.rxQBuff) - 1);
	}
	return offset;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief	Call to transmit data
 * @param 	esp: pointer to the ESP struct
 * @param 	queue: pointer to the queue from which to transmit
 * @return	Amount of data transmitted
 */
uint32_t ESP_USARTTransmit(ESP_td *esp, QUEUE_Typedef *queue)
{
	//Check if transmit busy
	if((LL_DMA_GetDataLength(espDriver.dma, espDriver.txDMAChan) > 0) || !LL_USART_IsActiveFlag_TC(espDriver.usart))
		return 0;

	//Calculate length to send
	uint32_t sizeToSend = utlMin(sizeof(espDriver.txQBuff), QUEUE_COUNT(queue));
	if(sizeToSend == 0)
		return 0;

	//Prepare the data
	QUEUE_ReadToArray(queue, 0, espDriver.txQBuff, sizeToSend);

	//Configure the USART
	LL_USART_ClearFlag_TC(espDriver.usart);

	//Cnfigure the DMA
	LL_DMA_DisableChannel(espDriver.dma, espDriver.txDMAChan);
	LL_DMA_SetDataLength(espDriver.dma, espDriver.txDMAChan, sizeToSend);
	LL_DMA_ClearFlag_TC7(espDriver.dma);
	LL_DMA_EnableChannel(espDriver.dma, espDriver.txDMAChan);

	return sizeToSend;
}

/*----------------------------------------------------------------------------*/
/**
  * @brief	Handle DMA transfer complete
  * @param	dma: pointer to the DMA
  * @retval	None
  */
void ESPDRV_DMATransferCompleteHandler(DMA_TypeDef * dma)
{
	LL_DMA_DisableIT_TC(espDriver.dma, espDriver.txDMAChan);
	LL_DMA_DisableChannel(espDriver.dma, espDriver.txDMAChan);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief	Weak call to GPIO change
 * @param 	esp: pointer to the ESP struct
 * @param 	pinState: desired state of ESP reset pin
 */
void ESP_GPIORESET(ESP_td *esp, uint8_t pinState)
{
	//Not currently connected
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief	Weak call to GPIO change
 * @param 	esp: pointer to the ESP struct
 * @param 	pinState: desired state of ESP enable
 */
void ESP_GPIOEN(ESP_td *esp, uint8_t pinState)
{
	HAL_GPIO_WritePin(GPIO_ESP_NPD_EN_GPIO_Port, GPIO_ESP_NPD_EN_Pin, pinState);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief	Weak call to GPIO change
 * @param 	esp: pointer to the ESP struct
 * @param 	pinState: desired state of ESP GPIO0
 */
void ESP_GPIO0(ESP_td *esp, uint8_t pinState)
{
	HAL_GPIO_WritePin(GPIO_ESP_FLASH_GPIO_Port, GPIO_ESP_FLASH_Pin, pinState);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief	Weak call to GPIO change
 * @param 	esp: pointer to the ESP struct
 * @param 	pinState: desired state of ESP GPIO2
 */
void ESP_GPIO2(ESP_td *esp, uint8_t pinState)
{
	//Not currently connected
}

