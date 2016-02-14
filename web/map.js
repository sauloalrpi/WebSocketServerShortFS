var GOOGLE_MAPS_EMBEDED_API = "AIzaSyDq3FQM7fki7hXaAMykZ2AJvBInWUIcWLA";
var GOOGLE_MAPS_BASE        = "https://www.google.com/maps/embed/v1/";
var GOOGLE_MAPS_VIEW_MODE   = "view";
var GOOGLE_MAPS_MAP_TYPE    = "satellite"; //roadmap 
var GOOGLE_MAPS_ZOOM        = 18;
var GOOGLE_MAPS_UNITS       = 'metric';
//https://developers.google.com/maps/documentation/embed/guide

function gen_map_url(info) {
  console.log('gen_map_url', info);
  
  var lat     = info.lat     || 0;
  var lon     = info.lon     || 0;
  var heading = info.heading || 0;
  var pitch   = info.pitch   || 0;
  var view    = info.view    || GOOGLE_MAPS_VIEW_MODE;
  var base    = info.base    || GOOGLE_MAPS_BASE;
  var api     = info.api     || GOOGLE_MAPS_EMBEDED_API;
  var zoom    = info.zoom    || GOOGLE_MAPS_ZOOM;
  var type    = info.type    || GOOGLE_MAPS_MAP_TYPE;
  var units   = info.units   || GOOGLE_MAPS_UNITS;

  //waypoints specifies one or more intermediary places to route directions through between the origin and destination. Multiple waypoints can be specified by using the pipe character (|) to separate places (e.g. Berlin,Germany|Paris,France). You can specify up to 20 waypoints. Each waypoint can be either a place name, address or place ID.
  
  var prefix  = base+view+"?key="+api;//+"&mode=walking";
  if ( view = "view" ) {
    return prefix+"&center="+lat+","+lon+"&zoom="+zoom+"&maptype="+type;//+"&units="+units;
  } else
  if ( view = "streetview" ) {
    return prefix+"&location="+lat+","+lon+"&heading="+heading+"&pitch="+pitch;
  }
}

function get_map(prepend) {
  var eID     = 'MAP';
 
  var el      = document.getElementById(eID);
  if ( ! el ) {
    var eType   = 'iframe';
    var par     = document.getElementsByTagName("body")[0];
   
    el = document.createElement(eType);
    if ( prepend ) {
      par.insertBefore( el, par.firstChild );
    } else {
      par.appendChild(el);
    }
    el.id = eID;
    el.setAttribute('allowfullscreen',       true);
  }
  
  return el;
}

function update_map(cfg) {
  console.log('update_map', cfg);
  var map = get_map(cfg.prepend || false);
  var src = gen_map_url(cfg);
  console.log('src', src);
  
  map.setAttribute('width'          ,      cfg.width       || "450"     );
  map.setAttribute('height'         ,      cfg.height      || "250"     );
  map.setAttribute('frameborder'    ,      cfg.frameborder || "0"       );
  map.setAttribute('style'          ,      cfg.style       || "border:0");
  map.setAttribute('src'            ,      src                          );
}