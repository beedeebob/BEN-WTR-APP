/**
  ******************************************************************************
  * @file     	espPktIds.h
  * @author		ben
  * @version	1V0
  * @date		Feb 29, 2024
  */

#ifndef ESPPKTIDS_H_
#define ESPPKTIDS_H_

/* Includes ------------------------------------------------------------------*/

/* Exported defines ----------------------------------------------------------*/

//espPkt_commands
#define espPkt_SetWeather				0x0001
/*
Set the weather values
BYTE    BIT       VALUE                     DESCRIPTION
0       -         espPkt_SetWeather         Packet
1-4                                         Temperature in 100 C (little endian)
5-8                                         Pressure in Pa X 256 (little endian)
9-12                                        Humidity in % X 1024 (little endian)
*/

#define espPkt_WifiConnect               0x0002
/*
Connect to a wifi
BYTE    BIT       VALUE                     DESCRIPTION
0       -         espPkt_WifiConnect        Packet
-                                           SSID string null terminated
-                                           Password string null terminated
*/

#define espPkt_SetMQTTBrokerHost        0x0003
/*
Startup the MQTT
BYTE    BIT       VALUE                     DESCRIPTION
0       -         espPkt_SetMQTTTopic       Packet
-                                           MQTT broker host null terminating string
*/

#define espPkt_SetMQTTBrokerLogin       0x0004
/*
Startup the MQTT
BYTE    BIT       VALUE                     DESCRIPTION
0       -         espPkt_SetMQTTTopic       Packet
-                                           MQTT broker username null terminating string
-                                           MQTT broker password null terminating string
*/

#define espPkt_SetMQTTTopic			    0x0005
/*
Startup the MQTT
BYTE    BIT       VALUE                     DESCRIPTION
0       -         espPkt_SetMQTTTopic       Packet
-                                           MQTT topic null terminating string
*/

#define espPkt_StartMQTT				    0x0006
/*
Startup the MQTT
BYTE    BIT       VALUE                     DESCRIPTION
0       -         espPkt_SetMQTTTopic       Packet
*/

#define espPkt_ACK				        0x0007
/*
Startup the MQTT
BYTE    BIT       VALUE                     DESCRIPTION
0       -         espPkt_ACK                Packet
1                                           Packet being acked
*/

#define espPkt_NACK				        0x0008
/*
Startup the MQTT
BYTE    BIT       VALUE                     DESCRIPTION
0       -         espPkt_NACK               Packet
1                                           Packet being nacked
*/

#define espPkt_StartWeb				    0x0009
/*
Startup the Website
BYTE    BIT       VALUE                     DESCRIPTION
0       -         espPkt_StartWeb           Packet
*/

#define espPkt_Status						0x000A
/*
Startup the Website
BYTE    BIT       VALUE                     DESCRIPTION
0       -         espPkt_Status           	Packet
1		0x01								WIFI Ready
		0x02								MQTT Ready
		0x04								MQTT Published
*/

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

#endif /* ESPPKTIDS_H_ */
