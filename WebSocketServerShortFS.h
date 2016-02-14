#ifndef _WEBSOCKERSERVERSHORTFS_H_
#define _WEBSOCKERSERVERSHORTFS_H_

/* IMPORTS */
extern "C" {
#include "user_interface.h"
}

/* IMPORTS - ARDUINO CORE */
#include <Arduino.h>
#include <Hash.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
//https://github.com/esp8266/arduino-esp8266fs-plugin/releases
#include <FS.h>


/* IMPORTS - ESP8266 CORE */
//http://esp8266.github.io/Arduino/versions/2.0.0/doc/reference.html
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266SSDP.h>
#include <WiFiClient.h>



/* IMPORTS - LOCAL */
//https://github.com/Links2004/arduinoWebSockets
//#define WEBSOCKETS_SERVER_CLIENT_MAX  (5)
#define DEBUG_WEBSOCKETS
#include "libs/websocket/WebSocketsServer.h"



#define QUOTE(...) #__VA_ARGS__



typedef struct self_data_s {
  String   mac;
  uint32_t chipId;
  uint32_t flashChipId;
  uint32_t flashChipSize;
  uint32_t flashChipSpeed;
  String   flashChipMode;
  uint32_t flashChipRealSize;
  uint32_t flashChipSizeByID;
  char *   SDKVersion;
  uint8_t  bootVersion;
  uint8_t  bootMode;
  uint32_t freeRAM;
  uint32_t freeHeap;
  uint32_t freeSketchSpace;
  uint32_t sketchSize;
  uint32_t systemInstructionCyclesPerSecond;
  uint8_t  cpuFreqMHz;
  String   systemUptime;
  String   lastSystemRestartInfo;
  //String   lastSystemRestartReason;
  String   hostname;
  
  
#ifdef WEBSOCKETS_SERVER_CLIENT_MAX
  uint32_t webSocketMaxClient;
#endif

#ifdef JSON_BUFFER_SIZE
  uint32_t jsonBufferSize;
#endif

#ifdef GPS_BAUDRATE
  uint32_t gpsBaudrate;
#endif

#ifdef GPS_RX_PORT
  uint32_t gpsRxPort;
#endif

#ifdef GPS_TX_PORT
  uint32_t gpsTxPort;
#endif

#ifdef GPS_READ_FOR
  uint32_t gpsReadFor;
#endif

#ifdef GPS_POOL_EVERY
  uint32_t gpsPoolEvery;
#endif

#ifdef GPS_START_FIELD
  String gpsStartField;
#endif

#ifdef GPS_END_FIELD
  String gpsEndField;
#endif

#ifdef GPS_FIELD_SEP
  String gpsRegisterFieldSep;
#endif

} self_data_s;






#endif //_WEBSOCKERSERVERSHORTFS_H_
