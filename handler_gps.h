#ifndef _HANDLER_GPS_H_
#define _HANDLER_GPS_H_

#ifdef USE_GPS
static void gpsLoop();

static void gpsLoop(){
  if ( busy ) {
    return;
  }
  if ( ! has_client ) {
#ifdef DEBUG_STEPS
    //DBG_SERIAL.println( "NO CLIENT." );
#endif
    return;
  }

  if ( ( millis() - last_gps_update ) >= self_data.gpsPoolEvery ) {
    /*
    update_location();
    */
    //DBG_SERIAL.write( "u "    );
    unsigned long start = millis();
    bool hasStarted     = false;
    bool hasEnded       = false;
    gps_register        = "";

    while ((( GPS_SERIAL.available() ) || ( ! hasEnded )) && ((millis() - start) < self_data.gpsReadFor)){
      if ( busy ) {
        break;
      }
      
      //GPS_SERIAL.find('\n');
      
      String line = GPS_SERIAL.readStringUntil('\n');
//      DBG_SERIAL.print( "line: " );
//      DBG_SERIAL.println( line );
      //delayy(10);
      yield();
      if ( line.startsWith(self_data.gpsStartField) ) {
        hasStarted = true;
//        DBG_SERIAL.println( "hasStarted" );
      }


      if ( hasStarted ) {
        line.trim();
        if (gps_register.length() == 0) {
          gps_register += line;
        } else {
          gps_register += self_data.gpsRegisterFieldSep + line;
        }
        //delayy(10);
        
        if ( line.startsWith(self_data.gpsEndField) ) {
          hasEnded  = true;
//          DBG_SERIAL.println( "hasEnded" );
          break;
        }
      }
    }

    if ( hasEnded ) {
//      DBG_SERIAL.print( "+ " );
      gps_register = "{\"_type\":\"gps_register\",\"data\":\""+gps_register+"\",\"_sep\":\""+self_data.gpsRegisterFieldSep+"\",\"_id\":"+millis()+"}";

#ifdef DEBUG_STEPS
      DBG_SERIAL.print( "BROADCAST REGISTER: " );
      DBG_SERIAL.println( gps_register );
#endif
      
      webSocket.broadcastTXT( gps_register );

      //delayy(100);

#ifdef DEBUG_STEPS
      DBG_SERIAL.println( "SENT" );
#endif
    } else {
#ifdef DEBUG_STEPS
      DBG_SERIAL.print( "- " );
#endif
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
    last_gps_update = millis();
  }
}
#endif //ifdef USE_GPS

#endif //ifndef _HANDLER_GPS_H_
