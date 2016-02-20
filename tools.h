#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <ArduinoJson.h>

void   delayy(int d);
String formatBytes(size_t bytes);
void   jsonToString(JsonObject& json, String& msg);



void   delayy(int d) {
  unsigned long start = millis();
  while( (millis() - start) < d ) {
    yield();
    delay(0);
  }
}



//format bytes
String formatBytes(size_t bytes) {
  if (bytes < 1024){
    return String(bytes)+" B";
  } else if(bytes < (1024 * 1024)){
    return String(bytes/1024.0)+" KB";
  } else if(bytes < (1024 * 1024 * 1024)){
    return String(bytes/1024.0/1024.0)+" MB";
  } else {
    return String(bytes/1024.0/1024.0/1024.0)+" GB";
  }
}



void   jsonToString(JsonObject& json, String& msg) {
  char temp[JSON_BUFFER_SIZE];
  json.printTo(temp, sizeof(temp)); 
  msg += temp;
}

#endif //ifndef _TOOLS_H_
