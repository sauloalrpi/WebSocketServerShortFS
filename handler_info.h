#ifndef _HANDLER_INFO_H_
#define _HANDLER_INFO_H_


static void infoToJson(JsonObject& json);
static void getSelfDynamicInfo();
static void getSelfStaticInfo();
static void print_self();


static void infoToJson(JsonObject& json) {
  json["_type"] = "info";
  json["_id"  ] = millis();

  JsonObject& j_info                     = json  .createNestedObject("data"  );
  JsonArray&  j_info_keys                = j_info.createNestedArray( "_keys");

  j_info_keys.add("global"            );
  JsonObject& j_info_global              = j_info.createNestedObject("global");
  j_info_global[ F("mac"                             )] = self_data.mac;
  j_info_global[ F("chipId"                          )] = self_data.chipId;
  j_info_global[ F("flashChipId"                     )] = self_data.flashChipId;
  j_info_global[ F("flashChipSize"                   )] = self_data.flashChipSize;
  j_info_global[ F("flashChipSpeed"                  )] = self_data.flashChipSpeed;
  j_info_global[ F("flashChipMode"                   )] = self_data.flashChipMode;
  j_info_global[ F("flashChipRealSize"               )] = self_data.flashChipRealSize;
  j_info_global[ F("flashChipSizeByID"               )] = self_data.flashChipSizeByID;
  
  j_info_global[ F("SDKVersion"                      )] = self_data.SDKVersion;
  j_info_global[ F("bootVersion"                     )] = self_data.bootVersion;
  j_info_global[ F("bootMode"                        )] = self_data.bootMode;
  j_info_global[ F("freeRAM"                         )] = self_data.freeRAM;
  j_info_global[ F("freeHeap"                        )] = self_data.freeHeap;
  j_info_global[ F("freeSketchSpace"                 )] = self_data.freeSketchSpace;
  j_info_global[ F("sketchSize"                      )] = self_data.sketchSize;
  j_info_global[ F("systemInstructionCyclesPerSecond")] = self_data.systemInstructionCyclesPerSecond;
  j_info_global[ F("cpuFreqMHz"                      )] = self_data.cpuFreqMHz;
  j_info_global[ F("systemUptime"                    )] = self_data.systemUptime;
  //j_info_global[ F("lastSystemRestartReason"         )] = self_data.lastSystemRestartReason;
  j_info_global[ F("lastSystemRestartInfo"           )] = self_data.lastSystemRestartInfo;
  j_info_global[ F("hostname"                        )] = self_data.hostname;


#ifdef GPS_POOL_EVERY
  j_info_global[ F("gpsPoolEvery"                    )] = self_data.gpsPoolEvery;
#endif
#ifdef GPS_START_FIELD
  j_info_global[ F("gpsStartField"                   )] = self_data.gpsStartField;
#endif

#ifdef GPS_END_FIELD
  j_info_global[ F("gpsEndField"                     )] = self_data.gpsEndField;
#endif

#ifdef GPS_FIELD_SEP
  j_info_global[ F("gpsRegisterFieldSep"             )] = self_data.gpsRegisterFieldSep;
#endif

#ifdef WEBSOCKETS_SERVER_CLIENT_MAX
  j_info_global[ F("webSocketMaxClient"               )] = self_data.webSocketMaxClient;
#endif

#ifdef JSON_BUFFER_SIZE
  j_info_global[ F("jsonBufferSize"                  )] = self_data.jsonBufferSize;
#endif

#ifdef GPS_BAUDRATE
  j_info_global[ F("gpsBaudrate"                     )] = self_data.gpsBaudrate;
#endif

#ifdef GPS_RX_PORT
  j_info_global[ F("gpsRxPort"                       )] = self_data.gpsRxPort;
#endif

#ifdef GPS_TX_PORT
  j_info_global[ F("gpsTxPort"                       )] = self_data.gpsTxPort;
#endif

#ifdef GPS_READ_FOR
  j_info_global[ F("gpsReadFor"                      )] = self_data.gpsReadFor;
#endif
}


static void getSelfDynamicInfo() {
  String duration1 = " ";
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

  self_data.freeRAM                          = (uint32_t)system_get_free_heap_size()/1024; 
  self_data.systemInstructionCyclesPerSecond = speedcnt*1000;
  self_data.systemUptime                     = duration1;
}


static void getSelfStaticInfo() {
  FlashMode_t    ideMode   = ESP.getFlashChipMode();
  
  self_data.mac                              = WiFi.macAddress();
  self_data.chipId                           = ESP.getChipId();
  self_data.flashChipId                      = ESP.getFlashChipId();
  self_data.flashChipSize                    = ESP.getFlashChipSize();
  self_data.flashChipSpeed                   = ESP.getFlashChipSpeed();
  self_data.flashChipMode                    = (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN");
  self_data.flashChipRealSize                = ESP.getFlashChipRealSize()/1024;
  self_data.flashChipSizeByID                = ESP.getFlashChipSizeByChipId()/1024;
  self_data.SDKVersion                       = (char *)ESP.getSdkVersion();
  self_data.bootVersion                      = ESP.getBootVersion();
  self_data.bootMode                         = ESP.getBootMode();
  self_data.freeHeap                         = ESP.getFreeHeap();  
  self_data.freeSketchSpace                  = ESP.getFreeSketchSpace()/1024;
  self_data.sketchSize                       = ESP.getSketchSize()/1024;
  self_data.cpuFreqMHz                       = ESP.getCpuFreqMHz();
  //self_data.lastSystemRestartReason          = ESP.getResetReason();
  self_data.lastSystemRestartInfo            = ESP.getResetInfo();
  self_data.hostname                         = WIFI_HOST;


#ifdef GPS_POOL_EVERY
  self_data.gpsPoolEvery                     = GPS_POOL_EVERY;
#endif

#ifdef GPS_START_FIELD
  self_data.gpsStartField                    = GPS_START_FIELD;
#endif

#ifdef GPS_END_FIELD
  self_data.gpsEndField                      = GPS_END_FIELD;
#endif

#ifdef GPS_FIELD_SEP
  self_data.gpsRegisterFieldSep              = GPS_FIELD_SEP;
#endif



#ifdef WEBSOCKETS_SERVER_CLIENT_MAX
  self_data.webSocketMaxClient               = WEBSOCKETS_SERVER_CLIENT_MAX;
#endif

#ifdef JSON_BUFFER_SIZE
  self_data.jsonBufferSize                   = JSON_BUFFER_SIZE;
#endif

#ifdef GPS_BAUDRATE
  self_data.gpsBaudrate                      = GPS_BAUDRATE;
#endif

#ifdef GPS_RX_PORT
  self_data.gpsRxPort                        = GPS_RX_PORT;
#endif

#ifdef GPS_TX_PORT
  self_data.gpsTxPort                        = GPS_TX_PORT;
#endif

#ifdef GPS_READ_FOR
  self_data.gpsReadFor                       = GPS_READ_FOR;
#endif
}


static void print_self() {
#ifdef DEBUG_STEPS
  DBG_SERIAL.print( F("MAC address                         : ")); DBG_SERIAL.println(self_data.mac                             );
  DBG_SERIAL.print( F("Chip Id                             : ")); DBG_SERIAL.println(self_data.chipId                          );
  DBG_SERIAL.print( F("Flash Chip Id                       : ")); DBG_SERIAL.println(self_data.flashChipId                     );
  DBG_SERIAL.print( F("Flash Chip Size                     : ")); DBG_SERIAL.println(self_data.flashChipSize                   );
  DBG_SERIAL.print( F("Flash Chip Speed                    : ")); DBG_SERIAL.println(self_data.flashChipSpeed                  );
  DBG_SERIAL.print( F("Flash Chip Mode                     : ")); DBG_SERIAL.println(self_data.flashChipMode                   );
  DBG_SERIAL.print( F("Flash Chip Real Size                : ")); DBG_SERIAL.println(self_data.flashChipRealSize               );
  DBG_SERIAL.print( F("Flash Chip Size By ID               : ")); DBG_SERIAL.println(self_data.flashChipSizeByID               );
  DBG_SERIAL.print( F("SDK Version                         : ")); DBG_SERIAL.println(self_data.SDKVersion                      );
  DBG_SERIAL.print( F("Boot Version                        : ")); DBG_SERIAL.println(self_data.bootVersion                     );
  DBG_SERIAL.print( F("Boot Mode                           : ")); DBG_SERIAL.println(self_data.bootMode                        );
  DBG_SERIAL.print( F("Free RAM                            : ")); DBG_SERIAL.print  (self_data.freeRAM                         ); DBG_SERIAL.println(F(" KBytes"));
  DBG_SERIAL.print( F("Free Heap                           : ")); DBG_SERIAL.println(self_data.freeHeap                        );
  DBG_SERIAL.print( F("Free Sketch Space                   : ")); DBG_SERIAL.print  (self_data.freeSketchSpace                 ); DBG_SERIAL.println(F(" KBytes"));
  DBG_SERIAL.print( F("Sketch Size                         : ")); DBG_SERIAL.print  (self_data.sketchSize                      ); DBG_SERIAL.println(F(" KBytes"));
  DBG_SERIAL.print( F("System Instruction Cycles Per Second: ")); DBG_SERIAL.println(self_data.systemInstructionCyclesPerSecond);
  DBG_SERIAL.print( F("CPU Speed                           : ")); DBG_SERIAL.print  (self_data.cpuFreqMHz                      ); DBG_SERIAL.println(F(" MHz"   ));
  DBG_SERIAL.print( F("System Uptime                       : ")); DBG_SERIAL.println(self_data.systemUptime                    ); 
  DBG_SERIAL.print( F("Last System Restart Info            : ")); DBG_SERIAL.println(self_data.lastSystemRestartInfo           );
  DBG_SERIAL.print( F("Hostname                            : ")); DBG_SERIAL.println(self_data.hostname                        );
  DBG_SERIAL.print( F("WEBSERVER_PORT                      : ")); DBG_SERIAL.println(WEBSERVER_PORT                            );
  DBG_SERIAL.print( F("DEBUG_WEBSOCKETS                    : ")); DBG_SERIAL.println(DEBUG_WEBSOCKETS                          );
  //DBG_SERIAL.print(  F("Last System Restart Reason         : ")); DBG_SERIAL.println(self_data.lastSystemRestartReason         );
#ifdef GPS_POOL_EVERY
  DBG_SERIAL.print( F("GPS Pool Every                      : ")); DBG_SERIAL.println(self_data.gpsPoolEvery);
#endif
#ifdef GPS_START_FIELD
  DBG_SERIAL.print( F("GPS Start Field                     : ")); DBG_SERIAL.println(self_data.gpsStartField);
#endif
#ifdef GPS_END_FIELD
  DBG_SERIAL.print( F("GPS End Field                       : ")); DBG_SERIAL.println(self_data.gpsEndField);
#endif
#ifdef GPS_FIELD_SEP
  DBG_SERIAL.print( F("GPS Register Field Separator        : ")); DBG_SERIAL.println(self_data.gpsRegisterFieldSep);
#endif
#ifdef WEBSOCKETS_SERVER_CLIENT_MAX
  DBG_SERIAL.print( F("WebSocket Max Client                : ")); DBG_SERIAL.println(self_data.webSocketMaxClient);
#endif
#ifdef JSON_BUFFER_SIZE
  DBG_SERIAL.print( F("JSON Buffer Size                    : ")); DBG_SERIAL.println(self_data.jsonBufferSize);
#endif
#ifdef GPS_BAUDRATE
  DBG_SERIAL.print( F("GPS Baudrate                        : ")); DBG_SERIAL.println(self_data.gpsBaudrate);
#endif
#ifdef GPS_RX_PORT
  DBG_SERIAL.print( F("GPS Rx Port                         : ")); DBG_SERIAL.println(self_data.gpsRxPort);
#endif
#ifdef GPS_TX_PORT
  DBG_SERIAL.print( F("GPS Tx Port                         : ")); DBG_SERIAL.println(self_data.gpsTxPort);
#endif
#ifdef GPS_READ_FOR
  DBG_SERIAL.print( F("GPS Read For                        : ")); DBG_SERIAL.println(self_data.gpsReadFor);
#endif


#ifdef ESP8266
  DBG_SERIAL.print( F("ESP8266                             : ")); DBG_SERIAL.println(ESP8266);
#endif
#ifdef NETWORK_ESP8266
  DBG_SERIAL.print( F("NETWORK_ESP8266                     : ")); DBG_SERIAL.println(NETWORK_ESP8266);
#endif
#ifdef WEBSOCKETS_SERVER_CLIENT_MAX
  DBG_SERIAL.print( F("WEBSOCKETS_SERVER_CLIENT_MAX        : ")); DBG_SERIAL.println(WEBSOCKETS_SERVER_CLIENT_MAX);
#endif
#ifdef WEBSOCKETS_NETWORK_TYPE
  DBG_SERIAL.print( F("WEBSOCKETS_NETWORK_TYPE             : ")); DBG_SERIAL.println(WEBSOCKETS_NETWORK_TYPE);
#endif
#ifdef WEBSOCKETS_USE_BIG_MEM
  DBG_SERIAL.print( F("WEBSOCKETS_USE_BIG_MEM              : ")); DBG_SERIAL.println(WEBSOCKETS_USE_BIG_MEM);
#endif

  DBG_SERIAL.flush();
}
#else
;
}
#endif

#endif //#ifndef _HANDLER_INFO_H_
