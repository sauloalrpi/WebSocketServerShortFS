/*
 WebSocketServer.ino
 Created on: 22.05.2015
 
  FSWebServer - Example WebServer with SPIFFS backend for esp8266
  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the ESP8266WebServer library for Arduino environment.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  upload the contents of the data folder with MkSPIFFS Tool ("ESP8266 Sketch Data Upload" in Tools menu in Arduino IDE)
  or you can upload the contents of a folder if you CD in that folder and run the following command:
  for file in `ls -A1`; do curl -F "file=@$PWD/$file" esp8266fs.local/edit; done
  
  access the sample web page at http://esp8266fs.local
  edit the page by going to http://esp8266fs.local/edit
*/

/* VARIABLES */
/* VARIABLES - WIFI */
#define WIFI_SSID       "change-me-ssid"
#define WIFI_PASS       "change-me-pass"
#define WIFI_HOST            "esp8266fs"

#include "personal.h"

/* VARIABLES - GLOBAL */
#define JSON_BUFFER_SIZE       1536


/* VARIABLES - SERVERS */
#define WEBSERVER_PORT           80
#define WEBSOCKET_PORT           81


/* VARIABLES - DEBUG */
#define DBG_SET_DEBUG          true
#define DBG_SERIAL           Serial
#define DBG_BAUDRATE         115200


/* VARIABLES - GPS */
#ifdef USE_GPS

#define GPS_POOL_EVERY         1100
#define GPS_READ_FOR            900
#define GPS_RX_PORT              13
#define GPS_TX_PORT              15
//#define GPS_BAUDRATE           9600
#define GPS_BAUDRATE          38400
#define GPS_START_FIELD    "$GPRMC"
#define GPS_END_FIELD      "$GPGLL"
#define GPS_FIELD_SEP           "|"

#endif //ifdef USE_GPS


#include "WebSocketServerShortFS.h"
#include "tools.h"

#ifdef USE_GPS
SoftwareSerial mySerial1(GPS_RX_PORT, GPS_TX_PORT); // RX, TX
#define GPS_SERIAL        mySerial1
#endif


/* DEFINITIONS */
ESP8266WiFiMulti WiFiMulti;
ESP8266WebServer server          = ESP8266WebServer(WEBSERVER_PORT);
WebSocketsServer webSocket       = WebSocketsServer(WEBSOCKET_PORT);
String           gps_register    = "";
bool             busy            = false;
bool             has_client      = false;
File             fsUploadFile;
unsigned long    last_gps_update;
self_data_s      self_data = {};


#include "handler_info.h"
#include "handler_server.h"
#include "handler_gps.h"
#include "handler_websocket.h"



static void   init_serial() {
#ifdef DEBUG_STEPS
  DBG_SERIAL.begin(DBG_BAUDRATE);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
 
  DBG_SERIAL.setDebugOutput(DBG_SET_DEBUG);

  DBG_SERIAL.println();
  DBG_SERIAL.println();
  DBG_SERIAL.println();

  for(uint8_t t = 4; t > 0; t--) {
#ifdef DEBUG_STEPS
    DBG_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    DBG_SERIAL.flush();
#endif

    delay(200);
  }
#endif
}

static void   init_spiffs() {
  if (SPIFFS.begin()) {
#ifdef DEBUG_STEPS
    DBG_SERIAL.println("Successfully started SPIFFS");

    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {    
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      DBG_OUTPUT_PORT.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    DBG_OUTPUT_PORT.printf("\n");
#endif
  } else {
#ifdef DEBUG_STEPS
    DBG_SERIAL.println("Failed to start SPIFFS");
#endif
  }
}

static void   init_wifi() {
#ifdef DEBUG_STEPS
  DBG_SERIAL.println ( F(""              ) );
  DBG_SERIAL.print   ( F("Connecting to ") );
  DBG_SERIAL.println ( WIFI_SSID           );
#endif
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);

/*
#ifdef DEBUG_WIFI

  if (String(WiFi.SSID()) != String(ssid)) {
    WiFi.begin(ssid, password);
  }

  WiFi.printDiag(DBG_SERIAL);
#endif
*/

#ifdef DEBUG_STEPS
  DBG_SERIAL.println(F("Connecting Wifi... "));
#endif
  
  while(WiFiMulti.run() != WL_CONNECTED) {

#ifdef DEBUG_STEPS
      DBG_SERIAL.print ( F(".") );
#endif
      delay(300);
  }

#ifdef DEBUG_STEPS  
  DBG_SERIAL.println ( F("WiFi connected") );
  DBG_SERIAL.print   ( F("IP address: "  ) );
  DBG_SERIAL.println ( WiFi.localIP()      );
#endif
}

static void   init_gps() {
#ifdef USE_GPS
#ifdef DEBUG_STEPS
  DBG_SERIAL.println( F("Starting GPS") );
  DBG_SERIAL.print  ( F("Baudrate:"   ) );
  DBG_SERIAL.println(self_data.gpsBaudrate);
#endif

  GPS_SERIAL.begin(self_data.gpsBaudrate);
  
//  for(uint8_t t = 4; t > 0; t--) {
//      DBG_SERIAL.printf("[SETUP GPS] BOOT WAIT %d...\n", t);
//      DBG_SERIAL.flush();
//      delay(1000);
//  }

#ifdef DEBUG_STEPS
  DBG_SERIAL.print(  F("WAITING FOR GPS: ") );
#endif
  while ( ! GPS_SERIAL ) {
    yield();
    delay(100);
#ifdef DEBUG_STEPS
    DBG_SERIAL.print(  F(".") );
#endif
  }
#ifdef DEBUG_STEPS
  DBG_SERIAL.println(  F("") );
  DBG_SERIAL.println(  F("GPS CONNECTED") );
#endif

  last_gps_update = millis();
#endif
}

static void   init_websocket() {
#ifdef DEBUG_STEPS  
  DBG_SERIAL.println ( F("Registering WebSocket") );
  DBG_SERIAL.print   ( F("Port:"                ) );
  DBG_SERIAL.println ( WEBSOCKET_PORT             );
#endif


  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

static void   init_mdns() {
  // Add service to MDNS
#ifdef DEBUG_STEPS
  DBG_SERIAL.println ( F("Starting MDNS responder") );
#endif

  if ( MDNS.begin(self_data.hostname.c_str()) ) {
#ifdef DEBUG_STEPS
    DBG_OUTPUT_PORT.print("Open http://");
    DBG_OUTPUT_PORT.print(host);
    DBG_OUTPUT_PORT.println(".local/edit to see the file browser");
    DBG_SERIAL.println ( F("MDNS responder started") );
#endif

    MDNS.addService( F("http"), F("tcp"), WEBSERVER_PORT);
    MDNS.addService( F("ws"  ), F("tcp"), WEBSOCKET_PORT);
  }
}

static void   init_webserver() {
#ifdef DEBUG_STEPS
  DBG_SERIAL.println( F("Registering WebServer") );
  DBG_SERIAL.print  ( F("Port:"                ) );
  DBG_SERIAL.println( WEBSERVER_PORT             );
   // handle index
#endif


  //server.on ( "/"     , handleRoot    );
  //server.on ( "/info" , handleInfo    );

  //SERVER INIT
  //list directory
  server.on("/list"  , HTTP_GET   , handleFileList);
  //format
  server.on("/format", HTTP_GET   , [](){ server.send(200, "text/html", F("<html><body><a href=\"list?dir=/\">List</a><br/><a href=\"all\">All</a>Formatting</body></html>")); }, handleFormatting);
  //load editor
  server.on("/edit"  , HTTP_GET   , [](){
    if(!handleFileRead("/edit.htm")) server.send(404, "text/plain", "FileNotFound");
  });
  //create file
  server.on("/edit"  , HTTP_PUT   , handleFileCreate);
  //delete file
  server.on("/edit"  , HTTP_DELETE, handleFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  server.on("/edit"  , HTTP_POST  , [](){ server.send(200, "text/html", F("<html><body>Uploading</br><a href=\"list?dir=/\">List</a> | <a href=\"stats\">Stats</a></body></html>")); }, handleFileUpload);

  //called when the url is not defined here
  //use it to load content from SPIFFS
  server.onNotFound([](){
    if(!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });

  //get heap status, analog input value and all GPIO statuses in one json call
  server.on("/stats", HTTP_GET, handleInfo);

  server.on("/upload", HTTP_GET, handleUploadPage);

  server.begin();
}





/* SETUP ... DUH */
void setup() {
  getSelfStaticInfo();

  init_serial();

  init_spiffs();
  
  init_wifi();

  init_gps();

  init_websocket();

  init_mdns();

  init_webserver();
}

/* LOOP ... DUH .. AGAIN */
void loop() {
  server.handleClient();
  
  delayy(10);

  webSocket.loop();

  delayy(10);
  
#ifdef USE_GPS
    gpsLoop();

    delayy(0);
#endif
}

