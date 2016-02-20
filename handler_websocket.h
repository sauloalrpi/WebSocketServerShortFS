#ifndef _HANDLER_WEBSOCKET_H_
#define _HANDLER_WEBSOCKET_H_

//https://github.com/Links2004/arduinoWebSockets
//#define WEBSOCKETS_SERVER_CLIENT_MAX  (5)
//#define DEBUG_WEBSOCKETS

#include "libs/websocket/WebSocketsServer.h"
#include <ArduinoJson.h>

struct websocket_data_t {
  uint8_t  message_websocket_id;
  uint32_t max_clients;
  uint8_t  has_client;
};

websocket_data_t websocket_data;
message_funcs_t  message_websocket_funcs;
message          message_websocket_msg;

WebSocketsServer webSocket = WebSocketsServer(WEBSOCKET_PORT);


void   init_websocket();

void   message_websocket_tester(    message* msg );
void   message_websocket_initer(    message* msg );
void   message_websocket_updater(   message* msg );
void   message_websocket_printer(   message* msg );
void   message_websocket_publisher( message* msg );
void   message_websocket_looper(    message* msg );
void   message_websocket_to_json(   message* msg );

void   webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght);
void   broadcast_json(JsonObject& json);
//void   send_info_json();


void   message_websocket_tester(    message* msg ) {}

void   message_websocket_initer(    message* msg ) {
  DBG_SERIAL.println( F("message_websocket_init START") );

  websocket_data.max_clients = WEBSOCKETS_SERVER_CLIENT_MAX;
  websocket_data.has_client  = 0;

  DBG_SERIAL.println ( F("Registering WebSocket") );
  DBG_SERIAL.print   ( F("Port:"                ) );
  DBG_SERIAL.println ( websocket_data.max_clients );

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  DBG_SERIAL.println( F("message_websocket_init END") );
  DBG_SERIAL.flush();
}

void   message_websocket_updater(   message* msg ) {
  DBG_SERIAL.println( F("message_websocket_updater START") );
  DBG_SERIAL.println( F("message_websocket_updater END") );
  DBG_SERIAL.flush();
}

void   message_websocket_printer(   message* msg ) {
  DBG_SERIAL.println( F("message_websocket_printer START") );
  
  DBG_SERIAL.print( F("max_clients                             : ")); DBG_SERIAL.println(websocket_data.max_clients);
  DBG_SERIAL.print( F("has_client                              : ")); DBG_SERIAL.println(websocket_data.has_client );

  DBG_SERIAL.println( F("message_websocket_printer END") );
  DBG_SERIAL.flush();
}

void   message_websocket_publisher( message* msg ) {
  DBG_SERIAL.println( F("message_websocket_publisher START") );
  
  String text;
  msg->pop_message(text);
  DBG_SERIAL.print("message_websocket_publisher");
  DBG_SERIAL.println(text);

#ifdef _HANDLER_WEBSOCKET_H_
  webSocket.broadcastTXT( text );
#endif

  DBG_SERIAL.println( F("message_websocket_publisher END") );
  DBG_SERIAL.flush();
}

void   message_websocket_looper(    message* msg ) {
//  DBG_SERIAL.println( F("message_websocket_looper START") );
  webSocket.loop();
//  DBG_SERIAL.println( F("message_websocket_looper END") );
//  DBG_SERIAL.flush();
}

void   message_websocket_to_json(   message* msg ) {
  DBG_SERIAL.println( F("message_websocket_to_json START") );

  message_websocket_updater(msg);

  StaticJsonBuffer<JSON_BUFFER_SIZE> jsonBuffer;
  JsonObject& json  = jsonBuffer.createObject();
  
  json["_type"] = "websocket_info";
  json["_id"  ] = millis();

  JsonObject& j_info                             = json  .createNestedObject("data"  );

  j_info[ F("max_clients"                         )] = websocket_data.max_clients;
  j_info[ F("has_client"                          )] = websocket_data.has_client;

  String text;
  jsonToString(json, text);
  msg->set_message(text);

  DBG_SERIAL.println( F("message_websocket_to_json END") );
  DBG_SERIAL.flush();
}





void   init_websocket() {
  DBG_SERIAL.println( F("init_websocket START") );
  
  message_websocket_funcs.tester      = message_websocket_tester   ;
  message_websocket_funcs.initer      = message_websocket_initer   ;
  message_websocket_funcs.printer     = message_websocket_printer  ;
  message_websocket_funcs.publisher   = message_websocket_publisher;
  message_websocket_funcs.updater     = message_websocket_updater  ;
  message_websocket_funcs.looper      = message_websocket_looper   ;
  websocket_data.message_websocket_id = messages.size();
  message_websocket_msg               = message("Websocket Info", -1, 0, message_websocket_funcs);
  messages.push_back( &message_websocket_msg );
  
  DBG_SERIAL.println( F("init_websocket END") );
  DBG_SERIAL.flush();
}












/*
#ifdef WEBSOCKETS_SERVER_CLIENT_MAX
  DBG_SERIAL.print( F("WEBSOCKETS_SERVER_CLIENT_MAX        : ")); DBG_SERIAL.println(WEBSOCKETS_SERVER_CLIENT_MAX);
#endif
#ifdef WEBSOCKETS_NETWORK_TYPE
  DBG_SERIAL.print( F("WEBSOCKETS_NETWORK_TYPE             : ")); DBG_SERIAL.println(WEBSOCKETS_NETWORK_TYPE);
#endif
#ifdef WEBSOCKETS_USE_BIG_MEM
  DBG_SERIAL.print( F("WEBSOCKETS_USE_BIG_MEM              : ")); DBG_SERIAL.println(WEBSOCKETS_USE_BIG_MEM);
#endif

#ifdef WEBSOCKETS_SERVER_CLIENT_MAX
  self_data.webSocketMaxClient               = WEBSOCKETS_SERVER_CLIENT_MAX;
#endif

 */

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
    switch(type) {
        case WStype_DISCONNECTED: {
#ifdef DEBUG_WS
            DBG_SERIAL.printf("[%u] Disconnected!\n", num);
            delayy(10);
            websocket_data.has_client -= 1;
#endif
            break;
        }
        
        case WStype_CONNECTED: {
#ifdef DEBUG_WS
            IPAddress ip = webSocket.remoteIP(num);
            DBG_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
#endif
            // send message to client
            String status_str  = "{\"_type\":\"status\",\"data\":\"Connected\",\"_id\":";
                   status_str += millis();
                   status_str += "}";
            
            webSocket.sendTXT(num, status_str);

            websocket_data.has_client += 1;
            
            break;
        }
        
        case WStype_TEXT: {
#ifdef DEBUG_WS
            DBG_SERIAL.printf("[%u] got Text: %s\n", num, payload);
            //delayy(100);
#endif
//            if(payload[0] == '#') {
//            }
            break;
        }
    }
}


void broadcast_json(JsonObject& json) {
  DBG_SERIAL.println( F("broadcast_json") );
  
  int len = json.measureLength();
  
  DBG_SERIAL.print(   F("measureLength: ") );
  DBG_SERIAL.println( len );

  char temp[len];
  json.printTo(temp, len); 
   
  DBG_SERIAL.print(   F("BROADCAST JSON: ") );
  DBG_SERIAL.print(   len );
  DBG_SERIAL.print(   " " );
  DBG_SERIAL.println( temp );
  DBG_SERIAL.flush();

  
  webSocket.broadcastTXT( temp, len );
  //delayy(100);

  DBG_SERIAL.println(   F("SENT") );
  DBG_SERIAL.flush();
}


/*
void send_info_json() {  
  busy    = true;
  StaticJsonBuffer<JSON_BUFFER_SIZE> jsonBuffer;
  JsonObject& json  = jsonBuffer.createObject();

  if ( json.success() ) {
    infoToJson(json);
    broadcast_json(json);
  
  } else {
#ifdef DEBUG_STEPS
    DBG_SERIAL.print("FAILED TO ALLOCATE MEMORY TO JSON");
#endif
  }
  busy    = false;
}
  */


#endif //ifndef _HANDLER_WEBSOCKET_H_
