#ifndef _TOOLS_H_
#define _TOOLS_H_

#ifndef JSON_BUFFER_SIZE
#define JSON_BUFFER_SIZE       1536
#endif

#include <ArduinoJson.h>

typedef StaticJsonBuffer<JSON_BUFFER_SIZE> jsonBuffer_t;
typedef std::vector<String>                string_vec;
typedef std::vector<string_vec>            string_vec_vec;

void   delayy(int d);
String formatBytes(size_t bytes);
void   jsonToString(JsonObject& json, String& msg);



void   delayy(int d) {
  long long start = millis();
  while( (millis() - start) < d ) {
    //yield();
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




void splitter(String origin, string_vec& res, char sep) {
  int maxIndex = origin.length() - 1;
  
  /*
  int start      = 0;
  int end        = 0;
  
  DBG_SERIAL.print  ( "origin: "   );
  DBG_SERIAL.println(  origin      );
  DBG_SERIAL.print  ( "sep: "      );
  DBG_SERIAL.println(  sep         );
  DBG_SERIAL.print  ( "maxIndex: " );
  DBG_SERIAL.println(  maxIndex    );
  */

  for ( int i=0; i<=maxIndex; i++ ) {
    // DBG_SERIAL.print  ( " i: " );
    // DBG_SERIAL.println(   i    );
    int k = i;
    if ( k!=0 ) {
      k = i + 1;
    }
    if ( origin.charAt(i)==sep || i == 0 ) {
      for ( int j=i+1; j<=maxIndex; j++ ) {
        // DBG_SERIAL.print  ( "  j: " );
        // DBG_SERIAL.println(    j    );
        if ( origin.charAt(j)==sep || j==maxIndex ) {
          if ( ( j - i ) > 0 ) {
            int l = j;
            if ( origin.charAt(k) == sep ) {
              k = k + 1;
            }
            if ( l==maxIndex ) {
              l = l+1;
            }
            String piece = origin.substring(k,l);
            // DBG_SERIAL.print  ( "   piece: " );
            // DBG_SERIAL.println(     piece    );
            res.push_back(piece);
          }
          i = j-1;
          break;
        }
      }
    }
  }  
}

#endif //ifndef _TOOLS_H_
