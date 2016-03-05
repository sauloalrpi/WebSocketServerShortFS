#ifndef _HANDLER_SERVER_H_
#define _HANDLER_SERVER_H_

/* VARIABLES - SERVERS */
#ifndef WEBSERVER_PORT
#define WEBSERVER_PORT           80
#endif

#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

ESP8266WebServer server          = ESP8266WebServer(WEBSERVER_PORT);
File             fsUploadFile;

struct webserver_data_t {
  uint8_t        message_webserver_id;
  string_vec_vec endpoints;
  String         endpoint_order           = "name,endpoint,method,compulsory parameters,optional parameters";
  String         endpoints_json           = "";
  String         URLinks                  = "";
  bool           busy                     = false;
};
webserver_data_t webserver_data;
message_funcs_t  message_webserver_funcs;
message          message_webserver_msg;

void        init_webserver       ();
void        server_init_webserver();
void        handleInfoWebserver  ();

void        message_webserver_tester   ( message* msg );
void        message_webserver_init     ( message* msg );
void        message_webserver_updater  ( message* msg );
void        message_webserver_printer  ( message* msg );
void        message_webserver_publisher( message* msg );
void        message_webserver_looper   ( message* msg );
void        message_webserver_to_json  ( message* msg );

void        getEndPointsStr            ( String& ep    );
void        genLinks                   ( String& links );
void        gen_endpoints_json         ( String& res   );

typedef ESP8266WebServer::THandlerFunction webserver_h_func;
void        addEndpoint( const String name, const String endpoint, const String compulsory_parameters, const String optional_parameters, HTTPMethod method, webserver_h_func handler, webserver_h_func handlern );
void        addEndpoint( const String name, const String endpoint, const String compulsory_parameters, const String optional_parameters, HTTPMethod method, webserver_h_func handler );
void        addEndpoint( const String name, const String endpoint, const String compulsory_parameters, const String optional_parameters, HTTPMethod method );




#include "handler_server_endpoints.h"




void        addEndpoint( const String name, const String endpoint, const String compulsory_parameters, const String optional_parameters, HTTPMethod method, webserver_h_func handler, webserver_h_func handlern ) {
  
  addEndpoint(name                     ,
              endpoint                 ,
              compulsory_parameters    ,
              optional_parameters      ,
              method
              );
              
  server.on( endpoint.c_str(), method, handler, handlern );
}

void        addEndpoint( const String name, const String endpoint, const String compulsory_parameters, const String optional_parameters, HTTPMethod method, webserver_h_func handler ) {
  
  addEndpoint(name                     ,
              endpoint                 ,
              compulsory_parameters    ,
              optional_parameters      ,
              method
              );
  
  server.on( endpoint.c_str(), method, handler );
}

void        addEndpoint( const String name, const String endpoint, const String compulsory_parameters, const String optional_parameters, HTTPMethod method ) {

  String methodStr = "";
  switch( method ) {
    case HTTP_ANY   : methodStr = "ANY"   ; break;
    case HTTP_GET   : methodStr = "GET"   ; break;
    case HTTP_POST  : methodStr = "POST"  ; break;
    case HTTP_PUT   : methodStr = "PUT"   ; break;
    case HTTP_PATCH : methodStr = "PATCH" ; break;
    case HTTP_DELETE: methodStr = "DELETE"; break;
    default:
      methodStr = "ANY";
  }
  
  DBG_SERIAL.print  ( "addEndpoint :: "          );
  DBG_SERIAL.print  ( " name: "                  );
  DBG_SERIAL.print  (   name                     );
  DBG_SERIAL.print  ( " endpoint: "              );
  DBG_SERIAL.print  (   endpoint                 );
  DBG_SERIAL.print  ( " method: "                );
  DBG_SERIAL.print  (   methodStr                );
  DBG_SERIAL.print  ( " compulsory_parameters: " );
  DBG_SERIAL.print  (   compulsory_parameters    );
  DBG_SERIAL.print  ( " optional_parameters: "   );
  DBG_SERIAL.println(   optional_parameters      );

  string_vec sv;
  sv.push_back(name                 );
  sv.push_back(endpoint             );
  sv.push_back(methodStr            );
  sv.push_back(compulsory_parameters);
  sv.push_back(optional_parameters  );

  webserver_data.endpoints.push_back(sv);
}




void        getEndPointsStr            ( String& ep    ) {
  DBG_SERIAL.println( "getEndPoints:" );

  int ce  = 0;
      ep += "[";

  for (auto& eps: webserver_data.endpoints) {
    ce += 1;
    
    if ( ce != 1 ) {
      ep += ",";
    }
    
    ep += "[";
    int cf = 0;
    DBG_SERIAL.print  ( " field: " );
    
    for (auto& f: eps) {
      DBG_SERIAL.print  ( " " );
      DBG_SERIAL.print  ( f   );

      cf += 1;
      if ( cf != 1 ) {
        ep += ",";
      }
      
      ep += "\"" + f + "\"";
    }
    DBG_SERIAL.println( "" );
    ep += "]";
  }
  ep += "]";
}

void        genLinks                   ( String& links ) {
  if ( webserver_data.URLinks.length() != 0 ) {
    links = webserver_data.URLinks.c_str();
    return;
  }

  DBG_SERIAL.println( "genLinks: " );

  for (auto& ep: webserver_data.endpoints) {
    auto& name     = ep[0];
    auto& endpoint = ep[1];
    auto& method   = ep[2];
    if ( method == "GET" ) {
      webserver_data.URLinks += " | <a href=\""+endpoint+"\">"+name+"</a>";
    }
  }
  webserver_data.URLinks += " | ";
  
  links = webserver_data.URLinks.c_str();
  DBG_SERIAL.print  ( "genLinks: " );
  DBG_SERIAL.println(     links    );
}

void        gen_endpoints_json         ( String& res   ) {
  if (webserver_data.endpoints_json.length() != 0) {
    res += webserver_data.endpoints_json.c_str();
    return;
  }
  
  DBG_SERIAL.println( F("gen_endpoints_json START") );
  DBG_SERIAL.flush();

  jsonBuffer_t        jsonBuffer;
  JsonObject& json  = jsonBuffer.createObject();
  
  json["_type"]     = "webserver/info";
  json["_id"  ]     = millis();  

  JsonObject& j_info                               = json.createNestedObject("data");
  
  JsonArray&  epa                                  = jsonBuffer.createArray();
  
  DBG_SERIAL.println( "creating JSON endpoints:" );
  for (auto& eps: webserver_data.endpoints) {
    JsonArray&  epf = jsonBuffer.createArray();
    DBG_SERIAL.print  ( " fields: " );
    
    for (auto& epv: eps) {
      DBG_SERIAL.print( " " );
      DBG_SERIAL.print( epv );
      epf.add(epv.c_str());
    }
    
    DBG_SERIAL.println( "" );
    DBG_SERIAL.flush();
    
    epa.add(epf);
    delay(0);
  }
  

  j_info[ F("endpoints"                        ) ] = epa;
  j_info[ F("endpointOrder"                    ) ] = webserver_data.endpoint_order;
  
  
  jsonToString(json, res);

  webserver_data.endpoints_json = res.c_str();
  
  DBG_SERIAL.print  ( F("gen_endpoints_json RES: ") );
  DBG_SERIAL.println( res                           );
  DBG_SERIAL.flush();
  
  DBG_SERIAL.println( F("gen_endpoints_json END") );
  DBG_SERIAL.flush();  
}





void        message_webserver_tester   ( message* msg ) {}

void        message_webserver_initer   ( message* msg ) {
  DBG_SERIAL.println( F("message_webserver_init START") );

  DBG_SERIAL.println( F("Registering WebServer") );
  DBG_SERIAL.print  ( F("Port:"                ) ); DBG_SERIAL.println( WEBSERVER_PORT             );
  
  //SERVER INIT
  addEndpoint( "create"   , "/edit"     , "path", ""         , HTTP_PUT   , handleFileCreate                            );
  addEndpoint( "delete"   , "/edit"     , "path", ""         , HTTP_DELETE, handleFileDelete                            );

  addEndpoint( "index"    , "/"         , ""    , ""         , HTTP_GET   , handleIndex                                 );
  addEndpoint( "edit"     , "/edit"     , ""    , ""         , HTTP_GET   , handleEdit                                  );

  addEndpoint( "format"   , "/format"   , ""    , ""         , HTTP_GET   , handleFormatPage                            );
  addEndpoint( "format"   , "/format"   , ""    , ""         , HTTP_DELETE, handleFormatQuery, handleFormatData         );

  addEndpoint( "upload"   , "/upload"   , ""    , "dir"      , HTTP_GET   , handleFileUploadPage                        );
  addEndpoint( "upload"   , "/upload"   , "path", ""         , HTTP_POST  , handleFileUploadQuery, handleFileUploadData );

  addEndpoint( "list"     , "/list"     , ""    , "dir,json" , HTTP_GET   , handleFileListPage                          );
    
  addEndpoint( "endpoints", "/endpoints", ""    , "json"     , HTTP_GET   , handleEndpointsPage                         );
  
  
  if(SPIFFS.exists("/favicon.ico")) {
    server.serveStatic("/favicon.ico", SPIFFS, "/favicon.ico");
  }
  
  server.onNotFound(handleNotFound); //called when the url is not defined here use it to load content from SPIFFS
  
  message_webserver_updater( msg );
  
  server.begin();

  DBG_SERIAL.println( F("message_webserver_init END") );
  DBG_SERIAL.flush();
}

void        message_webserver_updater  ( message* msg ) {
  DBG_SERIAL.println( F("message_webserver_updater START") );
  
  message_webserver_to_json( msg );
  
  DBG_SERIAL.println( F("message_webserver_updater END") );
  DBG_SERIAL.flush();
}

void        message_webserver_looper   ( message* msg ) {
//  DBG_SERIAL.println( F("message_webserver_looper START") );
  server.handleClient();
//  DBG_SERIAL.println( F("message_webserver_looper END") );
//  DBG_SERIAL.flush();
}

void        message_webserver_printer  ( message* msg ) {
  DBG_SERIAL.println( F("message_webserver_printer START") );
  
  String ep;
  getEndPointsStr( ep );
  DBG_SERIAL.print  ( F("endpoints                               : ")); DBG_SERIAL.println(ep                               );
  DBG_SERIAL.print  ( F("endpoint_order                          : ")); DBG_SERIAL.println(webserver_data.endpoint_order    );
  DBG_SERIAL.print  ( F("busy                                    : ")); DBG_SERIAL.println(webserver_data.busy              );

  DBG_SERIAL.println( F("message_webserver_printer END") );
  DBG_SERIAL.flush();
}

void        message_webserver_publisher( message* msg ) {
  DBG_SERIAL.println( F("message_webserver_publisher START") );
  
  String text;
  msg->pop_message(text);
  DBG_SERIAL.print(  "message_webserver_publisher" );
  DBG_SERIAL.println(text);

  broadcastMessage( text );

  DBG_SERIAL.println( F("message_webserver_publisher END") );
  DBG_SERIAL.flush();
}

void        message_webserver_to_json  ( message* msg ) {
  DBG_SERIAL.println( F("message_webserver_to_json START") );
  DBG_SERIAL.flush();
  
  String text;
  gen_endpoints_json(text);
  msg->set_message(text);

  DBG_SERIAL.println( F("message_webserver_to_json END") );
  DBG_SERIAL.flush();  
}







void        init_webserver       () {  
  DBG_SERIAL.println( F("init_webserver START") );
  
  message_webserver_funcs.tester      = message_webserver_tester   ;
  message_webserver_funcs.initer      = message_webserver_initer   ;
  message_webserver_funcs.printer     = message_webserver_printer  ;
  message_webserver_funcs.publisher   = message_webserver_publisher;
  message_webserver_funcs.updater     = message_webserver_updater  ;
  message_webserver_funcs.looper      = message_webserver_looper   ;
  webserver_data.message_webserver_id = messages.size();
  message_webserver_msg               = message("Webserver Info", -1, 0, message_webserver_funcs, true);
  messages.push_back( &message_webserver_msg );

  server_init_webserver();
  
  DBG_SERIAL.println( F("init_webserver END") );
}

void        server_init_webserver() {
  DBG_SERIAL.println( F("Registering /webserver/info") );
  addEndpoint("Webserver Info", "/webserver/info", "", "", HTTP_GET, handleInfoWebserver);
}

void        handleInfoWebserver  () {
  server_send_message( message_webserver_msg );
}


#endif //ifndef _HANDLER_SERVER_H_