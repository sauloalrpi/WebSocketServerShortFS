  /*
  "Source       : https://github.com/dmh2000/node-nmea";
  "Compression  : http://jscompress.com/";
  "Compression  : https://javascript-minifier.com/"
  "HTML Minifier: http://www.willpeavy.com/minifier/"
  "$GPRMC,004936.00,A,5158.43915,N,00540.36940,E,0.057,,110216,,,A*75";
  "$GPVTG,,T,,M,0.057,N,0.105,K,A*25";
  "$GPGGA,004936.00,5158.43915,N,00540.36940,E,1,08,1.29,21.1,M,46.1,M,,*6D";
  "$GPGSA,A,3,27,08,20,29,26,16,18,21,,,,,1.86,1.29,1.34*0C";
  "$GPGSV,4,1,13,04,49,165,25,07,10,333,14,08,12,278,18,10,17,159,18*7E";
  "$GPGSV,4,2,13,13,00,043,,16,72,257,31,18,36,126,29,20,28,054,23*72";
  "$GPGSV,4,3,13,21,66,077,36,26,59,180,33,27,42,281,28,29,11,092,30*7A";
  "$GPGSV,4,4,13,32,02,353,20*4F";
  "$GPGLL,5158.43915,N,00540.36940,E,004936.00,A,A*6B";
  */

var num_cols      = 8;
var start_time    = new Date();
var first_packet  = null;
var packet_number = 0;
//uncomment for relase, comment for testing
/*  
var websocket_url = "ws://"+location.hostname+":"+(getPort()+1)+"/";
var info_url      = location.protocol + "//" + location.hostname + ":" + getPort() + "/info";
*/
//uncomment for test, comment for release
var websocket_url = "ws://192.168.1.140:81/";
var info_url      = "http://192.168.1.140:80/info";

console.log("websocket_url", websocket_url);
console.log("info_url     ", info_url     );
//http://aprs.gids.nl/nmea/#gsa




function getPort(){
      if( location.port    != ""     )                                   { return location.port;  }
 else if((location.protocol== "http" ) || (location.protocol== "http:" )){ return 80;             }
 else if((location.protocol== "https") || (location.protocol== "https:")){ return 443;            }
 else                                                                    { alert("unknown port"); }
}

function getJSON(url) {
  return new Promise(function(resolve, reject) {
    var xhr = new XMLHttpRequest();
    xhr.open("get", url, true);
    xhr.responseType = "json";
    xhr.onload = function() {
      var status = xhr.status;
      if (status == 200) {
        resolve(xhr.response);
      } else {
        reject(status);
      }
    };
    xhr.send();
  });
};

function getElement(eid) {
  return document.getElementById(eid);
}

function createElement(eType) {
  return document.createElement(eType);
}

function createElementOn(eType, par, prepend) {
  var el = createElement(eType);
  if ( prepend ) {
    par.insertBefore( el, par.firstChild );
  } else {
    par.appendChild(el);
  }
  return el;
}

function createElementOnIfNotExists(eType, eID, par, prepend) {
  var el = getElement(eID);
  if ( ! el ) {
    el    = createElementOn(eType, par, prepend);
    el.id = eID;
  }
  return el;
}

function getBody() {
  return document.getElementsByTagName("body")[0];
}

function cleanClass(cls) {
  var els = document.getElementsByClassName("example");
  for ( var e = 0; e < els.length; e++ ) {
    var el = els[e];
    el.innerHTML = "";
  }
}

/* This is the pseudocode you need to follow:
 * It"s a modified version from 
 * http://en.wikipedia.org/wiki/Geographic_coordinate_conversion#Conversion_from_Decimal_Degree_to_DMS

function deg_to_dms ( degfloat )
   Compute degrees, minutes and seconds:
   deg ← integerpart ( degfloat )
   minfloat ← 60 * ( degfloat - deg )
   min ← integerpart ( minfloat )
   secfloat ← 60 * ( minfloat - min )
   Round seconds to desired accuracy:
   secfloat ← round( secfloat, digits )
   After rounding, the seconds might become 60. These two
   if-tests are not necessary if no rounding is done.
   if secfloat = 60
      min ← min + 1
      secfloat ← 0
   end if
   if min = 60
      deg ← deg + 1
      min ← 0
   end if
   Return output:
   return ( deg, min, secfloat )
end function
*/

//http://stackoverflow.com/questions/5786025/decimal-degrees-to-degrees-minutes-and-seconds-in-javascript
function deg_to_dms2(deg) {
   var d = Math.floor (deg);
   var minfloat = (deg-d)*60;
   var m = Math.floor(minfloat);
   var secfloat = (minfloat-m)*60;
   var s = Math.round(secfloat);
   // After rounding, the seconds might become 60. These two
   // if-tests are not necessary if no rounding is done.
   if (s==60) {
     m++;
     s=0;
   }
   if (m==60) {
     d++;
     m=0;
   }
   return ("" + d + ":" + m + ":" + s);
}

function deg_to_dms(dd, lon){
  if (! dd) {
    return '';
  }
  var deg = dd | 0; // truncate dd to get degrees
  var frac = Math.abs(dd - deg); // get fractional part
  var min = (frac * 60) | 0; // multiply fraction by 60 and truncate
  var sec = frac * 3600 - min * 60;
  if (sec==60) {
    min++;
    sec=0;
  }
  if (min==60) {
    deg++;
    min=0;
  }
  var dir = "N";
  if ( dd < 0 ) {
    dir = "S";
  }
  if (lon) {
    dir = "E";
    if ( dd < 0 ) {
      dir = "W";
    }
  }
  return deg + "&deg;" + min + "\'" + sec.toFixed(4) + "\" " + dir;
}

function deg_to_dms_when_requested(dd, lon) {
  var checker_lbl = createElementOnIfNotExists("label", "lat_lon_checker_lbl", getBody(), true);
  var checker     = createElementOnIfNotExists("input", "lat_lon_checker"    , getBody(), true);

  if (checker) {
    checker.setAttribute("type", "checkbox");
  }
  if (checker_lbl) {
    checker_lbl.innerHTML = "Coordinates in Degrees";
    checker_lbl.setAttribute("for", "lat_lon_checker");
  }
  if ( checker.checked ) {
    return deg_to_dms(dd, lon);
  } else {
    return dd;
  }
}

function addKV(class_id, tbl, key, desc, kvs, H) {
  var nkey      = "";
  
  if ( key ) {
    nkey = "_" + key;
  }
  
  if ( ! H ) {
    H = 3;
  }
  
  var H1        = "h"+H;
  var trh       = createElementOnIfNotExists( "tr" , class_id + nkey + "_trh", tbl );
  var tdh       = createElementOnIfNotExists( "td" , class_id + nkey + "_tdh", trh );
  var h1        = createElementOnIfNotExists( H1   , class_id + nkey + H1    , tdh );
  h1.innerHTML  = key;
  tdh.setAttribute("colspan", num_cols);
  
  if ( desc ) {
    var dsc       = createElementOnIfNotExists( "small", class_id + nkey + "_small", tdh );
    dsc.innerHTML = " (" + desc + ")";
  }
  
  var trv       = createElementOnIfNotExists( "tr", class_id + nkey + "_trv_0", tbl );
  var varc      = 0;
  var rowc      = 0;
  for ( var valk in kvs ) {
    if ( ( varc != 0 ) && ((num_cols - varc) == 0) ) {
      varc      = 0;
      rowc     += 1;
      trv       = createElementOnIfNotExists( "tr", class_id + nkey + "_trv_" + rowc, tbl );
    }
    varc       += 2;

    var tdn     = createElementOnIfNotExists( "td", class_id + nkey + "_tdn_" + valk, trv );
    var tdv     = createElementOnIfNotExists( "td", class_id + nkey + "_tdv_" + valk, trv );
    var tdnb    = createElementOnIfNotExists( "b" , class_id + nkey + "_b_"   + valk, tdn );
    var valv    = kvs[valk];
    var nvals   = genericTranslator(class_id, key, valk, valv);
    var valkk   = nvals[0];
    var valvv   = nvals[1];
    
    if ( valvv === "") {
      valvv     = "NaN";
    }
    
    if ( typeof(valvv) === "undefined" || valvv === null ) {
      valvv     = "NaN";
    }
    
    tdnb.innerHTML = valkk;
    tdv .innerHTML = valvv;
    if ( key ) {
      tdv .setAttribute("class", key)
    }
  }
  
  if ( varc != num_cols ) {
    var tdn = createElementOnIfNotExists( "td", class_id + nkey + "_tdn_LEFT", trv );
    tdn.setAttribute("colspan", (num_cols-varc));
  }
}

function nmeaParserGeneric(class_id, tbl, nmeaData, H) {
  var nmeaDataID   = nmeaData.talker_type_id;
  var nmeaDataDESC = nmeaData.talker_type_desc;
  var kvs          = {};
  
  cleanClass(nmeaDataID);
  
  for ( var key in nmeaData ) {
    if ( key == "talker_type_id"   ) { continue; }
    if ( key == "talker_type_desc" ) { continue; }
    if ( key == "id"               ) { continue; }
    
    var val       = nmeaData[key];
    
    if (typeof val === "object") {
      //console.log(nmeaDataID, "OBJ: key", key, "val", val);
      
    } else {
      //console.log(nmeaDataID, "VAL: key", key, "val", val); 
      kvs[ key ] = val;
      
    }
  }
  addKV(class_id, tbl, nmeaDataID, nmeaDataDESC, kvs, H);
}

function genericTranslator(class_id, key, valk, valv) {
  //console.log("genericTranslator ", "class_id", class_id, "key", key, "valk", valk, "valv", valv);
  var dict = global_converter;
  
  if ( class_id in dict ) {
    if ( key ) {
      if ( key in dict[class_id] ) {
        if ( valk in dict[class_id][key] ) {
          //console.log("C1");
          return dict[class_id][key][valk](valk,valv);
        } else 
        if ( "*" in dict[class_id][key] ) {
          //console.log("C2");
          return dict[class_id][key]["*"](valk,valv);
        } 
      } else 
      if ( "*" in dict[class_id] ) {
        if ( valk in dict[class_id]["*"] ) {
          //console.log("C3");
          return dict[class_id]["*"][valk](valk,valv);
        } else
        if ( "*" in dict[class_id]["*"] ) {
          //console.log("C4");
          return dict[class_id]["*"]["*"](valk,valv);
        }
      }
    } else {
      if ( valk in dict[class_id] ) {
        //console.log("C5");
        return dict[class_id][valk](valk,valv);
      } else 
      if ( "*" in dict[class_id] ) {
        //console.log("C6");
        return dict[class_id]["*"](valk,valv);
      }
    }
  }
  //console.log("C7");
  return [valk, valv];
}

function nmeaParserGPGSA(class_id, tbl, nmeaData, H) {
  /*
  GPGSA VAL: key mode val A
  GPGSA VAL: key fix  val 3
  GPGSA OBJ: key sat  val ["23", "26", "29", "21", "02", "05", "16", "31", "20"]
  GPGSA VAL: key pdop val 1.75
  GPGSA VAL: key hdop val 0.96
  GPGSA VAL: key vdop val 1.46
  */
  nmeaData.sat = nmeaData.sat.join(",");
  nmeaParserGeneric(class_id, tbl, nmeaData);
}

function nmeaParserGPGSV(class_id, tbl, nmeaData, H) {
  var nmeaDataID = nmeaData.talker_type_id;
  var mnum       = nmeaData.mnum;
  var sats       = nmeaData.sat;
  
  var rows       = [];
  var titles     = [];
  var satnums    = [];
  
  for ( var s=0; s < sats.length; s++ ) {
    var satnum = (((mnum-1)*4)+1) + s;
    var sat    = sats[s];
    var row    = ["sat #"+satnum];
    for ( var k in sat ) {
      var v       = sat[k];
      var nvals   = genericTranslator(class_id, nmeaDataID, k, v);
      var valkk   = nvals[0];
      var valvv   = nvals[1];
      
      row.push(valvv);
      if ( titles.indexOf(valkk) == -1 ) {
        titles.push(valkk);
      }
    }
    satnums.push(satnum);
    rows.push( row );
  }
  
  delete nmeaData.msgs;
  delete nmeaData.mnum;
  delete nmeaData.sat;

  /*
  GPGSV VAL: key talker_type_id val GSV
  GPGSV VAL: key talker_type_desc val Global Satellites in View
  GPGSV VAL: key msgs val 3
  GPGSV VAL: key mnum val 2
  GPGSV VAL: key count val 12
  GPGSV OBJ: key sat val [Object, Object, Object, Object]
  */
  
  nmeaParserGeneric(class_id, tbl, nmeaData);
  
  var tr1       = createElementOnIfNotExists( "tr"   , class_id + "statelites_tr"    , tbl  );
  var td1       = createElementOnIfNotExists( "td"   , class_id + "statelites_td"    , tr1  );
  var tbl2      = createElementOnIfNotExists( "table", class_id + "statelites_td_tbl", td1  );
  var tr2h      = createElementOnIfNotExists( "tr"   , class_id + "statelites_td_trh", tbl2 );
  
  td1 .setAttribute("colspan", num_cols);
  
  for ( var t=0; t < titles.length; t++ ) {
    var title     = titles[t];
    if ( t == 0 ) {
      var td2h      = createElementOnIfNotExists( "td"   , class_id + "statelites_td_tdh_title"  , tr2h );
      var b         = createElementOnIfNotExists( "b"    , class_id + "statelites_td_tdh_title_b", td2h );
      b.innerHTML   = "Sat Num";
    }
    var td2h      = createElementOnIfNotExists( "td"   , class_id + "statelites_td_tdh_"+title     , tr2h );
    var b         = createElementOnIfNotExists( "b"    , class_id + "statelites_td_tdh_"+title+"_b", td2h );
    b.innerHTML = title;
  }

  for ( var r=0; r<rows.length; r++ ) {
    var row       = rows[r];
    var satnum    = satnums[r];
    var tr3       = createElementOnIfNotExists( "tr"   , class_id + "statelites_td_tr_"+satnum, tbl2 );
    for ( var e=0; e<row.length; e++) {
      var el      = row[e];
      var td3     = createElementOnIfNotExists( "td"   , class_id + "statelites_td_td_"+satnum+"_"+e, tr3 );
      if ( e == 0 ) {
        var b     = createElementOnIfNotExists( "b"    , class_id + "statelites_td_td_"+satnum+"_"+e+"_b", td3 );
        b.innerHTML   = el;
      } else {
        td3.innerHTML = el;
      }
    }
  }
}

function processGpsRegister(obj) {
  var class_id  = "GPS_REGISTER";
  var sep       = obj._sep || "|";
  var _id       = obj._id;
  var data      = obj.data;
  var rows      = data.split(sep);
  /*
  console.log( "data", data );
  console.log( "sep" , sep  );
  console.log( "_id" , _id  );
  console.log( "rows", rows );
  */
  var base      = createElementOnIfNotExists( "div"  , class_id         , getBody() );
  var tbl       = createElementOnIfNotExists( "table", class_id + "_tbl", base      );
  var bth       = createElementOnIfNotExists( "tr"   , class_id + "_trh", tbl       );
  var btd       = createElementOnIfNotExists( "td"   , class_id + "_tdh", bth       );
  var bh2       = createElementOnIfNotExists( "h2"   , class_id + "_h2" , btd       );
  bh2.innerHTML = class_id;
  btd.setAttribute("colspan", num_cols);
  
  for ( var rn=0; rn < rows.length; rn++ ) {
    var row = rows[rn];
    //console.log( "rn", rn, "row", row );
    var nmeaData = null;
    
    try {
      var nmeaData = NMEA.parse(row);
    } catch(e) {
      console.warn("No native NMEA parser for", row);
      console.warn(e);
    }
    
    //id: "GPRMC", time: "192017.00", valid: "A", latitude: "51.97405683", longitude: "5.67269583"
    if ( nmeaData ) {
      var dataID = nmeaData.talker_type_id;
      if ( dataID in nmeaIdParsers ) {
        nmeaIdParsers[dataID](class_id, tbl, nmeaData);
      } else {
        console.warn( "no local printer for NMEA id:", dataID );
        console.warn( "data:", nmeaData );
      }
    }
  }
}

function processInfo(obj) {
  var class_id  = "INFO";
  var data      = obj.data;

  var base      = createElementOnIfNotExists( "div"  , class_id         , getBody() );
  var tbl       = createElementOnIfNotExists( "table", class_id + "_tbl", base      );
  var bth       = createElementOnIfNotExists( "tr"   , class_id + "_trh", tbl       );
  var btd       = createElementOnIfNotExists( "td"   , class_id + "_tdh", bth       );
  var bh2       = createElementOnIfNotExists( "h2"   , class_id + "_h2" , btd       );
  bh2.innerHTML = class_id;
  btd.setAttribute("colspan", num_cols);
  
  var keys = data._keys;
  for ( var k=0; k<keys.length; k++  ) {
    var key       = keys[k];
    
    addKV(class_id, tbl, key, null, data[key]);
  }
}

function processStatus(obj) {
   console.log("STATUS");
   var data = obj.data;
   console.log(data);
}

function processUnknown(obj) {
   console.warn("unknown type: ", type);
   console.warn(obj);
}

function processDataJson(data) {
  var type  = data._type;
  var proc  = null;
  try {
    proc = typesProcessors[type];
  } catch(err) {
    proc = typesProcessors["_unknown"];
  }
  proc( data );
}

function processDataText(data) {
 console.log("GOT TEXT");
 console.log(data);
}

function processData(data) {
  //console.log("processData", data);
  
  if (typeof data === "object") {
    processDataJson(data);
    return;
  } else {
    var isJSON = false;
    
    try {
      var jdata = JSON.parse(data);
      isJSON    = true;
      //console.log("processing as JSON");
    } catch(err) {
      //console.log("error processing as JSON");
    }
    
    if ( isJSON ) {
      processDataJson(jdata);
    } else {
      processDataText(data);
    }
  }
}

function connectWebSocket() {
  var connection    = new WebSocket(websocket_url, ["arduino"]);

  connection.onopen = function () {
    connection.send("Connect " + new Date());
  };
  
  connection.onerror = function (error) {
   console.log("WebSocket Error ", error);
  };
  
  connection.onclose = function(e) {
    console.warn("reconnecting");
    init();
  }
  
  var vc = 0;
  connection.onmessage = function (e) {
    //console.log("vc", vc);
    //vc += 1;
    packet_number += 1;
    if ( ! first_packet ) {
      first_packet = new Date();
    }

    update_status("Got Socket Packet #"+packet_number);
    processData(e.data);
    /*
    if ( vc == 2 ) {
      connection.close();
    }
    */
  };
}

function update_status(status) {
  var update_t  = new Date();
  var ela_t     = ((update_t - first_packet) / 1000.0);
  if (!first_packet) {
    ela_t       = 0;
  }
  var ela_t_s   = ela_t.toFixed(0);
  var freq      =    (packet_number / ela_t) .toFixed(2);
  var freq_rev  = (1/(packet_number / ela_t)).toFixed(2);
  
  var class_id  = "STATUS";

  var base      = createElementOnIfNotExists( "div"  , class_id         , getBody() );
  var tbl       = createElementOnIfNotExists( "table", class_id + "_tbl", base      );
  var bth       = createElementOnIfNotExists( "tr"   , class_id + "_trh", tbl       );
  var btd       = createElementOnIfNotExists( "td"   , class_id + "_tdh", bth       );
  var bh2       = createElementOnIfNotExists( "h2"   , class_id + "_h2" , btd       );
  bh2.innerHTML = class_id;
  btd.setAttribute("colspan", num_cols);
  
  var kvs       = {
    "Start Time"    : start_time   ,
    "Last Update"   : update_t     ,
    "Elapsed Time"  : ela_t_s      ,
    "Frequency"     : freq         ,
    "Time/Packet"   : freq_rev     ,
    "Packets"       : packet_number,
    "Status"        : status       ,
  };
  
  addKV(class_id, tbl, null, "Client status", kvs)
}

function init() {
  getJSON(info_url).then(
    function(data) {
      update_status("Got info");
      processData(data);
      connectWebSocket();
    }, 
    function(status) { //error detection....
      console.warn("not able to get info at: ", info_url);
      console.warn("status: ", status);
      console.warn("reconecting");
      init();
    }
  );
}




var typesProcessors = {
  "status"      : processStatus     ,
  "info"        : processInfo       ,
  "gps_register": processGpsRegister,
  "_unknown"    : processUnknown
}

var nmeaParserGPRMC = nmeaParserGeneric;
if (typeof GOOGLE_MAPS_EMBEDED_API !== "undefined") 
{
  console.log("parsing RMC with map");
  nmeaParserGPRMC = function (class_id, tbl, nmeaData, H) {
    var cfg = {
      "lat"    : nmeaData.latitude ,
      "lon"    : nmeaData.longitude,
      "heading": nmeaData.course   ,
      "prepend": true
    };
    console.log("parsing RMC", cfg);
    //update_map(cfg);
    nmeaParserGeneric(class_id, tbl, nmeaData, H);
  }
} else {
  console.log("parsing RMC default. no map");
}

var nmeaIdParsers = {
  "RMC": nmeaParserGPRMC  ,
  "GGA": nmeaParserGeneric,
  "VTG": nmeaParserGeneric,
  "GLL": nmeaParserGeneric,
  "GSV": nmeaParserGPGSV  ,
  "GSA": nmeaParserGPGSA
};

//http://www.gpsinformation.org/dale/nmea.htm#GGA
var VTG_modes={"A":"autonomous","D":"differential","E":"Estimated","N":"not valid","S":"Simulator"};

var GGA_fix_qualities=[
"invalid",
"GPS fix (SPS)",
"DGPS fix",
"PPS fix",
"Real Time Kinematic",
"Float RTK",
"estimated (dead reckoning) (2.3 feature)",
"Manual input mode",
"Simulation mode"
];

var global_converter  = {
  "STATUS": {
    "Elapsed Time": function(k,v) { var v2 = v + " s"  ; return [k,v2]; },
    "Frequency"   : function(k,v) { var v2 = v + " p/s"; return [k,v2]; },
    "Time/Packet" : function(k,v) { var v2 = v + " s"  ; return [k,v2]; },
  },
  "INFO": {
    "global": {
      "mac"              : function(k,v) { return ["MAC Address", v]; },
      "flashChipSize"    : function(k,v) { var v2 =  (v / 1024.0).toFixed(1) + " Kbp";           return [k,v2]; },
      "flashChipSpeed"   : function(k,v) { var v2 = ((v / 1024.0) / 1024.0).toFixed(1) + " Mhz"; return [k,v2]; },
      "flashChipRealSize": function(k,v) { var v2 =   v + " Kbp";                                return [k,v2]; },
      "flashChipSizeByID": function(k,v) { var v2 =   v + " Kbp";                                return [k,v2]; },
      "freeHeap"         : function(k,v) { var v2 =  (v / 1024.0).toFixed(1) + " Kbp";           return [k,v2]; },
      "freeSketchSpace"  : function(k,v) { var v2 =   v + " Kbp";                                return [k,v2]; },
      "sketchSize"       : function(k,v) { var v2 =   v + " Kbp";                                return [k,v2]; },
      "gpsPoolEvery"     : function(k,v) { var v2 =   v + " s";                                  return [k,v2]; },
      "htmlSize"         : function(k,v) { var v2 =   v + " chars";                              return [k,v2]; },
      "jsonBufferSize"   : function(k,v) { var v2 =   v + " chars";                              return [k,v2]; },
      "gpsReadFor"       : function(k,v) { var v2 =   v + " ms";                                 return [k,v2]; },
      "systemInstructionCyclesPerSecond": function(k,v) { var v2 = ((v / 1024.0) / 1024.0).toFixed(1) + " Mhz"; return [k,v2]; },
    }
  },
  "GPS_REGISTER": {
    "RMC":{
      "latitude" : function(k,v) { var v2 = deg_to_dms_when_requested(v      ); return [k,v2]; },
      "longitude": function(k,v) { var v2 = deg_to_dms_when_requested(v, true); return [k,v2]; },
      "time"     : function(k,v) { var v2 = Math.floor(v).toString().match(new RegExp(".{1," + 2 + "}", "g")).join(":"); return [k,v2]; },
      "date"     : function(k,v) { var v2 = Math.floor(v).toString().match(new RegExp(".{1," + 2 + "}", "g")).join("/"); return [k,v2]; },
      "status"   : function(k,v) { var v2=v; if (v == "A") { v2="Active"; } else { v2="Void"; }; return [k,v2]; },
      "sog"      : function(k,v) { return ["Speed over the ground (knots)", v]; },
      "variation": function(k,v) { return ["Magnetic Variation"           , v]; },
      "mode"     : function(k,v) { return ["Track angle (degrees)"        , v]; },
      "valid"    : function(k,v) { var v2=v; if (v == "A") { v2="Active"; } else { v2="Void"; }; return ["selection of 2D or 3D fix"      ,v2]; },
    },
    "VTG": {
      "mode"     : function(k,v) { return [k,VTG_modes[v]]; }
    },
    "GGA": {
      "latitude" : function(k,v) { var v2 = deg_to_dms_when_requested(v      ); return [k,v2]; },
      "longitude": function(k,v) { var v2 = deg_to_dms_when_requested(v, true); return [k,v2]; },
      "time"     : function(k,v) { var v2 = Math.floor(v).toString().match(new RegExp(".{1," + 2 + "}", "g")).join(":"); return [k,v2]; },
      "fix"      : function(k,v) { return ["Fix quality", GGA_fix_qualities[v]]; },
      "hdop"     : function(k,v) { return ["Horizontal dilution of position",v]; },
    },
    "GSA": {
      "mode"     : function(k,v) { var v2=v; if (v == "A") { v2="Automatic"; } else { v2="Manual"; }; return ["selection of 2D or 3D fix"      ,v2]; },
      "pdop"     : function(k,v) { return ["Dilution of precision"          ,v]; },
      "hdop"     : function(k,v) { return ["Horizontal dilution of position",v]; },
      "vdop"     : function(k,v) { return ["Vertical dilution of precision" ,v]; },
    },
    "GLL": {
      "utc_time" : function(k,v) { var v2 = Math.floor(v).toString().match(new RegExp(".{1," + 2 + "}", "g")).join(":"); return [k,v2]; },
      "latitude" : function(k,v) { var v2 = deg_to_dms_when_requested(v      ); return [k,v2]; },
      "longitude": function(k,v) { var v2 = deg_to_dms_when_requested(v, true); return [k,v2]; },
    },
    "GSV": {
      "prn"      : function(k,v) { return ["Satellite PRN number"                    ,v]; },
      "el"       : function(k,v) { return ["Elevation (degrees)"                     ,v]; },
      "az"       : function(k,v) { return ["Azimuth (degrees)"                       ,v]; },
      "ss"       : function(k,v) { return ["Signal to Noise Ratio (higher is better)",v]; },
      }
  }
};
/*
websocket_url ws://192.168.1.140:81/
info_url      http://192.168.1.140:80/info
class_id INFO         key global   valk mac                              valv 18:FE:34:FE:4E:CE
class_id INFO         key global   valk chipId                           valv 16666318
class_id INFO         key global   valk flashChipId                      valv 1458376
class_id INFO         key global   valk flashChipSize                    valv 4194304
class_id INFO         key global   valk flashChipSpeed                   valv 40000000
class_id INFO         key global   valk flashChipMode                    valv QIO
class_id INFO         key global   valk flashChipRealSize                valv 4096
class_id INFO         key global   valk flashChipSizeByID                valv 4096
class_id INFO         key global   valk SDKVersion                       valv 1.5.1(e67da894)
class_id INFO         key global   valk bootVersion                      valv 31
class_id INFO         key global   valk bootMode                         valv 1
class_id INFO         key global   valk freeRAM                          valv 19
class_id INFO         key global   valk freeHeap                         valv 23416
class_id INFO         key global   valk freeSketchSpace                  valv 736
class_id INFO         key global   valk sketchSize                       valv 284
class_id INFO         key global   valk systemInstructionCyclesPerSecond valv 163072000
class_id INFO         key global   valk cpuFreqMHz                       valv 160
class_id INFO         key global   valk systemUptime                     valv  00:01:26
class_id INFO         key global   valk lastSystemRestartInfo            valv Fatal exception:4 flag:1 (WDT) epc1:0x402012d8 epc2:0x00000000 epc3:0x00000000 excvaddr:0x00000000 depc:0x00000000
class_id INFO         key global   valk gpsPoolEvery                     valv 0
class_id INFO         key global   valk gpsStartField                    valv $GPRMC
class_id INFO         key global   valk gpsEndField                      valv $GPGLL
class_id INFO         key global   valk gpsRegisterFieldSep              valv |
class_id INFO         key global   valk webSocketMaxClient               valv 5
class_id INFO         key global   valk htmlSize                         valv 1100
class_id INFO         key global   valk jsonBufferSize                   valv 1536
class_id INFO         key global   valk gpsBaudrate                      valv 38400
class_id INFO         key global   valk gpsRxPort                        valv 13
class_id INFO         key global   valk gpsTxPort                        valv 15
class_id INFO         key global   valk gpsReadFor                       valv 2000
class_id GPS_REGISTER key RMC      valk time                             valv 124235.00
class_id GPS_REGISTER key RMC      valk valid                            valv A
class_id GPS_REGISTER key RMC      valk latitude                         valv 51.97370883
class_id GPS_REGISTER key RMC      valk longitude                        valv 5.67197900
class_id GPS_REGISTER key RMC      valk sog                              valv 0.031
class_id GPS_REGISTER key RMC      valk course                           valv 0
class_id GPS_REGISTER key RMC      valk date                             valv 130216
class_id GPS_REGISTER key RMC      valk mode                             valv 
class_id GPS_REGISTER key RMC      valk variation                        valv NaN
class_id GPS_REGISTER key VTG      valk course                           valv 0
class_id GPS_REGISTER key VTG      valk knots                            valv 0.031
class_id GPS_REGISTER key VTG      valk kph                              valv 0.057
class_id GPS_REGISTER key VTG      valk mode                             valv D
class_id GPS_REGISTER key GGA      valk time                             valv 124235.00
class_id GPS_REGISTER key GGA      valk latitude                         valv 51.97370883
class_id GPS_REGISTER key GGA      valk longitude                        valv 5.67197900
class_id GPS_REGISTER key GGA      valk fix                              valv 2
class_id GPS_REGISTER key GGA      valk satellites                       valv 7
class_id GPS_REGISTER key GGA      valk hdop                             valv 1.11
class_id GPS_REGISTER key GGA      valk altitude                         valv 74
class_id GPS_REGISTER key GGA      valk aboveGeoid                       valv 46.1
class_id GPS_REGISTER key GGA      valk dgpsUpdate                       valv 
class_id GPS_REGISTER key GGA      valk dgpsReference                    valv 0000
class_id GPS_REGISTER key GSA      valk mode                             valv A
class_id GPS_REGISTER key GSA      valk fix                              valv 3
class_id GPS_REGISTER key GSA      valk sat                              valv 13,09,07,20,02,05,28
class_id GPS_REGISTER key GSA      valk pdop                             valv 2.08
class_id GPS_REGISTER key GSA      valk hdop                             valv 1.11
class_id GPS_REGISTER key GSA      valk vdop                             valv 1.76
class_id GPS_REGISTER key GSV      valk count                            valv 14
class_id GPS_REGISTER key GSV_sat1 valk prn                              valv 2
class_id GPS_REGISTER key GSV_sat1 valk el                               valv 12
class_id GPS_REGISTER key GSV_sat1 valk az                               valv 223
class_id GPS_REGISTER key GSV_sat1 valk ss                               valv 36
class_id GPS_REGISTER key GSV_sat2 valk prn                              valv 5
class_id GPS_REGISTER key GSV_sat2 valk el                               valv 66
class_id GPS_REGISTER key GSV_sat2 valk az                               valv 259
class_id GPS_REGISTER key GSV_sat2 valk ss                               valv 44
class_id GPS_REGISTER key GSV_sat3 valk prn                              valv 7
class_id GPS_REGISTER key GSV_sat3 valk el                               valv 54
class_id GPS_REGISTER key GSV_sat3 valk az                               valv 63
class_id GPS_REGISTER key GSV_sat3 valk ss                               valv 29
class_id GPS_REGISTER key GSV      valk count                            valv 14
class_id GPS_REGISTER key GLL      valk latitude                         valv 51.97370883
class_id GPS_REGISTER key GLL      valk longitude                        valv 5.67197900
class_id GPS_REGISTER key GLL      valk utc_time                         valv 124235.00
class_id GPS_REGISTER key GLL      valk status                           valv data valid 
class_id GPS_REGISTER key GLL      valk mode                             valv DGPS
*/
init();