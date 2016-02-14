#ifndef _HANDLER_WEBSOCKET_H_
#define _HANDLER_WEBSOCKET_H_

static void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght);
static void broadcast_json(JsonObject& json);
static void send_info_json();

static void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
    switch(type) {
        case WStype_DISCONNECTED: {
#ifdef DEBUG_WS
            DBG_SERIAL.printf("[%u] Disconnected!\n", num);
            delayy(10);
            has_client = false;
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

            has_client = true;
            
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


static void broadcast_json(JsonObject& json) {
  int len = json.measureLength();
  
#ifdef DEBUG_STEPS
  DBG_SERIAL.print(   F("measureLength: ") );
  DBG_SERIAL.println( len );
#endif

  char temp[len];
  json.printTo(temp, len); 
   
#ifdef DEBUG_STEPS
  DBG_SERIAL.print(   F("BROADCAST JSON: ") );
  DBG_SERIAL.print(   len );
  DBG_SERIAL.print(   " " );
  DBG_SERIAL.println( temp );
  DBG_SERIAL.flush();
#endif    

  
  webSocket.broadcastTXT( temp, len );
  //delayy(100);

#ifdef DEBUG_STEPS
  DBG_SERIAL.println(   F("SENT") );
  DBG_SERIAL.flush();
#endif
}


static void send_info_json() {  
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


#endif //ifndef _HANDLER_WEBSOCKET_H_
