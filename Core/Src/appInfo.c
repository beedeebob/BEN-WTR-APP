/**
  ********************************************************************************************************************
  * @file           : appInfo.c
  * @author         : Dec 6, 2024
  * @date           : ben
  *
  ********************************************************************************************************************
  */

/* Includes --------------------------------------------------------------------------------------------------------*/
#include "appInfo.h"
#include "VersionControl.h"
#include "string.h"

/* Private includes ------------------------------------------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------------------------------------------*/
/* Private define --------------------------------------------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------------------------------*/
const APPINFO_AppTable APPINFO_appInfo __attribute__((section(".app_info"))) =
{
		{0x1a, 0xbc, 0x8c, 0x7d, 0x3d, 0xd4, 0xb9, 0xe3},	//Search
		1,		//Version
		APPCODE_BEN_WTR_APP,	//Product code
		vMajor,
		vMinor,
		__DATE__,
		__TIME__,
		{"01 Jan 1979"},
		{"00:00:00"},
};
APPINFO_BootTable APPINFO_bootInfo;

/* Private function prototypes -------------------------------------------------------------------------------------*/
/* Private functions -----------------------------------------------------------------------------------------------*/


/**
  * @brief	Read the boot table
  * @param[out]	table: pointer to the returned boot info table
  * @retval	Found
  */
bool APPINFO_ReadBootTable(APPINFO_BootTable *table)
{
	//Search for app block
	uint32_t offset = 0;
	while(offset < 0x800)
	{
		uint8_t key[] = {0x1a, 0xbc, 0x8c, 0x7d, 0x3d, 0xd4, 0xb9, 0xe3};
		uint8_t *check = (uint8_t*)(BOOTSTARTADDRESS + offset);
		if(memcmp(key, check, sizeof(key)) != 0)
		{
			offset++;
			continue;
		}

		memcpy(table, check, sizeof(APPINFO_BootTable));
		return true;
	}
	return false;
}

/*------------------------------------------------------------------------------------------------------------------*/
/**
  * @brief	Read the app table
  * @param[out]	table: pointer to the returned app info table
  * @retval	Found
  */
bool APPINFO_ReadAppTable(APPINFO_AppTable *table)
{
	//Search for app block
	uint32_t offset = 0;
	while(offset < 0x800)
	{
		uint8_t key[] = {0x1a, 0xbc, 0x8c, 0x7d, 0x3d, 0xd4, 0xb9, 0xe3};
		uint8_t *check = (uint8_t*)(APPSTARTADDRESS + offset);
		if(memcmp(key, check, 8) != 0)
		{
			offset++;
			continue;
		}

		memcpy(table, check, sizeof(APPINFO_AppTable));
		return true;
	}
	return false;
}
