#ifndef _HANDLER_BNO055_H_
#define _HANDLER_BNO055_H_

#ifdef  USE_BNO055

#define BNO055_KEEP_UPDATED   true
#define BNO055_POOL_EVERY      100
#define BNO055_SDA_PORT          0
#define BNO055_SCL_PORT          2


#ifdef _HANDLER_WEBSOCKET_H_
#define BNO055_UPDATE_ON_WEBSOCKET  true
#else
#define BNO055_UPDATE_ON_WEBSOCKET false
#endif

#if(BNO055_KEEP_UPDATED)
#undef BNO055_UPDATE_ON_WEBSOCKET
#define BNO055_UPDATE_ON_WEBSOCKET false
#else
#undef BNO055_UPDATE_ON_WEBSOCKET
#define BNO055_UPDATE_ON_WEBSOCKET  true
#endif





#include <Wire.h>
#include "../libs/BNO055/Adafruit_BNO055.h"
#include "../libs/BNO055/Adafruit_Sensor.h"
#include "../libs/BNO055/utility/imumaths.h"


/*
i2c stretching
https://forums.adafruit.com/viewtopic.php?f=57&t=90222&p=455154&hilit=BNO055#p455154
C:\Users\saulo\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.0.0\cores\esp8266\core_esp8266_si2c.c
#ifdef I2C_EXTRA_CLOCK_STRETCH
#if F_CPU == FCPU80
#define TWI_CLOCK_STRETCH 25600
#else
#define TWI_CLOCK_STRETCH 51200
#endif
#else
#if F_CPU == FCPU80
#define TWI_CLOCK_STRETCH 800
#else
#define TWI_CLOCK_STRETCH 1600
#endif
#endif

OR
after feb 18 206
Wire.setClockStretchLimit(8533); // limit in μs
 */

 
Adafruit_BNO055 bno = Adafruit_BNO055(55);


struct info_BNO055_t {
  uint8_t       message_BNO055_info_id;
  uint8_t       message_BNO055_data_id;
  uint32_t      poolEvery                           = BNO055_POOL_EVERY;
  uint32_t      SDAPort                             = BNO055_SDA_PORT;
  uint32_t      SCLPort                             = BNO055_SCL_PORT;
  uint32_t      last_loop                           =   0;
  bool          update_only_if_has_websocket_client = BNO055_UPDATE_ON_WEBSOCKET;

  char          sensor_name[12]                     =  ""; /**< sensor name */
  float         sensor_max_value                    = 0.0; /**< maximum value of this sensor's value in SI units */
  float         sensor_min_value                    = 0.0; /**< minimum value of this sensor's value in SI units */
  float         sensor_resolution                   = 0.0; /**< smallest difference between two values reported by this sensor */
  int32_t       sensor_version                      =   0; /**< version of the hardware + driver */
  int32_t       sensor_id                           =   0; /**< unique sensor identifier */
  int32_t       sensor_min_delay                    =   0; /**< min delay in microseconds between events. zero = not a constant rate */
};
info_BNO055_t      info_BNO055_data;
message_funcs_t    message_BNO055_info_funcs;
message            message_BNO055_info_msg;
message_funcs_t    message_BNO055_data_funcs;
message            message_BNO055_data_msg; 
 
void    displayCalStatus    (void);
void    displaySensorStatus (void);
void    displaySensorDetails(void);

void    init_BNO055();
void    server_init_BNO055();

void    message_BNO055_info_tester   ( message* msg );
void    message_BNO055_info_initer   ( message* msg );
void    message_BNO055_info_updater  ( message* msg );
void    message_BNO055_info_looper   ( message* msg );
void    message_BNO055_info_printer  ( message* msg );
void    message_BNO055_info_publisher( message* msg );
void    message_BNO055_info_to_json  ( message* msg );

void    message_BNO055_data_tester   ( message* msg );
void    message_BNO055_data_initer   ( message* msg );
void    message_BNO055_data_updater  ( message* msg );
void    message_BNO055_data_looper   ( message* msg );
void    message_BNO055_data_printer  ( message* msg );
void    message_BNO055_data_publisher( message* msg );
void    message_BNO055_data_to_json  ( message* msg );

 


void    message_BNO055_info_tester   ( message* msg ) {
  DBG_SERIAL.println( F("message_BNO055_info_tester") ); 
  DBG_SERIAL.flush(); 
}

void    message_BNO055_info_initer   ( message* msg ) {
  /* Initialise the sensor */

  DBG_SERIAL.println( F("Starting BNO055"    ) );
  DBG_SERIAL.print  ( F("SDA Port: "         ) );
  DBG_SERIAL.println( info_BNO055_data.SDAPort );

  DBG_SERIAL.print  ( F("SCL Port: "         ) );
  DBG_SERIAL.println( info_BNO055_data.SCLPort );

  DBG_SERIAL.flush();
  
  bno.setPorts(info_BNO055_data.SDAPort, info_BNO055_data.SCLPort);

  DBG_SERIAL.print(  F("WAITING FOR I2c: ") );
  DBG_SERIAL.flush();
  
  while(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    //DBG_SERIAL.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    //while(1);
    DBG_SERIAL.print(    "." );
    DBG_SERIAL.flush();
  }
  
  DBG_SERIAL.println(    "" );
  DBG_SERIAL.flush();
  DBG_SERIAL.println(    F("BNO055 CONNECTED") );
  DBG_SERIAL.flush();

  /* TODO. PUT SOFT DELAY */
  delay(1000);

  /* Display some basic information on this sensor */
  displaySensorDetails();

  /* Optional: Display current status */
  displaySensorStatus();

  bno.setExtCrystalUse(true);
  
  message_BNO055_info_updater( msg );
}

void    message_BNO055_info_updater  ( message* msg ) {
  sensor_t sensor;
  bno.getSensor(&sensor);

  //strcpy(mStr.message, myStr.c_str());
  strcpy(info_BNO055_data.sensor_name, sensor.name);
  //info_BNO055_data.sensor_name       = sensor.name;       /**< sensor name */
  info_BNO055_data.sensor_version    = sensor.version;    /**< version of the hardware + driver */
  info_BNO055_data.sensor_id         = sensor.sensor_id;  /**< unique sensor identifier */
  info_BNO055_data.sensor_max_value  = sensor.max_value;  /**< maximum value of this sensor's value in SI units */
  info_BNO055_data.sensor_min_value  = sensor.min_value;  /**< minimum value of this sensor's value in SI units */
  info_BNO055_data.sensor_resolution = sensor.resolution; /**< smallest difference between two values reported by this sensor */
  info_BNO055_data.sensor_min_delay  = sensor.min_delay;  /**< min delay in microseconds between events. zero = not a constant rate */

  message_BNO055_info_to_json( msg );
}

void    message_BNO055_info_looper   ( message* msg ) {}

void    message_BNO055_info_printer  ( message* msg ) {
  DBG_SERIAL.print( F( "Sensor:       " ) ); DBG_SERIAL.println( info_BNO055_data.sensor_name       );
  DBG_SERIAL.print( F( "Driver Ver:   " ) ); DBG_SERIAL.println( info_BNO055_data.sensor_version    );
  DBG_SERIAL.print( F( "Unique ID:    " ) ); DBG_SERIAL.println( info_BNO055_data.sensor_id         );
  DBG_SERIAL.print( F( "Max Value:    " ) ); DBG_SERIAL.println( info_BNO055_data.sensor_max_value  );
  DBG_SERIAL.print( F( "Min Value:    " ) ); DBG_SERIAL.println( info_BNO055_data.sensor_min_value  );
  DBG_SERIAL.print( F( "Resolution:   " ) ); DBG_SERIAL.println( info_BNO055_data.sensor_resolution );
  DBG_SERIAL.print( F( "Min Delay:    " ) ); DBG_SERIAL.println( info_BNO055_data.sensor_min_delay  );
  DBG_SERIAL.print( F( "SDA Port:     " ) ); DBG_SERIAL.println( info_BNO055_data.SDAPort           );
  DBG_SERIAL.print( F( "SCL Port:     " ) ); DBG_SERIAL.println( info_BNO055_data.SCLPort           );
}

void    message_BNO055_info_publisher( message* msg ) {
  String text;
  msg->pop_message( text );

  broadcastMessage( text );
}

void    message_BNO055_info_to_json  ( message* msg ) {
  DBG_SERIAL.println( F( "message_BNO055_info_to_json START" ) );
  
  jsonBuffer_t jsonBuffer;
  JsonObject& json  = jsonBuffer.createObject();
  
  json["_type"]     = "BNO055/info";
  json["_id"  ]     = millis();

  JsonObject& j_BNO055_info                       = json.createNestedObject("data"  );

  j_BNO055_info[ F( "poolEvery"               ) ] = info_BNO055_data.poolEvery;
  j_BNO055_info[ F( "sensorName"              ) ] = info_BNO055_data.sensor_name;
  j_BNO055_info[ F( "sensorVersion"           ) ] = info_BNO055_data.sensor_version;
  j_BNO055_info[ F( "sensorId"                ) ] = info_BNO055_data.sensor_id;
  j_BNO055_info[ F( "sensorMaxValue"          ) ] = info_BNO055_data.sensor_max_value;
  j_BNO055_info[ F( "sensorMinValue"          ) ] = info_BNO055_data.sensor_min_value;
  j_BNO055_info[ F( "sensorResolution"        ) ] = info_BNO055_data.sensor_resolution;
  j_BNO055_info[ F( "sensorMinDelay"          ) ] = info_BNO055_data.sensor_min_delay;
  j_BNO055_info[ F( "SDAPort"                 ) ] = info_BNO055_data.SDAPort;
  j_BNO055_info[ F( "SCLPort"                 ) ] = info_BNO055_data.SCLPort;
  
  String text;
  jsonToString(json, text);
  msg->set_message(text);

  DBG_SERIAL.println( F( "message_BNO055_info_to_json END" ) );
  DBG_SERIAL.flush();
}








void    message_BNO055_data_tester   ( message* msg ) {}

void    message_BNO055_data_initer   ( message* msg ) {
  info_BNO055_data.last_loop = millis();
}

void    message_BNO055_data_updater  ( message* msg ) {
#ifdef _HANDLER_SERVER_H_
  if ( webserver_data.busy ) {
    DBG_SERIAL.println( "message_BNO055_data_updater. server busy" );
    return;
  }
#endif

#ifdef _HANDLER_WEBSOCKET_H_
  if ( info_BNO055_data.update_only_if_has_websocket_client ) {
    DBG_SERIAL.println( "message_BNO055_data_updater. update_only_if_has_websocket_client" );
    if ( websocket_data.has_client == 0 ) {
      DBG_SERIAL.println( "message_BNO055_data_updater. update_only_if_has_websocket_client. no client" );
      //DBG_SERIAL.println( "NO CLIENT." );
      return;
    }
  }
#endif

  message_BNO055_data_to_json(msg);
}

void    message_BNO055_data_looper   ( message* msg ) {}

void    message_BNO055_data_printer  ( message* msg ) {}

void    message_BNO055_data_publisher( message* msg ) {
  String text;
  msg->pop_message( text );
  
  broadcastMessage( text );
}

void    message_BNO055_data_to_json  ( message* msg ) {
  DBG_SERIAL.println( F("message_BNO055_info_to_json START") );
  
  jsonBuffer_t jsonBuffer;
  JsonObject& json  = jsonBuffer.createObject();
  
  json["_type"]     = "BNO055/data";
  json["_id"  ]     = millis();

  JsonObject& j_BNO055_data                    = json.createNestedObject("data");

  uint8_t system_status, self_test_results, system_error;
  system_status = self_test_results = system_error = 0;

  bno.getSystemStatus(&system_status, &self_test_results, &system_error);

  /* Display the results in the Serial Monitor */
  /*
  Serial.println(system_status    , HEX);
  Serial.println(self_test_results, HEX);
  Serial.println(system_error     , HEX);
  */
  j_BNO055_data[ F("systemStatus"               ) ] = system_status;
  j_BNO055_data[ F("selfTestResults"            ) ] = self_test_results;
  j_BNO055_data[ F("systemError"                ) ] = system_error;
  
  
  
/* Get the four calibration values (0..3) */
  /* Any sensor data reporting 0 should be ignored, */
  /* 3 means 'fully calibrated" */
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  /* The data should be ignored until the system calibration is > 0 */
  DBG_SERIAL.print("\t");
  if (!system)
  {
    DBG_SERIAL.print("! ");
  }

  /* Display the individual values */
  DBG_SERIAL.print( system, DEC );
  DBG_SERIAL.print( gyro  , DEC );
  DBG_SERIAL.print( accel , DEC );
  DBG_SERIAL.print( mag   , DEC );
  
  j_BNO055_data[ F("calibrationSystem"               ) ] = system;
  j_BNO055_data[ F("calibrationGyro"                 ) ] = gyro;
  j_BNO055_data[ F("calibrationAccel"                ) ] = accel;
  j_BNO055_data[ F("calibrationMag"                  ) ] = mag;
  
  
  sensors_event_t event;
  bno.getEvent(&event);

  /* Display the floating point data */
  DBG_SERIAL.print(event.orientation.x, 4);
  DBG_SERIAL.print(event.orientation.y, 4);
  DBG_SERIAL.print(event.orientation.z, 4);
  DBG_SERIAL.print(bno.getTemp());
  j_BNO055_data[ F("orientationX"                    ) ] = event.orientation.x;
  j_BNO055_data[ F("orientationY"                    ) ] = event.orientation.y;
  j_BNO055_data[ F("orientationZ"                    ) ] = event.orientation.z;
  j_BNO055_data[ F("temperature"                     ) ] = bno.getTemp();

  
  
  
  String text;
  jsonToString(json, text);
  msg->set_message(text);

  DBG_SERIAL.println( F("message_BNO055_info_to_json END") );
  DBG_SERIAL.flush();
}








void    init_BNO055       () {
  DBG_SERIAL.println( F("init_BNO055 START") );
  DBG_SERIAL.flush();

  
  message_BNO055_info_funcs.tester             = message_BNO055_info_tester   ;
  message_BNO055_info_funcs.initer             = message_BNO055_info_initer   ;
  message_BNO055_info_funcs.printer            = message_BNO055_info_printer  ;
  message_BNO055_info_funcs.publisher          = message_BNO055_info_publisher;
  message_BNO055_info_funcs.updater            = message_BNO055_info_updater  ;
  message_BNO055_info_funcs.looper             = message_BNO055_info_looper   ;
  info_BNO055_data.message_BNO055_info_id      = messages.size();
  message_BNO055_info_msg                      = message("BNO055 Info", -1, -1, message_BNO055_info_funcs, true);
  messages.push_back( &message_BNO055_info_msg );


  message_BNO055_data_funcs.tester             = message_BNO055_data_tester   ;
  message_BNO055_data_funcs.initer             = message_BNO055_data_initer   ;
  message_BNO055_data_funcs.printer            = message_BNO055_data_printer  ;
  message_BNO055_data_funcs.publisher          = message_BNO055_data_publisher;
  message_BNO055_data_funcs.updater            = message_BNO055_data_updater  ;
  message_BNO055_data_funcs.looper             = message_BNO055_data_looper   ;
  info_BNO055_data.message_BNO055_data_id      = messages.size();
  message_BNO055_data_msg                      = message("BNO055 Data", info_BNO055_data.poolEvery, -1, message_BNO055_data_funcs);
  messages.push_back( &message_BNO055_data_msg );  

  server_init_BNO055();
  
  DBG_SERIAL.println( F("init_BNO055 END") );
  DBG_SERIAL.flush();
}


#ifndef _HANDLER_SERVER_H_
void    server_init_BNO055() {}
#else
void    handleBNO055Info  ();
void    handleBNO055data  ();

void    server_init_BNO055() {
  DBG_SERIAL.println( F("Registering /BNO055/info") );
  addEndpoint("BNO055 Info", "/BNO055/info", "", "", HTTP_GET, handleBNO055Info);

  DBG_SERIAL.println( F("Registering /BNO055/data") );
  addEndpoint("BNO055 Data", "/BNO055/data", "", "", HTTP_GET, handleBNO055data);
}

void    handleBNO055Info  () {
  server_send_message( message_BNO055_info_msg );
}

void    handleBNO055data  () {
  server_send_message( message_BNO055_data_msg );
}
#endif










/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void    displaySensorDetails(void) {
  sensor_t sensor;
  bno.getSensor(&sensor);
  DBG_SERIAL.println( F( "------------------------------------" ) );
  DBG_SERIAL.print  ( F( "Sensor:       " ) ); DBG_SERIAL.println( sensor.name       );
  DBG_SERIAL.print  ( F( "Driver Ver:   " ) ); DBG_SERIAL.println( sensor.version    );
  DBG_SERIAL.print  ( F( "Unique ID:    " ) ); DBG_SERIAL.println( sensor.sensor_id  );
  DBG_SERIAL.print  ( F( "Max Value:    " ) ); DBG_SERIAL.print(   sensor.max_value  ); DBG_SERIAL.println( F( " xxx" ) );
  DBG_SERIAL.print  ( F( "Min Value:    " ) ); DBG_SERIAL.print(   sensor.min_value  ); DBG_SERIAL.println( F( " xxx" ) );
  DBG_SERIAL.print  ( F( "Resolution:   " ) ); DBG_SERIAL.print(   sensor.resolution ); DBG_SERIAL.println( F( " xxx" ) );
  DBG_SERIAL.println( F("------------------------------------") );
  DBG_SERIAL.println( "" );
  delay(500);
}

/**************************************************************************/
/*
    Display some basic info about the sensor status
*/
/**************************************************************************/
void    displaySensorStatus (void) {
  /* Get the system status values (mostly for debugging purposes) */
  uint8_t system_status, self_test_results, system_error;
  system_status = self_test_results = system_error = 0;
  bno.getSystemStatus(&system_status, &self_test_results, &system_error);

  /* Display the results in the Serial Monitor */
  DBG_SERIAL.println( ""                       );
  DBG_SERIAL.print  ( F( "System Status: 0x" ) );
  DBG_SERIAL.println( system_status, HEX       );
  DBG_SERIAL.print  ( F( "Self Test:     0x" ) );
  DBG_SERIAL.println( self_test_results, HEX   );
  DBG_SERIAL.print  ( F( "System Error:  0x" ) );
  DBG_SERIAL.println( system_error, HEX        );
  DBG_SERIAL.println( ""                       );
  delay(500);
}

/**************************************************************************/
/*
    Display sensor calibration status
*/
/**************************************************************************/
void    displayCalStatus    (void) {
  /* Get the four calibration values (0..3) */
  /* Any sensor data reporting 0 should be ignored, */
  /* 3 means 'fully calibrated" */
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  /* The data should be ignored until the system calibration is > 0 */
  DBG_SERIAL.print( F( "\t" ) );
  if (!system)
  {
    DBG_SERIAL.print( F( "! " ) );
  }

  /* Display the individual values */
  DBG_SERIAL.print( F( "Sys:" ) );
  DBG_SERIAL.print( system, DEC );
  DBG_SERIAL.print( F( " G:" )  );
  DBG_SERIAL.print( gyro  , DEC );
  DBG_SERIAL.print( F( " A:" )  );
  DBG_SERIAL.print( accel , DEC );
  DBG_SERIAL.print( F( " M:" )  );
  DBG_SERIAL.print( mag   , DEC );
}

#endif
#endif