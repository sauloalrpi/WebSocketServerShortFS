#ifndef _HANDLER_SERVER_H_
#define _HANDLER_SERVER_H_

#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

ESP8266WebServer server          = ESP8266WebServer(WEBSERVER_PORT);
File             fsUploadFile;


//"/list"  , HTTP_GET   , handleFileList);
//"/format", HTTP_GET   , handleFormatting);
//"/edit"  , HTTP_GET   , handleEdit);
//"/edit"  , HTTP_PUT   , handleFileCreate);
//"/edit"  , HTTP_DELETE, handleFileDelete);
//"/edit"  , HTTP_POST  , handleFileUploadQuery, handleFileUploadData);
//"/info"  , HTTP_GET   , handleInfo);
//"/upload", HTTP_GET   , handleUploadPage);
//server.onNotFound(handleNotFound);



struct webserver_data_t {
  uint8_t  message_webserver_id;
  String   endpoints            = "edit,edit,GET,,|create,edit,PUT,path,|delete,edit,DELETE,path,|format,format,GET,,|list,list,GET,,dir,json|upload,upload,GET,,dir";
  String   endpoint_order       = "name|endpoint|method|compulsory parameters|optional parameters";
  String   endpoint_sep         = "|";
  String   endpoint_field_sep   = ",";
  bool     busy                 = false;
};
webserver_data_t webserver_data;
message_funcs_t  message_webserver_funcs;
message          message_webserver_msg;

void   init_webserver();

void   message_webserver_tester(    message* msg );
void   message_webserver_init(      message* msg );
void   message_webserver_updater(   message* msg );
void   message_webserver_printer(   message* msg );
void   message_webserver_publisher( message* msg );
void   message_webserver_looper(    message* msg );
void   message_webserver_to_json(   message* msg );


void   handleEdit();
void   handleFileCreate();
void   handleFileDelete();
void   handleFileList();
void   handleFileUploadQuery();
void   handleFileUploadData();
void   handleFormattingQuery();
void   handleFormattingData();
void   handleUploadPage();
bool   handleFileRead(    String path);
void   handleFileListJson(String &dirname);
void   handleFileListHtml(String &dirname);
void   handleNotFound();
const bool   ends_with(String& filename, const char* ext);
const bool   ends_with(String& filename, String      ext);
String formatBytes(size_t bytes);
String getContentType(String filename);
//void   handleRoot();



void   message_webserver_tester(    message* msg ) {}

void   message_webserver_initer(    message* msg ) {
  DBG_SERIAL.println( F("message_webserver_init START") );

  DBG_SERIAL.println( F("Registering WebServer") );
  DBG_SERIAL.print  ( F("Port:"                ) );
  DBG_SERIAL.println( WEBSERVER_PORT             );

  //                                   name, endpoint, method, compulsory parameters, optional parameters

  //SERVER INIT
  server.on( "/edit"  , HTTP_GET   , handleEdit                                  ); // load editor
  server.on( "/edit"  , HTTP_PUT   , handleFileCreate                            ); // create file
  server.on( "/edit"  , HTTP_DELETE, handleFileDelete                            ); // delete file
  server.on( "/edit"  , HTTP_POST  , handleFileUploadQuery, handleFileUploadData ); // first callback is called after the request has ended with all parsed arguments second callback handles file uploads at that location
  server.on( "/format", HTTP_GET   , handleFormattingQuery, handleFormattingData ); // format
  server.on( "/list"  , HTTP_GET   , handleFileList                              ); // list directory
  server.on( "/upload", HTTP_GET   , handleUploadPage                            ); // upload page
  server.onNotFound(handleNotFound); //called when the url is not defined here use it to load content from SPIFFS
  server.begin();


  DBG_SERIAL.println( F("message_webserver_init END") );
  DBG_SERIAL.flush();
}

void   message_webserver_updater(    message* msg )  {
  DBG_SERIAL.println( F("message_webserver_updater START") );
  DBG_SERIAL.println( F("message_webserver_updater END") );
  DBG_SERIAL.flush();
}

void   message_webserver_printer(    message* msg ) {
  DBG_SERIAL.println( F("message_webserver_printer START") );
  
  DBG_SERIAL.print( F("endpoints                               : ")); DBG_SERIAL.println(webserver_data.endpoints         );
  DBG_SERIAL.print( F("endpoint_order                          : ")); DBG_SERIAL.println(webserver_data.endpoint_order    );
  DBG_SERIAL.print( F("endpoint_sep                            : ")); DBG_SERIAL.println(webserver_data.endpoint_sep      );
  DBG_SERIAL.print( F("endpoint_field_sep                      : ")); DBG_SERIAL.println(webserver_data.endpoint_field_sep);
  DBG_SERIAL.print( F("busy                                    : ")); DBG_SERIAL.println(webserver_data.busy              );

  DBG_SERIAL.println( F("message_webserver_printer END") );
  DBG_SERIAL.flush();
}

void   message_webserver_publisher(    message* msg ) {
  DBG_SERIAL.println( F("message_webserver_publisher START") );
  
  String text;
  msg->pop_message(text);
  DBG_SERIAL.print("message_webserver_publisher");
  DBG_SERIAL.println(text);

#ifdef _HANDLER_WEBSOCKET_H_
  webSocket.broadcastTXT( text );
#endif

  DBG_SERIAL.println( F("message_webserver_publisher END") );
  DBG_SERIAL.flush();
}

void   message_webserver_looper(    message* msg ) {
//  DBG_SERIAL.println( F("message_webserver_looper START") );
  server.handleClient();
//  DBG_SERIAL.println( F("message_webserver_looper END") );
//  DBG_SERIAL.flush();
}

void   message_webserver_to_json(    message* msg ) {
  DBG_SERIAL.println( F("message_webserver_to_json START") );

  StaticJsonBuffer<JSON_BUFFER_SIZE> jsonBuffer;
  JsonObject& json  = jsonBuffer.createObject();

  message_webserver_updater(msg);
  
  json["_type"] = "webserver_info";
  json["_id"  ] = millis();

  JsonObject& j_info                             = json  .createNestedObject("data"  );

  j_info[ F("endpoints"                         )] = webserver_data.endpoints;
  j_info[ F("endpoint_order"                    )] = webserver_data.endpoint_order;
  j_info[ F("endpoint_sep"                      )] = webserver_data.endpoint_sep;
  j_info[ F("endpoint_field_sep"                )] = webserver_data.endpoint_field_sep;
  j_info[ F("busy"                              )] = webserver_data.busy;

  String text;
  jsonToString(json, text);
  msg->set_message(text);

  DBG_SERIAL.println( F("message_webserver_to_json END") );
  DBG_SERIAL.flush();
}





void   init_webserver() {  
  DBG_SERIAL.println( F("init_webserver START") );
  
  message_webserver_funcs.tester      = message_webserver_tester   ;
  message_webserver_funcs.initer      = message_webserver_initer   ;
  message_webserver_funcs.printer     = message_webserver_printer  ;
  message_webserver_funcs.publisher   = message_webserver_publisher;
  message_webserver_funcs.updater     = message_webserver_updater  ;
  message_webserver_funcs.looper      = message_webserver_looper   ;
  webserver_data.message_webserver_id = messages.size();
  message_webserver_msg               = message("Webserver Info", -1, 0, message_webserver_funcs);
  messages.push_back( &message_webserver_msg );

  DBG_SERIAL.println( F("init_webserver END") );
}












void   handleEdit() {
 if(!handleFileRead("/edit.htm")) {
  server.send(404, "text/plain", "FileNotFound");
 }
}

void   handleFileCreate() {
  if(server.args() == 0) {
    return server.send(500, "text/plain", "No path given");
  }
  
  if(!server.hasArg("path")) { 
    return server.send(500, "text/plain", "No path given");
  }
  
  String path = server.arg("path");
  
  DBG_SERIAL.println("handleFileCreate: " + path);
  if(path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if(SPIFFS.exists(path))
    return server.send(500, "text/plain", "FILE EXISTS");
  File file = SPIFFS.open(path, "w");
  if(file)
    file.close();
  else
    return server.send(500, "text/plain", "CREATE FAILED");
  server.send(200, "text/plain", "");
  path = String();
}

void   handleFileDelete() {
  if(server.args() == 0) {
    return server.send(500, "text/plain", "No args given");
  }

  if(!server.hasArg("path")) { 
    return server.send(500, "text/plain", "No path given");
  }

  String path = server.arg("path");

  DBG_SERIAL.println("handleFileDelete: " + path);
  if(path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if(!SPIFFS.exists(path))
    return server.send(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  server.send(200, "text/plain", "");
  path = String();
}

void   handleFileList() {
  String dirname = "/";
  
  if(server.hasArg("dir")) { 
    dirname = server.arg("dir");
  }

  if((dirname != "/") && (!SPIFFS.exists(dirname))) { 
    server.send(404, "text/plain", "FileNotFound: " + dirname);
    return;
  }
  
  if(server.hasArg("json")) { 
    handleFileListJson(dirname);
  } else {
    handleFileListHtml(dirname);
  }  
}


void   handleFileUploadQuery() {
  server.send(200, "text/html", F("<html><body>Uploading</br><a href=\"list?dir=/\">List</a> | <a href=\"stats\">Stats</a></body></html>"));
}

void   handleFileUploadData() {
  if(server.uri() != "/edit") return;
  
  HTTPUpload& upload = server.upload();

  if(upload.status == UPLOAD_FILE_START){
    String filename = upload.filename;
    if(!filename.startsWith("/")) {
      if ( server.hasArg("dir") ) {
        filename = server.arg("dir")+"/"+filename;
        if(!filename.startsWith("/")) {
          filename = "/"+filename;
        }
      } else {
        filename = "/"+filename;
      }
    }
    DBG_SERIAL.print("handleFileUpload Name: "); DBG_SERIAL.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    //DBG_SERIAL.print("handleFileUpload Data: "); 
    DBG_SERIAL.println(upload.currentSize);
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile)
      fsUploadFile.close();
    DBG_SERIAL.print("handleFileUpload Size: "); DBG_SERIAL.println(upload.totalSize);
  }
}

void   handleFormattingQuery() {
  server.send(200, "text/html", F("<html><body><a href=\"list?dir=/\">List</a><br/><a href=\"all\">All</a>Formatting</body></html>"));
}

void   handleFormattingData() {
  SPIFFS.format();
}


void   handleUploadPage() {
  String dirname = "/";
  
  if(server.hasArg("dir")) { 
    dirname = server.arg("dir");
  }

  String html  = "<html><body>";
         html += "<h2>Uploading to: "+dirname+"</h2>";
         html += "<form action=\"edit?dir"+dirname+"\" method=\"post\" enctype=\"multipart/form-data\">Select file to upload: ";
         html += "<input type=\"file\" id=\"path\" name=\"path\"></input>";
         html += "<input type=\"submit\" name=\"send\" value=\"Upload\"></input>";
         html += "</form>";
         html += "<a href=\"list?dir="+dirname+"\">List</a> | <a href=\"status\">Status</a>";
         html += "</body></html>";
         
  server.send(200, "text/html", html);
  html = String();
}

String getContentType(String filename) {
       if(server.hasArg("download") ) return "application/octet-stream";
  else if(filename.endsWith(".htm" )) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css" )) return "text/css";
  else if(filename.endsWith(".js"  )) return "application/javascript";
  else if(filename.endsWith(".png" )) return "image/png";
  else if(filename.endsWith(".gif" )) return "image/gif";
  else if(filename.endsWith(".jpg" )) return "image/jpeg";
  else if(filename.endsWith(".ico" )) return "image/x-icon";
  else if(filename.endsWith(".xml" )) return "text/xml";
  else if(filename.endsWith(".pdf" )) return "application/x-pdf";
  else if(filename.endsWith(".zip" )) return "application/x-zip";
  else if(filename.endsWith(".gz"  )) return "application/x-gzip";
  return "text/plain";
}

bool   handleFileRead(String path) {
  DBG_SERIAL.println("handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path);
  String pathWithGz  = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file   = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void   handleFileListJson(String &dirname) {
  DBG_SERIAL.println("handleFileList JSON: " + dirname);
  Dir dir = SPIFFS.openDir(dirname);

  String output = "[";
  while(dir.next()){
    File   entry = dir.openFile("r");
    String fn    = String(entry.name()).substring(1);
    bool isDir   =  false;
    DBG_SERIAL.println("handleFileList HTML :: filename: " + fn);
    if (output != "[") output += ',';
    output += "{\"type\":\"";
    output += (isDir)?"dir":"file";
    output += "\",\"name\":\"";
    output += fn;
    output += "\"}";
    entry.close();
  }
  
  output += "]";
  server.send(200, "text/json", output);
}

void   handleFileListHtml(String &dirname) {
  DBG_SERIAL.println("handleFileList HTML: " + dirname);
  Dir dir     = SPIFFS.openDir(dirname);

  String output  = "<html><body><h1>List: "+dirname+"</h1></br>";
         output += "<table><tr><td>Filename</td><td>Size</td><td>Action</td></tr>";
         
  while(dir.next()){
    File   entry      = dir.openFile("r");
    String fn         = String(entry.name()).substring(1);
    DBG_SERIAL.println("handleFileList HTML :: filename: " + fn);
    bool isDir        =  false;
    output           += "<tr>";
    if ( isDir ) {
      output         += "<td><a href=\"/list?dir=" + dirname + "/" + fn + "\">" + fn + "</a></td>";
      output         += "<td>DIR</td>";
      output         += "<td></td>";
    } else {
      if ( fn.endsWith(".gz") ) {
        String fname  = fn.substring(0, fn.length()-3);
        output       += "<td><a href=\""+ fname + "\">" + fname + "</a> <a href=\""+ fn + "\">.gz</a></td>";
      } else {
        output       += "<td><a href=\""+ fn + "\">" + fn + "</a></td>";
      }
      output         += "<td>" + formatBytes(entry.size()) + "</td>";
      output         += "<td><input type=\"submit\" name=\"Delete\" value=\"Delete\" onclick=\"x=new XMLHttpRequest();x.open('DELETE', '/edit?path="+dirname+fn+"', true);x.send(null);location.reload()\"></input></form></td>";
    }
    output           += "</tr>";
    entry.close();
  }
  output             += "</table></br><a href=\"list?dir=/\">List</a> | <a href=\"upload?dir="+dirname+"\">Upload</a> | <a href=\"info\">info</a> | <a href=\"status\">status</a><br/></body></html>";
  server.send(200, "text/html", output);
}

void   handleNotFound(){
  if(!handleFileRead(server.uri())) {
    server.send(404, "text/plain", "FileNotFound");
  }
}


/*
//http://stackoverflow.com/questions/20446201/how-to-check-if-string-ends-with-txt
const bool ends_with(String& filename, const char* ext) {
  return ends_with(filename, String(ext)) {  
}
const bool ends_with(String& filename, String      ext) {
  return filename.substr(std::max(ext.size(), filename.size())-ext.size()) == ext);
}
*/

/*
void handleRoot()   {
  webserver_data.busy = true; 

  
  if (SPIFFS.exists("/index.html")) {
    File file = SPIFFS.open("/index.html", "r");
    if ( file ) {
#ifdef DEBUG_STEPS
      DBG_SERIAL.println("successfully openned file");
      DBG_SERIAL.print  ("size: ");
      DBG_SERIAL.println(file.size());
#endif

      file.close();
      
    } else {
#ifdef DEBUG_STEPS
      DBG_SERIAL.println("file open failed");
#endif
      
    }
  } else {
#ifdef DEBUG_STEPS
    DBG_SERIAL.println("no file");
#endif
    server.send ( 200, "text/html", F("<html><head><style>td{white-space:nowrap}h1,h2,h3,h4,h5,h6{margin-top:0;margin-bottom:0;display:inline}</style></head><body></body><footer><p>Please upload a SPIFS index.html <small>By Saulo Aflitos - 2016</small></p></footer></html>") );
  }

  yield(); 
  
  webserver_data.busy = false; 
}
*/







#endif //ifndef _HANDLER_SERVER_H_
