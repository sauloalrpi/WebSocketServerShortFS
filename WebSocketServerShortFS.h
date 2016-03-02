#ifndef _WEBSOCKERSERVERSHORTFS_H_
#define _WEBSOCKERSERVERSHORTFS_H_

/* IMPORTS */
extern "C" {
#include "user_interface.h"
}

/* IMPORTS - STANDARD C++ */
//https://github.com/maniacbug/StandardCplusplus
//#include <StandardCplusplus.h>
#include <string>
#include <vector>
#include <iterator>

//using namespace std;

/* IMPORTS - ARDUINO CORE */
#include <Arduino.h>
#include <Hash.h>
//https://github.com/esp8266/arduino-esp8266fs-plugin/releases
#include <FS.h>


/* IMPORTS - ESP8266 CORE */
//http://esp8266.github.io/Arduino/versions/2.0.0/doc/reference.html
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <ESP8266SSDP.h>
#include <WiFiClient.h>



/* IMPORTS - LOCAL */
#include "dummy_serial.h"


#define QUOTE(...) #__VA_ARGS__



/* VARIABLES */
#include "personal.h"
#define _WEBSOCKETSERVERSHORTFS_VERSION_ "0.5"

/* VARIABLES - WIFI */
#ifndef WIFI_SSID
#define WIFI_SSID       "change-me-ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS       "change-me-pass"
#endif
#ifndef WIFI_HOST
#define WIFI_HOST            "esp8266fs"
#endif


/* VARIABLES - GLOBAL */
#define JSON_BUFFER_SIZE       1536


/* VARIABLES - SERVERS */
#define WEBSERVER_PORT           80
#define WEBSOCKET_PORT           81


/* VARIABLES - DEBUG */
#define DBG_SET_DEBUG
#define DBG_BAUDRATE         115200


/* VARIABLES SSDP */
#define SSDP_NAME             "ESP8266 FS Web Socket Server"
#define SSDP_URL              "index.html"
#define SSDP_MODEL_NAME       "FS GPS"
#define SSDP_MODEL_NUMBER     _WEBSOCKETSERVERSHORTFS_VERSION_
#define SSDP_MODEL_URL        "https://github.com/sauloalrpi/WebSocketServerShortFS"
#define SSDP_MANUFACTURER     "Espressif"
#define SSDP_MANUFACTURER_URL "http://espressif.com/en/products/esp8266/"
#define SSDP_TTL              2


/* VARIABLES EXTERNAL SENSORS */
#define USE_GPS
#define USE_BNO055




#include "tools.h"


#ifdef  DBG_SET_DEBUG
#define DBG_SERIAL             Serial
#else
#define DBG_SERIAL      dummySerial()
#endif


/* DEFINITIONS */
ESP8266WiFiMulti WiFiMulti;


#endif //_WEBSOCKERSERVERSHORTFS_H_
