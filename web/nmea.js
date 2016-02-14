//Source: https://github.com/nherment/node-nmea/tree/master/lib/codes
/** NMEA-0183 Parser-Encoder */


/** NMEA module */
var NMEA = ( function() {

    "use strict";

    /** NMEA public API */
    var nmea = {
    };

    /** private module variables */
    var m_parserList = [];
    var m_encoderList = [];
    var m_errorHandler = null;

    
    
    // =============================================
    // Helper functions
    // =============================================
    
    nmea.m_hex = '0123456789ABCDEF'.split(''),

    nmea.toHexString = function(v) {
      var lsn;
      var msn;

      msn = (v >> 4) & 0x0f;
      lsn = (v >> 0) & 0x0f;
      return m_hex[msn] + m_hex[lsn];
  };

    nmea.padLeft = function(s, len, ch) {
      while(s.length < len) {
          s = ch + s;
      }
      return s;
  };

  // verify the checksum
    nmea.verifyChecksum = function(sentence, checksum) {
      var q;
      var c1;
      var c2;
      var i;

      // skip the $
      i = 1;

      // init to first character
      c1 = sentence.charCodeAt(i);

      // process rest of characters, zero delimited
      for( i = 2; i < sentence.length; ++i) {
          c1 = c1 ^ sentence.charCodeAt(i);
      }

      // checksum is a 2 digit hex value
      c2 = parseInt(checksum, 16);

      // should be equal
      return ((c1 & 0xff) === c2);
  };

  // generate a checksum for  a sentence (no trailing *xx)
    nmea.computeChecksum = function(sentence) {
      var c1;
      var i;

      // skip the $
      i = 1;

      // init to first character    var count;

      c1 = sentence.charCodeAt(i);

      // process rest of characters, zero delimited
      for( i = 2; i < sentence.length; ++i) {
          c1 = c1 ^ sentence.charCodeAt(i);
      }

      return '*' + nmea.toHexString(c1);
  };

  // =========================================
  // field encoders
  // =========================================

  // encode latitude
  // input: latitude in decimal degrees
  // output: latitude in nmea format
  // ddmm.mmm
    nmea.encodeLatitude = function(lat) {
      var d;
      var m;
      var f;
      var h;
      var s;
      var t;
      if(lat === undefined) {
          return '';
      }

      if(lat < 0) {
          h = 'S';
          lat = -lat;
      } else {
          h = 'N';
      }
      // get integer degrees
      d = Math.floor(lat);
      // degrees are always 2 digits
      s = d.toString();
      if(s.length < 2) {
          s = '0' + s;
      }
      // get fractional degrees
      f = lat - d;
      // convert to fractional minutes
      m = (f * 60.0);
      // format the fixed point fractional minutes
      t = m.toFixed(3);
      if(m < 10) {
          // add leading 0
          t = '0' + t;
      }

      s = s + t + ',' + h;
      return s;
  };

  // encode longitude
  // input: longitude in decimal degrees
  // output: longitude in nmea format
  // dddmm.mmm
    nmea.encodeLongitude = function(lon) {
      var d;
      var m;
      var f;
      var h;
      var s;
      var t;

      if(lon === undefined) {
          return '';
      }

      if(lon < 0) {
          h = 'W';
          lon = -lon;
      } else {
          h = 'E';
      }

      // get integer degrees
      d = Math.floor(lon);
      // degrees are always 3 digits
      s = d.toString();
      while(s.length < 3) {
          s = '0' + s;
      }

      // get fractional degrees
      f = lon - d;
      // convert to fractional minutes and round up to the specified precision
      m = (f * 60.0);
      // minutes are always 6 characters = mm.mmm
      t = m.toFixed(3);
      if(m < 10) {
          // add leading 0
          t = '0' + t;
      }
      s = s + t + ',' + h;
      return s;
  };

  // 1 decimal, always meters
    nmea.encodeAltitude = function(alt) {
      if(alt === undefined) {
          return ',';
      }
      return alt.toFixed(1) + ',M';
  };

  // magnetic variation
    nmea.encodeMagVar = function(v) {
      var a;
      var s;
      if(v === undefined) {
          return ',';
      }
      a = Math.abs(v);
      s = (v < 0) ? (a.toFixed(1) + ',E') : (a.toFixed(1) + ',W');
      return nmea.padLeft(s, 7, '0');
  };

  // degrees
    nmea.encodeDegrees = function(d) {
      if(d === undefined) {
          return '';
      }
      return nmea.padLeft(d.toFixed(1), 5, '0');
  };

    nmea.encodeDate = function(d) {
      var yr;
      var mn;
      var dy;

      if(d === undefined) {
          return '';
      }
      yr = d.getUTCFullYear();
      mn = d.getUTCMonth() + 1;
      dy = d.getUTCDate();
      return nmea.padLeft(dy.toString(), 2, '0') + nmea.padLeft(mn.toString(), 2, '0') + yr.toString().substr(2);
  };

    nmea.encodeTime = function(d) {
      var h;
      var m;
      var s;

      if(d === undefined) {
          return '';
      }
      h = d.getUTCHours();
      m = d.getUTCMinutes();
      s = d.getUTCSeconds();
      return nmea.padLeft(h.toString(), 2, '0') + nmea.padLeft(m.toString(), 2, '0') + nmea.padLeft(s.toString(), 2, '0');
  };

    nmea.encodeKnots = function(k) {
      if(k === undefined) {
          return '';
      }
      return nmea.padLeft(k.toFixed(1), 5, '0');
  };

    nmea.encodeValue = function(v) {
      if(v === undefined) {
          return '';
      }
      return v.toString();
  };

    nmea.encodeFixed = function(v, f) {
      if(v === undefined) {
          return '';
      }
      return v.toFixed(f);
  };

  // =========================================
  // field parsers
  // =========================================

  // separate number and units
    nmea.parseAltitude = function(alt, units) {
      var scale = 1.0;
      if(units === 'F') {
          scale = 0.3048;
      }
      return parseFloat(alt) * scale;
  };

  // separate degrees value and quadrant (E/W)
    nmea.parseDegrees = function(deg, quadrant) {
      var q = (quadrant === 'E') ? -1.0 : 1.0;

      return parseFloat(deg) * q;
  };

  // fields can be empty so have to wrap the global parseFloat
    nmea.parseFloatX = function(f) {
      if(f === '') {
          return 0.0;
      }
      return parseFloat(f);
  };

  // decode latitude
  // input : latitude in nmea format
  //      first two digits are degress
  //      rest of digits are decimal minutes
  // output : latitude in decimal degrees
    nmea.parseLatitude = function(lat, hemi) {
      var h = (hemi === 'N') ? 1.0 : -1.0;
      var a;
      var dg;
      var mn;
      var l;
      a = lat.split('.');
      if(a[0].length === 4) {
          // two digits of degrees
          dg = lat.substring(0, 2);
          mn = lat.substring(2);
      } else if(a[0].length === 3) {
          // 1 digit of degrees (in case no leading zero)
          dg = lat.substring(0, 1);
          mn = lat.substring(1);
      } else {
          // no degrees, just minutes (nonstandard but a buggy unit might do this)
          dg = '0';
          mn = lat;
      }
      // latitude is usually precise to 5-8 digits
      return ((parseFloat(dg) + (parseFloat(mn) / 60.0)) * h).toFixed(8);
  };

  // decode longitude
  // first three digits are degress
  // rest of digits are decimal minutes
    nmea.parseLongitude = function(lon, hemi) {
      var h;
      var a;
      var dg;
      var mn;
      h = (hemi === 'E') ? 1.0 : -1.0;
      a = lon.split('.');
      if(a[0].length === 5) {
          // three digits of degrees
          dg = lon.substring(0, 3);
          mn = lon.substring(3);
      } else if(a[0].length === 4) {
          // 2 digits of degrees (in case no leading zero)
          dg = lon.substring(0, 2);
          mn = lon.substring(2);
      } else if(a[0].length === 3) {
          // 1 digit of degrees (in case no leading zero)
          dg = lon.substring(0, 1);
          mn = lon.substring(1);
      } else {
          // no degrees, just minutes (nonstandard but a buggy unit might do this)
          dg = '0';
          mn = lon;
      }
      // longitude is usually precise to 5-8 digits
      return ((parseFloat(dg) + (parseFloat(mn) / 60.0)) * h).toFixed(8);
  };

  // fields can be empty so have to wrap the global parseInt
    nmea.parseIntX = function(i) {
      if(i === '') {
          return 0;
      }
      return parseInt(i, 10);
  };

    nmea.parseDateTime = function(date, time) {
      var h = parseInt(time.slice(0, 2), 10);
      var m = parseInt(time.slice(2, 4), 10);
      var s = parseInt(time.slice(4, 6), 10);
      var D = parseInt(date.slice(0, 2), 10);
      var M = parseInt(date.slice(2, 4), 10);
      var Y = parseInt(date.slice(4, 6), 10);
      // hack : GPRMC date doesn't specify century. GPS came out in 1973
      // so if year is less than 73 its 2000, otherwise 1900
      if (Y < 73) {
          Y = Y + 2000;
      }
      else {
          Y = Y + 1900;
      }

      return new Date(Date.UTC(Y, M, D, h, m, s));
  };

    
    
    // =============================================
    // public API functions
    // =============================================

    // function to add parsers
    nmea.addParser = function(sentenceParser) {
        if(sentenceParser === null) {
            this.error('invalid sentence parser : null');
            return;
        }
        m_parserList.push(sentenceParser);
    };

    /** function to add encoders */
    nmea.addEncoder = function(sentenceEncoder) {
        if(sentenceEncoder === null) {
            this.error('invalid  sentence encoder : null');
            return;
        }
        m_encoderList.push(sentenceEncoder);
    };

    /** master parser function
     * handle string tokenizing, find the associated parser and call it if there is one
     */
    nmea.parse = function(sentence) {
        var i;
        var tokens;
        var id;
        var result;
        var checksum;
        var status;
        if(( typeof sentence) !== 'string') {
            this.error('sentence is not a string');
            return null;
        }

        // find the checksum and remove it prior to tokenizing
        checksum = sentence.split('*');
        if(checksum.length === 2) {
            // there is a checksum
            sentence = checksum[0];
            checksum = checksum[1];
        } else {
            checksum = null;
        }

        tokens = sentence.split(',');
        if(tokens.length < 1) {
            this.error('must at least have a header');
            return null;
        }

        // design decision: the 5 character header field determines the sentence type
        // this field could be handled in two different ways
        // 1. split it into the 2 character 'talker id' + 3 character 'sentence id' e.g. $GPGGA : talker=GP id=GGA
        //    this would leave more room for customization of proprietary talkers that give standard sentences,
        //    but it would be more complex to deal with
        // 2. handle it as a single 5 character id string
        //    much simpler.  for a proprietary talker + standard string, just instantiate the parser twice
        // This version implements approach #2
        id = tokens[0].substring(1);
        if(id.length !== 5) {
            this.error('i must be exactly 5 characters');
            return null;
        }

        // checksum format = *HH where HH are hex digits that convert to a 1 byte value
        if(checksum !== null) {
            // there is a checksum, replace the last token and verify the checksum
            status = nmea.verifyChecksum(sentence, checksum);
            if(status === false) {
                this.error('checksum mismatch');
                return null;
            }
        }

        // try all id's until one matches
        result = null;
        for( i = 0; i < m_parserList.length; ++i) {
            if(id === m_parserList[i].id) {
                try {
                    result = m_parserList[i].parse(tokens);
                } catch(err) {
                    nmea.error(err.message);
                }
                break;
            }
        }
        if(result === null) {
            this.error('sentence id not found');
        }
        return result;
    };

    /** master encoder
     * find the specified id encoder and give it the data to encode. return the result;
     */
    nmea.encode = function(id, data) {
        var i;
        var result;
        var cks;
        result = null;
        for( i = 0; i < m_encoderList.length; ++i) {
            if(id === m_encoderList[i].id) {
                try {
                    result = m_encoderList[i].encode(id, data);
                } catch(err) {
                    nmea.error(err.message);
                }
            }
        }
        if(result === null) {
            this.error('sentence id not found');
            return null;
        }

        // add the checksum
        cks = nmea.computeChecksum(result);
        result = result + cks;

        return result;
    };

    /** public function to print/handle errors */
    nmea.error = function(msg) {
        if(m_errorHandler !== null) {
            // call the existing handler
            m_errorHandler(msg);
        }
    };

    /** public function to  set error handler */
    nmea.setErrorHandler = function(e) {
        m_errorHandler = e;
    };

    // =======================================================
    // initialize the handlers
    // =======================================================

    // add the standard error handler
    nmea.setErrorHandler(function(e) {
        throw new Error('ERROR:' + e);
    });

    
    
    nmea.DBT = {
      /*
       === DBT - Depth below transducer ===

       ------------------------------------------------------------------------------
       *******1   2 3   4 5   6 7
       *******|   | |   | |   | |
       $--DBT,x.x,f,x.x,M,x.x,F*hh<CR><LF>
       ------------------------------------------------------------------------------

       Field Number:

       1. Depth, feet
       2. f = feet
       3. Depth, meters
       4. M = meters
       5. Depth, Fathoms
       6. F = Fathoms
       7. Checksum
       */

        'Decoder': function (id) {
            this.id = id;
            this.talker_type_id = "DBT";
            this.talker_type_desc = "Depth Below Transducer";
            this._format = function(char){
                  switch(char){
                    case "F" : return "fathoms"
                    case "M" : return "meters"
                    case "f" : return "feet"
                }
            };

            this.parse = function (tokens) {
                if (tokens.length < 6) {
                    throw new Error('DBT : not enough tokens');
                }

                var model = {
                    id: tokens[0].substr(1),
                    talker_type_id: this.talker_type_id,
                    talker_type_desc: this.talker_type_desc,
                };

                model[this._format(tokens[2])] = nmea.parseFloatX(tokens[1]);
                model[this._format(tokens[4])] = nmea.parseFloatX(tokens[3]);
                model[this._format(tokens[6])] = nmea.parseFloatX(tokens[5]);

                return model;
            };
        }
      }



    nmea.GGA = {
        /** nmea encoder object
         * $--GGA,hhmmss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh

         GGA  = Global Positioning System Fix Data

         1    = UTC of Position
         2    = Latitude
         3    = N or S
         4    = Longitude
         5    = E or W
         6    = GPS quality indicator (0=invalid; 1=GPS fix; 2=Diff. GPS fix)
         7    = Number of satellites in use [not those in view]
         8    = Horizontal dilution of position
         9    = Antenna altitude above/below mean sea level (geoid)
         10   = Meters  (Antenna height unit)
         11   = Geoidal separation (Diff. between WGS-84 earth ellipsoid and
         mean sea level.  -=geoid is below WGS-84 ellipsoid)
         12   = Meters  (Units of geoidal separation)
         13   = Age in seconds since last update from diff. reference station
         14   = Diff. reference station ID#
         15   = Checksum

         * input data:
         * {
             date         : DateTime object, UTC (year,month,day ignored)
             latitude     : decimal degrees (north is +)
             longitude    : decimal degreees (east is +)
             fix          : integer 0,1,2
             satellites   : integer 0..32
             hdop         : float
             altitude     : decimal altitude in meters
             aboveGeoid   : decimal altitude in meters
             dgpsUpdate   : time in seconds since last dgps update
             dgpsReference: differential reference station id
             * }
         * any undefined values will be left blank ',,' which is allowed in the nmea specification
         */
        'Encoder': function(id) {
            this.id = id;
            this.encode = function(id, data) {
                var a = [];
                var gga;

                a.push('$' + id);
                a.push(nmea.encodeTime(data.date));
                a.push(nmea.encodeLatitude(data.lat, 3));
                a.push(nmea.encodeLongitude(data.lon, 3));
                a.push(nmea.encodeValue(data.fix));
                a.push(nmea.encodeValue(nmea.padLeft(data.satellites.toString(), 2, '0')));
                a.push(nmea.encodeFixed(data.hdop, 1));
                a.push(nmea.encodeAltitude(data.altitude));
                a.push(nmea.encodeAltitude(data.aboveGeoid));
                a.push(nmea.encodeFixed(data.dgpsUpdate, 0));
                a.push(nmea.encodeValue(data.dgpsReference));

                gga = a.join();

                return gga;
            };
        },
        //TODO: format codes 
        'Decoder': function(id) {
            this.id = id;
            this.talker_type_id = "GGA";
            this.talker_type_desc = "Global Positioning System Fix Data"; 
            this.parse = function(tokens) {
                var i;
                var gga;
                if(tokens.length < 14) {
                    throw new Error('GGA - not enough tokens (13): '+tokens.length+', tokens: '+tokens);
                }

                // trim whitespace
                // some parsers may not want the tokens trimmed so the individual parser has to do it if applicable
                for( i = 0; i < tokens.length; ++i) {
                    tokens[i] = tokens[i].trim();
                }

                return {
                    id : tokens[0].substr(1),
                    talker_type_id: this.talker_type_id,
                    talker_type_desc: this.talker_type_desc,
                    time : tokens[1],
                    latitude : nmea.parseLatitude(tokens[2], tokens[3]),
                    longitude : nmea.parseLongitude(tokens[4], tokens[5]),
                    fix : nmea.parseIntX(tokens[6], 10),
                    satellites : nmea.parseIntX(tokens[7], 10),
                    hdop : nmea.parseFloatX(tokens[8]),
                    altitude : nmea.parseAltitude(tokens[9], tokens[10]),
                    aboveGeoid : nmea.parseAltitude(tokens[11], tokens[12]),
                    dgpsUpdate : tokens[13],
                    dgpsReference : tokens[14]
                };
            };
        }
      }



    nmea.GLL = {
        /** 

         === GLL - Geographic Position: Latitude, Longitude and time. ===

          ------------------------------------------------------------------------------
         *******1   2 3   4 5 6 7
         *******|   | |   | | | |
         $--GLL,x.x,N,x.x,W,A,A*hh<CR><LF>
         ------------------------------------------------------------------------------

         Field Number:

         1    = Latitude of fix
         2    = N or S
         3    = Longitude of fix
         4    = E or W
         5    = Status (A=data valid or V=data not valid)
         6    = Mode (A=Autonomous, D=DGPS, E=DR, Only present in NMEA version 3.00)
         7    = Checksum

         */

        'Decoder': function(id) {
            this.id = id;
            this.talker_type_id = "GLL";
            this.talker_type_desc = "Geographic Latitude Longitude";

            this._status = function(char){
                switch(char){
                    case "A" : return "data valid "
                    case "V" : return "data not valid"
                }
            };

            this._mode = function(char){
                switch(char){
                    case "A" : return "Autonomous"
                    case "D" : return "DGPS"
                    case "E" : return "DR";
                }
            };

            this.parse = function(tokens) {
                if(tokens.length < 8) {
                    throw new Error('GLL : not enough tokens');
                }
                return {
                    id : tokens[0].substr(1),
                    talker_type_id: this.talker_type_id,
                    talker_type_desc: this.talker_type_desc,
                    latitude : nmea.parseLatitude(tokens[1], tokens[2]),
                    longitude : nmea.parseLongitude(tokens[3], tokens[4]),
                    utc_time : tokens[5],
                    status : this._status(tokens[6]),
                    mode : this._mode(tokens[7])
                };
            };
        }
      }



    nmea.GSA = {
        //TODO: add docbloc
        'Decoder': function(id) {
            this.id = id;
            this.talker_type_id = "GSA";
            this.talker_type_desc = "Global Navigation Satellite Systems, Dilution of Precision, and Active Satellites";

            this.parse = function(tokens) {
                var gsa;
                var i;
                if(tokens.length < 17) {
                    throw new Error('GSA : not enough tokens');
                }

                // trim whitespace
                // some parsers may not want the tokens trimmed so the individual parser has to do it if applicable
                for(i=0;i<tokens.length;++i) {
                    tokens[i] = tokens[i].trim();
                }


                gsa = {
                    id : tokens[0].substr(1),
                    talker_type_id: this.talker_type_id,
                    talker_type_desc: this.talker_type_desc,
                    mode: tokens[1],
                    fix: tokens[2],
                    sat :[],
                    pdop: nmea.parseFloatX(tokens[15]),
                    hdop: nmea.parseFloatX(tokens[16]),
                    vdop: nmea.parseFloatX(tokens[17])
                };

               // extract up to 4 sets of sat data
                for(i=3;i<15;i+= 1) {
                    if(tokens[i] !== '') {
                        gsa.sat.push(tokens[i]);
                    }
                }
                return gsa;
            };
        }
      }



    nmea.GSV = {
        //TODO: docbloc
        'Decoder': function(id) {
            this.id               = id;
            this.talker_type_id   = "GSV";
            this.talker_type_desc = "Global Satellites in View";
            this.parse = function(tokens) {
                var gsv;
                var i;
                var sat;
                if(tokens.length < 7) {
                    throw new Error('GSV - not enough tokens (13): '+tokens.length+', tokens: '+tokens);
                }

                // trim whitespace
                // some parsers may not want the tokens trimmed so the individual parser has to do it if applicable
                for(i=0;i<tokens.length;++i) {
                    tokens[i] = tokens[i].trim();
                }

                gsv = {
                    id : tokens[0].substr(1),
                    talker_type_id: this.talker_type_id,
                    talker_type_desc: this.talker_type_desc,
                    msgs: nmea.parseIntX(tokens[1],10),
                    mnum: nmea.parseIntX(tokens[2],10),
                    count: nmea.parseIntX(tokens[3],10),
                    sat:[]
                };

                // extract up to 4 sets of sat data
                for(i=4;i<tokens.length;i+= 4) {
                    sat = {
                        prn: nmea.parseIntX(tokens[i+0],10),
                        el:nmea.parseIntX(tokens[i+1],10),
                        az:nmea.parseIntX(tokens[i+2],10),
                        ss:nmea.parseIntX(tokens[i+3],10)
                    };

                    gsv.sat.push(sat);
                }
                return gsv;
            };
        }
      }



    nmea.HDG = {
        /*
         === HDG - Heading, Deviation and Variation ===

         ------------------------------------------------------------------------------
         ******* 1   2  3 4   5  6  
         ******* |   |  | |   |  |  
         $--HDG,x.x,x.x,x,x*hh<CR><LF>
         ------------------------------------------------------------------------------

         Field Number:

        1. Magnetic sensor heading, degrees, to the nearest 0.1 degree.
        2. Magnetic deviation, degrees east or west, to the nearest 0.1 degree.
        3. E if field 2 is degrees East W if field 2 is degrees West
        4. Magnetic variation, degrees east or west, to the nearest 0.1 degree.
        5. E if field 4 is degrees East W if field 4 is degrees West
        6. Checksum

        */

        'Decoder': function (id) {
            this.id = id;
            this.talker_type_id = "HDG";
            this.talker_type_desc = "Heading, Deviation and Variation";

            this.parse = function (tokens) {
                if (tokens.length < 6) {
                    throw new Error('DBT : not enough tokens');
                }
                return {
                    id: tokens[0].substr(1),
                    talker_type_id: this.talker_type_id,
                    talker_type_desc: this.talker_type_desc,
                    heading: nmea.parseFloatX(tokens[1]),
                    deviation: nmea.parseFloatX(tokens[2]),
                    deviation_direction: tokens[3],
                    variation: nmea.parseFloatX(tokens[4]),
                    variation_direction: tokens[5]
                }
            };
        }
      }



    nmea.MWV = {
        /*
         ------------------------------------------------------------------------------
         *******1   2 3   4 5
         *******|   | |   | |
         $--MWV,x.x,a,x.x,a,a*hh<CR><LF>
         ------------------------------------------------------------------------------

         Field Number:

         1. Wind Angle, 0 to 360 degrees
         2. Reference, R = Relative, T = True
         3. Wind Speed
         4. Wind Speed Units, K/M/N
         5. Status, A = Data Valid
         6. Checksum
         */

        'Decoder': function (id) {
            this.id = id;
            this.talker_type_id = "MWV";
            this.talker_type_desc = "Wind Speed and Angle";
            this._unit = function(char){
                  switch(char){
                    case "N" : return "knots"
                    case "M" : return "meters per second"
                    case "K" : return "kilometers per hour"
                }
            };
            this._reference = function(char){
                switch(char){
                    case "T" : return "true"
                    case "R" : return "relative"
                }  
            };
            this._status = function(char){
                switch(char){
                    case "A" : return "data valid"
                    case "V" : return "data not valid"
                }
            };

            this.parse = function (tokens) {
                var i;
                if (tokens.length < 5) {
                    throw new Error('MWV : not enough tokens');
                }

                // trim whitespace
                // some parsers may not want the tokens trimmed so the individual parser has to do it if applicable
                for (i = 0; i < tokens.length; ++i) {
                    tokens[i] = tokens[i].trim();
                }

                return {
                    id: tokens[0].substr(1),
                    talker_type_id: this.talker_type_id,
                    talker_type_desc: this.talker_type_desc,
                    apparent_wind_angle: nmea.parseFloatX(tokens[1]),
                    reference: this._reference(tokens[2]),
                    apparent_wind_speed: nmea.parseFloatX(tokens[3]),
                    units: this._unit(tokens[4]),
                    status: this._status(tokens[5])
                }
            };
        }
      }



    nmea.RMC = {
        /** RMC encoder object
         * $GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a*hh

         RMC  = Recommended Minimum Specific GPS/TRANSIT Data 

         1    = UTC of position fix
         2    = Data status (V=navigation receiver warning)
         3    = Latitude of fix
         4    = N or S
         5    = Longitude of fix
         6    = E or W
         7    = Speed over ground in knots
         8    = Track made good in degrees True
         9    = UT date
         10   = Magnetic variation degrees (Easterly var. subtracts from true course)
         11   = E or W
         12   = Checksum

         input: {
             date:Date UTC
             status:String (single character)
             latitude:decimal degrees (N is +)
             longitude:decimal degrees (E is +)
             speed:decimal knots
             course:decimal degrees
             variation:decimal magnetic variation (E is -)
             }
         */
        'Encoder': function(id) {
            this.id = id;
            this.encode = function(id, data) {
                var a = [];
                var rmc;
                // $GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a*hh

                a.push('$' + id);
                a.push(nmea.encodeTime(data.date));
                a.push(nmea.encodeValue(data.status));
                a.push(nmea.encodeLatitude(data.lat, 3));
                a.push(nmea.encodeLongitude(data.lon, 3));
                a.push(nmea.encodeKnots(data.speed));
                a.push(nmea.encodeDegrees(data.course));
                a.push(nmea.encodeDate(data.date));
                a.push(nmea.encodeMagVar(data.variation));

                rmc = a.join();

        //        rmc += data.mode

                return rmc;
            };
        },

        /** GPRMC parser object */
        'Decoder': function(id) {
            this.id = id;
            this.talker_type_id = "RMC";
            this.talker_type_desc = "Recommended Minimum Navigation Information";
            this.parse = function(tokens) {
                if(tokens.length < 12) {
                    throw new Error('RMC : not enough tokens');
                }
                return {
                    id : tokens[0].substr(1),
                    talker_type_id: this.talker_type_id,
                    talker_type_desc: this.talker_type_desc,
                    time : tokens[1],
                    valid : tokens[2],
                    latitude : nmea.parseLatitude(tokens[3], tokens[4]),
                    longitude : nmea.parseLongitude(tokens[5], tokens[6]),
                    sog : nmea.parseFloatX(tokens[7]),
                    course : nmea.parseFloatX(tokens[8]),
                    date : tokens[9],
                    mode: tokens[11].substr(0,1),
                    variation : nmea.parseDegrees(tokens[10], tokens[11])
                };
            };
        }
      }



    nmea.VHW = {
        /*
         === VHW – Water Speed and Heading ===
        The compass heading to which the vessel is currently pointing, and the speed of the vessel through the water.
         ------------------------------------------------------------------------------
         *******1   2   3 4 5   6   7
         *******|   |   | | |   |   |
         $--VHW,x.x,M,x.x,T,x.x,x.x*hh<CR><LF>
         ------------------------------------------------------------------------------

         Field Number:

         1. Heading degrees true
         2. M = magnetic, T = true
         3. Heading degrees magnetic 
         4. M = magnetic, T = true
         5. Speed, Knots
         6. Speed KMH
         7. checksum
         */

        'Decoder': function (id) {
            this.id = id;
            this.talker_type_id = "VHW";
            this.talker_type_desc = "Water, Speed and Heading";
            this._reference = function(char){
                switch(char){
                    case "T" : return "true"
                    case "M" : return "magnetic"
                }  
            };

            this.parse = function (tokens) {
                if (tokens.length < 7) {
                    throw new Error('VHW : not enough tokens');
                }

                return {
                    id: tokens[0].substr(1),
                    talker_type_id: this.talker_type_id,
                    talker_type_desc: this.talker_type_desc,
                    heading1: nmea.parseFloatX(tokens[1]),
                    reference1:this._reference(tokens[2]),
                    heading2: nmea.parseFloatX(tokens[3]),
                    reference2:this._reference(tokens[4]),
                    sow_knots: nmea.parseFloatX(tokens[5]),
                    sow_kph: nmea.parseFloatX(tokens[6])
                }
            };
        }
      }



    nmea.VTG = {
        'Decoder': function(id) {
            this.id = id;
            this.talker_type_id = "VTG";
            this.talker_type_desc = "Velocity Track Made Good";  
            this.parse = function(tokens) {


                var vtg;
                var i;
                if(tokens.length < 9) {
                    throw new Error('VTG : not enough tokens');
                }

                // trim whitespace
                // some parsers may not want the tokens trimmed so the individual parser has to do it if applicable
                for(i=0;i<tokens.length;++i) {
                    tokens[i] = tokens[i].trim();
                }

                return  {
                    id : tokens[0].substr(1),
                    talker_type_id: this.talker_type_id,
                    talker_type_desc: this.talker_type_desc,
                    course: nmea.parseFloatX(tokens[1]),
                    knots: nmea.parseFloatX(tokens[5]),
                    kph: nmea.parseFloatX(tokens[7]),
                    mode: tokens[9]
                };
            };
        }
      }

    
    
    
    // add the standard decoders
    nmea.addParser(new nmea.GGA.Decoder("GPGGA"));
    nmea.addParser(new nmea.RMC.Decoder("GPRMC"));
    nmea.addParser(new nmea.GSV.Decoder("GPGSV"));
    nmea.addParser(new nmea.GSA.Decoder("GPGSA"));
    nmea.addParser(new nmea.VTG.Decoder("GPVTG"));
    nmea.addParser(new nmea.MWV.Decoder("IIMWV"));
    nmea.addParser(new nmea.MWV.Decoder("WIMWV"));
    nmea.addParser(new nmea.DBT.Decoder("IIDBT"));
    nmea.addParser(new nmea.DBT.Decoder("SDDBT"));
    nmea.addParser(new nmea.GLL.Decoder("GPGLL"));
    nmea.addParser(new nmea.HDG.Decoder("HCHDG"));
    nmea.addParser(new nmea.VHW.Decoder("VWVHW"));
    // add the standard encoders
    nmea.addEncoder(new nmea.GGA.Encoder("GPGGA"));
    nmea.addEncoder(new nmea.RMC.Encoder("GPRMC"));

    // return the module object
    return nmea;
}());