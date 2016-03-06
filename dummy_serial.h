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

    int available(void) { return 1; };
    int peek(void) { return 1; };
    int read(void) { return 1; };
    int availableForWrite(void) { return 1; };
    void flush(void) {};
    size_t write(uint8_t) { return 1; };
	inline size_t write(unsigned long n) { return 1; }
    inline size_t write(long n) { return 1; }
    inline size_t write(unsigned int n) { return 1;  }
    inline size_t write(int n) { return 1; }

    void    printf(String&, int) {};
    void    printf(const char[], int) {};

    size_t print(const __FlashStringHelper *) { return 1; };
    size_t print(const String &) { return 1; };
    size_t print(const char[]) { return 1; };
    size_t print(char) { return 1; };
    size_t print(unsigned char, int = DEC) { return 1; };
    size_t print(int, int = DEC) { return 1; };
    size_t print(unsigned int, int = DEC) { return 1; };
    size_t print(long, int = DEC) { return 1; };
    size_t print(unsigned long, int = DEC) { return 1; };
    size_t print(double, int = 2) { return 1; };
    size_t print(const Printable&) { return 1; };

    size_t println(const __FlashStringHelper *) { return 1; };
    size_t println(const String &s) { return 1; };
    size_t println(const char[]) { return 1; };
    size_t println(char) { return 1; };
    size_t println(unsigned char, int = DEC) { return 1; };
    size_t println(int, int = DEC) { return 1; };
    size_t println(unsigned int, int = DEC) { return 1; };
    size_t println(long, int = DEC) { return 1; };
    size_t println(unsigned long, int = DEC) { return 1; };
    size_t println(double, int = 2) { return 1; };
    size_t println(const Printable&) { return 1; };
    size_t println(void) { return 1; };

};
