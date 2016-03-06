#ifndef _WEBSOCKERSERVERSHORTFS_H_
#define _WEBSOCKERSERVERSHORTFS_H_

/*
  https://github.com/nodemcu/nodemcu-devkit
  TOUT       ADC_0 -+- D_0  GPIO_16 USER  WAKE
                NC -+- D_1  GPIO_5                  GPS_RX
                NC -+- D_2  GPIO_4                  GPS_TX
                NC -+- D_3  GPIO_0  FLASH           BNO055_SDA
  I_5V          NC -+- D_4  GPIO_2  TX_D1           BNO055_SCL
  I_3V3         NC -+- 3V3
  I_GND         NC -+- GND
  GPIO_PWM     GND -+- D_5  GPIO_14       H_SPI_CLK
  GPIO_W_PWM   3V3 -+- D_6  GPIO_12       H_SPI_Q
  UART         GND -+- D_7  GPIO_13 RX_D2 H_SPI_D   
  H_SPI        3V3 -+- D_8  GPIO_15 TX_D2 H_SPI_S   
  KEY           EN -+- D_9  GPIO_3  RX_D0
  SYSTEM       RST -+- D_10 GPIO_1  RX_D0
  ADC          GND -+- GND
  NOT_CONNECT   5V -+- 3V3
*/


/* IMPORTS - ARDUINO CORE */
#include <Arduino.h>
#include <Hash.h>
//https://github.com/esp8266/arduino-esp8266fs-plugin/releases
#include <FS.h>

/* IMPORTS */
extern "C" {
#include "user_interface.h"
}

/* IMPORTS - LOCAL */
#include "dummy_serial.h"

/* IMPORTS - STANDARD C++ */
//https://github.com/maniacbug/StandardCplusplus
//#include <StandardCplusplus.h>
#include <string>
#include <vector>
#include <iterator>

//using namespace std;


/* IMPORTS - ESP8266 CORE */
//http://esp8266.github.io/Arduino/versions/2.0.0/doc/reference.html
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <ESP8266SSDP.h>
#include <WiFiClient.h>

#define QUOTE(...) #__VA_ARGS__




/* VARIABLES - DEBUG */
#define DBG_SET_DEBUG
#define DBG_BAUDRATE           115200

#ifdef  DBG_SET_DEBUG
#define DBG_SERIAL             Serial
#else
#define DBG_SERIAL      dummySerial()
#endif

#define DEBUG_ESP_PORT     DBG_SERIAL



#define _WEBSOCKETSERVERSHORTFS_VERSION_ "0.6"



/* VARIABLES */
#include "personal.h"


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

/* VARIABLES SSDP */
#define SSDP_NAME             "ESP8266 FS Web Socket Server"
#define SSDP_URL              "index.html"
#define SSDP_MODEL_NAME       "FS GPS BNO055"
#define SSDP_MODEL_NUMBER     _WEBSOCKETSERVERSHORTFS_VERSION_
#define SSDP_MODEL_URL        "https://github.com/sauloalrpi/WebSocketServerShortFS"
#define SSDP_MANUFACTURER     "Espressif"
#define SSDP_MANUFACTURER_URL "http://espressif.com/en/products/esp8266/"
#define SSDP_TTL              1


/* VARIABLES - GLOBAL - TOOLS */
#define JSON_BUFFER_SIZE       1536


/* VARIABLES - SERVERS */
/* VARIABLES - SERVERS - webserver */
#define WEBSERVER_PORT           80

/* VARIABLES - SERVERS - webscocket */
#define WEBSOCKET_PORT           81
#ifdef  DBG_SET_DEBUG
#define DEBUG_WS                 true
#else
#define DEBUG_WS                false
#endif


/* VARIABLES - INFO */
#define __ESP8266_VERSION__                           "2.1.0"
#define __BNO055_VERSION__                            "1.1.1"
#define __ARDUINOJSON_VERSION__                       "5.1.1"
#define __WEBSOCKER_VERSION__                    "2015_05_20"
#define __STANDARDCPP_VERSION__          "2013_09_22_af18241"
#define WIFI_HOST                                 "esp8266fs"



/* VARIABLES EXTERNAL SENSORS */
/* VARIABLES EXTERNAL SENSORS - BNO055 */
#define USE_BNO055
#define BNO055_KEEP_UPDATED        true
#define BNO055_UPDATE_ON_WEBSOCKET true
#define BNO055_POOL_EVERY           100
#define BNO055_SDA_PORT               0
#define BNO055_SCL_PORT               2
#define BNO055_INIT_ID               55

/* VARIABLES EXTERNAL SENSORS - GPS */
#define USE_GPS
#define GPS_KEEP_UPDATED         true
#define GPS_UPDATE_ON_WEBSOCKET false
#define GPS_POOL_EVERY           1100
#define GPS_READ_FOR              900
#define GPS_RX_PORT                 5
#define GPS_TX_PORT                 4
#define GPS_BAUDRATE            38400
#define GPS_START_FIELD      "$GPRMC"
#define GPS_END_FIELD        "$GPGLL"



#include "tools.h"





#endif //_WEBSOCKERSERVERSHORTFS_H_
