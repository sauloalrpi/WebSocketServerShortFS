- Tested with
  - Arduino IDE 1.6.7
  - esp8266/Arduino library v 2.1.0-rc2 (https://github.com/esp8266/Arduino/releases)
    - the best way to install the beta is to install the stable release and replace the contents of the 2.0.0 folder by the contents of the 2.1.0 zip file.
    - the folder can be found in: C:\Users\<USERNAME>\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266
  - standard C++ library
    - https://github.com/maniacbug/StandardCplusplus - af18241  on 22 Sep 2013
    - uncompress in <SKETCHS FOLDER>/libraries
    
- The HTML should be minimized using: http://www.willpeavy.com/minifier/
- All javascript should be mified using: https://javascript-minifier.com/
- All CSS should be minified using: http://cssminifier.com/
- All images should be optmized using: http://pngcrush.com/ or http://jpgoptimiser.com/
  - Alternatively, images can be embedded as base64 directly in the HTML:
    - http://stackoverflow.com/questions/17450228/convert-html-images-to-inline-base64
- All files should be compressed at highest rate using GZIP and saving with a extra .gz extension
- Files can be listed and deleted:
  - http://esp8266fs.local/list
- Files can be uploaded to the sketch:
  - http://esp8266fs.local/upload
- You can format the SPIFF disk by:
  - http://esp8266fs.local/format
- You can get all system information by:
  - http://esp8266fs.local/stats
  
Current size using ESP8266-12E:
Sketch uses            294,516 bytes (28%) of program storage space. 
             Maximum 1,044,464 bytes.
Global variables use    40,004 bytes (48%) of dynamic memory, 
             leaving    41,916 bytes for local variables. 
             Maximum    81,920 bytes.
             
By Saulo Aflitos - 2016 - https://github.com/sauloalrpi/WebSocketServerShortFS