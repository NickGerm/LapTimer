/// 
/// @mainpage	Receiver_Lap_Timer 
///
/// @details	IR Receiver for lap Timer
/// @n 		
/// @n 
/// @n @a		Developed with [embedXcode+](http://embedXcode.weebly.com)
/// 
/// @author		Nicholas Germon
/// @author		Nicholas Germon
/// @date		12/6/14 12:46 AM
/// @version	<#version#>
/// 
/// @copyright	(c) Nicholas Germon, 2014
/// @copyright	CC = BY SA NC
///
/// @see		ReadMe.txt for references
///


///
/// @file		Receiver_Lap_Timer.ino
/// @brief		Main sketch
///
/// @details	<#details#>
/// @n @a		Developed with [embedXcode+](http://embedXcode.weebly.com)
/// 
/// @author		Nicholas Germon
/// @author		Nicholas Germon
/// @date		12/6/14 12:46 AM
/// @version	<#version#>
/// 
/// @copyright	(c) Nicholas Germon, 2014
/// @copyright	CC = BY SA NC
///
/// @see		ReadMe.txt for references
/// @n
///



#include "Arduino.h"
#include <EasyTransfer.h>

//create object
EasyTransfer ET;

/*IR receiver for ranch racing lap timer
 all communcation is done through the serial port
 DO NOT CHANGE ANYTHING
 there is nothing to customize in this sketch
 */



unsigned int lapDelay = 6000;// set the default lap delay to 20 seconds unless updated from SD card file.
unsigned long start[3] = {
    500,500,500};//set values (system will random asign if left blank) will be changed later
unsigned long finish[3] = {
    0,0,0};//set values (system will random asign if left blank) will be changed later

unsigned long lapTime[3] = {
    0,0,0};//set values (system will random asign if left blank) will be changed later

boolean goneThrough [3] = {
    false,false,false};

byte lapIDKart[3] = {
    1,1,1};

unsigned long sendTimeD = 0;


#define IRpin_PIN      PIND // assign pin 2 raw mapping to IRpin 2
#define IRpin          2




struct SEND_DATA_STRUCTURE{
    //put your variable definitions here for the data you want to send
    //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
    char verify[3];
    byte kartID;
    byte lapID;
    unsigned long lapTime;
};
SEND_DATA_STRUCTURE myData;


#define MAXhighPULSE 2000 // Time counts to reset tokens to zero if no beam detected incase of false trigger, 2000 * RESOLUTION = 40ms
#define MAXlowPULSEA 600 // the maximum pulse count we need. 560us / RESOLUTION = 28
#define MINlowPULSEA 200 // the minimum pulse count we need. 360us / RESOLUTION = 18
#define MAXlowPULSEB 1500 // the maximum pulse count we need. 560us / RESOLUTION = 28
#define MINlowPULSEB 900 // the minimum pulse count we need. 360us / RESOLUTION = 18
#define MAXlowPULSEC 28 // the maximum pulse count we need. 560us / RESOLUTION = 28
#define MINlowPULSEC 18 // the minimum pulse count we need. 360us / RESOLUTION = 18

#define RESOLUTION 20 // Multiplier – will be used to take reading of IRpin every 20ish microseconds
int lowpulse = 0; // start with 0 for the pulse length.
int highpulse = 0; // start with 0 for the pulse length.
int token[3] = {
    0,0.0}; // start with 0 count for tokens. We’ll need 3 tokens to trigger the timer.



byte dataSend [6];

void setup() {
    Serial.begin(9600);
    ET.begin(details(myData), &Serial);
    myData.verify[0] = 'R';
    myData.verify[1] = 'E';
    myData.verify[2] = 'C';
    
    
    pinMode(4,OUTPUT);
    pinMode(5,OUTPUT);
    
    
}

void SendResult(byte kart){
    unsigned long laptime;// Calculate lap time
    
    
    myData.lapID = lapIDKart[kart-1];
    myData.lapTime = lapTime[kart-1];
    start[kart-1]=finish[kart-1]; // set the new lap start time to the same value as the last lap finish time
    myData.kartID = kart -1;
    unsigned long currentTime = millis();
    int tester =20;
    ET.sendData();
    if (goneThrough[0])
    {
        if(goneThrough[1])
        {
            goneThrough[0] = false;
            goneThrough[1] = false;
            goneThrough[2] = false;
            for(int x=1; x<3;x++)
            {
                delay(100);
                myData.lapID = lapIDKart[x];
                myData.kartID = x;
                myData.lapTime = laptime;
                ET.sendData();
            }
        }
    }
}



void loop() {
    highpulse = lowpulse = 0; // start out with no pulse length
    while (IRpin_PIN & (1 << IRpin)) { // while irpin is high – no beam is beam is being received.
        
        highpulse++;
        delayMicroseconds(RESOLUTION);// wait some time, the multiplier.
        
        if (highpulse >= MAXhighPULSE) { /* check to see if no signal has been received for over 40ms (we should
                                          recieve 3 tokens in 4ms, so if we only receive one or two tokens in 40ms it must be a false signal),
                                          then reset the token count if true as no/false signal is being received.
                                          */
            token[0] = 0;
            token[1] = 0;
            token[2] = 0;
            highpulse = 0;// reset high pulse counter
            return;
        }
    }
    while (! (IRpin_PIN & _BV(IRpin))) {// while irpin is low, signal being received
        lowpulse= pulseIn(IRpin, LOW);
    }
    
    if ((lowpulse >= MINlowPULSEA)  && (lowpulse <= MAXlowPULSEA)) {// was the pulse length what we expected?
        token[0] ++; // if so then count 1 token, we need 3 tokens to trigger
    }
    if ((lowpulse >= MINlowPULSEB)  && (lowpulse <= MAXlowPULSEB)) {// was the pulse length what we expected?
        token[1] ++; // if so then count 1 token, we need 3 tokens to trigger
    }
    if ((lowpulse >= MINlowPULSEC)  && (lowpulse <= MAXlowPULSEC)) {// was the pulse length what we expected?
        token[2] ++; // if so then count 1 token, we need 3 tokens to trigger
    }
    if (token[0] >= 6 ) { // If we receive 3 correct pulses then trigger the lap timer.
        unsigned long currentTime = millis();
        if (currentTime - finish[0] > lapDelay)
        {
            finish[0]=millis();// Record the time
            goneThrough[0] = true;
            if(lapIDKart[0] >= 255)
            {
                lapIDKart[0] = 0;
            }
            lapIDKart[0]++;
            lapTime[0]=finish[0]-start[0];
            SendResult(1);// run through the display procedure
            
            
            
        }
        else
        {
            token[0]=0;
        }
        
    }
    if (token[1] == 6) { // If we receive 3 correct pulses then trigger the lap timer.
        unsigned long currentTime = millis();
        if (currentTime - finish[1] > lapDelay)
        {
            finish[1]=millis();// Record the time
            goneThrough[1] = true;
            if(lapIDKart[1] >= 255)
            {
                lapIDKart[1] = 0;
            }
            lapIDKart[1]++;
            lapTime[1]=finish[1]-start[1];
            SendResult(2);// run through the display procedure
            
            
        }
        else
        {
            token[1]=0;
        }
    }
    if (token[2] == 6) { // If we receive 3 correct pulses then trigger the lap timer.
        unsigned long currentTime = millis();
        if (currentTime - finish[2] > lapDelay)
        {
            finish[2]=millis();// Record the time
            goneThrough[2] = true;
            if(lapIDKart[2] >= 255)
            {
                lapIDKart[2] = 0;
            }
            lapIDKart[2]++;
            SendResult(3);// run through the display procedure
            
        }
        else
        {
            token[2]=0;
        }
    }
    
}




