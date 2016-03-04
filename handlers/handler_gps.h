#ifndef _HANDLER_GPS_H_
#define _HANDLER_GPS_H_

#ifdef  USE_GPS

#define GPS_POOL_EVERY         1100
#define GPS_READ_FOR            900
#define GPS_RX_PORT              13
#define GPS_TX_PORT              15
//#define GPS_BAUDRATE           9600
#define GPS_BAUDRATE          38400
#define GPS_START_FIELD    "$GPRMC"
#define GPS_END_FIELD      "$GPGLL"
#define GPS_FIELD_SEP           "|"

#include <SoftwareSerial.h>
#include <ArduinoJson.h>

struct info_gps_data_t {
  uint8_t       message_info_gps_id;
  uint8_t       message_data_gps_id;
  uint32_t      gpsBaudrate         = GPS_BAUDRATE;
  uint32_t      gpsRxPort           = GPS_RX_PORT;
  uint32_t      gpsTxPort           = GPS_TX_PORT;
  uint32_t      gpsReadFor          = GPS_READ_FOR;
  uint32_t      gpsPoolEvery        = GPS_POOL_EVERY;
  String        gpsStartField       = GPS_START_FIELD;
  String        gpsEndField         = GPS_END_FIELD;
  String        gpsRegisterFieldSep = GPS_FIELD_SEP;
};
info_gps_data_t info_gps_data;
message_funcs_t message_gps_info_funcs;
message_funcs_t message_gps_data_funcs;
message         message_gps_info_msg;
message         message_gps_data_msg;


SoftwareSerial mySerial1(GPS_RX_PORT, GPS_TX_PORT); // RX, TX
#define GPS_SERIAL        mySerial1


void    init_gps();
void    handleGPSinfo();
void    handleGPSdata();
void    message_gps_info_tester(    message* msg );
void    message_gps_info_initer(    message* msg );
void    message_gps_info_updater(   message* msg );
void    message_gps_info_printer(   message* msg );
void    message_gps_info_publisher( message* msg );
void    message_gps_info_looper(    message* msg );
void    message_gps_info_to_json(   message* msg );

void    message_gps_data_tester(    message* msg );
void    message_gps_data_initer(    message* msg );
void    message_gps_data_updater(   message* msg );
void    message_gps_data_printer(   message* msg );
void    message_gps_data_publisher( message* msg );
void    message_gps_data_looper(    message* msg );
void    message_gps_data_to_json(   message* msg, String& reg );



void    message_gps_info_tester(    message* msg ) { 
  DBG_SERIAL.println( F("Tester    Extended") ); 
  DBG_SERIAL.flush(); 
}

void    message_gps_info_initer(    message* msg ) {
  DBG_SERIAL.println( F("info_gps_init START") );

  DBG_SERIAL.println( F("Starting GPS") );
  DBG_SERIAL.print  ( F("Baudrate:"   ) );
  DBG_SERIAL.println( info_gps_data.gpsBaudrate );

  GPS_SERIAL.begin(   info_gps_data.gpsBaudrate );
  
  DBG_SERIAL.print(  F("WAITING FOR GPS: ") );
  DBG_SERIAL.flush();
  
  while ( ! GPS_SERIAL ) {
    delay(100);
    DBG_SERIAL.print(    "." );
    DBG_SERIAL.flush();
  }
  
  DBG_SERIAL.println(    "" );
  DBG_SERIAL.flush();
  DBG_SERIAL.println(    F("GPS CONNECTED") );
  DBG_SERIAL.flush();

  message_gps_info_updater( msg );
  
  DBG_SERIAL.println( F("info_gps_init END") );
  DBG_SERIAL.flush();
}

void    message_gps_info_updater(   message* msg ) {
  //msg->init();
  message_gps_info_to_json( msg );
}

void    message_gps_info_printer(   message* msg ) {
  //DBG_SERIAL.println( F("info_gps_printer START") );
  
  DBG_SERIAL.print( F("GPS Pool Every                      : ") ); DBG_SERIAL.println(info_gps_data.gpsPoolEvery       );
  DBG_SERIAL.print( F("GPS Start Field                     : ") ); DBG_SERIAL.println(info_gps_data.gpsStartField      );
  DBG_SERIAL.print( F("GPS End Field                       : ") ); DBG_SERIAL.println(info_gps_data.gpsEndField        );
  DBG_SERIAL.print( F("GPS Register Field Separator        : ") ); DBG_SERIAL.println(info_gps_data.gpsRegisterFieldSep);
  DBG_SERIAL.print( F("GPS Baudrate                        : ") ); DBG_SERIAL.println(info_gps_data.gpsBaudrate        );
  DBG_SERIAL.print( F("GPS Rx Port                         : ") ); DBG_SERIAL.println(info_gps_data.gpsRxPort          );
  DBG_SERIAL.print( F("GPS Tx Port                         : ") ); DBG_SERIAL.println(info_gps_data.gpsTxPort          );
  DBG_SERIAL.print( F("GPS Read For                        : ") ); DBG_SERIAL.println(info_gps_data.gpsReadFor         );

//  DBG_SERIAL.println( F("info_gps_printer END") );
//  DBG_SERIAL.flush();
}

void    message_gps_info_publisher( message* msg ) {
  //DBG_SERIAL.println( F("info_gps_publisher START") );

  String text;
  msg->pop_message( text );

#ifdef _HANDLER_WEBSOCKET_H_
  webSocket.broadcastTXT( text );
#else
  DBG_SERIAL.print( F("BROADCAST REGISTER: ") );
  DBG_SERIAL.println( text );
#endif
  //delayy(100);

  //DBG_SERIAL.println( F("SENT") );
  
  //DBG_SERIAL.println( F("info_gps_publisher END") );
  //DBG_SERIAL.flush();
}

void    message_gps_info_looper(    message* msg ) {}

void    message_gps_info_to_json(   message* msg ) {
  DBG_SERIAL.println( F("info_gps_to_json START") );
  
  jsonBuffer_t jsonBuffer;
  JsonObject& json  = jsonBuffer.createObject();
  
  json["_type"] = "gps/info";
  json["_id"  ] = millis();

  JsonObject& j_gps_info                            = json.createNestedObject("data");

  j_gps_info[ F("gpsPoolEvery"                  ) ] = info_gps_data.gpsPoolEvery;
  j_gps_info[ F("gpsStartField"                 ) ] = info_gps_data.gpsStartField;
  j_gps_info[ F("gpsEndField"                   ) ] = info_gps_data.gpsEndField;
  j_gps_info[ F("gpsRegisterFieldSep"           ) ] = info_gps_data.gpsRegisterFieldSep;
  j_gps_info[ F("gpsBaudrate"                   ) ] = info_gps_data.gpsBaudrate;
  j_gps_info[ F("gpsRxPort"                     ) ] = info_gps_data.gpsRxPort;
  j_gps_info[ F("gpsTxPort"                     ) ] = info_gps_data.gpsTxPort;
  j_gps_info[ F("gpsReadFor"                    ) ] = info_gps_data.gpsReadFor;


  String text;
  jsonToString(json, text);
  msg->set_message(text);

  DBG_SERIAL.println( F("info_gps_to_json END") );
  DBG_SERIAL.flush();
}
    




void    message_gps_data_tester(    message* msg ) {}
    
void    message_gps_data_initer(    message* msg ) {}
    
void    message_gps_data_printer(   message* msg ) {}

void    message_gps_data_updater(   message* msg ) {
#ifdef _HANDLER_SERVER_H_
    if ( webserver_data.busy ) {
      return;
    }
#endif

#ifdef _HANDLER_WEBSOCKET_H_
  if ( websocket_data.has_client == 0 ) {
    //DBG_SERIAL.println( "NO CLIENT." );
    return;
  }
#endif

  /*
  update_location();
  */
  //DBG_SERIAL.write( "u "    );
  unsigned long start        = millis();
  bool          hasStarted   = false;
  bool          hasEnded     = false;
  String        gps_register = "[";

  while ((( GPS_SERIAL.available() ) || ( ! hasEnded )) && ((millis() - start) < info_gps_data.gpsReadFor)){
#ifdef _HANDLER_SERVER_H_
    if ( webserver_data.busy ) {
      break;
    }
#endif

    String line = GPS_SERIAL.readStringUntil('\n');
    // DBG_SERIAL.print( "line: " );
    // DBG_SERIAL.println( line );
    //delayy(10);
    delay(0);
    if ( line.startsWith(info_gps_data.gpsStartField) ) {
      hasStarted = true;
      // DBG_SERIAL.println( "hasStarted" );
    }


    if ( hasStarted ) {
      line.trim();
      if (gps_register.length() != 1) {
        gps_register += ",";
      }
      gps_register += "\"" + line + "\"";
      delayy(0);
      
      if ( line.startsWith(info_gps_data.gpsEndField) ) {
        hasEnded  = true;
        // DBG_SERIAL.println( "hasEnded" );
        break;
      }
    }
  }

  if ( hasEnded ) {
    // DBG_SERIAL.print( "+ " );
    gps_register += "]";
    message_gps_data_to_json(msg, gps_register);
  } else {
    DBG_SERIAL.print( "- " );
  }
  
  /*
    DBG_SERIAL.print( "DATA: " );
    DBG_SERIAL.println( gps_register.length() );
    DBG_SERIAL.println( gps_register );
    DBG_SERIAL.println( "\n" );
  */
  
  /*
  $GPRMC,004936.00,A,5158.43915,N,00540.36940,E,0.057,,110216,,,A*75
  $GPVTG,,T,,M,0.057,N,0.105,K,A*25
  $GPGGA,004936.00,5158.43915,N,00540.36940,E,1,08,1.29,21.1,M,46.1,M,,*6D
  $GPGSA,A,3,27,08,20,29,26,16,18,21,,,,,1.86,1.29,1.34*0C
  $GPGSV,4,1,13,04,49,165,25,07,10,333,14,08,12,278,18,10,17,159,18*7E
  $GPGSV,4,2,13,13,00,043,,16,72,257,31,18,36,126,29,20,28,054,23*72
  $GPGSV,4,3,13,21,66,077,36,26,59,180,33,27,42,281,28,29,11,092,30*7A
  $GPGSV,4,4,13,32,02,353,20*4F
  $GPGLL,5158.43915,N,00540.36940,E,004936.00,A,A*6B
  */
}

void    message_gps_data_to_json( message* msg, String& reg ) {
  String gps_register = "{\"_type\":\"gps/data\",\"_id\":"+String(millis())+",\"data\":{\"register\":"+reg+"}}";

  //,\"sep\":\""+info_gps_data.gpsRegisterFieldSep+"\"

  msg->set_message(gps_register);

  DBG_SERIAL.println(gps_register);
}

void    message_gps_data_publisher( message* msg ) {
//  DBG_SERIAL.println( "data_gps_publisher START" );

  String text;
  msg->pop_message( text );
#ifdef _HANDLER_WEBSOCKET_H_
  webSocket.broadcastTXT( text );
#else
  //DBG_SERIAL.print( F("BROADCAST REGISTER: ") );
  //DBG_SERIAL.println( text );
#endif

}

void    message_gps_data_looper(    message* msg ) {}






void    init_gps() {
  DBG_SERIAL.println( F("init_gps START") );
  DBG_SERIAL.flush();

  
  message_gps_info_funcs.tester     = message_gps_info_tester   ;
  message_gps_info_funcs.initer     = message_gps_info_initer   ;
  message_gps_info_funcs.printer    = message_gps_info_printer  ;
  message_gps_info_funcs.publisher  = message_gps_info_publisher;
  message_gps_info_funcs.updater    = message_gps_info_updater  ;
  message_gps_info_funcs.looper     = message_gps_info_looper   ;
  info_gps_data.message_info_gps_id = messages.size();
  message_gps_info_msg              = message("GPS Info", -1, -1, message_gps_info_funcs, true);
  messages.push_back( &message_gps_info_msg );



  message_gps_data_funcs.tester     = message_gps_data_tester   ;
  message_gps_data_funcs.initer     = message_gps_data_initer   ;
  message_gps_data_funcs.printer    = message_gps_data_printer  ;
  message_gps_data_funcs.publisher  = message_gps_data_publisher;
  message_gps_data_funcs.updater    = message_gps_data_updater  ;
  message_gps_data_funcs.looper     = message_gps_data_looper   ;
  info_gps_data.message_data_gps_id = messages.size();
  message_gps_data_msg              = message("GPS Data", info_gps_data.gpsPoolEvery, -1, message_gps_data_funcs);
  messages.push_back( &message_gps_data_msg );

  
  
  DBG_SERIAL.println( F("Registering /gps/info") );
  addEndpoint("GPS Info", "/gps/info", "", "", HTTP_GET, handleGPSinfo);

  DBG_SERIAL.println( F("Registering /gps/data") );
  addEndpoint("GPS Data", "/gps/data", "", "", HTTP_GET, handleGPSdata);


  
  DBG_SERIAL.println( F("init_gps END") );
  DBG_SERIAL.flush();
}

void handleGPSinfo () {
  String res;
  message_to_json( message_gps_info_msg, res );
  
  server.send( 200, "application/json", res );
}

void handleGPSdata () {
  String res;
  message_to_json( message_gps_data_msg, res );
  
  server.send( 200, "application/json", res );
}




#endif //ifdef USE_GPS

#endif //ifndef _HANDLER_GPS_H_
