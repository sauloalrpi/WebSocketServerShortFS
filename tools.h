#ifndef _TOOLS_H_
#define _TOOLS_H_

static void delayy(int d);

static void delayy(int d) {
  unsigned long start = millis();
  while( (millis() - start) < d ) {
    yield();
    delay(0);
  }
}


#endif //ifndef _TOOLS_H_
