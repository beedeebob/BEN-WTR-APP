/**
  ******************************************************************************
  * @file     	espDefs.h
  * @author		beede
  * @version	1V0
  * @date		Sep 2, 2024
  * @brief
  */


#ifndef ESP01_ESPDEFS_H_
#define ESP01_ESPDEFS_H_

/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
#define ESPPKT_MAXDATALENGTH      		100
#if (ESPCOMMS_BUFFERSIZE >= (ESPPKT_MAXDATALENGTH + 12))
#error ESPCOMMS_BUFFERSIZE should be large enough to hold ESPPKT_MAXDATALENGTH + 12
#endif

#define ESP_BUFFERSIZE					256				//Must be a power of 2
#if(ESP_BUFFERSIZE & (ESP_BUFFERSIZE - 1))
#error ESP_BUFFERSIZE must be a power of 2
#endif

#define ESP_CMD_BUFFERSIZE				256				//Must be a power of 2
#if(ESP_CMD_BUFFERSIZE & (ESP_CMD_BUFFERSIZE - 1))
#error ESP_CMD_BUFFERSIZE must be a power of 2
#endif

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* ESP01_ESPDEFS_H_ */
