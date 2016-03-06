#ifndef _HANDLER_SERVER_ENDPOINTS_H_
#define _HANDLER_SERVER_ENDPOINTS_H_

/*
addEndpoint( "list"  , "/list"  , ""    , "dir,json" , HTTP_GET   , handleFileList                              );
addEndpoint( "upload", "/upload", ""    , "dir"      , HTTP_GET   , handleUploadPage                            );
addEndpoint( "upload", "/upload", "path", ""         , HTTP_POST  , handleFileUploadQuery, handleFileUploadData );
addEndpoint( "edit"  , "/edit"  , ""    , ""         , HTTP_GET   , handleEdit                                  );
addEndpoint( "create", "/edit"  , "path", ""         , HTTP_PUT   , handleFileCreate                            );
addEndpoint( "delete", "/edit"  , "path", ""         , HTTP_DELETE, handleFileDelete                            );
addEndpoint( "format", "/format", ""    , ""         , HTTP_GET   , handleFormatPage                        );
addEndpoint( "format", "/format", ""    , ""         , HTTP_DELETE, handleFormatData                        );

server.serveStatic("/favicon.ico", SPIFFS, "/favicon.ico");
server.onNotFound(handleNotFound); //called when the url is not defined here use it to load content from SPIFFS
*/

String getContentType( const String& filename );

void   handleFileCreate();
void   handleFileDelete();
bool   handleFileRead( const String& path );

void   handleIndex();
void   handleEdit();

void   handleFormatPage();
void   handleFormatQuery();
void   handleFormatData();

void   handleFileUploadPage();
void   handleFileUploadQuery();
void   handleFileUploadData();

void   handleFileListPage();
void   handleFileListJson( Dir& dir );
void   handleFileListHtml( Dir& dir, const String dirname );

void   handleEndpointsPage();
void   handleEndpointsJson();  
void   handleEndpointsHtml();

void   handleNotFound();






String getContentType(const String& filename) {
       if(server.hasArg("download" )) return "application/octet-stream";
  else if(filename.endsWith(".gz"  )) return "application/x-gzip";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css" )) return "text/css";
  else if(filename.endsWith(".json")) return "application/json";
  else if(filename.endsWith(".js"  )) return "application/javascript";
  else if(filename.endsWith(".png" )) return "image/png";
  else if(filename.endsWith(".ico" )) return "image/x-icon";
  else if(filename.endsWith(".gif" )) return "image/gif";
  else if(filename.endsWith(".jpg" )) return "image/jpeg";
  else if(filename.endsWith(".htm" )) return "text/html";
  else if(filename.endsWith(".xml" )) return "text/xml";
  else if(filename.endsWith(".pdf" )) return "application/x-pdf";
  else if(filename.endsWith(".zip" )) return "application/x-zip";
  return "text/plain";
}







void   handleFileCreate() {
  if (  server.args() == 0   ) { return server.send(500, "text/plain", "No path given"); }
  if ( !server.hasArg("path")) { return server.send(500, "text/plain", "No path given"); }
  
  String path = server.arg("path");
  
  DBG_SERIAL.println("handleFileCreate: " + path);
  if( path == "/"            ) { return server.send(500, "text/plain", "BAD PATH"   );   }
  if( SPIFFS.exists(path)    ) { return server.send(500, "text/plain", "FILE EXISTS");   }
  
  File file = SPIFFS.open(path, "w");
  if(file) {
    file.close();
  }
  else {
    return server.send(500, "text/plain", "CREATE FAILED");
  }
    
  server.send(200, "text/plain", "");
  path = String();
}

void   handleFileDelete() {
  if(  server.args() == 0   ) { return server.send(500, "text/plain", "No args given"); }
  if( !server.hasArg("path")) { return server.send(500, "text/plain", "No path given"); }

  String path = server.arg("path");

  DBG_SERIAL.println("handleFileDelete: " + path);
  if(  path == "/"          ) { return server.send(500, "text/plain", "BAD PATH"    );  }
  if( !SPIFFS.exists(path)  ) { return server.send(404, "text/plain", "FileNotFound");  }
  
  SPIFFS.remove(path);
  server.send(200, "text/plain", "");
  path = String();
}

bool   handleFileRead( const String& rpath ) {
  String path = rpath;
  
  DBG_SERIAL.println("handleFileRead: " + path);
  
  if(path.endsWith("/")) {
    path += "index.html";
  }
  
  String contentType    = getContentType(path);
  String pathWithGz     = path         + ".gz";
  String pathWithHTML   = path         + ".html";
  String pathWithHTMLGz = pathWithHTML + ".gz";
  
  if (  SPIFFS.exists(pathWithGz) || SPIFFS.exists(pathWithHTML) || SPIFFS.exists(pathWithHTMLGz) || SPIFFS.exists(path) ) {
    if( SPIFFS.exists(pathWithGz) ) {
      path        = pathWithGz;
    }
    else if( SPIFFS.exists(pathWithHTML) ) {
      path        = pathWithHTML;
      contentType = getContentType(pathWithHTML);
    }
    else if( SPIFFS.exists(pathWithHTMLGz) ) {
      path        = pathWithHTMLGz;
      contentType = getContentType(pathWithHTML);
    }
    
    File   file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();

    if (sent == file.size()) {
      return true;
    }
    else {
      return false;
    }
  }
  
  return false;
}



void   handleIndex() {
  if ( handleFileRead(server.uri()) ) { return; }
    
  String links = "";
  genLinks(links);
  server.send(200, "text/html", "<html><h1>Index</h1><br/>"+links+"</body></html>" );  
}

void   handleEdit() {
  if( !handleFileRead("/edit.html") ) {
    server.send(404, "text/plain", "FileNotFound");
  }
}



void   handleFormatPage() {
  if ( handleFileRead(server.uri()) ) { return; }
  
  String links = "";
  genLinks(links);
  
  String html  = "<html><body><h1>Format SPIF FS</h1><br/>";
  html += "<form action=\""+server.uri()+"\" method=\"DELETE\">Format Disk: ";
  html += "<input type=\"submit\" name=\"YES\" value=\"YES\"></input>";
  html += "</form>";
  html += "<br/>";
  html += links+"</body></html>";
  
  server.send(200, "text/html", html );
}

void   handleFormatQuery() {
  if ( handleFileRead(server.uri()) ) { return; }
  
  String links = "";
  genLinks(links);
  server.send(200, "text/html", "<html><body><h1>Format SPIF FS</h1><h2>Formatting</h2><br/>"+links+"</body></html>" );
}

void   handleFormatData() {
  SPIFFS.format();
}



void   handleFileUploadPage() {
  if ( handleFileRead(server.uri()) ) { return; }
  
  String dirname = "/";
  
  if(server.hasArg("dir")) { 
    dirname = server.arg("dir");
  }

  String links = "";
  genLinks(links);
  
  String html  = "<html><body><h1>Upload</h1>";
         html += "<h2>Uploading to: "+dirname+"</h2>";
         html += "<form action=\""+server.uri()+"?dir"+dirname+"\" method=\"POST\" enctype=\"multipart/form-data\">Select file to upload: ";
         html += "<input type=\"file\" id=\"path\" name=\"path\"></input>";
         html += "<input type=\"submit\" name=\"send\" value=\"Upload\"></input>";
         html += "</form>";
         html += "<a href=\"list?dir="+dirname+"\">Refresh</a>";
         html += links;
         html += "</body></html>";
         
  server.send(200, "text/html", html);
  html = String();
}

void   handleFileUploadQuery() {
  String links = "";
  genLinks(links);
  server.send(200, "text/html", "<html><body><h1>Upload</h1><h2>Uploading</h2></br>"+links+"</body></html>");
}

void   handleFileUploadData() {
  if( server.uri() != "/upload" ) { return; }
  
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
    filename     = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    //DBG_SERIAL.print("handleFileUpload Data: "); 
    DBG_SERIAL.println(upload.currentSize);
    
    if(fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
    }
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile) {
      fsUploadFile.close();
    }
    
    DBG_SERIAL.print("handleFileUpload Size: "); DBG_SERIAL.println(upload.totalSize);
  }
}



void   handleFileListPage() {
  String dirname = "/";
  
  if( server.hasArg("dir") ) { dirname = server.arg("dir"); }

  if((dirname != "/") && (!SPIFFS.exists(dirname))) { 
    server.send(404, "text/plain", "FileNotFound: " + dirname);
    return;
  }

  Dir dir = SPIFFS.openDir(dirname);
  
  if(server.hasArg("json")) { 
    handleFileListJson(dir);
  } else {
    handleFileListHtml(dir, dirname);
  }
}

void   handleFileListJson(Dir& dir) {
  String output = "[";
  
  while( dir.next() ) {
    File   entry = dir.openFile("r");
    String fn    = String(entry.name()).substring(1);
    bool isDir   =  false;
    DBG_SERIAL.println("handleFileList HTML :: filename: " + fn);
    
    if (output.length() != 1) {
      output += ',';
    }
    
    output += "{\"type\":\"";
    output += (isDir)?"dir":"file";
    output += "\",\"name\":\"";
    output += fn;
    output += "\",\"size\":";
    output += String(entry.size());
    output += "}";
    
    entry.close();
  }
  
  output += "]";
  output  = "{\"_type\":\"filelist\",\"data\":"+output+"}";
  server.send(200, "application/json", output);
}

void   handleFileListHtml(Dir& dir, const String dirname) {
  String output  = "<html><body><h1>List: "+dirname+"</h1></br>";
         output += "<table><tr><td>Filename</td><td>Size</td><td>Action</td></tr>";
         
  while(dir.next()){
    File   entry      = dir.openFile("r");
    String fn         = String(entry.name()).substring(1);
    DBG_SERIAL.println("handleFileList HTML :: filename: " + fn);
    bool isDir        =  false;
    output           += "<tr>";
    if ( isDir ) {
      output         += "<td><a href=\"/"+server.uri()+"?dir=" + dirname + "/" + fn + "\">" + fn + "</a></td>";
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
  String links = "";
  genLinks(links);

  output             += "</table></br><a href=\""+server.uri()+"?dir=/\">Refresh</a>";
  output             += links;
  //<a href=\"upload?dir="+dirname+"\">Upload</a> | <a href=\"info\">info</a> | <a href=\"status\">status</a>
  output             += "</body></html>";
  server.send(200, "text/html", output);
}



void   handleEndpointsPage() {
  if(server.hasArg("json")) { 
    handleEndpointsJson();
  } else {
    handleEndpointsHtml();
  }
}

void   handleEndpointsJson() {
  String output;
  gen_endpoints_json(output);
  
  DBG_SERIAL.print  ( "handleEndpointsJson: " );
  DBG_SERIAL.println( output                  );
  
  server.send(200, "application/json", output);
}

void   handleEndpointsHtml() {
  String output = "<html><body><h1>Endpoints</h1><br/><table>";
  int rc = 0;
  for (auto& eps: webserver_data.endpoints) {    
    rc += 1;
    if ( rc == 1 ) {
      output += "<tr><th>Name</th><th>EndPoint</th><th>Method</th><th>Compulsory Parameters</th><th>Optional Parameters</th></tr>";
    }
    
    output += "<tr>";
    
    auto& name                  = eps[0];
    auto& endpoint              = eps[1];
    auto& method                = eps[2];
    auto& compulsory_parameters = eps[3];
    auto& optional_parameters   = eps[4];

    output += "<td>" + name    +"</td>";
    
    if ( method == "GET" ) {
      output += "<td><a href=\"" + endpoint + "\">" + endpoint + "</a></td>";
    } else {
      output += "<td>" + endpoint + "</td>";
    }
    
    output += "<td>" + method                + "</td>";
    output += "<td>" + compulsory_parameters + "</td>";
    output += "<td>" + optional_parameters   + "</td>";
    
    output += "</tr>";
  }
  output += "</table></body></html>";
  
  server.send(200, "text/html", output);
}



void   handleNotFound(){
  DBG_SERIAL.print  ( "handleNotFound: " );
  DBG_SERIAL.println( server.uri()       );
  
  if( !handleFileRead(server.uri()) ) {
    DBG_SERIAL.println( "handleNotFound: DOES NOT EXISTS" );
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


#endif //_HANDLER_SERVER_ENDPOINTS_H_