/**
  ******************************************************************************
  * @file     	espPost.c
  * @author		beede
  * @version	1V0
  * @date		Sep 2, 2024
  * @brief
  */
/**
  ******************************************************************************
  * @file     	esp.c
  * @author		ben
  * @version	1V0
  * @date		Mar 5, 2024
  * @brief
  */


/* Includes ------------------------------------------------------------------*/
#include "espPost.h"
#include "espCommand.h"
#include "esp.h"
#include "espPktIds.h"
#include "stdbool.h"
#include "main.h"

/* Private define ------------------------------------------------------------*/
#define WTRPST_TIMEOUT					5000

//Post State
enum
{
	WTRPST_STATE_IDLE = 0,
	WTRPST_STATE_AWAITSTABLEPOWER,
	WTRPST_STATE_AWAITWIFICONNECTACK,
	WTRPST_STATE_AWAITWIFICONNECTED,
	WTRPST_STATE_AWAITMQTTCONNECTACK,
	WTRPST_STATE_AWAITMQTTCONNECTED,
	WTRPST_STATE_AWAITMQTTPOSTACK,
	WTRPST_STATE_AWAITMQTTPOST,
	WTRPST_STATE_SHUTDOWN,

};
typedef struct
{
	bool post:1;							//Set when weather update should be posted
	bool wifiConnectACK:1;					//Set when wifi connected command has been received
	bool wifiConnected:1;					//Set when wifi has connected
	bool mqttConnectACK:1;					//Set when mqtt connection command has been received
	bool mqttConnected:1;					//Set when mqtt connected and ready
	bool weatherPostACK:1;					//Set when weather post is acknowledged
	bool weatherPost:1;						//Set when the weather has been posted
	bool posted:1;							//Set when post is complete
	bool complete:1;						//Set when post attempt has completed
}WTRPST_Flags;

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern ESP_td esp;
extern ESP_CMD_td espCmd;
static ESP_CMDInterface_td espPacketInterface;

static uint8_t state = WTRPST_STATE_IDLE;
static uint16_t tmr;
static uint8_t txBuffer[30];
static WTRPST_Flags flags;

static uint32_t wtrPressure;
static int32_t wtrTemperature;
static uint32_t wtrHumidity;

/* Private function prototypes -----------------------------------------------*/
static void WTRPST_ESPCommandHandler(void *espCmd, ESPPKT_RxPacket_TD *packet);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief	ESP control millisecond tick
  * @param	None
  * @retval	None
  */
void WTRPST_tick(void)
{
	if(isESPReservedForUSB)
	{
		state = WTRPST_STATE_IDLE;
		return;
	}

	//Timer
	if(tmr)
		tmr--;

	//State machine
	switch(state)
	{
	case WTRPST_STATE_IDLE:
		if(!flags.post)
			break;

		espPacketInterface.packetReceived = WTRPST_ESPCommandHandler;
		ESP_CMD_SubscribeToPackets(&espCmd, &espPacketInterface);

		//Hardware power up
		ESP_SetMode(&esp, ESP_BOOTMODE_Application);
		tmr = 500;
		state = WTRPST_STATE_AWAITSTABLEPOWER;
		break;

	case WTRPST_STATE_AWAITSTABLEPOWER:
		if(tmr != 0)
			break;

		if(ESP_CMD_SendCommand(&espCmd, espPkt_WifiConnect, NULL, 0) == ESP_OK)
		{
			state = WTRPST_STATE_AWAITWIFICONNECTACK;
			tmr = 100;
		}
		break;

	case WTRPST_STATE_AWAITWIFICONNECTACK:
		if(flags.wifiConnectACK)
		{
			state = WTRPST_STATE_AWAITWIFICONNECTED;
			tmr = WTRPST_TIMEOUT;
		}
		else if (tmr == 0)
		{
			state = WTRPST_STATE_AWAITSTABLEPOWER;
		}
		break;

	case WTRPST_STATE_AWAITWIFICONNECTED:
		if(flags.wifiConnected)
		{
			if(ESP_CMD_SendCommand(&espCmd, espPkt_StartMQTT, NULL, 0) == ESP_OK)
			{
				state = WTRPST_STATE_AWAITMQTTCONNECTACK;
				tmr = 100;
			}
		}
		else if (tmr == 0)
		{
			state = WTRPST_STATE_SHUTDOWN;
		}
		break;

	case WTRPST_STATE_AWAITMQTTCONNECTACK:
		if(flags.mqttConnectACK)
		{
			state = WTRPST_STATE_AWAITMQTTCONNECTED;
			tmr = WTRPST_TIMEOUT;
		}
		else if (tmr == 0)
		{
			state = WTRPST_STATE_AWAITWIFICONNECTED;
		}
		break;

	case WTRPST_STATE_AWAITMQTTCONNECTED:
		if(flags.mqttConnected)
		{
			uint8_t len = 0;
			txBuffer[len++] = (uint8_t)(wtrTemperature);
			txBuffer[len++] = (uint8_t)(wtrTemperature >> 8);
			txBuffer[len++] = (uint8_t)(wtrTemperature >> 16);
			txBuffer[len++] = (uint8_t)(wtrTemperature >> 24);
			txBuffer[len++] = (uint8_t)(wtrPressure);
			txBuffer[len++] = (uint8_t)(wtrPressure >> 8);
			txBuffer[len++] = (uint8_t)(wtrPressure >> 16);
			txBuffer[len++] = (uint8_t)(wtrPressure >> 24);
			txBuffer[len++] = (uint8_t)(wtrHumidity);
			txBuffer[len++] = (uint8_t)(wtrHumidity >> 8);
			txBuffer[len++] = (uint8_t)(wtrHumidity >> 16);
			txBuffer[len++] = (uint8_t)(wtrHumidity >> 24);
			if(ESP_CMD_SendCommand(&espCmd, espPkt_SetWeather, txBuffer, len) == ESP_OK)
			{
				state = WTRPST_STATE_AWAITMQTTPOSTACK;
				tmr = 100;
			}
		}
		else if (tmr == 0)
		{
			state = WTRPST_STATE_SHUTDOWN;
		}
		break;

	case WTRPST_STATE_AWAITMQTTPOSTACK:
		if(flags.weatherPostACK)
		{
			state = WTRPST_STATE_AWAITMQTTPOST;
			tmr = WTRPST_TIMEOUT;
		}
		else if (tmr == 0)
		{
			state = WTRPST_STATE_AWAITMQTTCONNECTED;
		}
		break;

	case WTRPST_STATE_AWAITMQTTPOST:
		if(flags.weatherPost)
		{
			flags.posted = true;
			state = WTRPST_STATE_SHUTDOWN;
		}
		else if (tmr == 0)
		{
			state = WTRPST_STATE_SHUTDOWN;
		}
		break;

	case WTRPST_STATE_SHUTDOWN:
		ESP_SetMode(&esp, ESP_BOOTMODE_Disabled);

		flags.post = false;
		flags.wifiConnectACK = false;
		flags.wifiConnected = false;
		flags.mqttConnectACK = false;
		flags.mqttConnected = false;
		flags.weatherPostACK = false;
		flags.weatherPost = false;
		flags.complete = true;

		state = WTRPST_STATE_IDLE;
		break;
	}

	//Request update
	static uint8_t tmrUpdate;
	if(tmrUpdate)
		tmrUpdate--;

	if(!tmrUpdate && (state != WTRPST_STATE_IDLE))
		tmrUpdate = (ESP_CMD_SendCommand(&espCmd, espPkt_Status, NULL, 0) == ESP_OK) ? 100 : 10;
}

/*----------------------------------------------------------------------------*/
/**
  * @brief	ESP state control system
  * @param	None
  * @retval	None
  */
uint8_t commands[8];
uint8_t commandsCount;
static void WTRPST_ESPCommandHandler(void *espCmd, ESPPKT_RxPacket_TD *packet)
{
	commands[commandsCount] = packet->data[0];
	commandsCount = (commandsCount + 1) & (sizeof(commands) - 1);
	switch(packet->data[0])
	{
	case espPkt_ACK:
		if(packet->data[1] == espPkt_WifiConnect)
			flags.wifiConnectACK = true;
		else if(packet->data[1] == espPkt_StartMQTT)
			flags.mqttConnectACK = true;
		else if (packet->data[1] == espPkt_SetWeather)
			flags.weatherPostACK = true;
		break;
	case espPkt_Status:
			flags.wifiConnected = (packet->data[1] & 0x01) ? true : false;
			flags.mqttConnected = (packet->data[1] & 0x02) ? true : false;
			flags.weatherPost = (packet->data[1] & 0x04) ? true : false;
		break;
	}
}

/*----------------------------------------------------------------------------*/
/**
  * @brief	Post weather readings
  * @param	pressure
  * @param	temperature
  * @param	humidity
  * @retval	Posting
  */
bool WTRPST_PostWeatherUpdate(uint32_t pressure, int32_t temperature, uint32_t humidity)
{
	if(flags.post)
		return false;

	wtrPressure = pressure;
	wtrTemperature = temperature;
	wtrHumidity = humidity;
	flags.posted = false;
	flags.complete = false;
	flags.post = true;
	return true;
}

/*----------------------------------------------------------------------------*/
/**
  * @brief	Get whether weather has been posted
  * @param	NOne
  * @retval	posted
  */
bool WTRPST_GetHasPosted(void)
{
	return flags.posted;
}
