#include <Arduino.h>
#include <Thermal.h>



Thermal::Thermal(int RX_Pin, int TX_Pin, long baudRate) : SoftwareSerial(RX_Pin, TX_Pin)

{

	pinMode(RX_Pin, INPUT);

	pinMode(TX_Pin, OUTPUT);

	begin(baudRate);



	int zero = 0;



	heatTime = 80; // 80 is default from page 23 of datasheet. Controls speed of printing and darkness
	heatInterval = 2; // 2 is default from page 23 of datasheet. Controls speed of printing and darkness
	printDensity = 15; // Not sure what the defaut is. Testing shows the max helps darken text. From page 23.
	printBreakTime = 15; // Not sure what the defaut is. Testing shows the max helps darken text. From page 23.


	setHeatTime(heatTime);
	setPrintDensity(printDensity);


	setDefault();
}

void Thermal::setDefault()

{
	wake();
	justify('L');
	inverseOff();
	doubleHeightOff();
	setLineHeight(32);
	boldOff();
	underlineOff();
	setBarcodeHeight(50);
	setSize('s');
}

void Thermal::test()

{
	println("Hello World!");
	feed(2);
}

void Thermal::setBarcodeHeight(int val)

{
	// default is 50
	writeBytes(29, 104, val);
}

void Thermal::timeoutSet(unsigned long x) {
  resumeTime = micros() + x;
}

void Thermal::printBarcode(char * text){
writeBytes(29, 107, 0); // GS, K, m!
  
  for(int i = 0; i < strlen(text); i ++){
    write(text[i]); //Data
  }
 
  write(zero); //Terminator
  
  delay(3000); //For some reason we can't immediately have line feeds here
  feed(2);
}

void Thermal::printFancyBarcode(char * text){
  writeBytes(29, 107, 4); // GS, K, Fancy!
  
  for(int i = 0; i < strlen(text); i ++){
    write(text[i]); //Data
  }
 
  write(zero); //Terminator
  
  delay(3000); //For some reason we can't immediately have line feeds here
  feed(2);
}

void Thermal::writeBytes(uint8_t a, uint8_t b)

{
	write(a);
	write(b);
}

void Thermal::writeBytes(uint8_t a, uint8_t b, uint8_t c)

{
	write(a);
	write(b);
	write(c);
}

void Thermal::writeBytes(uint8_t a, uint8_t b, uint8_t c, uint8_t d)

{
	write(a);
	write(b);
	write(c);
	write(d);
}

void Thermal::inverseOn()

{
	writeBytes(29, 'B', 1);
}

void Thermal::inverseOff()

{
	writeBytes(29, 'B', 0, 10);
}

void Thermal::doubleHeightOn()

{
	writeBytes(27, 14);
}

void Thermal::doubleHeightOff()

{
	writeBytes(27, 20);
}

void Thermal::boldOn()

{
	writeBytes(27, 69, 1);
}

void Thermal::boldOff()

{
	writeBytes(27, 69, 0);
	if (linefeedneeded)
		feed();

	linefeedneeded = false;
}

void Thermal::justify(char value)

{
	uint8_t pos = 0;


	if(value == 'l' || value == 'L') pos = 0;
	if(value == 'c' || value == 'C') pos = 1;
	if(value == 'r' || value == 'R') pos = 2;
  
	writeBytes(0x1B, 0x61, pos);
}

void Thermal::feed(uint8_t x)

{
	while (x--)
		write(10);
}

void Thermal::setSize(char value)

{
	int size = 0;


	if(value == 's' || value == 'S') size = 0;
	if(value == 'm' || value == 'M') size = 10;
	if(value == 'l' || value == 'L') size = 25;
  
	writeBytes(29, 33, size, 10);
	// if (linefeedneeded)
	//  println("lfn"); //feed();
	//linefeedneeded = false;
}

void Thermal::underlineOff()

{
	writeBytes(27, 45, 0, 10);
}


void Thermal::underlineOn()

{
	writeBytes(27, 45, 1);
}

void Thermal::printBitmap(int w, int h, const uint8_t *bitmap, bool fromProgMem) 
{
unsigned long resumeTime;
	int dotPrintTime = 30000; // See comments near top of file for
  int rowBytes, rowBytesClipped, rowStart, chunkHeight, x, y, i;

  rowBytes        = (w + 7) / 8; // Round up to next byte boundary
  rowBytesClipped = (rowBytes >= 48) ? 48 : rowBytes; // 384 pixels max width

  for(i=rowStart=0; rowStart < h; rowStart += 255) {
    // Issue up to 255 rows at a time:
    chunkHeight = h - rowStart;
    if(chunkHeight > 255) chunkHeight = 255;

    writeBytes(18, 42, chunkHeight, rowBytesClipped);

    for(y=0; y < chunkHeight; y++) {
      for(x=0; x < rowBytesClipped; x++, i++) {
        write(fromProgMem ? pgm_read_byte(bitmap + i) : *(bitmap+i));
      }
      i += rowBytes - rowBytesClipped;
    }
    timeoutSet(chunkHeight * dotPrintTime);
  }
  prevByte = '\n';
}

void Thermal::wake()

{
	writeBytes(27, 61, 1);
}

void Thermal::sleep()

{
		writeBytes(27, 61, 0);
}

////////////////////// not working?
void Thermal::tab()

{
	write(9);
}


void Thermal::setCharSpacing(int spacing)

{
	writeBytes(27, 32, 0, 10);
}


void Thermal::setLineHeight(int val)

{
	writeBytes(27, 51, val); // default is 32
}



void Thermal::setHeatTime(int vHeatTime)

{

	heatTime = vHeatTime;

	write(27);

	write(55);

	write(7); //Default 64 dots = 8*('7'+1)

	write(heatTime); //Default 80 or 800us

	write(heatInterval); //Default 2 or 20us	

}



void Thermal::setHeatInterval(int vHeatInterval)

{

	heatInterval = vHeatInterval;

	write(27);

	write(55);

	write(7); //Default 64 dots = 8*('7'+1)

	write(heatTime); //Default 80 or 800us

	write(heatInterval); //Default 2 or 20us	

}



void Thermal::setPrintDensity(char vPrintDensity)

{

	//Modify the print density and timeout

	printDensity = vPrintDensity;

	write(18);

	write(35);

  	int printSetting = (printDensity<<4) | printBreakTime;

	write(printSetting); //Combination of printDensity and printBreakTime

}



void Thermal::setPrintBreakTime(char vPrintBreakTime)

{

	//Modify the print density and timeout

	printBreakTime = vPrintBreakTime;

	write(18);

	write(35);

  	int printSetting = (printDensity<<4) | printBreakTime;

	write(printSetting); //Combination of printDensity and printBreakTime

}
