//http://forum.arduino.cc/index.php?topic=41665.0

class dummySerial {
 private:
   //uint8_t _uart;
   void printNumber(unsigned long, uint8_t);
 public:
   dummySerial() {};
   dummySerial(uint8_t) {};

   void    setDebugOutput(bool) {};
   explicit operator bool() const { return true; }

    void begin(unsigned long baud) {  }
    void begin(unsigned long, uint8_t) {};
    void end() {};

    int available(void) {};
    int peek(void) {};
    int read(void) {};
    int availableForWrite(void) {};
    void flush(void) {};
    size_t write(uint8_t) {};
    inline size_t write(unsigned long n) {  }
    inline size_t write(long n) {  }
    inline size_t write(unsigned int n) { }
    inline size_t write(int n) {  }

    void    printf(String&, int) {};
    void    printf(const char[], int) {};

    size_t print(const __FlashStringHelper *) {};
    size_t print(const String &) {};
    size_t print(const char[]) {};
    size_t print(char) {};
    size_t print(unsigned char, int = DEC) {};
    size_t print(int, int = DEC) {};
    size_t print(unsigned int, int = DEC) {};
    size_t print(long, int = DEC) {};
    size_t print(unsigned long, int = DEC) {};
    size_t print(double, int = 2) {};
    size_t print(const Printable&) {};

    size_t println(const __FlashStringHelper *) {};
    size_t println(const String &s) {};
    size_t println(const char[]) {};
    size_t println(char) {};
    size_t println(unsigned char, int = DEC) {};
    size_t println(int, int = DEC) {};
    size_t println(unsigned int, int = DEC) {};
    size_t println(long, int = DEC) {};
    size_t println(unsigned long, int = DEC) {};
    size_t println(double, int = 2) {};
    size_t println(const Printable&) {};
    size_t println(void) {};

};
