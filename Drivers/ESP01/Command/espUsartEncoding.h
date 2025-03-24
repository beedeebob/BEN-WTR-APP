/**
  ******************************************************************************
  * @file     	benPacket.c
  * @author		beede
  * @version	1V0
  * @date		Feb 26, 2024
  * @brief
  */

#ifndef BEN_PACKET_H
#define BEN_PACKET_H

/* Includes ------------------------------------------------------------------*/
#include "espCore.h"
#include "espDefs.h"


/* Exported defines ----------------------------------------------------------*/
#define ESPPKT_OVERHEAD           (1 /*STX*/ + 2/*Length*/ + 4/*HCRC*/ + 4/*DCRC*/ + 1/*ETX*/)
#define ESPPKT_PACKETSIZE(N)      (ESPPKT_OVERHEAD + N)

/* Exported types ------------------------------------------------------------*/
typedef enum
{
	ESPPKT_OK = 0,
    ESPPKT_NOTENOUGHDATA = -1,
    ESPPKT_STX = -2,
    ESPPKT_LENGTH = -3,
    ESPPKT_HCRC = -4,
    ESPPKT_ETX = -5,
    ESPPKT_DCRC = -6,
    ESPPKT_NOTENOUGHSPACE = -7,
    ESPPKT_EXCEEDSMAXSIZE = -8
}ESPPKT_DECODEEnum;

typedef struct
{
    uint8_t data[ESPPKT_MAXDATALENGTH];
    uint16_t length;
}ESPPKT_RxPacket_TD;

/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
ESPPKT_DECODEEnum ESPPKT_Decode(QUEUE_Typedef *queue, ESPPKT_RxPacket_TD *packet);
ESP_Result ESPPKT_Encode(uint8_t *data, uint16_t length, QUEUE_Typedef *queue);
ESP_Result ESPPKT_EncodeStart(QUEUE_Typedef *queue, uint16_t totallength, uint8_t *partData, uint16_t partlength, uint32_t *crc);
ESP_Result ESPPKT_EncodeStartArray(uint8_t **data, uint16_t totallength, uint8_t *partData, uint16_t partlength, uint32_t *crc);
ESP_Result ESPPKT_EncodePart(QUEUE_Typedef *queue, uint8_t *partData, uint16_t partlength, uint32_t *crc);
ESP_Result ESPPKT_EncodePartArray(uint8_t **data, uint8_t *partData, uint16_t partlength, uint32_t *crc);
ESP_Result ESPPKT_EncodeEnd(QUEUE_Typedef *queue, uint8_t *data, uint16_t length, uint32_t *crc);
ESP_Result ESPPKT_EncodeEndArray(uint8_t **data, uint8_t *partData, uint16_t length, uint32_t *crc);

#endif /* BEN_PACKET_H */
