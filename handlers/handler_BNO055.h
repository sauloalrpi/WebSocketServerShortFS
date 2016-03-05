#ifndef _HANDLER_BNO055_H_
#define _HANDLER_BNO055_H_

#ifdef  USE_BNO055

#ifndef BNO055_KEEP_UPDATED
#define BNO055_KEEP_UPDATED   true
#endif
#ifndef BNO055_POOL_EVERY
#define BNO055_POOL_EVERY      100
#endif
#ifndef BNO055_SDA_PORT
#define BNO055_SDA_PORT          0
#endif
#ifndef BNO055_SCL_PORT
#define BNO055_SCL_PORT          2
#endif
#ifndef BNO055_INIT_ID
#define BNO055_INIT_ID          55
#endif



#if(BNO055_KEEP_UPDATED)
#ifdef _HANDLER_WEBSOCKET_H_
#undef BNO055_UPDATE_ON_WEBSOCKET
#define BNO055_UPDATE_ON_WEBSOCKET false
#endif
#else
#ifdef _HANDLER_WEBSOCKET_H_
#undef BNO055_UPDATE_ON_WEBSOCKET
#define BNO055_UPDATE_ON_WEBSOCKET  true
#endif
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


 
Adafruit_BNO055 bno = Adafruit_BNO055(BNO055_INIT_ID);

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

/*
typedef adafruit_bno055_offsets_t offset_t;
class   BNO055_callibration {
  private:
    String   config_file       = "/BNO055_callibration.json";
    uint32_t config_file_ctime = 0;
    
  public:
    BNO055_callibration();
    BNO055_callibration(String c_file);
    
    bool     run();
    bool     is_calibrated();
    bool     has_callibration_file();
    bool     delete_callibration_file();
    uint32_t get_ctime();
    uint32_t get_age();
    void     print_callibration();
    bool     store_callibration();
    bool     load_callibration();
};
*/

struct info_BNO055_t {
  uint8_t       message_BNO055_info_id;
  uint8_t       message_BNO055_data_id;
  uint32_t      poolEvery                           = BNO055_POOL_EVERY;
  uint32_t      SDAPort                             = BNO055_SDA_PORT;
  uint32_t      SCLPort                             = BNO055_SCL_PORT;
  bool          update_only_if_has_websocket_client = BNO055_UPDATE_ON_WEBSOCKET;
  uint32_t      last_loop                           =   0;

  char          sensor_name[12]                     =  ""; /**< sensor name */
  float         sensor_max_value                    = 0.0; /**< maximum value of this sensor's value in SI units */
  float         sensor_min_value                    = 0.0; /**< minimum value of this sensor's value in SI units */
  float         sensor_resolution                   = 0.0; /**< smallest difference between two values reported by this sensor */
  int32_t       sensor_version                      =   0; /**< version of the hardware + driver */
  int32_t       sensor_id                           =   0; /**< unique sensor identifier */
  int32_t       sensor_min_delay                    =   0; /**< min delay in microseconds between events. zero = not a constant rate */
  
  // BNO055_callibration cal;
};
info_BNO055_t      info_BNO055_data;

message_funcs_t    message_BNO055_info_funcs;
message            message_BNO055_info_msg;
message_funcs_t    message_BNO055_data_funcs;
message            message_BNO055_data_msg; 
 

 

 


void    message_BNO055_info_tester   ( message* msg ) {
  DBG_SERIAL.println( F("message_BNO055_info_tester") ); 
  DBG_SERIAL.flush(); 
}

void    message_BNO055_info_initer   ( message* msg ) {
  message_BNO055_info_updater  ( msg );
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
  // DBG_SERIAL.println( F( "message_BNO055_info_to_json START" ) );
  
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

  // DBG_SERIAL.println( F( "message_BNO055_info_to_json END" ) );
  // DBG_SERIAL.flush();
}








void    message_BNO055_data_tester   ( message* msg ) {}

void    message_BNO055_data_initer   ( message* msg ) {
  /* Initialise the sensor */

  DBG_SERIAL.println( F("Starting BNO055"    ) );
  DBG_SERIAL.print  ( F("SDA Port: "         ) ); DBG_SERIAL.println( info_BNO055_data.SDAPort );
  DBG_SERIAL.print  ( F("SCL Port: "         ) ); DBG_SERIAL.println( info_BNO055_data.SCLPort );

  DBG_SERIAL.flush();
  
  bno.setPorts( info_BNO055_data.SDAPort, info_BNO055_data.SCLPort );

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
  
  message_BNO055_data_updater( msg );
  
  //BNO055_callibration();
  
  //info_BNO055_data.cal       = BNO055_callibration();

  info_BNO055_data.last_loop = millis();
}

void    message_BNO055_data_updater  ( message* msg ) {
#ifdef _HANDLER_SERVER_H_
  if ( webserver_data.busy ) {
    DBG_SERIAL.println( "message_BNO055_data_updater. server busy" );
    DBG_SERIAL.flush();
    return;
  }
#endif

#ifdef _HANDLER_WEBSOCKET_H_
  if ( info_BNO055_data.update_only_if_has_websocket_client ) {
    DBG_SERIAL.println( "message_BNO055_data_updater. update_only_if_has_websocket_client" );
    DBG_SERIAL.flush();
    if ( websocket_data.has_client == 0 ) {
      DBG_SERIAL.println( "message_BNO055_data_updater. update_only_if_has_websocket_client. no client" );
      DBG_SERIAL.flush();
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
  // DBG_SERIAL.println( F("message_BNO055_info_to_json START") );
  
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
  DBG_SERIAL.print( F("systemStatus: "                  ) ); DBG_SERIAL.print  ( system_status     );
  DBG_SERIAL.print( F(" selfTestResults: "              ) ); DBG_SERIAL.print  ( self_test_results );
  DBG_SERIAL.print( F(" systemError: "                  ) ); DBG_SERIAL.println( system_error      );
  */

  j_BNO055_data[    F("systemStatus"                    ) ] = system_status;
  j_BNO055_data[    F("selfTestResults"                 ) ] = self_test_results;
  j_BNO055_data[    F("systemError"                     ) ] = system_error;
  
  
  /* Get the four calibration values (0..3) */
  /* Any sensor data reporting 0 should be ignored, */
  /* 3 means 'fully calibrated" */
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  /* The data should be ignored until the system calibration is > 0 */
  /* Display the individual values */
  /*
  DBG_SERIAL.print( F("calibrationSystem: "             ) ); DBG_SERIAL.print  ( system, DEC );
  DBG_SERIAL.print( F(" calibrationGyro: "              ) ); DBG_SERIAL.print  ( gyro  , DEC );
  DBG_SERIAL.print( F(" calibrationAccel: "             ) ); DBG_SERIAL.print  ( accel , DEC );
  DBG_SERIAL.print( F(" calibrationMag: "               ) ); DBG_SERIAL.println( mag   , DEC );
  */
  
  j_BNO055_data[    F("calibrationSystem"               ) ] = system;
  j_BNO055_data[    F("calibrationGyro"                 ) ] = gyro;
  j_BNO055_data[    F("calibrationAccel"                ) ] = accel;
  j_BNO055_data[    F("calibrationMag"                  ) ] = mag;
  
  
  sensors_event_t event;
  bno.getEvent(&event);

  /* Display the floating point data */
  /*
  DBG_SERIAL.print( F("orientationX: "                  ) ); DBG_SERIAL.print  ( event.orientation.x, 4 );
  DBG_SERIAL.print( F(" orientationY: "                 ) ); DBG_SERIAL.print  ( event.orientation.y, 4 );
  DBG_SERIAL.print( F(" orientationZ: "                 ) ); DBG_SERIAL.print  ( event.orientation.z, 4 );
  DBG_SERIAL.print( F(" temperature: "                  ) ); DBG_SERIAL.println( bno.getTemp()          );
  */
  
  j_BNO055_data[    F("orientationX"                    ) ] = event.orientation.x;
  j_BNO055_data[    F("orientationY"                    ) ] = event.orientation.y;
  j_BNO055_data[    F("orientationZ"                    ) ] = event.orientation.z;
  j_BNO055_data[    F("temperature"                     ) ] = bno.getTemp();

  
  
  
  String text;
  jsonToString(json, text);
  msg->set_message(text);

  // DBG_SERIAL.println( F("message_BNO055_info_to_json END") );
  // DBG_SERIAL.flush();
}








/*
https://github.com/adafruit/Adafruit_BNO055/blob/master/examples/restore_offsets/restore_offsets.ino
*/


/*
BNO055_callibration::BNO055_callibration()                                   { DBG_SERIAL.println("BNO055_callibration :: init"); }
BNO055_callibration::BNO055_callibration(String c_file): config_file(c_file) { DBG_SERIAL.print  ("BNO055_callibration :: init: config file: "); DBG_SERIAL.println(c_file); }

bool BNO055_callibration::run() {
  DBG_SERIAL.println("BNO055_callibration :: run");
  if ( is_calibrated() ) { // is calibrated
    DBG_SERIAL.println("BNO055_callibration :: run :: is calibrated");
    if ( has_callibration_file() ) { // is calibrated. has file = end
      DBG_SERIAL.println("BNO055_callibration :: run :: is calibrated :: has file");
      return true;
    } else { // is calibrated. no file = save calibration
      DBG_SERIAL.println("BNO055_callibration :: run :: is calibrated :: no file :: storing");
      if ( store_callibration() ) { // save calibration
        DBG_SERIAL.println("BNO055_callibration :: run :: is calibrated :: no file :: storing :: stored");
        return true;
      } else { // failed saving file
        DBG_SERIAL.println("BNO055_callibration :: run :: is calibrated :: no file :: failed storing");
        if ( has_callibration_file() ) {
          DBG_SERIAL.println("BNO055_callibration :: run :: is calibrated :: no file :: failed storing :: deleting");
          delete_callibration_file();
        }
        return false;
      }
    }
  } else { // not calibrated
    DBG_SERIAL.println("BNO055_callibration :: run :: not calibrated");
    if ( has_callibration_file() ) { // not calibrated. has file
      DBG_SERIAL.println("BNO055_callibration :: run :: not calibrated :: has file :: loading");
      if ( load_callibration() ) { // success loading file = end
        DBG_SERIAL.println("BNO055_callibration :: run :: not calibrated :: has file :: loading :: loaded");
        return true;
      } else { // failed loading file. delete broken file
        DBG_SERIAL.println("BNO055_callibration :: run :: not calibrated :: has file :: loading :: failed loading");
        if ( delete_callibration_file() ) {
          DBG_SERIAL.println("BNO055_callibration :: run :: not calibrated :: has file :: loading :: failed loading :: failed delete");
          return false;
        }
        return false;
      }
    } else {  // not calibrated. no file = sit and wait
      DBG_SERIAL.println("BNO055_callibration :: run :: not calibrated :: no file");
      return false;
    }
  }
}

bool BNO055_callibration::is_calibrated() {
  bool c = bno.isFullyCalibrated();
  DBG_SERIAL.print  ("BNO055_callibration :: is calibrated: "); DBG_SERIAL.println(c);
  return c;
}

bool BNO055_callibration::has_callibration_file() {
  bool e = SPIFFS.exists(config_file);
  DBG_SERIAL.print  ("BNO055_callibration :: exists: "); DBG_SERIAL.println(e);
  return e;
}

bool BNO055_callibration::delete_callibration_file() {
  DBG_SERIAL.print  ("BNO055_callibration :: delete: "); 
  if ( has_callibration_file() ) {
    DBG_SERIAL.println("BNO055_callibration :: delete: exists"); 
    return SPIFFS.remove(config_file);
  } else {
    DBG_SERIAL.println("BNO055_callibration :: delete: does not exists"); 
    return false;
  }
}

uint32_t BNO055_callibration::get_ctime() { 
  DBG_SERIAL.print  ("BNO055_callibration :: ctime: "); DBG_SERIAL.println(config_file_ctime);
  return config_file_ctime; 
}

uint32_t BNO055_callibration::get_age()   { 
  if (config_file_ctime == 0) {
    DBG_SERIAL.print  ("BNO055_callibration :: age: "); DBG_SERIAL.println(0);
    return 0; 
  } else {
    uint32_t age = millis() - config_file_ctime;
    DBG_SERIAL.print  ("BNO055_callibration :: age: "); DBG_SERIAL.println(age);
    return age; 
  }
}

void BNO055_callibration::print_callibration() {
  offset_t offsets_data;
  
  bno.getSensorOffsets(offsets_data);
  
  DBG_SERIAL.println("CALLIBRATION DATA:");
  DBG_SERIAL.print  (" Accelerometer: ");
  DBG_SERIAL.print  (offsets_data.accel_offset_x); DBG_SERIAL.print  (" ");
  DBG_SERIAL.print  (offsets_data.accel_offset_y); DBG_SERIAL.print  (" ");
  DBG_SERIAL.print  (offsets_data.accel_offset_z); DBG_SERIAL.println(" ");

  DBG_SERIAL.print  (" Gyro: ");
  DBG_SERIAL.print  (offsets_data.gyro_offset_x ); DBG_SERIAL.print  (" ");
  DBG_SERIAL.print  (offsets_data.gyro_offset_y ); DBG_SERIAL.print  (" ");
  DBG_SERIAL.print  (offsets_data.gyro_offset_z ); DBG_SERIAL.println(" ");

  DBG_SERIAL.print  (" Mag: ");
  DBG_SERIAL.print  (offsets_data.mag_offset_x  ); DBG_SERIAL.print  (" ");
  DBG_SERIAL.print  (offsets_data.mag_offset_y  ); DBG_SERIAL.print  (" ");
  DBG_SERIAL.print  (offsets_data.mag_offset_z  ); DBG_SERIAL.println(" ");

  DBG_SERIAL.print  (" Accel Radius: ");
  DBG_SERIAL.println(offsets_data.accel_radius);

  DBG_SERIAL.print  (" Mag Radius: ");
  DBG_SERIAL.println(offsets_data.mag_radius);
}

bool BNO055_callibration::store_callibration() {
  // get data from bno
  // write json
  
  DBG_SERIAL.println("BNO055_callibration :: store_callibration");
  
  if ( !is_calibrated() ) {
    DBG_SERIAL.println("BNO055_callibration :: store_callibration :: Not calibrated yet");
    return false;
  }
  
  offset_t offsets_data;
  bno.getSensorOffsets(offsets_data);
  
  jsonBuffer_t jsonBuffer;
  JsonObject& json            = jsonBuffer.createObject();
  
  if (!json.success()) {
    DBG_SERIAL.println("BNO055_callibration :: store_callibration :: Failed to allocating memory");
    return false;
  }
  
  config_file_ctime           = millis();
  json[ F("time"          ) ] = config_file_ctime;
  
  json[ F("accel_offset_x") ] = offsets_data.accel_offset_x;
  json[ F("accel_offset_y") ] = offsets_data.accel_offset_y;
  json[ F("accel_offset_z") ] = offsets_data.accel_offset_z;

  json[ F("gyro_offset_x" ) ] = offsets_data.gyro_offset_x;
  json[ F("gyro_offset_y" ) ] = offsets_data.gyro_offset_y;
  json[ F("gyro_offset_z" ) ] = offsets_data.gyro_offset_z;

  json[ F("mag_offset_x"  ) ] = offsets_data.mag_offset_x;
  json[ F("mag_offset_y"  ) ] = offsets_data.mag_offset_y;
  json[ F("mag_offset_z"  ) ] = offsets_data.mag_offset_z;

  json[ F("accel_radius"  ) ] = offsets_data.accel_radius;
  json[ F("mag_radius"    ) ] = offsets_data.mag_radius;

  File configFile = SPIFFS.open(config_file, "w");
  if (!configFile) {
    DBG_SERIAL.println("BNO055_callibration :: store_callibration :: Failed to open config file for writing");
    return false;
  }

  DBG_SERIAL.println("BNO055_callibration :: store_callibration :: saving config file");
  json.printTo(configFile);
  DBG_SERIAL.println("BNO055_callibration :: store_callibration :: saved config file");
  return true;
}

bool BNO055_callibration::load_callibration() {
  // load json
  // write to bno
  
  DBG_SERIAL.println("BNO055_callibration :: load_callibration");
  
  if ( !has_callibration_file() ) {
    DBG_SERIAL.print  ("BNO055_callibration :: load_callibration :: No calibration file: ");
    DBG_SERIAL.println(config_file);
    return false;
  }
  
  File configFile = SPIFFS.open(config_file, "r");
  if (!configFile) {
    DBG_SERIAL.print  ("BNO055_callibration :: load_callibration :: Failed to open config file: ");
    DBG_SERIAL.println(config_file);
    return false;
  }

  size_t size = configFile.size();
  if (size > JSON_BUFFER_SIZE) {
    DBG_SERIAL.print  ("BNO055_callibration :: load_callibration :: Config file size is too large. max size: ");
    DBG_SERIAL.print  ( JSON_BUFFER_SIZE );
    DBG_SERIAL.print  ( " file size: "   );
    DBG_SERIAL.println( size             );
    return false;
  }
  
  //https://github.com/esp8266/Arduino/blob/master/libraries/esp8266/examples/ConfigFile/ConfigFile.ino
  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[JSON_BUFFER_SIZE]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);
  
  
  jsonBuffer_t jsonBuffer;
  JsonObject& json            = jsonBuffer.parseObject(buf.get());
  
  if (!json.success()) {
    DBG_SERIAL.print  ("BNO055_callibration :: load_callibration :: Failed to parse config file");
    return false;
  }
  
  offset_t offsets_data;
  config_file_ctime           = json[ F("time"          ) ];
  
  offsets_data.accel_offset_x = json[ F("accel_offset_x") ];
  offsets_data.accel_offset_y = json[ F("accel_offset_y") ];
  offsets_data.accel_offset_z = json[ F("accel_offset_z") ];

  offsets_data.gyro_offset_x  = json[ F("gyro_offset_x" ) ];
  offsets_data.gyro_offset_y  = json[ F("gyro_offset_y" ) ];
  offsets_data.gyro_offset_z  = json[ F("gyro_offset_z" ) ];

  offsets_data.mag_offset_x   = json[ F("mag_offset_x"  ) ];
  offsets_data.mag_offset_y   = json[ F("mag_offset_y"  ) ];
  offsets_data.mag_offset_z   = json[ F("mag_offset_z"  ) ];

  offsets_data.accel_radius   = json[ F("accel_radius"  ) ];
  offsets_data.mag_radius     = json[ F("mag_radius"    ) ];

  DBG_SERIAL.print  ("BNO055_callibration :: load_callibration :: Setting offset");
  bno.setSensorOffsets(offsets_data);
  DBG_SERIAL.print  ("BNO055_callibration :: load_callibration :: Offset set");
  
  return true;
}
*/








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
  message_BNO055_data_msg.set_keep_updated(true);
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