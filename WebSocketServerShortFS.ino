/*
 WebSocketServer.ino
 Created on: 22.05.2015

 access the sample web page at http://esp8266fs.local
 edit the page by going to http://esp8266fs.local/edit
*/

/* VARIABLES */
#include "WebSocketServerShortFS.h"

static void init_system();
static void init_serial();
static void init_spiffs();
static void init_wifi();
static void init_mdns();
static void init_ssdp();

class   message;
void broadcastMessage(    String&  text );
void server_send_message( message& msg  );

#include "messenger.h"
#include "handlers/handler_websocket.h"
#include "handlers/handler_server.h"
#include "handlers/handler_info.h"
#include "handlers/handler_gps.h"
#include "handlers/handler_BNO055.h"


void broadcastMessage( String& text ) {
#ifdef _HANDLER_WEBSOCKET_H_
  webSocket.broadcastTXT( text );
#else
  DBG_SERIAL.print("broadcastMessage");
  DBG_SERIAL.println(text);
#endif
}


#ifndef _HANDLER_SERVER_H_
void server_send_message( message& msg ) {}
#else
void server_send_message( message& msg ) {
  String res;
  message_to_json( msg, res );
  server.send( 200, "application/json", res );
}
#endif




/* SETUP ... DUH */
void setup() {
  init_system();

#ifdef _HANDLER_INFO_H_
  DBG_SERIAL.println("SETUP INFO");
  init_info();
#endif

#ifdef _HANDLER_SERVER_H_
  DBG_SERIAL.println("SETUP WEBSERVER");
  init_webserver();
#endif

#ifdef _HANDLER_WEBSOCKET_H_
  DBG_SERIAL.println("SETUP WEBSOCKER");
  init_websocket();
#endif


#ifdef _HANDLER_GPS_H_
#ifdef USE_GPS
  DBG_SERIAL.println("SETUP GPS");
  init_gps();
#endif
#endif


#ifdef USE_BNO055
#ifdef _HANDLER_BNO055_H_
  DBG_SERIAL.println("SETUP GPS");
  init_BNO055();
#endif
#endif

  DBG_SERIAL.println("END SETUP");
}

/* LOOP ... DUH .. AGAIN */
void loop() {
  //DBG_SERIAL.print("out: ");
  //DBG_SERIAL.println(messages.size());
  
  for (auto m: messages) {
    m->go();
  }
}





static void init_system() {
  init_serial();

  DBG_SERIAL.println("SETUP SPIFFS");
  init_spiffs();

  DBG_SERIAL.println("SETUP WIFI");
  init_wifi();

  DBG_SERIAL.println("SETUP MDNS");
  init_mdns();

  DBG_SERIAL.println("SETUP SSDP");
  init_ssdp();
}

static void   init_serial() {
  DBG_SERIAL.begin(DBG_BAUDRATE);

  while (!DBG_SERIAL) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

#ifdef DBG_SET_DEBUG
  DBG_SERIAL.setDebugOutput(true);
#endif

  DBG_SERIAL.println();
  DBG_SERIAL.println();
  DBG_SERIAL.println();

  for(uint8_t t = 4; t > 0; t--) {
    DBG_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    DBG_SERIAL.flush();
    delay(200);
  }
}

static void   init_spiffs() {
  if (SPIFFS.begin()) {
    DBG_SERIAL.println("Successfully started SPIFFS");

    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {    
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      DBG_SERIAL.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    DBG_SERIAL.printf("\n");
  } else {
    DBG_SERIAL.println("Failed to start SPIFFS");
  }
}

static void   init_wifi() {
  DBG_SERIAL.println ( F(""              ) );
  DBG_SERIAL.print   ( F("Connecting to ") );
  DBG_SERIAL.println ( WIFI_SSID           );

  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);


//#ifdef DEBUG_WIFI
//
//  if (String(WiFi.SSID()) != String(ssid)) {
//    WiFi.begin(ssid, password);
//  }
//
//  WiFi.printDiag(DBG_SERIAL);
//#endif

  DBG_SERIAL.println(F("Connecting Wifi... "));
  
  while(WiFiMulti.run() != WL_CONNECTED) {
      DBG_SERIAL.print ( F(".") );
      delay(300);
  }

  DBG_SERIAL.println ( F("WiFi connected") );
  DBG_SERIAL.print   ( F("IP address: "  ) );
  DBG_SERIAL.println ( WiFi.localIP()      );
}

static void   init_mdns() {
  // Add service to MDNS
  DBG_SERIAL.println ( F("Starting MDNS responder") );

  if ( MDNS.begin( WIFI_HOST ) ) {
    DBG_SERIAL.print("Open http://");
    DBG_SERIAL.print(WIFI_HOST);
    DBG_SERIAL.println( F(".local/edit to see the file browser") );
    DBG_SERIAL.println ( F("MDNS responder started") );

#ifdef _HANDLER_SERVER_H_
    MDNS.addService( F("http"), F("tcp"), WEBSERVER_PORT);
#endif
#ifdef _HANDLER_WEBSOCKET_H_
    MDNS.addService( F("ws"  ), F("tcp"), WEBSOCKET_PORT);
#endif
  }
}

static void   init_ssdp() {
#ifdef _HANDLER_SERVER_H_
  server.on("/description.xml", HTTP_GET, [](){
    SSDP.schema(server.client());
  });
#endif

  DBG_SERIAL.printf("Starting SSDP...\n");
  SSDP.setSchemaURL(       "description.xml"     );
  SSDP.setHTTPPort(        WEBSERVER_PORT        );
  SSDP.setName(            SSDP_NAME             );
  SSDP.setSerialNumber(    ESP.getChipId()       );
  SSDP.setURL(             SSDP_URL              );
  SSDP.setModelName(       SSDP_MODEL_NAME       );
  SSDP.setModelNumber(     SSDP_MODEL_NUMBER     );
  SSDP.setModelURL(        SSDP_MODEL_URL        );
  SSDP.setManufacturer(    SSDP_MANUFACTURER     );
  SSDP.setManufacturerURL( SSDP_MANUFACTURER_URL );
  SSDP.setTTL(             SSDP_TTL              );
  SSDP.begin();
}

