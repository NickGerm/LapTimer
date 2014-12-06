/// 
/// @mainpage	TVOut 
///
/// @details	TV Out for lap Timer
/// @n 		
/// @n 
/// @n @a		Developed with [embedXcode+](http://embedXcode.weebly.com)
/// 
/// @author		Nicholas Germon
/// @author		Nicholas Germon
/// @date		12/6/14 1:06 AM
/// @version	<#version#>
/// 
/// @copyright	(c) Nicholas Germon, 2014
/// @copyright	CC = BY SA NC
///
/// @see		ReadMe.txt for references
///


///
/// @file		TVOut.ino
/// @brief		Main sketch
///
/// @details	<#details#>
/// @n @a		Developed with [embedXcode+](http://embedXcode.weebly.com)
/// 
/// @author		Nicholas Germon
/// @author		Nicholas Germon
/// @date		12/6/14 1:06 AM
/// @version	<#version#>
/// 
/// @copyright	(c) Nicholas Germon, 2014
/// @copyright	CC = BY SA NC
///
/// @see		ReadMe.txt for references
/// @n
///


// Core library for code-sense - IDE-based
#include "Arduino.h"


#include <EasyTransfer.h>


#include <TVout.h>
#include <fontALL.h>
#include "TVOlogo.h"


TVout TV;
EasyTransfer ET;

//int zOff = 150;
//int xOff = 0;
//int yOff = 0;
//int cSize = 50;
//int view_plane = 64;
//float angle = PI/60;
int currentScreen = 1;

char tempLapTimeString[9];
unsigned long prevChange = 0;
long screenDelay = 10000;



//currentRacers
struct race
{
    
    unsigned long lastLap[4];
    unsigned long totalTime[4];
    unsigned long bestLap[4];
    int lapCount[2];
    int lapsInRace;
};
typedef struct race Race;
Race racers;            //info on current race, info is transferred to player after the race then stored in memory

void ShowCurrentRace();
void convertTimeString(unsigned long timeToConvert);
void ShowLastRace();
void intro();

void setup() {
    
    
    racers.lastLap[2] = 16045;
    racers.lastLap[3] = 13067;
    racers.totalTime[2] = 936045;
    racers.totalTime[3] = 936045;
    racers.bestLap[2] = 97045;
    racers.bestLap[3] = 96067;
    
    
    
    racers.lastLap[0] = 36045;
    racers.lastLap[1] = 23067;
    racers.totalTime[0] = 136045;
    racers.totalTime[1] = 236045;
    racers.bestLap[0] = 27045;
    racers.bestLap[1] = 16067;
    racers.lapCount[0] = 2;
    racers.lapCount[1] = 3;
    racers.lapsInRace = 6;
    Serial.begin(9600);
    //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc.
    ET.begin(details(racers), &Serial);
    TV.begin(NTSC,112,72);
    TV.select_font(font6x8);
    intro();
    
    
    
    //    TV.print(0,88," Total 280:12:35");
    
    
    //fonts
    TV.clear_screen();
    
    ShowCurrentRace();
}

void loop() {
    unsigned long currentTime = millis();
    
    if(currentTime - prevChange >= screenDelay)
    {
        if(currentScreen)
        {
            ShowCurrentRace();
            currentScreen--;
        }
        else
        {
            ShowLastRace();
            currentScreen++;
        }
        prevChange = currentTime;
    }
    if(ET.receiveData())
    {
        ShowCurrentRace();
        prevChange = currentTime;
        currentScreen=1;
    }
    
}


void ShowCurrentRace()
{
    TV.clear_screen();
    TV.printPGM(0,0,PSTR("Kart A"));
    TV.printPGM(0,8,PSTR("Last "));
    convertTimeString(racers.lastLap[0]);
    if (tempLapTimeString)
    {
        TV.print(0,16,tempLapTimeString);
    }
    TV.printPGM(0,24,PSTR("Best "));
    convertTimeString(racers.bestLap[0]);
    if (tempLapTimeString)
    {
        TV.print(0,32,tempLapTimeString);
    }
    TV.printPGM(0,40,PSTR("Total "));
    convertTimeString(racers.totalTime[0]);
    
    if (tempLapTimeString)
    {
        TV.print(0,48,tempLapTimeString);
    }
    TV.printPGM(0,56,PSTR("Lap "));
    TV.print(0,64,racers.lapCount[0]);
    TV.printPGM(PSTR(" of "));
    TV.print(racers.lapsInRace);
    
    
    TV.printPGM(60,0,PSTR("Kart B"));
    TV.printPGM(60,8,PSTR("Last "));
    convertTimeString(racers.lastLap[1]);
    TV.print(60,16,tempLapTimeString);
    TV.printPGM(60,24,PSTR("Best "));
    convertTimeString(racers.bestLap[1]);
    TV.print(60,32,tempLapTimeString);
    TV.printPGM(60,40,PSTR("Total "));
    convertTimeString(racers.totalTime[1]);
    TV.print(60,48,tempLapTimeString);
    TV.printPGM(60,56,PSTR("Lap "));
    TV.print(60,64,racers.lapCount[1]);
    TV.printPGM(PSTR(" of "));
    TV.print(racers.lapsInRace);
}


void ShowLastRace()
{
    TV.clear_screen();
    TV.printPGM(0,0,PSTR("LastRace"));
    TV.printPGM(0,8,PSTR("Last "));
    convertTimeString(racers.lastLap[2]);
    TV.print(0,16,tempLapTimeString);
    TV.printPGM(0,24,PSTR("Best "));
    convertTimeString(racers.bestLap[2]);
    TV.print(0,32,tempLapTimeString);
    TV.printPGM(0,40,PSTR("Total "));
    convertTimeString(racers.totalTime[2]);
    TV.print(0,48,tempLapTimeString);
    TV.printPGM(0,56,PSTR("Lap "));
    TV.printPGM(0,64,PSTR("Finished"));
    
    TV.printPGM(60,0,PSTR("LastRace"));
    TV.printPGM(60,8,PSTR("Last "));
    convertTimeString(racers.lastLap[3]);
    TV.print(60,16,tempLapTimeString);
    TV.printPGM(60,24,PSTR("Best "));
    convertTimeString(racers.bestLap[3]);
    TV.print(60,32,tempLapTimeString);
    TV.printPGM(60,40,PSTR("Total "));
    convertTimeString(racers.totalTime[3]);
    TV.print(60,48,tempLapTimeString);
    TV.printPGM(60,56,PSTR("Lap "));
    TV.printPGM(60,64,PSTR("Finished"));
}


void intro() {
    unsigned char w,l,wb;
    int index;
    w = pgm_read_byte(TVOlogo);
    l = pgm_read_byte(TVOlogo+1);
    if (w&7)
        wb = w/8 + 1;
    else
        wb = w/8;
    index = wb*(l-1) + 2;
    for ( unsigned char i = 1; i < l; i++ ) {
        TV.bitmap((TV.hres() - w)/2,0,TVOlogo,index,w,i);
        index-= wb;
        TV.delay(50);
    }
    for (unsigned char i = 0; i < (TV.vres() - l)/2; i++) {
        TV.bitmap((TV.hres() - w)/2,i,TVOlogo);
        TV.delay(50);
    }
    TV.printPGM(24,64,PSTR("Ranch Racing"));
    TV.delay(3000);
    TV.clear_screen();
}

void convertTimeString(unsigned long timeToConvert)
{
    char tempArr[4];
    char tempArr2[4];
    char tempArr3[4];
    char laptimeString[9];
    int minute = timeToConvert/60000;
    int seconds = (timeToConvert/1000)%60;
    int milliseconds = (timeToConvert%60000)%1000;
    itoa(minute, tempArr, 10);
    itoa(seconds, tempArr2, 10);
    itoa(milliseconds, tempArr3, 10);
    if (tempArr[1]) {
        laptimeString[0]=tempArr[0];
        laptimeString[1]=tempArr[1];
    }
    else
    {
        laptimeString[0] = '0';
        laptimeString[1] = tempArr[0];
    }
    laptimeString[2] = ':';
    
    if (tempArr2[1]) {
        laptimeString[3]=tempArr2[0];
        laptimeString[4]=tempArr2[1];
    }
    else
    {
        laptimeString[3] = '0';
        laptimeString[4] = tempArr2[0];
    }
    laptimeString[5] = ':';
    
    if (tempArr3[1]) {
        laptimeString[6]=tempArr3[0];
        laptimeString[7]=tempArr3[1];
    }
    else
    {
        laptimeString[6] = '0';
        laptimeString[7] = tempArr3[0];
    }
    laptimeString[8] = '\0';
    for (int i=0; i<11; i++) {
        tempLapTimeString[i] = laptimeString[i];
    }
    
}

