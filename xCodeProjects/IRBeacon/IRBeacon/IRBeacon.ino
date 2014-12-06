/// 
/// @mainpage	IRBeacon 
///
/// @details	IR Beacon for lap Timer
/// @n 		
/// @n 
/// @n @a		Developed with [embedXcode+](http://embedXcode.weebly.com)
/// 
/// @author		Nicholas Germon
/// @author		Nicholas Germon
/// @date		12/6/14 12:52 AM
/// @version	<#version#>
/// 
/// @copyright	(c) Nicholas Germon, 2014
/// @copyright	CC = BY SA NC
///
/// @see		ReadMe.txt for references
///


///
/// @file		IRBeacon.ino
/// @brief		Main sketch
///
/// @details	<#details#>
/// @n @a		Developed with [embedXcode+](http://embedXcode.weebly.com)
/// 
/// @author		Nicholas Germon
/// @author		Nicholas Germon
/// @date		12/6/14 12:52 AM
/// @version	<#version#>
/// 
/// @copyright	(c) Nicholas Germon, 2014
/// @copyright	CC = BY SA NC
///
/// @see		ReadMe.txt for references
/// @n
///


#include "Arduino.h"

/////////////false = car 1
////////////true = car 2

boolean Car2 = true;

////////that is all you need to change here






int delayTime=0;


int IRledPin =  3;    // LED connected to digital pin 3
int powerLed = 13;    //Power LED is connected to pin 13



int delaySend;



void setup()   {
    if (Car2)
    {
        delaySend = 1200;
    }
    else
    {
        delaySend = 416;
    }
    pinMode(IRledPin, OUTPUT);    // initialize the IR digital pin as an output:
    pinMode(powerLed, OUTPUT);
    digitalWrite(powerLed, HIGH);
    
}

void pulseIR(long microsecs)
{
    cli();  // this turns off any background interrupts
    while (microsecs > 0) { // we’ll count down from the number of microseconds we are told to wait
        // 38 kHz is about 26 microseconds – to give 50% duty cycle 13 microseconds high and 13 microseconds low
        digitalWrite(IRledPin, HIGH);  // this takes about 3 microseconds to happen
        delayMicroseconds(10);         // hang out for 10 microseconds
        digitalWrite(IRledPin, LOW);   // this also takes about 3 microseconds
        delayMicroseconds(10);         // hang out for 10 microseconds
        // so 26 microseconds altogether
        microsecs -= 26;
    }
    sei();  // this turns them back on
}

void loop()
{
    
    
    pulseIR(delaySend);
    delayMicroseconds(1200);
    pulseIR(delaySend);
    delayMicroseconds(1200);
    pulseIR(delaySend);
    delayMicroseconds(6000);
    
}






