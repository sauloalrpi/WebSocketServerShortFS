#ifndef _HANDLER_INFO_H_
#define _HANDLER_INFO_H_

#include <ArduinoJson.h>

struct info_static_data_t {
  uint8_t  message_static_id;
  uint8_t  info_dynamic_id;
  String   version                = _WEBSOCKETSERVERSHORTFS_VERSION_;
  String   hostname               = WIFI_HOST;
  String   packages_versions      = String()+"ESP8266 "+__ESP8266_VERSION__+"; BNO055 "+__BNO055_VERSION__+"; WebSocket "+__WEBSOCKER_VERSION__+"; ArduinoJson "+__ARDUINOJSON_VERSION__+"; StandardCPP "+__STANDARDCPP_VERSION__;
  uint32_t jsonBufferSize         = JSON_BUFFER_SIZE;
  uint32_t chipId;
  uint32_t flashChipId;
  uint32_t flashChipSize;
  uint32_t flashChipSpeed;
  uint32_t flashChipRealSize;
  uint32_t flashChipSizeByID;
  uint32_t freeSketchSpace;
  uint32_t sketchSize;
  String   mac;
  String   flashChipMode;
  String   lastSystemRestartInfo;
  String   lastSystemRestartReason;
  char *   SDKVersion;
  uint8_t  bootVersion;
  uint8_t  bootMode;
  uint8_t  cpuFreqMHz;
};

struct info_dynamic_data_t {
  uint32_t  freeRAM;
  uint32_t  freeHeap;
  uint32_t  systemInstructionCyclesPerSecond;
  uint32_t  numMessages;
  String    systemUptime;
};

info_static_data_t  info_static_data;
info_dynamic_data_t info_dynamic_data;
message_funcs_t     message_static_info_funcs;
message_funcs_t     message_dynamic_info_funcs;
message             message_static_info_msg;
message             message_dynamic_info_msg;




void    init_info();

void    message_static_info_tester(     message* msg );
void    message_static_info_initer(     message* msg );
void    message_static_info_updater(    message* msg );
void    message_static_info_printer(    message* msg );
void    message_static_info_publisher(  message* msg );
void    message_static_info_looper(     message* msg );
void    message_static_info_to_json(    message* msg );

void    message_dynamic_info_tester(    message* msg );
void    message_dynamic_info_initer(    message* msg );
void    message_dynamic_info_updater(   message* msg );
void    message_dynamic_info_printer(   message* msg );
void    message_dynamic_info_publisher( message* msg );
void    message_dynamic_info_looper(    message* msg );
void    message_dynamic_info_to_json(   message* msg );


void    handleInfoStatic();
void    handleInfoDynamic();
//static void   updateExtraInfo();


void    message_static_info_tester(     message* msg ) {}

void    message_static_info_initer(     message* msg ) {
  DBG_SERIAL.println( F("info_static_init START") );

  message_static_info_updater(    msg );

  DBG_SERIAL.println( F("info_static_init END") );
  DBG_SERIAL.flush();
}

void    message_static_info_updater(    message* msg ) {
  DBG_SERIAL.println( F("info_static_updater START") );
  
  FlashMode_t    ideMode                            = ESP.getFlashChipMode();
  
  info_static_data.mac                              = WiFi.macAddress();
  info_static_data.chipId                           = ESP.getChipId();
  info_static_data.flashChipId                      = ESP.getFlashChipId();
  info_static_data.flashChipSize                    = ESP.getFlashChipSize();
  info_static_data.flashChipSpeed                   = ESP.getFlashChipSpeed();
  info_static_data.flashChipMode                    = (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN");
  info_static_data.flashChipRealSize                = ESP.getFlashChipRealSize()/1024;
  info_static_data.flashChipSizeByID                = ESP.getFlashChipSizeByChipId()/1024;
  info_static_data.SDKVersion                       = (char *)ESP.getSdkVersion();
  info_static_data.bootVersion                      = ESP.getBootVersion();
  info_static_data.bootMode                         = ESP.getBootMode();
  info_static_data.freeSketchSpace                  = ESP.getFreeSketchSpace()/1024;
  info_static_data.sketchSize                       = ESP.getSketchSize()/1024;
  info_static_data.cpuFreqMHz                       = ESP.getCpuFreqMHz();
  info_static_data.lastSystemRestartInfo            = ESP.getResetInfo();
  info_static_data.lastSystemRestartReason          = ESP.getResetReason();

  message_static_info_to_json( msg );
  
  DBG_SERIAL.println( F("info_static_updater END") );
  DBG_SERIAL.flush();
}

void    message_static_info_printer(    message* msg ) {
  //DBG_SERIAL.println( F("info_static_printer START") );
  
  DBG_SERIAL.print( F("Version                             : ") ); DBG_SERIAL.println( info_static_data.version                         );
  DBG_SERIAL.print( F("MAC address                         : ") ); DBG_SERIAL.println( info_static_data.mac                             );
  DBG_SERIAL.print( F("Chip Id                             : ") ); DBG_SERIAL.println( info_static_data.chipId                          );
  DBG_SERIAL.print( F("Flash Chip Id                       : ") ); DBG_SERIAL.println( info_static_data.flashChipId                     );
  DBG_SERIAL.print( F("Flash Chip Size                     : ") ); DBG_SERIAL.println( info_static_data.flashChipSize                   );
  DBG_SERIAL.print( F("Flash Chip Speed                    : ") ); DBG_SERIAL.println( info_static_data.flashChipSpeed                  );
  DBG_SERIAL.print( F("Flash Chip Mode                     : ") ); DBG_SERIAL.println( info_static_data.flashChipMode                   );
  DBG_SERIAL.print( F("Flash Chip Real Size                : ") ); DBG_SERIAL.println( info_static_data.flashChipRealSize               );
  DBG_SERIAL.print( F("Flash Chip Size By ID               : ") ); DBG_SERIAL.println( info_static_data.flashChipSizeByID               );
  DBG_SERIAL.print( F("SDK Version                         : ") ); DBG_SERIAL.println( info_static_data.SDKVersion                      );
  DBG_SERIAL.print( F("Boot Version                        : ") ); DBG_SERIAL.println( info_static_data.bootVersion                     );
  DBG_SERIAL.print( F("Boot Mode                           : ") ); DBG_SERIAL.println( info_static_data.bootMode                        );
  DBG_SERIAL.print( F("Free Sketch Space                   : ") ); DBG_SERIAL.print  ( info_static_data.freeSketchSpace                 ); DBG_SERIAL.println( F(" KBytes") );
  DBG_SERIAL.print( F("Sketch Size                         : ") ); DBG_SERIAL.print  ( info_static_data.sketchSize                      ); DBG_SERIAL.println( F(" KBytes") );
  DBG_SERIAL.print( F("CPU Speed                           : ") ); DBG_SERIAL.print  ( info_static_data.cpuFreqMHz                      ); DBG_SERIAL.println( F(" MHz"   ) );
  DBG_SERIAL.print( F("Last System Restart Info            : ") ); DBG_SERIAL.println( info_static_data.lastSystemRestartInfo           );
  DBG_SERIAL.print( F("Last System Restart Reason          : ") ); DBG_SERIAL.println( info_static_data.lastSystemRestartReason         );
  DBG_SERIAL.print( F("Hostname                            : ") ); DBG_SERIAL.println( info_static_data.hostname                        );
  DBG_SERIAL.print( F("WEBSERVER_PORT                      : ") ); DBG_SERIAL.println( WEBSERVER_PORT                                   );
  
  DBG_SERIAL.print( F("JSON Buffer Size                    : ") ); DBG_SERIAL.println( info_static_data.jsonBufferSize                  );

#ifdef ESP8266
  DBG_SERIAL.print( F("ESP8266                             : ") ); DBG_SERIAL.println(ESP8266);
#endif

#ifdef NETWORK_ESP8266
  DBG_SERIAL.print( F("NETWORK_ESP8266                     : ") ); DBG_SERIAL.println(NETWORK_ESP8266);
#endif

  // DBG_SERIAL.println( F("info_static_printer END") );
  DBG_SERIAL.flush();
}

void    message_static_info_publisher(  message* msg ) {
//  DBG_SERIAL.println( F("info_static_publisher START") );
  
  String text;
  msg->pop_message(text);

#ifdef _HANDLER_WEBSOCKET_H_
  webSocket.broadcastTXT( text );
#else
  DBG_SERIAL.print("info_static_publisher");
  DBG_SERIAL.println(text);
#endif
//  DBG_SERIAL.println( F("info_static_publisher END") );
//  DBG_SERIAL.flush();
}

void    message_static_info_looper(     message* msg ) {}

void    message_static_info_to_json(    message* msg ) {
  DBG_SERIAL.println( F("info_static_to_json START") );

  jsonBuffer_t jsonBuffer;
  JsonObject& json  = jsonBuffer.createObject();

  //message_static_info_updater(msg);
  
  json["_type"]     = "info/static";
  json["_id"  ]     = millis();

  JsonObject& j_info                              = json.createNestedObject("data");

  j_info[ F("version"                         ) ] = info_static_data.version;
  j_info[ F("mac"                             ) ] = info_static_data.mac;
  j_info[ F("packagesVersions"                ) ] = info_static_data.packages_versions;
  j_info[ F("chipId"                          ) ] = info_static_data.chipId;
  j_info[ F("flashChipId"                     ) ] = info_static_data.flashChipId;
  j_info[ F("flashChipSize"                   ) ] = info_static_data.flashChipSize;
  j_info[ F("flashChipSpeed"                  ) ] = info_static_data.flashChipSpeed;
  j_info[ F("flashChipMode"                   ) ] = info_static_data.flashChipMode;
  j_info[ F("flashChipRealSize"               ) ] = info_static_data.flashChipRealSize;
  j_info[ F("flashChipSizeByID"               ) ] = info_static_data.flashChipSizeByID;
  
  j_info[ F("SDKVersion"                      ) ] = info_static_data.SDKVersion;
  j_info[ F("bootVersion"                     ) ] = info_static_data.bootVersion;
  j_info[ F("bootMode"                        ) ] = info_static_data.bootMode;
  j_info[ F("freeSketchSpace"                 ) ] = info_static_data.freeSketchSpace;
  j_info[ F("sketchSize"                      ) ] = info_static_data.sketchSize;
  j_info[ F("cpuFreqMHz"                      ) ] = info_static_data.cpuFreqMHz;
  j_info[ F("lastSystemRestartInfo"           ) ] = info_static_data.lastSystemRestartInfo;
  j_info[ F("lastSystemRestartReason"         ) ] = info_static_data.lastSystemRestartReason;
  j_info[ F("hostname"                        ) ] = info_static_data.hostname;
  j_info[ F("jsonBufferSize"                  ) ] = info_static_data.jsonBufferSize;


  String text;
  jsonToString(json, text);
  msg->set_message(text);


  DBG_SERIAL.println( F("info_static_to_json END") );
  DBG_SERIAL.flush();
}








void    message_dynamic_info_tester(    message* msg ) {}

void    message_dynamic_info_initer(    message* msg ) {
//  DBG_SERIAL.println( F("info_dynamic_init START") );

  message_dynamic_info_updater(msg);

//  DBG_SERIAL.println( F("info_dynamic_init END") );
//  DBG_SERIAL.flush();
}

void    message_dynamic_info_updater(   message* msg ) {
//  DBG_SERIAL.println( F("info_dynamic_updater START") );

  String duration1 = "";
  int    hr,mn,st;
  
  st = millis() / 1000;
  mn = st       /   60;
  hr = st       / 3600;
  st = st - mn  *   60;
  mn = mn - hr  *   60;
  
  if (hr<10) { duration1 += ("0"); }
  duration1 += (hr);
  duration1 += (":");
  
  if (mn<10) { duration1 += ("0"); }
  duration1 += (mn);
  duration1 += (":");
  
  if (st<10) { duration1 += ("0"); }
  duration1 += (st);
  
  unsigned long  spdcount  = ESP.getCycleCount();
  delay(1);
  unsigned long  spdcount1 = ESP.getCycleCount();
  unsigned long  speedcnt  = spdcount1-spdcount; 

  info_dynamic_data.freeHeap                         = ESP.getFreeHeap();  
  info_dynamic_data.freeRAM                          = (uint32_t)system_get_free_heap_size()/1024; 
  info_dynamic_data.systemInstructionCyclesPerSecond = speedcnt*1000;
  info_dynamic_data.systemUptime                     = duration1;
  info_dynamic_data.numMessages                      = messages.size();

  message_dynamic_info_to_json( msg );
//  DBG_SERIAL.println( F("info_dynamic_updater END") );
//  DBG_SERIAL.flush();
}

void    message_dynamic_info_printer(   message* msg ) {
//  DBG_SERIAL.println( F("info_dynamic_printer START") );
  
  DBG_SERIAL.print( F("Free RAM                            : ") ); DBG_SERIAL.print  ( info_dynamic_data.freeRAM                          ); DBG_SERIAL.println( F(" KBytes") );
  DBG_SERIAL.print( F("Free Heap                           : ") ); DBG_SERIAL.println( info_dynamic_data.freeHeap                         );
  DBG_SERIAL.print( F("System Uptime                       : ") ); DBG_SERIAL.println( info_dynamic_data.systemUptime                     ); 
  DBG_SERIAL.print( F("Number of Messages                  : ") ); DBG_SERIAL.println( info_dynamic_data.numMessages                      );
  DBG_SERIAL.print( F("System Instruction Cycles Per Second: ") ); DBG_SERIAL.println( info_dynamic_data.systemInstructionCyclesPerSecond );  
  
//  DBG_SERIAL.println( F("info_dynamic_printer END") );
  // DBG_SERIAL.flush();
}

void    message_dynamic_info_publisher( message* msg ) {
//  DBG_SERIAL.println( F("info_dynamic_publisher START") );
  
  String text;
  msg->pop_message(text);
  
#ifdef _HANDLER_WEBSOCKET_H_
  webSocket.broadcastTXT( text );
#else
  DBG_SERIAL.print("info_dynamic_publisher");
  DBG_SERIAL.println(text);
  DBG_SERIAL.flush();
#endif

//  DBG_SERIAL.println( F("info_dynamic_publisher END") );
//  DBG_SERIAL.flush();
}

void    message_dynamic_info_looper(    message* msg ) {  }

void    message_dynamic_info_to_json(   message* msg ) {
//  DBG_SERIAL.println( F("info_dynamic_to_json START") );

  jsonBuffer_t jsonBuffer;
  JsonObject& json  = jsonBuffer.createObject();
  
  json["_type"]     = "info/dynamic";
  json["_id"  ]     = millis();

  JsonObject& j_info                              = json.createNestedObject("data");

  j_info[ F("systemUptime"                    ) ] = info_dynamic_data.systemUptime;
  j_info[ F("freeRAM"                         ) ] = info_dynamic_data.freeRAM;
  j_info[ F("freeHeap"                        ) ] = info_dynamic_data.freeHeap;
  j_info[ F("numMessages"                     ) ] = info_dynamic_data.numMessages;
  j_info[ F("systemInstructionCyclesPerSecond") ] = info_dynamic_data.systemInstructionCyclesPerSecond;

  String text;
  jsonToString(json, text);
  msg->set_message(text);

//  DBG_SERIAL.println( F("info_dynamic_to_json END") );
//  DBG_SERIAL.flush();
}


/*
static void   updateExtraInfo() {
  uint8_t fid = 0;
  for (auto m: messages) {
    if ( messages[fid].pool ) {
      if ( messages[fid].update_every != 0 ) {
        if (( messages[fid].last_update == 0 ) || ((millis() - messages[fid].last_update) >= messages[fid].update_every)) {
          jsonBuffer_t jsonBuffer;
          JsonObject& json  = jsonBuffer.createObject();
          String msg;
          m.toJson(json);
          jsonToString(json, msg);
          messages[fid].message      = msg.c_str();
          messages[fid].updated      = true;
          messages[fid].last_update  = millis();
        }
      }
    }
    fid++;
  }
}
*/









void   init_info() {
  DBG_SERIAL.println( F("init_info START") );
  
  message_static_info_funcs.tester     = message_static_info_tester   ;
  message_static_info_funcs.initer     = message_static_info_initer   ;
  message_static_info_funcs.printer    = message_static_info_printer  ;
  message_static_info_funcs.publisher  = message_static_info_publisher;
  message_static_info_funcs.updater    = message_static_info_updater  ;
  message_static_info_funcs.looper     = message_static_info_looper   ;
  info_static_data.message_static_id   = messages.size();
  DBG_SERIAL.println( F("init_info CREATING static") ); DBG_SERIAL.flush();
  message_static_info_msg              = message( "Static Info", -1, -1, message_static_info_funcs, true );
  DBG_SERIAL.println( F("init_info PUSHING static") ); DBG_SERIAL.flush();
  messages.push_back( &message_static_info_msg );

  
  message_dynamic_info_funcs.tester    = message_dynamic_info_tester   ;
  message_dynamic_info_funcs.initer    = message_dynamic_info_initer   ;
  message_dynamic_info_funcs.printer   = message_dynamic_info_printer  ;
  message_dynamic_info_funcs.publisher = message_dynamic_info_publisher;
  message_dynamic_info_funcs.updater   = message_dynamic_info_updater  ;
  message_dynamic_info_funcs.looper    = message_dynamic_info_looper   ;
  info_static_data.info_dynamic_id     = messages.size();
  DBG_SERIAL.println( F("init_info CREATING dynamic") ); DBG_SERIAL.flush();
  message_dynamic_info_msg             = message( "Dynamic Info", 10000, -1, message_dynamic_info_funcs, true, true );
  DBG_SERIAL.println( F("init_info PUSHING dynamic") ); DBG_SERIAL.flush();
  messages.push_back( &message_dynamic_info_msg );

  
  
  DBG_SERIAL.println( F("Registering /info/static") );
  addEndpoint("Info Static", "/info/static"  , "", "", HTTP_GET, handleInfoStatic);

  DBG_SERIAL.println( F("Registering /info/dynamic") );
  addEndpoint("Info Dynamic", "/info/dynamic", "", "", HTTP_GET, handleInfoDynamic);  
  
  
  
  DBG_SERIAL.println( F("init_info END") );
  DBG_SERIAL.flush();
}




void   handleInfoStatic() {
  String res;
  message_to_json( message_static_info_msg, res );
  
  server.send( 200, "application/json", res );
}

void   handleInfoDynamic() {
  String res;
  message_to_json( message_dynamic_info_msg, res );
  
  server.send( 200, "application/json", res );
}

#endif //#ifndef _HANDLER_INFO_H_
