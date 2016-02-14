#ifndef _HANDLER_SERVER_H_
#define _HANDLER_SERVER_H_


static String formatBytes(size_t bytes);
static String getContentType(String filename);
static bool   handleFileRead(String path);
static void   handleUploadPage();
static void   handleFileUpload();
static void   handleFileDelete();
static void   handleFileCreate();
static void   handleFileListJson(String &dirname);
static void   handleFileListHtml(String &dirname);
static void   handleFileList();
static void   handleFormatting();
static void   handleInfo();
static void   handleRoot();

//format bytes
static String formatBytes(size_t bytes) {
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

static String getContentType(String filename) {
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

static bool   handleFileRead(String path) {
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

static void   handleUploadPage() {
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

static void   handleFileUpload() {
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

static void   handleFileDelete() {
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

static void   handleFileCreate() {
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

static void   handleFileListJson(String &dirname) {
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

static void   handleFileListHtml(String &dirname) {
  DBG_SERIAL.println("handleFileList HTML: " + dirname);
  Dir dir     = SPIFFS.openDir(dirname);

  String output  = "<html><body><h1>List: "+dirname+"</h1></br>";
         output += "<table><tr><td>Filename</td><td>Size</td><td>Action</td></tr>";
         
  while(dir.next()){
    File   entry  = dir.openFile("r");
    String fn     = String(entry.name()).substring(1);
    DBG_SERIAL.println("handleFileList HTML :: filename: " + fn);
    bool isDir    =  false;
    if ( isDir ) {
      output       += "<tr>";
      output       += "<td><a href=\"/list?dir=" + dirname + "/" + fn + "\">" + fn + "</a></td>";
      output       += "<td>DIR</td>";
      output       += "<td></td>";
      output       += "</tr>";
    } else {
      output       += "<tr>";
      output       += "<td><a href=\""+ fn + "\">" + fn + "</a></td>";
      output       += "<td>" + formatBytes(entry.size()) + "</td>";
      output       += "<td><input type=\"submit\" name=\"Delete\" value=\"Delete\" onclick=\"x=new XMLHttpRequest();x.open('DELETE', '/edit?path="+dirname+fn+"', true);x.send(null);location.reload()\"></input></form></td>";
      output       += "</tr>";
    }
    entry.close();
  }
  output           += "</table></br><a href=\"list?dir=/\">List</a> | <a href=\"upload?dir="+dirname+"\">Upload</a> | <a href=\"stats\">Stats</a><br/><table></table></body></html>";
  server.send(200, "text/html", output);
}

static void   handleFileList() {
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

static void   handleFormatting() {
  SPIFFS.format();
}

static void   handleInfo() {
  busy    = true;
  
  StaticJsonBuffer<JSON_BUFFER_SIZE> jsonBuffer;
  JsonObject& json  = jsonBuffer.createObject();

  //DBG_SERIAL.print("json.success(): ");
  //DBG_SERIAL.println(json.success());

  getSelfDynamicInfo();
  infoToJson(json);
  
  int len = json.measureLength();
  
#ifdef DEBUG_STEPS
  DBG_SERIAL.print(   F("handleInfo measureLength: ") );
  DBG_SERIAL.println( len );
  DBG_SERIAL.flush();
#endif

  char temp[JSON_BUFFER_SIZE];
  json.printTo(temp, sizeof(temp)); 
  
#ifdef DEBUG_STEPS
  DBG_SERIAL.print(   F("INFO JSON: ") );
  DBG_SERIAL.println( temp             );
  DBG_SERIAL.println( sizeof(temp)     );
  DBG_SERIAL.flush();
#endif

#ifdef DEBUG_STEPS
  DBG_SERIAL.println(   F("INFO JSON SENDING") );
#endif

  server.send( 200, "application/json", temp );

#ifdef DEBUG_STEPS
  DBG_SERIAL.println(   F("INFO JSON SENT") );
#endif

  //delayy(10);

  busy    = false;
}


/*
static void handleRoot()   {
  busy = true; 

  
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
  
  busy = false; 
}
*/


#endif //ifndef _HANDLER_SERVER_H_
