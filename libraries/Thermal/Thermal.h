#ifndef Thermal_h
#define Thermal_h

#include <Arduino.h>
#include <SoftwareSerial.h>

#define SERIAL_IMPL      SoftwareSerial
#define PRINTER_PRINT(a) _printer->write(a);

class Thermal : public SoftwareSerial

{

  public:

    Thermal(int, int, long); // constructor
    void setDefault();
    void test();



    void inverseOn();
    void inverseOff();
    void doubleHeightOn();
    void doubleHeightOff();
    void boldOn();
    void boldOff();
    void underlineOn();
    void underlineOff();



    void justify(char value);
    void feed(uint8_t x = 1);
    void sleep();
    void wake();



    void setCharSpacing(int spacing);
    void setSize(char value);
    void setLineHeight(int val = 32);



    void printBarcode(char * text);
    void printFancyBarcode(char * text);
    void setBarcodeHeight(int val);
    



    void printBitmap(int w, int h, const uint8_t *bitmap, bool fromProgMem = true);

    // ??
    void tab();



    void setHeatTime(int vHeatTime);

    void setHeatInterval(int vHeatInterval);

    void setPrintDensity(char vPrintDensity);

    void setPrintBreakTime(char vPrintBreakTime);
    void timeoutSet(unsigned long);

    
  private:
    boolean linefeedneeded;


    // little helpers to make code easier to read&use
    void writeBytes(uint8_t a, uint8_t b);
    void writeBytes(uint8_t a, uint8_t b, uint8_t c);
    void writeBytes(uint8_t a, uint8_t b, uint8_t c, uint8_t d);


    int zero;

	uint8_t prevByte;
	
	SERIAL_IMPL *_printer;

    int heatTime;
    int heatInterval;
    char printDensity;
    char printBreakTime;
    unsigned long resumeTime;
    
};

#endif
