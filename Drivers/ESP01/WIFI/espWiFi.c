/**
  ********************************************************************************************************************
  * @file           : espWiFi.c
  * @author         : Mar 17, 2025
  * @date           : ben
  *
  ********************************************************************************************************************
  */

/* Includes --------------------------------------------------------------------------------------------------------*/
#include "espCore.h"
#include "stdbool.h"

/* Private includes ------------------------------------------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------------------------------------------*/
typedef struct
{

}ESP_WIFI_td;

/* Private define --------------------------------------------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------------------------------------------*/
/* Private functions -----------------------------------------------------------------------------------------------*/

/**
  * @brief	None
  * @param	None
  * @retval	None
  */
//ESP_Result ESPWIFI_Init(ESP_WIFI_td *wifi, ESP_CMD_td *espCmd, ESP_td *esp)
//{
//
//}

/*------------------------------------------------------------------------------------------------------------------*/
/**
  * @brief	None
  * @param	None
  * @retval	None
  */

//ESP_Result ESPWIFI_ConnectToAP(ESP_WIFI_td *wifi, char *ap, char *password)
//{
//	return ESP_OK;
//}
//
//bool ESPWIFI_IsConnected(ESP_WIFI_td *wifi)
//{
//	return false;
//}

/*
 * Perhaps "init" should link wifi struct to core and cmd system
 * "Connect to ap" will then connect and subscribe to callbacks and possibly start CMD interface if not already started
 * Wait for connection state to become connected using the struct
 */
