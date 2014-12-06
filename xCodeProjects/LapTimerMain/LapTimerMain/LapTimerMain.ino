///
/// @mainpage	lapTimerMainUnit
///
/// @details	main unit for arduino based lap timing syetem
/// @n
/// @n
/// @n @a		Developed with [embedXcode+](http://embedXcode.weebly.com)
///
/// @author		Nicholas Germon
/// @author		Nicholas Germon
/// @date		5/11/14 12:04 AM
/// @version	<#version#>
///
/// @copyright	(c) Nicholas Germon, 2014
/// @copyright	GNU
///
/// @see		ReadMe.txt for references
///


///
/// @file		lapTimerMainUnit.ino
/// @brief		Main sketch
///
/// @details	<#details#>
/// @n @a		Developed with [embedXcode+](http://embedXcode.weebly.com)
///
/// @author		Nicholas Germon
/// @author		Nicholas Germon
/// @date		5/11/14 12:04 AM
/// @version	<#version#>nb
///
/// @copyright	(c) Nicholas Germon, 2014
/// @copyright	All rights reserved
///
/// @see		ReadMe.txt for references
/// @n
///

#include "Arduino.h"
#include <DS1302.h>
#include <EEPROM.h>
#include <UTFT.h>
#include <UTouch.h>
#include <Wire.h>
#include "EEPROMAnything.h"
#include <SPI.h>
#include <SoftwareSerial.h>
#include "Thermal.h"
#include <MFRC522.h>
#include <EasyTransfer.h>
#include "logo.h"
#include <avr/pgmspace.h>




/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////Change These To Customize Your  Timer//////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

//Choose Your Model of screen, this sketch should work with all sizes
// I highly recommend 4.3 inch screen if you do not want to mess with anything
// My unit uses the 4.3 inch so that one has been extensively tested
//UTFT        myGLCD(CTE70, 38,39,40,41);   // Remember to change the model parameter to suit your display module!
//UTFT        myGLCD(SSD1289, 38,39,40,41);   // Remember to change the model parameter to suit your display module!
UTFT        myGLCD(ITDB43, 38,39,40,41);   // Remember to change the model parameter to suit your display module!
//UTFT        myGLCD(ITDB32, 38,39,40,41);   // Remember to change the model parameter to suit your display module!

byte lcdSize = 2; //1 = 3.2inch, 2 = 4.3inch and above
static char headerBar[20] = {"Ranch Racing"}; //This will be at the top of the Display at almost all times Max 19 characters
static char subLine[20] = {"Racing At The Ranch"}; //This will be at the top of the Printed receipt Max 19 characters
static char urlLine[20] = {"ranchracing.com"}; //This will be at the bottom of the Printed receipt Max 19 characters
static char kartAPrintName[9] = {"Kart 1"}; // Kart Name To be Displayed Max 8 characters
static char kartBPrintName[9] = {"Kart 2"}; // Kart Name To be Displayed Max 8 characters


/////////////Set the Clock for the first Run/////////////////////////////////////////////
static uint8_t monthSet = 7; //set the clock on first run Month
static uint8_t daySet = 19; //set the clock on first run Day
static uint16_t yearSet = 2014; //set the clock on first run Year

static uint8_t hourSet = 13; //set the clock on first run Hour   24hr time eg 5pm = 17       12am = 0
static uint8_t minuteSet = 32; //set the clock on first run Minute
static uint8_t secondSet = 5; //set the clock on first run Second

char trackName1[9] = {"full"}; //Max 8 characters




/////////////Erase all memory and start over

static boolean forceFirst = false;  // if set to true this will erase all system memory clock will not be reset

/////////////Change the time and date without Erasing the memory

static boolean forceTimeSet = false;  // if set to true this will set the time and date with the variables set above under the "Set the Clock for the First Run" heading


/////////////NO Passing Rule in Effect
static boolean noPassing = true;



//To change the logo on the receipt
//Follow this tutorial Here
//https://learn.adafruit.com/mini-thermal-receipt-printer/bitmap-printing
//then replace the content of the logo_h array with your own(in the file logo.h)
//make sure the image size is 376*90

/////Advanced customization////////////////////////
//Change Variables to custom files if you want if you do not know how do not change this
//it will use the standard font inside utft
extern uint8_t BigFont[];
extern uint8_t SmallFont[];
//font size variables Only change this if you change your font
//it will affect how the layout looks
byte fontAY = 32; //largerFont
byte fontAX = 24; //largerFont
byte fontBY = 16; //smallerFont
byte fontBX = 16; //smallerFont

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////End Of Customization////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////



#define SS_PIN 53
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance.


///Setup Hardware Pins


DS1302 rtc(10, 11, 12);  //Real Time Clock
UTouch      myTouch(6,5,4,3,2);
int x, y; //touch variables for xy position of finger

//printer pins using software serial
int printer_RX_Pin = 2;  // This is the green wire
int printer_TX_Pin = 13;  // This is the yellow wire







///Declare objects

EasyTransfer ET, ET2;
Thermal printer(printer_RX_Pin, printer_TX_Pin,19200);

byte redYellowGreenPins[3] = {A0,A1,A2};


int firstRun = true;

//screen Variables
int screenXSize;
int screenYSize;
int rightButtonX1;
int rightButtonY1;
int rightButtonLabelY;
int leftButtonX2;
int rightButtonLabelX;
int leftButtonLabelX;
int bottomX1;
int bottomX2;




//printer variables
int heatTime = 80; //80 is default from page 23 of datasheet. Controls speed of printing and darkness
int heatInterval = 2; //2 is default from page 23 of datasheet. Controls speed of printing and darkness
char printDensity = 15; //Not sure what the defaut is. Testing shows the max helps darken text. From page 23.
char printBreakTime = 15; //Not sure what the defaut is. Testing shows the max helps darken text. From page 23.
#define FALSE  0
#define TRUE  1
int printOnBlack = FALSE;
int printUpSideDown = FALSE;

boolean stopLightConnected = false;

//Player Variables
struct player
{
    char name [10];
    byte number;
    byte lastRaceLaps;
    unsigned long lastBestLap;
    unsigned long lastTotalTime;
    byte lastRaceMonth;
    byte lastRaceDay;
    byte lastRaceHour;
    byte lastRaceMinute;
    byte lastRaceTrack;
    unsigned long lapTimes[20];
    int kartUsedLast;
    int numberOfRaces;
    int kartUsed[4];
    byte cardID[5];
    byte nameSize;
    unsigned long bestLap[2];
    
};
typedef struct player Player;
Player player[2]; //current players that have been loaded from memory
Player prevPlayer[2];//players that just raced
Player tempPlayer; //temporary loaded player when card is swiped and used to print players player is loaded into temp from memory then transferred to player[0] or 1 if racing is chosen
Player tempPlayerRFID[2]; //current players that have been loaded from memory


///Structs For Send And Receive
struct IN_FROM_RECEIVER{
    char verify[3];
    byte kartID;
    byte lapID;
    unsigned long lapTime;
};
IN_FROM_RECEIVER myLap; //ir receiver // raw laptime from the receiver



struct topTenSend
{
    char name[8];
    char nameA[8];
    char nameB[8];
    byte topTenList;
    byte rank;
    byte playerNumber;
    unsigned long lapTime;
    byte isFinishResults;
    unsigned long totalTime[2];
    unsigned long bestLap[2];
};
typedef struct topTenSend TopTenSend;
TopTenSend sendTopTen;

///Top Ten Standings Variables
struct topTen
{
    byte player[10];
    unsigned long lapTime[10];
};
typedef struct topTen TopTen;
TopTen kartTopTen[12]; //0-3 track1 4-7 trackTwo 8-11 trackThree {top ten scores for kart 1 kart 2, daily top ten kart 1 and 2}

//Memory Locations
int playerMemoryLocation[26] ={
    0,150,300,450,600,750,900,1050,1200,1350,1500,1650,1800,1950,2100,2250,2400,2550,2700,2850,3000,3150,3300,3450,3600,3750}; // can hold 20 players in memory
int playerNumMemLoc = 4003;  // how many players have been created
int firstTimeSetupLocation = 4001;   //is first time setup
int topTenMemoryLocations[2] = {
    3900,3950};  ///top ten lists with times and names

///currentRacers
struct race
{
    byte minute;
    byte hour;
    unsigned long lastLap;
    unsigned long totalTime;
    unsigned long bestLap;
    unsigned long lapTimes[20];
    int lapCount;
    int lapsInRace;
    boolean isFinished;
};
typedef struct race Race;
Race racer[2];            //info on current race, info is transferred to player after the race then stored in memory


//laps in current race info
struct lap
{
    byte lapID;
    unsigned long lapTime;
    byte kart;
};
typedef struct lap Lap;
Lap lapIn;      //transferred from myLap when a new unique lap comes in
Lap lastLap[2]; //both racers last lap



//race status variables
boolean raceOn;
boolean quitRace = false;
byte currentLightOn = 1;
boolean soloMode = false;
byte soloModeKart;
byte currentRaceTrack;
boolean notRacing = true;
int lapsInCurrentRace = 5;


//navigation variables
byte currentScreen;
boolean isNewPlayer = false;
char nameAddInput [10];
int nameAddInputLen;
byte activatedPlayers;
boolean rfidReady =true;
byte currentTheme = 5;
byte firstTimeSetup = 0;
boolean kartChosen[2] ={false,false};

unsigned long checkBoxesDelay = 12000;
unsigned long checkBoxesPCheck = 0;
boolean checkedBoxes = false;
boolean connectedToIRBox = false;

//dateTime variable inc conversion


char tempLapTimeString[10];
unsigned long delayTimer;
unsigned long stopLightDelay = 0;

int currentTimeDate[6] = {
    0,0,0,0,0,0}; //month,day,year,hour,minute,second


static char* comeBacks[]={"Do You Quit In", "Everything or", "Just Racing?",
    "Good Move", "You Would Have","Lost!!",
    "No Problem","I knew You","Couldnt Compete",
    "Player Banned","For Quitting! j/k","You do suck though",
    "You Fooled Me,","For a Sec I Thought","You had Balls",
    "You Went Longer","Than We Thought","You Would"};


unsigned long cardLastRead = 0;

//currentRacers
struct tvout
{
    boolean raceOver;
    char nameA[10];
    char nameB[10];
    char nameC[10];
    char nameD[10];
    unsigned long lastLap[4];
    unsigned long totalTime[4];
    unsigned long bestLap[4];
    int lapCount[2];
    int lapsInRace;
};
typedef struct tvout Tvout;
Tvout outToTV;            //info on current race, info is transferred to player after the race then stored in memory


/////////////////////////////////////////////////////////////////////////////
void SetTime()
{
    
    rtc.writeProtect(false);
    rtc.setTime(hourSet, minuteSet, secondSet);     // Set the time to 12:00:00 (24hr format)
    delay(20);
    rtc.setDate(daySet, monthSet, yearSet);   // Set the date to july 13th, 2014    day, month, year
    
}





#pragma mark Prototypes/////////////////////////////////////

void BackLightOnOff (boolean yesNo);
void FillScreenColor();
void ChooseSmallerFont();
void TextColor();
void BackgroundColor();
void ChooseLargerFont();
void ButtonColor();
void ButtonLabelColor();
void VariableColor();
void Setupplayer0();

int GetLabelX (int buttonX1, int buttonX2, int lengthWord);
int GetLabelY (int buttonY1, int buttonY2);
void RedYellowGreenChange(byte lightOn);
void UpdateLapsTimes();
void EndRace();
void TopTenTimes(int whichKart, int whichList);
void convertTimeString(unsigned long timeToConvert);
void MarkTouched (int x1, int y1, int x2, int y2);
void RunModesDraw();
void RacerAorBDraw();
void PrintLastRace();
void SoloModeAorBDraw();
void startRace();
void PrintRacerAorBDraw();
void ButtonPressedColor();
void CompareLastRacePlayerMemory(int whichPlayer);
void ReceiveLapTime();
void MainScreenTouch();
void NameUpdateStr(int val);
void NameUpdateDelStr();
void restartButton();
int GetLabelXLargerFont (int buttonX1, int buttonX2, int lengthWord);
int GetLabelYLargerFont (int buttonY1, int buttonY2);
void NameInputDraw();
void AfterRaceScreenDraw();
void CreateNewPlayerDraw();
#pragma mark End Of Prototypes///////////////////////////////




void Setupplayer0()  ////used during first time setup for a NA player located at position 0
{
    player[0].name [0]='N';
    player[0].name [1]='/';
    player[0].name [2]='A';
    //player[0].name [3]='';
    player[0].name [3]='\0';
    player[0].number = 0;
    player[0].lastRaceLaps = 5;
    player[0].lastBestLap =160000;
    player[0].lastTotalTime =1040000;
    player[0].lastRaceMonth =12;
    player[0].lastRaceDay =14;
    player[0].lastRaceHour = 6;
    player[0].lastRaceMinute = 30;
    
    player[0].bestLap[0] = 9;
    player[0].bestLap[1] = 10;
    player[0].lapTimes[0] = 160000;
    player[0].lapTimes[1] = 239860;
    player[0].lapTimes[2] = 334562;
    player[0].lapTimes[3] = 412355;
    player[0].lapTimes[4] = 512455;
    player[0].lapTimes[5] = 512455;
    player[0].kartUsedLast = 1;
    player[0].numberOfRaces = 5;
    player[0].kartUsed[0] = 0;
    player[0].kartUsed[1] = 4;
    player[0].kartUsed[2] = 3;
    player[0].kartUsed[3] = 2;
    player[0].cardID[0] = 234;
    player[0].cardID[1] = 34;
    player[0].cardID[2] = 250;
    player[0].cardID[3] = 240;
    player[0].cardID[4] = 220;
    player[0].nameSize = 4;
}





void screenSize()   //setup screen size and button variables
{
    
    screenXSize = myGLCD.getDisplayXSize();
    screenYSize = myGLCD.getDisplayYSize();
    rightButtonX1 =(screenXSize / 2) + (screenXSize / 8);
    rightButtonY1 = screenYSize - (screenYSize/6);
    rightButtonLabelY = (((screenYSize - rightButtonY1) - fontAY)/2) + rightButtonY1;
    leftButtonX2 =(screenXSize / 2) - (screenXSize / 8);
    bottomX1 =(screenXSize / 2) + (screenXSize / 4);
    bottomX2 =(screenXSize / 2) - (screenXSize / 4);
    
}






#pragma mark Screens

void DrawHeaderBar() //clears screen and draws the header bar
{
    myGLCD.fillScr(0, 0, 0);
    FillScreenColor();
    ChooseSmallerFont();
    TextColor();
    myGLCD.print(headerBar, CENTER, 0);
    
}



void QuitterDraw() //clears screen and draws the header bar
{
    randomSeed(analogRead(0));
    DrawHeaderBar();
    int randNumber = random(5);
    randNumber = randNumber*3;
    ChooseLargerFont();
    VariableColor();
    for (int x1 = 2; x1<5; x1++) {
        myGLCD.print(comeBacks[randNumber], CENTER, x1*fontAY);
        randNumber++;
    }
    
    
    
    
}


void DrawSwipeScreen()
{
    DrawHeaderBar();
    ChooseLargerFont();
    VariableColor();
    myGLCD.print("Swipe Card", CENTER, 2*fontAY);
}


void StartScreenDraw() // Screen Used to Start the race AKA currentScreen == 1
{
    DrawHeaderBar();
    int baseline = fontBY+(screenYSize/20)+ (fontBY/3);
    currentScreen = 1;
    int firstRowYVariable = baseline + fontBY + (fontBY/3);
    int firstColumnBase = screenXSize/20;
    int secondColumnBase = (screenXSize/2) + (screenXSize/20);
    ChooseLargerFont();
    ButtonColor();
    myGLCD.fillRoundRect(bottomX1, rightButtonY1, bottomX2, screenYSize-1);
    //Settings and Run Mode Buttons
    ButtonLabelColor();
    myGLCD.print("Start", CENTER, rightButtonLabelY);
    TextColor();
    ChooseLargerFont();
    myGLCD.print("Start", CENTER, baseline);
}

void StartScreenTouch()  // Screen Used to Start the race AKA currentScreen == 1
{
    myTouch.read();    // check for touch
    x=myTouch.getX();  // x coordinate of touch
    y=myTouch.getY();  // Y coordinate of touch
    BackgroundColor();
    if((y>=rightButtonY1) && (y<=screenYSize - 3))
    {
        MarkTouched(bottomX1, rightButtonY1, bottomX2,  screenYSize);
        startRace();
    }
}


void ShowRacersDraw() // Screen Used to Start the race AKA currentScreen == 1
{
    DrawHeaderBar();
    int baseline = fontBY+(screenYSize/20)+ (fontBY/3);
    currentScreen = 2;
    int firstRowYVariable = baseline + fontBY + (fontBY/3);
    int firstColumnBase = screenXSize/20;
    int secondColumnBase = (screenXSize/2) + (screenXSize/20);
    ChooseLargerFont();
    ButtonColor();
    myGLCD.fillRoundRect(bottomX1, rightButtonY1, bottomX2, screenYSize-1);
    //Settings and Run Mode Buttons
    ButtonLabelColor();
    myGLCD.print("Done", CENTER, rightButtonLabelY);
    TextColor();
    ChooseSmallerFont();
    int xx;
    if (activatedPlayers > 10) {
        xx = 10;
    }
    else
    {
        xx = activatedPlayers-1;
    }
    for (int x1=1; x1<=xx; x1++) {
        if (x1%2) {
            TextColor();
        }
        else
        {
            VariableColor();
        }
        EEPROM_readAnything(playerMemoryLocation[x1], tempPlayer);
        myGLCD.print(tempPlayer.name, 2*fontBX, baseline);
        myGLCD.printNumI(x1, LEFT, baseline);
        baseline = baseline + fontBY;
    }
    if (activatedPlayers > 10) {
        for (int x1=11; x1<activatedPlayers; x1++) {
            if (x%2) {
                TextColor();
            }
            else
            {
                VariableColor();
            }
            EEPROM_readAnything(playerMemoryLocation[x1], tempPlayer);
            myGLCD.print(tempPlayer.name, 2*fontBX, baseline);
            myGLCD.printNumI(x1, LEFT, baseline);
            baseline = baseline + fontBY;
        }
    }
}


void ShowRacersTouch()  // Screen Used to Start the race AKA currentScreen == 1
{
    myTouch.read();    // check for touch
    x=myTouch.getX();  // x coordinate of touch
    y=myTouch.getY();  // Y coordinate of touch
    BackgroundColor();
    if((y>=rightButtonY1) && (y<=screenYSize - 3))
    {
        MarkTouched(bottomX1, rightButtonY1, bottomX2,  screenYSize);
        AfterRaceScreenDraw();
    }
}
int racerToDelete = 2;
void DeleteRacerDraw() // Screen Used to Choose amount of laps in race AKA currentScreen == 10
{
    DrawHeaderBar();
    currentScreen = 11;
    ///////////////////////////bottom button
    ButtonColor();
    ChooseLargerFont();
    myGLCD.fillRoundRect(bottomX1, rightButtonY1, bottomX2, screenYSize-1);
    ButtonLabelColor();
    myGLCD.print("Delete", CENTER, rightButtonLabelY);
    ////middle button locations
    int buttonHeightWidth = screenYSize/6;
    int buttonSpace = screenYSize/12;
    int inputWidth = fontAX*4;
    int baseline = fontBY + (screenYSize/15);
    int button2y1 = baseline + buttonHeightWidth + buttonSpace;
    int button2y2 = button2y1 + buttonHeightWidth;
    int buttonLX2 = (screenXSize/2) - (inputWidth/2) - 2;
    int buttonLX1 = buttonLX2 - buttonHeightWidth;
    int buttonRX1 = (screenXSize/2) + (inputWidth/2) + 2;
    int buttonRX2 = buttonRX1 + buttonHeightWidth;
    int fontAYOffset = 3;
    TextColor();
    ChooseSmallerFont();
    myGLCD.print("Delete Racer",CENTER,GetLabelY(button2y1, button2y2) - fontBY);
    ButtonColor();
    myGLCD.fillRoundRect (buttonRX1, button2y1, buttonRX2, button2y2); //plus button
    myGLCD.fillRoundRect (buttonLX1, button2y1, buttonLX2, button2y2); //minus  button
    
    ButtonLabelColor();
    ChooseLargerFont();
    myGLCD.print("+", GetLabelXLargerFont(buttonRX1, buttonRX2, 1), GetLabelYLargerFont(button2y1, button2y2) + fontAYOffset); //plus button symbol
    myGLCD.print("-", GetLabelXLargerFont(buttonLX1, buttonLX2, 1), GetLabelYLargerFont(button2y1, button2y2) + fontAYOffset); //minus button symbol
    VariableColor();
    if (racerToDelete < 2 || racerToDelete > activatedPlayers - 1)
    {
        racerToDelete = activatedPlayers - 1;
    }
    myGLCD.printNumI(racerToDelete, CENTER, GetLabelY(button2y1, button2y2)); //laps in race field  print to field
}



void DeleteRacerTouch(){
    myTouch.read();    // check for touch
    x=myTouch.getX();  // x coordinate of touch
    y=myTouch.getY();  // Y coordinate of touch
    ////middle button locations
    int buttonHeightWidth = screenYSize/6;
    int buttonSpace = screenYSize/12;
    int inputWidth = fontAX*4;
    int baseline = fontBY + (screenYSize/15);
    int button2y1 = baseline + buttonHeightWidth + buttonSpace;
    int button2y2 = button2y1 + buttonHeightWidth;
    int button3y1 = button2y1 + buttonHeightWidth + buttonSpace;
    int button3y2 = button3y1 + buttonHeightWidth;
    int buttonLX2 = (screenXSize/2) - (inputWidth/2) - 2;
    int buttonLX1 = buttonLX2 - buttonHeightWidth;
    int buttonRX1 = (screenXSize/2) + (inputWidth/2) + 2;
    int buttonRX2 = buttonRX1 + buttonHeightWidth;
    //cancel button
    if((y>=rightButtonY1) && (y<=screenYSize - 3))
    {
        MarkTouched(bottomX1, rightButtonY1, bottomX2, screenYSize-1);
        if (racerToDelete == activatedPlayers -1) {
            activatedPlayers--;
        }
        else{
            EEPROM_readAnything(playerMemoryLocation[activatedPlayers -1], tempPlayer);
            tempPlayer.number = racerToDelete;
            EEPROM_writeAnything(playerMemoryLocation[racerToDelete], tempPlayer);
            activatedPlayers--;
        }
        StartScreenDraw();
    }
    if((y>=button2y1) && (y<=button2y2))
    {
        if ((x>=buttonLX1) && (x<=buttonLX2)){ //minus button
            MarkTouched(buttonLX1, button2y1, buttonLX2, button2y2);
            racerToDelete = racerToDelete--;
            if (racerToDelete < 1)
            {
                racerToDelete = 1;
            }
            VariableColor();
            myGLCD.print("    ", CENTER, GetLabelY(button2y1, button2y2)); //  erase  field
            myGLCD.printNumI(racerToDelete, CENTER, GetLabelY(button2y1, button2y2)); //  print to field
        }
        
        if ((x>=buttonRX1) && (x<=buttonRX2)){ //plus button
            MarkTouched(buttonRX1, button2y1, buttonRX2, button2y2);
            racerToDelete = racerToDelete++;
            if (racerToDelete > activatedPlayers -1)
            {
                racerToDelete = activatedPlayers -1;
            }
            VariableColor();
            myGLCD.print("    ", CENTER, GetLabelY(button2y1, button2y2)); //  erase  field
            myGLCD.printNumI(racerToDelete,CENTER, GetLabelY(button2y1, button2y2)); //  print to field
        }
    }
    
    
}


void AmountOfLapsDraw() // Screen Used to Choose amount of laps in race AKA currentScreen == 10
{
    DrawHeaderBar();
    currentScreen = 10;
    ///////////////////////////bottom button
    ButtonColor();
    ChooseLargerFont();
    myGLCD.fillRoundRect(bottomX1, rightButtonY1, bottomX2, screenYSize-1);
    ButtonLabelColor();
    myGLCD.print("Confirm", CENTER, rightButtonLabelY);
    ////middle button locations
    int buttonHeightWidth = screenYSize/6;
    int buttonSpace = screenYSize/12;
    int inputWidth = fontAX*4;
    int baseline = fontBY + (screenYSize/15);
    int button2y1 = baseline + buttonHeightWidth + buttonSpace;
    int button2y2 = button2y1 + buttonHeightWidth;
    int buttonLX2 = (screenXSize/2) - (inputWidth/2) - 2;
    int buttonLX1 = buttonLX2 - buttonHeightWidth;
    int buttonRX1 = (screenXSize/2) + (inputWidth/2) + 2;
    int buttonRX2 = buttonRX1 + buttonHeightWidth;
    int fontAYOffset = 3;
    TextColor();
    ChooseSmallerFont();
    myGLCD.print("Laps",CENTER,GetLabelY(button2y1, button2y2) - fontBY);
    ButtonColor();
    myGLCD.fillRoundRect (buttonRX1, button2y1, buttonRX2, button2y2); //plus button
    myGLCD.fillRoundRect (buttonLX1, button2y1, buttonLX2, button2y2); //minus  button
    
    ButtonLabelColor();
    ChooseLargerFont();
    myGLCD.print("+", GetLabelXLargerFont(buttonRX1, buttonRX2, 1), GetLabelYLargerFont(button2y1, button2y2) + fontAYOffset); //plus button symbol
    myGLCD.print("-", GetLabelXLargerFont(buttonLX1, buttonLX2, 1), GetLabelYLargerFont(button2y1, button2y2) + fontAYOffset); //minus button symbol
    VariableColor();
    if (lapsInCurrentRace < 1 || lapsInCurrentRace > 20)
    {
        lapsInCurrentRace = 7;
    }
    myGLCD.printNumI(lapsInCurrentRace, CENTER, GetLabelY(button2y1, button2y2)); //laps in race field  print to field
}

void AmountOfLapsTouch(){
    myTouch.read();    // check for touch
    x=myTouch.getX();  // x coordinate of touch
    y=myTouch.getY();  // Y coordinate of touch
    ////middle button locations
    int buttonHeightWidth = screenYSize/6;
    int buttonSpace = screenYSize/12;
    int inputWidth = fontAX*4;
    int baseline = fontBY + (screenYSize/15);
    int button2y1 = baseline + buttonHeightWidth + buttonSpace;
    int button2y2 = button2y1 + buttonHeightWidth;
    int button3y1 = button2y1 + buttonHeightWidth + buttonSpace;
    int button3y2 = button3y1 + buttonHeightWidth;
    int buttonLX2 = (screenXSize/2) - (inputWidth/2) - 2;
    int buttonLX1 = buttonLX2 - buttonHeightWidth;
    int buttonRX1 = (screenXSize/2) + (inputWidth/2) + 2;
    int buttonRX2 = buttonRX1 + buttonHeightWidth;
    //cancel button
    if((y>=rightButtonY1) && (y<=screenYSize - 3))
    {
        MarkTouched(bottomX1, rightButtonY1, bottomX2, screenYSize-1);
        StartScreenDraw();
    }
    if((y>=button2y1) && (y<=button2y2))
    {
        if ((x>=buttonLX1) && (x<=buttonLX2)){ //minus button
            MarkTouched(buttonLX1, button2y1, buttonLX2, button2y2);
            lapsInCurrentRace = lapsInCurrentRace--;
            if (lapsInCurrentRace < 0)
            {
                lapsInCurrentRace = 0;
            }
            VariableColor();
            myGLCD.print("    ", CENTER, GetLabelY(button2y1, button2y2)); //  erase  field
            myGLCD.printNumI(lapsInCurrentRace, CENTER, GetLabelY(button2y1, button2y2)); //  print to field
        }
        
        if ((x>=buttonRX1) && (x<=buttonRX2)){ //plus button
            MarkTouched(buttonRX1, button2y1, buttonRX2, button2y2);
            lapsInCurrentRace = lapsInCurrentRace++;
            if (lapsInCurrentRace > 20)
            {
                lapsInCurrentRace = 20;
            }
            VariableColor();
            myGLCD.print("    ", CENTER, GetLabelY(button2y1, button2y2)); //  erase  field
            myGLCD.printNumI(lapsInCurrentRace,CENTER, GetLabelY(button2y1, button2y2)); //  print to field
        }
    }
    
    
}

void WaitingScreenDraw() // Screen Used for waiting for another player AKA currentScreen == 2
{
    DrawHeaderBar();
    int baseline = fontBY+(screenYSize/20)+ (fontBY/3);
    currentScreen = 2;
    TextColor();
    ChooseSmallerFont();
    myGLCD.print("Waiting For Another", CENTER, baseline);
    ChooseLargerFont();
    myGLCD.print("Player", CENTER, baseline +fontAY);
}


void AfterRaceScreenDraw() // Shows the stats after a race AKA currentScreen == 0
{
    if (!prevPlayer[0].number && !prevPlayer[1].number) {
        DrawSwipeScreen();
        return;
    }
    DrawHeaderBar();
    int baseline = fontBY+(screenYSize/20)+ (fontBY/3);
    currentScreen = 0;
    BackgroundColor();
    delay(10);
    int firstRowYVariable = baseline + (fontBY) + (fontBY/3);
    int firstColumnBase = screenXSize/20;
    int secondColumnBase = (screenXSize/2) + (screenXSize/20);
    ChooseLargerFont();
    ButtonColor();
    myGLCD.fillRoundRect(bottomX1, rightButtonY1, bottomX2, screenYSize-1);
    ButtonLabelColor();
    myGLCD.print("Rematch", CENTER, rightButtonLabelY);
    TextColor();
    ChooseSmallerFont();
    myGLCD.print(kartAPrintName, firstColumnBase, baseline);
    VariableColor();
    ChooseLargerFont();
    myGLCD.print(prevPlayer[0].name, firstColumnBase, firstRowYVariable);
    TextColor();
    ChooseSmallerFont();
    myGLCD.print(kartBPrintName, secondColumnBase, baseline);
    VariableColor();
    ChooseLargerFont();
    myGLCD.print(prevPlayer[1].name, secondColumnBase, firstRowYVariable);
    TextColor();
    ChooseSmallerFont();
    myGLCD.print("Last", firstColumnBase, firstRowYVariable + fontAY);
    VariableColor();
    ChooseLargerFont();
    convertTimeString(prevPlayer[0].lapTimes[prevPlayer[0].lastRaceLaps]);
    myGLCD.print(tempLapTimeString, firstColumnBase, firstRowYVariable + fontAY +fontBY);
    TextColor();
    ChooseSmallerFont();
    myGLCD.print("Last", secondColumnBase, firstRowYVariable + fontAY);
    VariableColor();
    ChooseLargerFont();
    int lastLaps = prevPlayer[1].lastRaceLaps;
    convertTimeString(prevPlayer[1].lapTimes[lastLaps]);
    myGLCD.print(tempLapTimeString, secondColumnBase, firstRowYVariable  + fontAY +fontBY);
    TextColor();
    ChooseSmallerFont();
    myGLCD.print("Best", firstColumnBase, firstRowYVariable  + (2*fontAY) +fontBY);
    VariableColor();
    ChooseLargerFont();
    convertTimeString(prevPlayer[0].lastBestLap);
    myGLCD.print(tempLapTimeString, firstColumnBase, firstRowYVariable  + (2*fontAY) +(2*fontBY));
    TextColor();
    ChooseSmallerFont();
    myGLCD.print("Best", secondColumnBase, firstRowYVariable  + (2*fontAY) +fontBY);
    VariableColor();
    ChooseLargerFont();
    convertTimeString(prevPlayer[1].lastBestLap);
    myGLCD.print(tempLapTimeString, secondColumnBase, firstRowYVariable  + (2*fontAY) +(2*fontBY));
    TextColor();
    ChooseSmallerFont();
    myGLCD.print("Total", firstColumnBase, firstRowYVariable  + (3*fontAY) +(2*fontBY));
    VariableColor();
    ChooseLargerFont();
    convertTimeString(prevPlayer[0].lastTotalTime);
    myGLCD.print(tempLapTimeString, firstColumnBase, firstRowYVariable  + (3*fontAY) +(3*fontBY));
    TextColor();
    ChooseSmallerFont();
    myGLCD.print("Total", secondColumnBase, firstRowYVariable  + (3*fontAY) +(2*fontBY));
    VariableColor();
    ChooseLargerFont();
    convertTimeString(prevPlayer[1].lastTotalTime);
    myGLCD.print(tempLapTimeString, secondColumnBase, firstRowYVariable  + (3*fontAY) +(3*fontBY));
    // if the unit has just been powered on the screen will go to FirstScreen which says swipe card
    if (prevPlayer[0].number == 0 && prevPlayer[1].number == 0) {
        DrawSwipeScreen();
    }
}


void AfterRaceScreenTouch() //touch control and variable updating for mainScreen Screen 0
{
    myTouch.read();    // check for touch
    x=myTouch.getX();  // x coordinate of touch
    y=myTouch.getY();  // Y coordinate of touch
    //middle colum y coordinates
    int middleTopLabel = (screenYSize/4) - ((fontBY +fontAY)/2);
    int middleBottomLabel = (screenYSize - (screenYSize/4)) - ((fontBY +fontAY)/2);
    BackgroundColor();
    if((y>=rightButtonY1) && (y<=screenYSize - 3))
    {
        MarkTouched(bottomX1, rightButtonY1, bottomX2,  screenYSize);
        
        if (!raceOn) {
            restartButton();
        }
    }
}



void MainScreenDraw() // main screen shown during a race
{
    currentScreen = 0;
    if (!player[0].number && !player[1].number) {
        DrawSwipeScreen();
        return;
    }
    DrawHeaderBar();
    
    int baseline = fontBY+(screenYSize/27);
    BackgroundColor();
    delay(10);
    int firstRowYVariable = baseline + (fontBY) + (fontBY/3);
    int firstColumnBase = screenXSize/20;
    int secondColumnBase = (screenXSize/2) + (screenXSize/20);
    
    ChooseSmallerFont();
    TextColor();
    myGLCD.print("Current Lap", firstColumnBase, screenYSize - 1 - fontAY - fontBY);
    myGLCD.print("Current Lap", secondColumnBase, screenYSize - 1 - fontAY - fontBY);
    VariableColor();
    ChooseLargerFont();
    
    if (raceOn) {
        if (racer[0].lapCount <= racer[0].lapsInRace) {
            if (player[0].number) {
                myGLCD.printNumI(racer[0].lapCount, firstColumnBase, screenYSize - 1 - fontAY);
                myGLCD.print("of", firstColumnBase + (2*fontAY), screenYSize - 1 - fontAY);
                myGLCD.printNumI(racer[0].lapsInRace, firstColumnBase + (4*fontAY), screenYSize - 1 - fontAY);
            }
            
        }
        else
        {
            leftButtonLabelX = (leftButtonX2 - (fontAX * 8))/2;
            myGLCD.print("Finished", firstColumnBase, screenYSize - 1 - fontAY);
        }
        
        if (racer[1].lapCount <= racer[1].lapsInRace) {
            if (player[1].number) {
                myGLCD.printNumI(racer[1].lapCount, secondColumnBase, screenYSize - 1 - fontAY);
                myGLCD.print("of", secondColumnBase + (2*fontAY), screenYSize - 1 - fontAY);
                myGLCD.printNumI(racer[1].lapsInRace, secondColumnBase + (4*fontAY), screenYSize - 1 - fontAY);
            }
        }
        else
        {
            myGLCD.print("Finished", secondColumnBase, screenYSize - 1 - fontAY);
        }
    }
    TextColor();
    ChooseSmallerFont();
    myGLCD.print(kartAPrintName, firstColumnBase, baseline);
    VariableColor();
    ChooseLargerFont();
    myGLCD.print(player[0].name, firstColumnBase, firstRowYVariable);
    TextColor();
    ChooseSmallerFont();
    myGLCD.print(kartBPrintName, secondColumnBase, baseline);
    VariableColor();
    ChooseLargerFont();
    myGLCD.print(player[1].name, secondColumnBase, firstRowYVariable);
    TextColor();
    ChooseSmallerFont();
    myGLCD.print("Last", firstColumnBase, firstRowYVariable + fontAY);
    if (racer[0].lapCount > 1) {
        if (player[0].number) {
            VariableColor();
            ChooseLargerFont();
            convertTimeString(racer[0].lastLap);
            myGLCD.print(tempLapTimeString, firstColumnBase, firstRowYVariable + fontAY +fontBY);
        }
    }
    TextColor();
    ChooseSmallerFont();
    myGLCD.print("Last", secondColumnBase, firstRowYVariable + fontAY);
    if (racer[1].lapCount > 1) {
        if (player[1].number) {
            VariableColor();
            ChooseLargerFont();
            convertTimeString(racer[1].lastLap);
            myGLCD.print(tempLapTimeString, secondColumnBase, firstRowYVariable  + fontAY +fontBY);
        }
    }
    
    TextColor();
    ChooseSmallerFont();
    myGLCD.print("Best", firstColumnBase, firstRowYVariable  + (2*fontAY) +fontBY);
    if (player[0].number) {
        VariableColor();
        ChooseLargerFont();
        convertTimeString(racer[0].bestLap);
        myGLCD.print(tempLapTimeString, firstColumnBase, firstRowYVariable  + (2*fontAY) +(2*fontBY));
    }
    TextColor();
    ChooseSmallerFont();
    myGLCD.print("Best", secondColumnBase, firstRowYVariable  + (2*fontAY) +fontBY);
    if (player[1].number) {
        VariableColor();
        ChooseLargerFont();
        convertTimeString(racer[1].bestLap);
        myGLCD.print(tempLapTimeString, secondColumnBase, firstRowYVariable  + (2*fontAY) +(2*fontBY));
    }
    TextColor();
    ChooseSmallerFont();
    myGLCD.print("Total", firstColumnBase, firstRowYVariable  + (3*fontAY) +(2*fontBY));
    if (player[0].number) {
        VariableColor();
        ChooseLargerFont();
        convertTimeString(racer[0].totalTime);
        myGLCD.print(tempLapTimeString, firstColumnBase, firstRowYVariable  + (3*fontAY) +(3*fontBY));
    }
    TextColor();
    ChooseSmallerFont();
    myGLCD.print("Total", secondColumnBase, firstRowYVariable  + (3*fontAY) +(2*fontBY));
    if (player[1].number) {
        VariableColor();
        ChooseLargerFont();
        convertTimeString(racer[1].totalTime);
        myGLCD.print(tempLapTimeString, secondColumnBase, firstRowYVariable  + (3*fontAY) +(3*fontBY));
    }
    if (player[0].number == 0 && player[1].number == 0) {
        DrawSwipeScreen();
    }
}

//void AdminDraw()  // settings screen also screen 13
//{
//    DrawHeaderBar();
//    ButtonColor();
//    ChooseLargerFont();
//    myGLCD.fillRoundRect(bottomX1, rightButtonY1, bottomX2, screenYSize-1);
//    ButtonLabelColor();
//    myGLCD.print("Cancel", CENTER, rightButtonLabelY);
//    currentScreen = 12;
//    ///////////////////////////setup and clear screen
//    int baseline = fontBY+(screenYSize/20);
//    int boxHeight = (screenYSize*14)/100;
//    int lButtonsX1 = screenXSize/20;
//    int spacer = (screenYSize/10) + boxHeight;
//    int rButtonsX2 = screenXSize - (screenXSize/20);
//    int row1YLabel = GetLabelY(baseline, baseline + boxHeight);
//    
//    // First Column of buttons
//    for (y=1; y<2; y++)
//    {
//        ButtonColor();
//        myGLCD.fillRoundRect(lButtonsX1, baseline +(y*spacer), leftButtonX2, (baseline + boxHeight) +(y*spacer));
//    }
//    
//    //Second Column of buttons
//    for (y=1; y<2; y++)
//    {
//        ButtonColor();
//        myGLCD.fillRoundRect(rightButtonX1, baseline +(y*spacer), rButtonsX2, (baseline + boxHeight) +(y*spacer));
//    }
//    
//    
//    ButtonLabelColor();
//    ChooseSmallerFont();
//    myGLCD.print("Show Pl", GetLabelX(lButtonsX1, leftButtonX2, 8), GetLabelY(baseline +(1*spacer), (baseline + boxHeight) +(1*spacer)));
//    myGLCD.print("Delete Pl.", GetLabelX(rightButtonX1, rButtonsX2, 5), GetLabelY(baseline +(1*spacer), (baseline + boxHeight) +(1*spacer)));
//    
//    TextColor();
//    ChooseLargerFont();
//    myGLCD.print("                ", CENTER, baseline);
//    myGLCD.print(tempPlayer.name, CENTER, baseline);
//    myGLCD.print("                ", CENTER, baseline + (1*fontAY));
//    myGLCD.print("Choose", CENTER, baseline + (1*fontAY));
//    
//    
//}
//
//
//void AdminTouch(){ //screen 1
//    myTouch.read();    // check for touch
//    x=myTouch.getX();  // x coordinate of touch
//    y=myTouch.getY();  // Y coordinate of touch
//    ///////////////////////////setup and clear screen
//    int baseline = fontBY+(screenYSize/20);
//    int boxHeight = (screenYSize*14)/100;
//    int lButtonsX1 = screenXSize/20;
//    int spacer = (screenYSize/10) + boxHeight;
//    int rButtonsX2 = screenXSize - (screenXSize/20);
//    int row1YLabel = GetLabelY(baseline, baseline + boxHeight);
//    //cancel button
//    if((y>=rightButtonY1) && (y<=screenYSize - 3))
//    {
//        MarkTouched(bottomX1, rightButtonY1, bottomX2, screenYSize-1);
//        if (!notRacing) {
//            MainScreenDraw();
//        }
//        else if (kartChosen[0] || kartChosen[1])
//        {
//            WaitingScreenDraw();
//        }
//        else
//        {
//            AfterRaceScreenDraw();
//        }
//    }
//    
//    if ((x>=lButtonsX1) && (x<=leftButtonX2))  // first colum buttons
//    {
//        if ((y>=baseline +(1*spacer)) && (y<=(baseline + boxHeight) +(1*spacer)))
//        {
//            MarkTouched(lButtonsX1, baseline +(1*spacer), leftButtonX2, (baseline + boxHeight) +(1*spacer));
//            ShowRacersDraw();
//            
//        }
//    }
//    if ((x>=rightButtonX1) && (x<=rButtonsX2))  // second column buttons
//    {
//        if ((y>=baseline +(1*spacer)) && (y<=(baseline + boxHeight) +(1*spacer)))  // Prints last race
//        {
//            MarkTouched(rightButtonX1, baseline +(1*spacer), rButtonsX2, (baseline + boxHeight) +(1*spacer));
//            DeleteRacerDraw();
//        }
//        
//    }
//}


void PrintOrRaceDraw()  // settings screen also screen 13
{
    DrawHeaderBar();
    ButtonColor();
    ChooseLargerFont();
    myGLCD.fillRoundRect(bottomX1, rightButtonY1, bottomX2, screenYSize-1);
    ButtonLabelColor();
    myGLCD.print("Cancel", CENTER, rightButtonLabelY);
    currentScreen = 3;
    ///////////////////////////setup and clear screen
    int baseline = fontBY+(screenYSize/20);
    int boxHeight = (screenYSize*14)/100;
    int lButtonsX1 = screenXSize/20;
    int spacer = (screenYSize/10) + boxHeight;
    int rButtonsX2 = screenXSize - (screenXSize/20);
    int row1YLabel = GetLabelY(baseline, baseline + boxHeight);
    
    // First Column of buttons
    for (y=1; y<3; y++)
    {
        ButtonColor();
        myGLCD.fillRoundRect(lButtonsX1, baseline +(y*spacer), leftButtonX2, (baseline + boxHeight) +(y*spacer));
    }
    
    //Second Column of buttons
    for (y=1; y<2; y++)
    {
        ButtonColor();
        myGLCD.fillRoundRect(rightButtonX1, baseline +(y*spacer), rButtonsX2, (baseline + boxHeight) +(y*spacer));
    }
    if (tempPlayer.number == 1) {
        for (y=2; y<3; y++)
        {
            ButtonColor();
            myGLCD.fillRoundRect(rightButtonX1, baseline +(y*spacer), rButtonsX2, (baseline + boxHeight) +(y*spacer));
        }
    }
    
    
    ButtonLabelColor();
    ChooseSmallerFont();
    if (tempPlayer.number == player[0].number || tempPlayer.number == player[1].number) {
        myGLCD.print("Not Race", GetLabelX(lButtonsX1, leftButtonX2, 8), GetLabelY(baseline +(1*spacer), (baseline + boxHeight) +(1*spacer)));
    }
    else
    {
        if (notRacing) {
            myGLCD.print("Race", GetLabelX(lButtonsX1, leftButtonX2, 4), GetLabelY(baseline +(1*spacer), (baseline + boxHeight) +(1*spacer)));
        }
        
    }
    myGLCD.print("Edit", GetLabelX(lButtonsX1, leftButtonX2, 4), GetLabelY(baseline +(2*spacer), (baseline + boxHeight) +(2*spacer)));
    myGLCD.print("Print", GetLabelX(rightButtonX1, rButtonsX2, 5), GetLabelY(baseline +(1*spacer), (baseline + boxHeight) +(1*spacer)));
//    if (tempPlayer.number == 1) {
//        myGLCD.print("Admin", GetLabelX(rightButtonX1, rButtonsX2, 5), GetLabelY(baseline +(2*spacer), (baseline + boxHeight) +(2*spacer)));
//    }
    
    TextColor();
    ChooseLargerFont();
    myGLCD.print("                ", CENTER, baseline);
    myGLCD.print(tempPlayer.name, CENTER, baseline);
    myGLCD.print("                ", CENTER, baseline + (1*fontAY));
    myGLCD.print("Choose", CENTER, baseline + (1*fontAY));
    
    
}


void PrintOrRaceTouch(){ //screen 1
    myTouch.read();    // check for touch
    x=myTouch.getX();  // x coordinate of touch
    y=myTouch.getY();  // Y coordinate of touch
    ///////////////////////////setup and clear screen
    int baseline = fontBY+(screenYSize/20);
    int boxHeight = (screenYSize*14)/100;
    int lButtonsX1 = screenXSize/20;
    int spacer = (screenYSize/10) + boxHeight;
    int rButtonsX2 = screenXSize - (screenXSize/20);
    int row1YLabel = GetLabelY(baseline, baseline + boxHeight);
    //cancel button
    if((y>=rightButtonY1) && (y<=screenYSize - 3))
    {
        MarkTouched(bottomX1, rightButtonY1, bottomX2, screenYSize-1);
        if (!notRacing) {
            MainScreenDraw();
        }
        else if (kartChosen[0] || kartChosen[1])
        {
            WaitingScreenDraw();
        }
        else
        {
            AfterRaceScreenDraw();
        }
    }
    
    if ((x>=lButtonsX1) && (x<=leftButtonX2))  // first colum buttons
    {
        if ((y>=baseline +(1*spacer)) && (y<=(baseline + boxHeight) +(1*spacer)))
        {
            //checks to see if player is queued for racing
            if (tempPlayer.number == player[0].number || tempPlayer.number == player[1].number)
            {
                MarkTouched(lButtonsX1, baseline +(1*spacer), leftButtonX2, (baseline + boxHeight) +(1*spacer));
                //checks if player is player 0
                if (tempPlayer.number == player[0].number)
                {
                    //checks to see if the race has not started yet and kicks them from queue
                    if (notRacing) {
                        kartChosen[0]=false;
                        EEPROM_readAnything(playerMemoryLocation[0], player[0]);
                        MainScreenDraw();
                    }
                    //if race has started the player quits and both racers are booted
                    else
                    {
                        TextColor();
                        ChooseSmallerFont();
                        baseline = baseline + fontAY;
                        DrawHeaderBar();
                        myGLCD.print("You Quit in the Middle", CENTER, baseline);
                        ChooseLargerFont();
                        myGLCD.print("OF RACING", CENTER, baseline +fontAY);
                        delay(2000);
                        EndRace();
                        EEPROM_readAnything(playerMemoryLocation[0], player[0]);
                        EEPROM_readAnything(playerMemoryLocation[0], player[1]);
                    }
                    
                }
                //same procedure as above checking if they are player 1
                if (tempPlayer.number == player[1].number)
                {
                    if (notRacing) {
                        kartChosen[1]=false;
                        EEPROM_readAnything(playerMemoryLocation[0], player[1]);
                        MainScreenDraw();
                    }
                    else
                    {
                        TextColor();
                        ChooseSmallerFont();
                        baseline = baseline + fontAY;
                        DrawHeaderBar();
                        myGLCD.print("You Quit in the Middle", CENTER, baseline);
                        ChooseLargerFont();
                        myGLCD.print("OF RACING", CENTER, baseline +fontAY);
                        quitRace=true;
                        delay(2000);
                        EndRace();
                        EEPROM_readAnything(playerMemoryLocation[0], player[0]);
                        EEPROM_readAnything(playerMemoryLocation[0], player[1]);
                    }
                }
            }
            //if not racing and player is not queued they can then pick a car
            else if (notRacing)
            {
                MarkTouched(lButtonsX1, baseline +(1*spacer), leftButtonX2, (baseline + boxHeight) +(1*spacer));
                RacerAorBDraw();
            }
        }
        if ((y>=baseline +(2*spacer)) && (y<=(baseline + boxHeight) +(2*spacer)))
        {
            MarkTouched(lButtonsX1, baseline +(2*spacer), leftButtonX2, (baseline + boxHeight) +(2*spacer));
            NameInputDraw();
        }
    }
    if ((x>=rightButtonX1) && (x<=rButtonsX2))  // second column buttons
    {
        if ((y>=baseline +(1*spacer)) && (y<=(baseline + boxHeight) +(1*spacer)))  // Prints last race
        {
            MarkTouched(rightButtonX1, baseline +(1*spacer), rButtonsX2, (baseline + boxHeight) +(1*spacer));
            PrintLastRace();
            if (!notRacing) {
                AfterRaceScreenDraw();
            }
            else if (kartChosen[0] || kartChosen[1])
            {
                WaitingScreenDraw();
            }
            else
            {
                MainScreenDraw();
            }
        }
//        if ((y>=baseline +(2*spacer)) && (y<=(baseline + boxHeight) +(2*spacer)))
//        {
//            MarkTouched(rightButtonX1, baseline +(2*spacer), rButtonsX2, (baseline + boxHeight) +(2*spacer));
//            if (tempPlayer.number == 1)
//            {
//                AdminDraw();
//            }
//        }
    }
}


void RacerAorBDraw()  // choose what kart the racer will be in
{
    DrawHeaderBar();
    ButtonColor();
    ChooseLargerFont();
    myGLCD.fillRoundRect(bottomX1, rightButtonY1, bottomX2, screenYSize-1);
    ButtonLabelColor();
    myGLCD.print("Cancel", CENTER, rightButtonLabelY);
    currentScreen = 4;
    ///////////////////////////setup and clear screen
    int baseline = fontBY+(screenYSize/20);
    int boxHeight = (screenYSize*14)/100;
    int lButtonsX1 = screenXSize/20;
    int spacer = (screenYSize/10) + boxHeight;
    int rButtonsX2 = screenXSize - (screenXSize/20);
    int row1YLabel = GetLabelY(baseline, baseline + boxHeight);
    
    if (!kartChosen[0]) {
        for (y=1; y<2; y++)
        {
            ButtonColor();
            myGLCD.fillRoundRect(lButtonsX1, baseline +(y*spacer), leftButtonX2, (baseline + boxHeight) +(y*spacer));
            ButtonLabelColor();
            ChooseSmallerFont();
            myGLCD.print("Kart A", GetLabelX(lButtonsX1, leftButtonX2, 6), GetLabelY(baseline +(1*spacer), (baseline + boxHeight) +(1*spacer)));
        }
    }
    
    
    if (!kartChosen[1]) {
        for (y=1; y<2; y++)
        {
            ButtonColor();
            myGLCD.fillRoundRect(rightButtonX1, baseline +(y*spacer), rButtonsX2, (baseline + boxHeight) +(y*spacer));
            ButtonLabelColor();
            ChooseSmallerFont();
            myGLCD.print("Kart B", GetLabelX(rightButtonX1, rButtonsX2, 6), GetLabelY(baseline +(1*spacer), (baseline + boxHeight) +(1*spacer)));
        }
    }
    if (!kartChosen[1] && !kartChosen[1]) {
        ButtonColor();
        myGLCD.fillRoundRect(rightButtonX1, baseline +(2*spacer), rButtonsX2, (baseline + boxHeight) +(2*spacer));
        ButtonLabelColor();
        ChooseSmallerFont();
        myGLCD.print("SoloMode", GetLabelX(rightButtonX1, rButtonsX2, 8), GetLabelY(baseline +(2*spacer), (baseline + boxHeight) +(2*spacer)));
    }
    TextColor();
    ChooseLargerFont();
    myGLCD.print("                ", CENTER, baseline);
    myGLCD.print(tempPlayer.name, CENTER, baseline);
    myGLCD.print("                ", CENTER, baseline + (1*fontAY));
    myGLCD.print("Choose", CENTER, baseline + (1*fontAY));
    
    
}

void RaceAorBTouch(){ //screen 1
    myTouch.read();    // check for touch
    x=myTouch.getX();  // x coordinate of touch
    y=myTouch.getY();  // Y coordinate of touch
    ///////////////////////////setup and clear screen
    int baseline = fontBY+(screenYSize/20);
    int boxHeight = (screenYSize*14)/100;
    int lButtonsX1 = screenXSize/20;
    int spacer = (screenYSize/10) + boxHeight;
    int rButtonsX2 = screenXSize - (screenXSize/20);
    int row1YLabel = GetLabelY(baseline, baseline + boxHeight);
    
    
    //cancel and confirm buttons
    if((y>=rightButtonY1) && (y<=screenYSize - 3))
    {
        MarkTouched(bottomX1, rightButtonY1, bottomX2, screenYSize-1);
        if (!notRacing) {
            MainScreenDraw();
        }
        else if (kartChosen[0] || kartChosen[1])
        {
            WaitingScreenDraw();
        }
        else
        {
            MainScreenDraw();
        }
        
    }
    
    if ((x>=lButtonsX1) && (x<=leftButtonX2))  // first colum buttons
    {
        if ((y>=baseline +(1*spacer)) && (y<=(baseline + boxHeight) +(1*spacer)))  // EC button
        {
            MarkTouched(lButtonsX1, baseline +(1*spacer), leftButtonX2, (baseline + boxHeight) +(1*spacer));
            soloMode = false;
            kartChosen[0] = true;
            player[0] = tempPlayer;
            if (kartChosen[0] && kartChosen[1])
            {
                AmountOfLapsDraw();
            }
            else
            {
                WaitingScreenDraw();
            }
        }
    }
    if ((x>=rightButtonX1) && (x<=rButtonsX2))  // second column buttons
    {
        if ((y>=baseline +(1*spacer)) && (y<=(baseline + boxHeight) +(1*spacer)))  // Nutes button
        {
            MarkTouched(rightButtonX1, baseline +(1*spacer), rButtonsX2, (baseline + boxHeight) +(1*spacer));
            kartChosen[1] = true;
            soloMode = false;
            player[1] = tempPlayer;
            if (kartChosen[0] && kartChosen[1])
            {
                AmountOfLapsDraw();
            }
            else
            {
                WaitingScreenDraw();
            }
        }
        if ((y>=baseline +(2*spacer)) && (y<=(baseline + boxHeight) +(2*spacer)))  // Nutes button
        {
            if (!kartChosen[1] && !kartChosen[1]) {
                MarkTouched(rightButtonX1, baseline +(2*spacer), rButtonsX2, (baseline + boxHeight) +(2*spacer));
                SoloModeAorBDraw();
            }
        }
        
    }
}


void SoloModeAorBDraw()  // settings screen also screen 13
{
    DrawHeaderBar();
    ButtonColor();
    ChooseLargerFont();
    myGLCD.fillRoundRect(bottomX1, rightButtonY1, bottomX2, screenYSize-1);
    ButtonLabelColor();
    myGLCD.print("Cancel", CENTER, rightButtonLabelY);
    currentScreen = 5;
    ///////////////////////////setup and clear screen
    int baseline = fontBY+(screenYSize/20);
    int boxHeight = (screenYSize*14)/100;
    int lButtonsX1 = screenXSize/20;
    int spacer = (screenYSize/10) + boxHeight;
    int rButtonsX2 = screenXSize - (screenXSize/20);
    int row1YLabel = GetLabelY(baseline, baseline + boxHeight);
    
    if (!kartChosen[0]) {
        for (y=1; y<2; y++)
        {
            ButtonColor();
            myGLCD.fillRoundRect(lButtonsX1, baseline +(y*spacer), leftButtonX2, (baseline + boxHeight) +(y*spacer));
            ButtonLabelColor();
            ChooseSmallerFont();
            myGLCD.print("Kart A", GetLabelX(lButtonsX1, leftButtonX2, 6), GetLabelY(baseline +(1*spacer), (baseline + boxHeight) +(1*spacer)));
        }
    }
    
    
    if (!kartChosen[1]) {
        for (y=1; y<2; y++)
        {
            ButtonColor();
            myGLCD.fillRoundRect(rightButtonX1, baseline +(y*spacer), rButtonsX2, (baseline + boxHeight) +(y*spacer));
            ButtonLabelColor();
            ChooseSmallerFont();
            myGLCD.print("Kart B", GetLabelX(rightButtonX1, rButtonsX2, 6), GetLabelY(baseline +(1*spacer), (baseline + boxHeight) +(1*spacer)));
        }
    }
    
    TextColor();
    ChooseLargerFont();
    myGLCD.print("                ", CENTER, baseline);
    myGLCD.print(tempPlayer.name, CENTER, baseline);
    myGLCD.print("                ", CENTER, baseline + (1*fontAY));
    myGLCD.print("Choose", CENTER, baseline + (1*fontAY));
    
    
}

void SoloModeAorBTouch(){ //screen 1
    myTouch.read();    // check for touch
    x=myTouch.getX();  // x coordinate of touch
    y=myTouch.getY();  // Y coordinate of touch
    ///////////////////////////setup and clear screen
    int baseline = fontBY+(screenYSize/20);
    int boxHeight = (screenYSize*14)/100;
    int lButtonsX1 = screenXSize/20;
    int spacer = (screenYSize/10) + boxHeight;
    int rButtonsX2 = screenXSize - (screenXSize/20);
    int row1YLabel = GetLabelY(baseline, baseline + boxHeight);
    
    
    //cancel and confirm buttons
    if((y>=rightButtonY1) && (y<=screenYSize - 3))
    {
        MarkTouched(bottomX1, rightButtonY1, bottomX2, screenYSize-1);
        if (!notRacing) {
            MainScreenDraw();
        }
        else if (kartChosen[0] || kartChosen[1])
        {
            WaitingScreenDraw();
        }
        else
        {
            MainScreenDraw();
        }
    }
    
    if ((x>=lButtonsX1) && (x<=leftButtonX2))  // first colum buttons
    {
        if ((y>=baseline +(1*spacer)) && (y<=(baseline + boxHeight) +(1*spacer)))  // EC button
        {
            MarkTouched(lButtonsX1, baseline +(1*spacer), leftButtonX2, (baseline + boxHeight) +(1*spacer));
            kartChosen[0] = true;
            player[0] = tempPlayer;
            soloMode =true;
            soloModeKart = 0;
            AmountOfLapsDraw();
            
        }
    }
    if ((x>=rightButtonX1) && (x<=rButtonsX2))  // second column buttons
    {
        if ((y>=baseline +(1*spacer)) && (y<=(baseline + boxHeight) +(1*spacer)))  // Nutes button
        {
            MarkTouched(rightButtonX1, baseline +(1*spacer), rButtonsX2, (baseline + boxHeight) +(1*spacer));
            kartChosen[1] = true;
            player[1] = tempPlayer;
            soloMode =true;
            soloModeKart = 1;
            AmountOfLapsDraw();
        }
        
    }
}



void PrintRacerAorBDraw()  // settings screen also screen 13
{
    DrawHeaderBar();
    ButtonColor();
    ChooseLargerFont();
    myGLCD.fillRoundRect(bottomX1, rightButtonY1, bottomX2, screenYSize-1);
    ButtonLabelColor();
    myGLCD.print("Cancel", CENTER, rightButtonLabelY);
    currentScreen = 6;
    ///////////////////////////setup and clear screen
    int baseline = fontBY+(screenYSize/20);
    int boxHeight = (screenYSize*14)/100;
    int lButtonsX1 = screenXSize/20;
    int spacer = (screenYSize/10) + boxHeight;
    int rButtonsX2 = screenXSize - (screenXSize/20);
    int row1YLabel = GetLabelY(baseline, baseline + boxHeight);
    
    ButtonColor();
    myGLCD.fillRoundRect(lButtonsX1, baseline +(1*spacer), leftButtonX2, (baseline + boxHeight) +(1*spacer));
    ButtonLabelColor();
    ChooseSmallerFont();
    myGLCD.print("Kart A", GetLabelX(lButtonsX1, leftButtonX2, 6), GetLabelY(baseline +(1*spacer), (baseline + boxHeight) +(1*spacer)));
    
    ButtonColor();
    myGLCD.fillRoundRect(rightButtonX1, baseline +(1*spacer), rButtonsX2, (baseline + boxHeight) +(1*spacer));
    ButtonLabelColor();
    ChooseSmallerFont();
    myGLCD.print("Kart B", GetLabelX(rightButtonX1, rButtonsX2, 6), GetLabelY(baseline +(1*spacer), (baseline + boxHeight) +(1*spacer)));
    
    
    TextColor();
    ChooseLargerFont();
    myGLCD.print("                ", CENTER, baseline);
    myGLCD.print(tempPlayer.name, CENTER, baseline);
    myGLCD.print("                ", CENTER, baseline + (1*fontAY));
    myGLCD.print("Choose", CENTER, baseline + (1*fontAY));
    
    
}

void PrintRacerAorBTouch(){ //screen 1
    myTouch.read();    // check for touch
    x=myTouch.getX();  // x coordinate of touch
    y=myTouch.getY();  // Y coordinate of touch
    ///////////////////////////setup and clear screen
    int baseline = fontBY+(screenYSize/20);
    int boxHeight = (screenYSize*14)/100;
    int lButtonsX1 = screenXSize/20;
    int spacer = (screenYSize/10) + boxHeight;
    int rButtonsX2 = screenXSize - (screenXSize/20);
    int row1YLabel = GetLabelY(baseline, baseline + boxHeight);
    
    
    //cancel and confirm buttons
    if((y>=rightButtonY1) && (y<=screenYSize - 3))
    {
        MarkTouched(bottomX1, rightButtonY1, bottomX2, screenYSize-1);
        MainScreenDraw();
        
    }
    
    if ((x>=lButtonsX1) && (x<=leftButtonX2))  // first colum buttons
    {
        if ((y>=baseline +(1*spacer)) && (y<=(baseline + boxHeight) +(1*spacer)))  // EC button
        {
            MarkTouched(lButtonsX1, baseline +(1*spacer), leftButtonX2, (baseline + boxHeight) +(1*spacer));
            tempPlayer = player[0];
            PrintLastRace();
            FillScreenColor();
            TextColor();
            ChooseLargerFont();
            myGLCD.print("Printing", CENTER, CENTER);
            delay(500);
            myGLCD.print("        ", CENTER, CENTER);
            delay(500);
            myGLCD.print("Printing", CENTER, CENTER);
            delay(500);
            myGLCD.print("        ", CENTER, CENTER);
            delay(500);
            myGLCD.print("Printing", CENTER, CENTER);
            delay(500);
            myGLCD.print("        ", CENTER, CENTER);
            delay(500);
            MainScreenDraw();
            
        }
    }
    if ((x>=rightButtonX1) && (x<=rButtonsX2))  // second column buttons
    {
        if ((y>=baseline +(1*spacer)) && (y<=(baseline + boxHeight) +(1*spacer)))  // Nutes button
        {
            MarkTouched(rightButtonX1, baseline +(1*spacer), rButtonsX2, (baseline + boxHeight) +(1*spacer));
            tempPlayer = player[1];
            PrintLastRace();
            MainScreenDraw();
        }
        
    }
}


#pragma mark Name Enter



void NameInputDraw()
{
    
    currentScreen = 7;
    DrawHeaderBar();
    ///////////////////////////setup and clear screen
    int baseline = fontBY+(screenYSize/20);
    int boxHeight = (screenYSize*14)/100;
    int lButtonsX1 = screenXSize/20;
    int spacer = (screenYSize/10) + boxHeight;
    int rButtonsX2 = screenXSize - (screenXSize/20);
    int row1YLabel = GetLabelY(baseline, baseline + boxHeight);
    int spaceDiv = screenXSize/68;
    int spacing = spaceDiv*2;
    int buttonWidth = spaceDiv*5;
    int blockLength = spacing + buttonWidth;
    
    
    
    
    
    int y1 = (screenYSize/3) - spacing;
    int y2 = y1 + buttonWidth;
    int y3 = y2 + spacing;
    int y4 = y3 + buttonWidth;
    int y5 = y4 + spacing;
    int y6 = y5 + buttonWidth;
    
    int BYL1 = y1 + ((buttonWidth - fontAY) / 2);
    int BYL2 = y3 + ((buttonWidth - fontAY) / 2);
    int BYL3 = y5 + ((buttonWidth - fontAY) / 2);
    int BXL1 = ((buttonWidth - fontAX) / 2);
    
    int numberLabel = ((((buttonWidth+(7*blockLength)) - (6*(spacing + buttonWidth))) - (3*fontAX))/2) + (6*(spacing + buttonWidth));
    
    //First row of buttons
    ButtonColor();
    for (x=0; x<10; x++)
    {
        
        int x1 = x*(spacing + buttonWidth);
        int x2 = (buttonWidth)+(x*blockLength);
        if (x2 >= screenXSize) {
            x2 = screenXSize - 1;
        }
        myGLCD.fillRoundRect (x1, y1, x2,y2) ;
        myGLCD.fillRoundRect (x1, y3, x2, y4);
        myGLCD.fillRoundRect (x1, y5, x2, y6);
        
    }
    myGLCD.fillRoundRect (6*(spacing + buttonWidth), y5, (buttonWidth)+(7*blockLength), y6);
    
    
    
    
    
    ButtonColor();
    myGLCD.fillRoundRect(bottomX1, rightButtonY1, bottomX2, screenYSize-1);
    //Settings and Run Mode Buttons
    ButtonLabelColor();
    myGLCD.print("Finish", CENTER, rightButtonLabelY);
    
    ButtonLabelColor();
    ChooseLargerFont();
    myGLCD.print("a", BXL1, BYL1);
    myGLCD.print("b", BXL1+(1*blockLength), BYL1);
    myGLCD.print("c", BXL1+(2*blockLength), BYL1);
    myGLCD.print("d", BXL1+(3*blockLength), BYL1);
    myGLCD.print("e", BXL1+(4*blockLength), BYL1);
    myGLCD.print("f", BXL1+(5*blockLength), BYL1);
    myGLCD.print("g", BXL1+(6*blockLength), BYL1);
    myGLCD.print("h", BXL1+(7*blockLength), BYL1);
    myGLCD.print("i", BXL1+(8*blockLength), BYL1);
    myGLCD.print("j", BXL1+(9*blockLength), BYL1);
    
    
    //Second row of buttons
    myGLCD.print("k", BXL1, BYL2);
    myGLCD.print("l", BXL1+(1*blockLength), BYL2);
    myGLCD.print("m", BXL1+(2*blockLength), BYL2);
    myGLCD.print("n", BXL1+(3*blockLength), BYL2);
    myGLCD.print("o", BXL1+(4*blockLength), BYL2);
    myGLCD.print("p", BXL1+(5*blockLength), BYL2);
    myGLCD.print("q", BXL1+(6*blockLength), BYL2);
    myGLCD.print("r", BXL1+(7*blockLength), BYL2);
    myGLCD.print("s", BXL1+(8*blockLength), BYL2);
    myGLCD.print("t", BXL1+(9*blockLength), BYL2);
    
    //Third row of buttons
    myGLCD.print("u", BXL1, BYL3);
    myGLCD.print("v", BXL1+(1*blockLength), BYL3);
    myGLCD.print("w", BXL1+(2*blockLength), BYL3);
    myGLCD.print("x", BXL1+(3*blockLength), BYL3);
    myGLCD.print("y", BXL1+(4*blockLength), BYL3);
    myGLCD.print("z", BXL1+(5*blockLength), BYL3);
    myGLCD.print("<", BXL1+(8*blockLength), BYL3);
    myGLCD.print("<", BXL1+(9*blockLength), BYL3);
    
    
    myGLCD.print(" ", numberLabel, BYL3);
    
    
    VariableColor();
    delay(10);
    ChooseSmallerFont();
    nameAddInputLen = 0;
    nameAddInput[0] = '\0';
    myGLCD.print(nameAddInput, CENTER, 52);
    
}


void NameInputTouch(){ //screen 16
    myTouch.read();    // check for touch
    x=myTouch.getX();  // x coordinate of touch
    y=myTouch.getY();  // Y coordinate of touch
    ///////////////////////////setup and clear screen
    int baseline = fontBY+(screenYSize/20);
    int boxHeight = (screenYSize*14)/100;
    int lButtonsX1 = screenXSize/20;
    int spacer = (screenYSize/10) + boxHeight;
    int rButtonsX2 = screenXSize - (screenXSize/20);
    int row1YLabel = GetLabelY(baseline, baseline + boxHeight);
    
    //cancel and confirm buttons
    if((y>=rightButtonY1) && (y<=screenYSize - 3))
    {
        MarkTouched(bottomX1, rightButtonY1, bottomX2, screenYSize-1);
        if (nameAddInputLen > 0){
            for (int i = 0; i<8; i++) {
                tempPlayer.name[i] = nameAddInput[i];
            }
            
            int memoryLocale = tempPlayer.number;
            EEPROM_writeAnything(playerMemoryLocation[memoryLocale], tempPlayer);
            if (isNewPlayer) {
                isNewPlayer = false;
                ChooseSmallerFont();
                VariableColor();
                DrawHeaderBar();
                myGLCD.print("New Player Created", CENTER, screenYSize/2);
                delayTimer = millis();
                currentScreen = 8;
                activatedPlayers++;
                EEPROM.write(playerNumMemLoc, activatedPlayers);
            }
            else
            {
                DrawHeaderBar();
                VariableColor();
                ChooseLargerFont();
                myGLCD.print("New Name Saved", CENTER, screenYSize/2);
                delayTimer = millis();
                currentScreen = 8;
            }
            
            
        }
    }
    
    
    int spaceDiv = screenXSize/68;
    int spacing = spaceDiv*2;
    int buttonWidth = spaceDiv*5;
    int blockLength = spacing + buttonWidth;
    
    
    int y1 = (screenYSize/3) - spacing;
    int y2 = y1 + buttonWidth;
    int y3 = y2 + spacing;
    int y4 = y3 + buttonWidth;
    int y5 = y4 + spacing;
    int y6 = y5 + buttonWidth;
    
    int BYL1 = y1 + ((buttonWidth - fontAY) / 2);
    int BYL2 = y3 + ((buttonWidth - fontAY) / 2);
    int BYL3 = y5 + ((buttonWidth - fontAY) / 2);
    int BXL1 = ((buttonWidth - fontAX) / 2);
    
    
    
    if ((y>=y1) && (y<=y2))  // First row
    {
        if ((x>=1) && (x<=buttonWidth))  // Button: a
        {
            MarkTouched(1, y1, buttonWidth, y2);
            NameUpdateStr('A');
        }
        if ((x>=1*(spacing + buttonWidth)) && x<= (buttonWidth)+(1*blockLength))  // Button: b
        {
            MarkTouched(1*(spacing + buttonWidth), y1, (buttonWidth)+(1*blockLength), y2);
            NameUpdateStr('B');
        }
        if ((x>=2*(spacing + buttonWidth)) && x<= (buttonWidth)+(2*blockLength))  // Button: c
        {
            MarkTouched(2*(spacing + buttonWidth), y1, (buttonWidth)+(2*blockLength), y2);
            NameUpdateStr('C');
        }
        if ((x>=3*(spacing + buttonWidth)) && x<= (buttonWidth)+(3*blockLength))  // Button: d
        {
            MarkTouched(3*(spacing + buttonWidth), y1, (buttonWidth)+(3*blockLength), y2);
            NameUpdateStr('D');
        }
        if ((x>=4*(spacing + buttonWidth)) && x<= (buttonWidth)+(4*blockLength))  // Button: e
        {
            MarkTouched(4*(spacing + buttonWidth), y1, (buttonWidth)+(4*blockLength), y2);
            NameUpdateStr('E');
        }
        if ((x>=5*(spacing + buttonWidth)) && x<= (buttonWidth)+(5*blockLength))  // Button: f
        {
            MarkTouched(5*(spacing + buttonWidth), y1, (buttonWidth)+(5*blockLength), y2);
            NameUpdateStr('F');
        }
        if ((x>=6*(spacing + buttonWidth)) && x<= (buttonWidth)+(6*blockLength))  // Button: g
        {
            MarkTouched(6*(spacing + buttonWidth), y1, (buttonWidth)+(6*blockLength), y2);
            NameUpdateStr('G');
        }
        if ((x>=7*(spacing + buttonWidth)) && x<= (buttonWidth)+(7*blockLength))  // Button: h
        {
            MarkTouched(7*(spacing + buttonWidth), y1, (buttonWidth)+(7*blockLength), y2);
            NameUpdateStr('H');
        }
        if ((x>=8*(spacing + buttonWidth)) && x<= (buttonWidth)+(8*blockLength))  // Button: i
        {
            MarkTouched(8*(spacing + buttonWidth), y1, (buttonWidth)+(8*blockLength), y2);
            NameUpdateStr('I');
        }
        if ((x>=9*(spacing + buttonWidth)) && x<= (buttonWidth)+(9*blockLength))  // Button: j
        {
            MarkTouched(9*(spacing + buttonWidth), y1, (buttonWidth)+(9*blockLength), y2);
            NameUpdateStr('J');
        }
    }
    
    if ((y>=y3) && (y<=y4))  // Second row
    {
        if ((x>=1) && (x<=buttonWidth))  // Button: a
        {
            MarkTouched(1, y3, buttonWidth, y4);
            NameUpdateStr('K');
        }
        if ((x>=1*(spacing + buttonWidth)) && x<= (buttonWidth)+(1*blockLength))  // Button: b
        {
            MarkTouched(1*(spacing + buttonWidth), y3, (buttonWidth)+(1*blockLength), y4);
            NameUpdateStr('L');
        }
        if ((x>=2*(spacing + buttonWidth)) && x<= (buttonWidth)+(2*blockLength))  // Button: c
        {
            MarkTouched(2*(spacing + buttonWidth), y3, (buttonWidth)+(2*blockLength), y4);
            NameUpdateStr('M');
        }
        if ((x>=3*(spacing + buttonWidth)) && x<= (buttonWidth)+(3*blockLength))  // Button: d
        {
            MarkTouched(3*(spacing + buttonWidth), y3, (buttonWidth)+(3*blockLength), y4);
            NameUpdateStr('N');
        }
        if ((x>=4*(spacing + buttonWidth)) && x<= (buttonWidth)+(4*blockLength))  // Button: e
        {
            MarkTouched(4*(spacing + buttonWidth), y3, (buttonWidth)+(4*blockLength), y4);
            NameUpdateStr('O');
        }
        if ((x>=5*(spacing + buttonWidth)) && x<= (buttonWidth)+(5*blockLength))  // Button: f
        {
            MarkTouched(5*(spacing + buttonWidth), y3, (buttonWidth)+(5*blockLength), y4);
            NameUpdateStr('P');
        }
        if ((x>=6*(spacing + buttonWidth)) && x<= (buttonWidth)+(6*blockLength))  // Button: g
        {
            MarkTouched(6*(spacing + buttonWidth), y3, (buttonWidth)+(6*blockLength), y4);
            NameUpdateStr('Q');
        }
        if ((x>=7*(spacing + buttonWidth)) && x<= (buttonWidth)+(7*blockLength))  // Button: h
        {
            MarkTouched(7*(spacing + buttonWidth), y3, (buttonWidth)+(7*blockLength), y4);
            NameUpdateStr('R');
        }
        if ((x>=8*(spacing + buttonWidth)) && x<= (buttonWidth)+(8*blockLength))  // Button: i
        {
            MarkTouched(8*(spacing + buttonWidth), y3, (buttonWidth)+(8*blockLength), y4);
            NameUpdateStr('S');
        }
        if ((x>=9*(spacing + buttonWidth)) && x<= (buttonWidth)+(9*blockLength))  // Button: j
        {
            MarkTouched(9*(spacing + buttonWidth), y3, (buttonWidth)+(9*blockLength), y4);
            NameUpdateStr('T');
        }
    }
    
    
    if ((y>=y5) && (y<=y6))  // Third row
    {
        if ((x>=1) && (x<=buttonWidth))  // Button: a
        {
            MarkTouched(1, y5, buttonWidth, y6);
            NameUpdateStr('U');
        }
        if ((x>=1*(spacing + buttonWidth)) && x<= (buttonWidth)+(1*blockLength))  // Button: b
        {
            MarkTouched(1*(spacing + buttonWidth), y5, (buttonWidth)+(1*blockLength), y6);
            NameUpdateStr('V');
        }
        if ((x>=2*(spacing + buttonWidth)) && x<= (buttonWidth)+(2*blockLength))  // Button: c
        {
            MarkTouched(2*(spacing + buttonWidth), y5, (buttonWidth)+(2*blockLength), y6);
            NameUpdateStr('W');
        }
        if ((x>=3*(spacing + buttonWidth)) && x<= (buttonWidth)+(3*blockLength))  // Button: d
        {
            MarkTouched(3*(spacing + buttonWidth), y5, (buttonWidth)+(3*blockLength), y6);
            NameUpdateStr('X');
        }
        if ((x>=4*(spacing + buttonWidth)) && x<= (buttonWidth)+(4*blockLength))  // Button: e
        {
            MarkTouched(4*(spacing + buttonWidth), y5, (buttonWidth)+(4*blockLength), y6);
            NameUpdateStr('Y');
        }
        if ((x>=5*(spacing + buttonWidth)) && x<= (buttonWidth)+(5*blockLength))  // Button: f
        {
            MarkTouched(5*(spacing + buttonWidth), y5, (buttonWidth)+(5*blockLength), y6);
            NameUpdateStr('Z');
        }
        if ((x>=6*(spacing + buttonWidth)) && x<= (buttonWidth)+(7*blockLength))  // Button: g
        {
            MarkTouched(6*(spacing + buttonWidth), y5, (buttonWidth)+(7*blockLength), y6);
            NameUpdateStr(' ');
        }
        if ((x>=8*(spacing + buttonWidth)) && x<= (buttonWidth)+(8*blockLength))  // Button: i
        {
            MarkTouched(8*(spacing + buttonWidth), y5, (buttonWidth)+(8*blockLength), y6);
            NameUpdateDelStr();
        }
        if ((x>=9*(spacing + buttonWidth)) && x<= (buttonWidth)+(9*blockLength))  // Button: j
        {
            MarkTouched(9*(spacing + buttonWidth), y5, (buttonWidth)+(9*blockLength), y6);
            NameUpdateDelStr();
        }
    }
    
}

// adds a character in the Name string
void NameUpdateStr(int val)
{
    
    if (nameAddInputLen<8)
    {
        nameAddInput[nameAddInputLen]=val;
        nameAddInput[nameAddInputLen+1]='\0';
        nameAddInputLen++;
        ChooseSmallerFont();
        VariableColor();
        myGLCD.print(nameAddInput, CENTER, 52);
    }
    
}




// deletes the last character in the string for Name
void NameUpdateDelStr()
{
    VariableColor();
    myGLCD.print("         ", CENTER, 52);
    if (nameAddInputLen>0)
    {
        nameAddInput[nameAddInputLen-1]='\0';
        nameAddInputLen = nameAddInputLen - 1;
        ChooseSmallerFont();
        myGLCD.print(nameAddInput, CENTER, 52);
        
    }
    
}





#pragma mark ThemeColor And Options


//puts red bounding boxes on touched buttons to give user feedback
void MarkTouched (int x1, int y1, int x2, int y2)
{
    delay(20);
    ButtonPressedColor();
    for (x=0; x<4; x++)
    {
        myGLCD.drawRoundRect (x1 + x, y1 + x, x2 - x, y2 - x);
        delay(10);
    }
    
    while (myTouch.dataAvailable())
        myTouch.read();
    ButtonColor();
    for (x=0; x<4; x++)
    {
        myGLCD.drawRoundRect (x1 + x, y1 + x, x2 - x, y2 - x);
        delay(10);
    }
    
}



void ButtonColor()
{
    myGLCD.setColor(255, 255, 255);
}
void ButtonLabelColor()
{
    myGLCD.setBackColor(255, 255, 255);
    myGLCD.setColor(0, 0, 0);
}

void ButtonPressedColor()
{
    myGLCD.setColor(131, 131, 131);
}


void TextColor()
{
    myGLCD.setBackColor(0, 0, 0);
    myGLCD.setColor(255, 255, 255);
}
void VariableColor()
{
    myGLCD.setBackColor(0, 0, 0);
    myGLCD.setColor(255, 255, 255);
}

void BackgroundColor()
{
    myGLCD.setColor(0, 0, 0);
}
void FillScreenColor()
{
    myGLCD.fillScr(0, 0, 0);
}


#pragma mark FontsForTheme

void ChooseLargerFont()
{
    switch (lcdSize) {
        case 1:
            myGLCD.setFont(SmallFont);
            break;
        case 2:
            myGLCD.setFont(BigFont);
            break;
        default:
            break;
    }
}

void ChooseSmallerFont()
{
    switch (lcdSize) {
        case 1:
            myGLCD.setFont(SmallFont);
            break;
        case 2:
            myGLCD.setFont(SmallFont);
            break;
        default:
            break;
    }
}

#pragma mark LabelAlignment


int GetLabelX (int buttonX1, int buttonX2, int lengthWord)
{
    int labelX = ((buttonX2 - buttonX1 - (lengthWord*fontBX))/2) + buttonX1;
    return labelX;
}
int GetLabelY (int buttonY1, int buttonY2)
{
    int labelY = ((buttonY2 - buttonY1 - fontBY)/2) + buttonY1;
    return labelY;
}

int GetLabelXLargerFont (int buttonX1, int buttonX2, int lengthWord)
{
    int labelX = ((buttonX2 - buttonX1 - (lengthWord*fontAX))/2) + buttonX1;
    return labelX;
}
int GetLabelYLargerFont (int buttonY1, int buttonY2)
{
    int labelY = ((buttonY2 - buttonY1 - fontAY)/2) + buttonY1;
    return labelY;
}

#pragma mark Communication


void OutToTVScores()
{
    
    
    outToTV.bestLap[0] = racer[0].bestLap;
    outToTV.bestLap[1] = racer[1].bestLap;
    if (racer[0].lapCount > 1) {
        outToTV.lastLap[0] = racer[0].lastLap;
    }
    else{
        outToTV.lastLap[0] = 0;
    }
    if (racer[1].lapCount > 1) {
        outToTV.lastLap[1] = racer[1].lastLap;
    }
    else{
        outToTV.lastLap[1] = 0;
    }
    outToTV.totalTime[0] = racer[0].totalTime;
    outToTV.totalTime[1] = racer[1].totalTime;
    outToTV.lapCount[0] = racer[0].lapCount;
    outToTV.lapCount[1] = racer[1].lapCount;
    if (racer[0].lapsInRace) {
        outToTV.lapsInRace = racer[0].lapsInRace;
    }
    else
    {
        outToTV.lapsInRace = racer[1].lapsInRace;
    }
    
    outToTV.bestLap[2] = prevPlayer[0].lastBestLap;
    outToTV.bestLap[3] = prevPlayer[1].lastBestLap;
    outToTV.lastLap[2] = prevPlayer[0].lapTimes[prevPlayer[0].lastRaceLaps];
    outToTV.lastLap[3] = prevPlayer[1].lapTimes[prevPlayer[1].lastRaceLaps];
    outToTV.totalTime[2] = prevPlayer[0].lastTotalTime;
    outToTV.totalTime[3] = prevPlayer[1].lastTotalTime;
    
    for (int xx = 0; xx<10; xx++) {
        outToTV.nameA[xx] = player[0].name[xx];
        outToTV.nameB[xx] = player[1].name[xx];
        outToTV.nameC[xx] = prevPlayer[0].name[xx];
        outToTV.nameD[xx] = prevPlayer[1].name[xx];
    }
    
    ET2.sendData();
    
    
    
    
    
}



void RedYellowGreenChange(byte lightOn)
{
    if (firstRun) {
        return;
    }
    currentLightOn = lightOn;
    digitalWrite(redYellowGreenPins[lightOn], LOW);
    for (byte x1 = 0; x1<3; x1++) {
        if (x1 != lightOn) {
            digitalWrite(redYellowGreenPins[x1], HIGH);
        }
    }
    
    
}


void receiveLapTime()
{
    if(ET.receiveData()){
        //this is how you access the variables. [name of the group].[variable name]
        //since we have data, we will blink it out.
        lapIn.kart = myLap.kartID;
        lapIn.lapID = myLap.lapID;
        lapIn.lapTime = myLap.lapTime;
        Serial.println("incoming");
        Serial.println("time");
        Serial.println(lapIn.lapTime);
        Serial.println("ID");
        Serial.println(lapIn.lapID);
        Serial.println("Kart");
        Serial.println(lapIn.kart);
        UpdateLapsTimes();
    }
    
}





#pragma mark StartEndRace

void startRace()
{
    notRacing = false;
    outToTV.raceOver = false;
    RedYellowGreenChange(2);
    raceOn = true;
    racer[0].lapsInRace = lapsInCurrentRace;
    racer[1].lapsInRace = lapsInCurrentRace;
    if (!firstRun) {
        MainScreenDraw();
    }
    OutToTVScores();
    
    
}


void EndRace()
{
    RedYellowGreenChange(0);
    
    if (!quitRace || firstRun) {
        if (soloModeKart == 0 || !soloMode || firstRun) {
            CompareLastRacePlayerMemory(0);
            TopTenTimes(0,0);
            TopTenTimes(0,1);
        }
        
        if (soloModeKart == 1 || !soloMode || firstRun)
        {
            CompareLastRacePlayerMemory(1);
            TopTenTimes(1,2);
            TopTenTimes(1,3);
        }
    }
    else
    {
        quitRace = false;
    }
    
    raceOn = false;
    kartChosen[0] = false;
    kartChosen[1] = false;
    notRacing=true;
    prevPlayer[0] = player[0];
    prevPlayer[1] = player[1];
    EEPROM_readAnything(playerMemoryLocation[0], player[0]);
    EEPROM_readAnything(playerMemoryLocation[0], player[1]);
    for (int i = 0; i < 2; i++) {
        racer[i].lapCount = 0;
        racer[i].totalTime = 0;
        racer[i].lastLap = 0;
        racer[i].bestLap = 0;
        racer[i].minute = currentTimeDate[4];
        racer[i].hour = currentTimeDate[3];
        racer[i].isFinished = false;
        for(int i2 = 0;i2<20;i2++)
        {
            racer[i].lapTimes[i2] = 0;
            racer[i].lapTimes[i2] = 0;
        }
        racer[i].lapsInRace = 0;
    }
    if (!firstRun) {
        AfterRaceScreenDraw();
    }
    outToTV.raceOver = true;
    OutToTVScores();
    
}

#pragma mark UpdateStats

void UpdateLapsTimes()
{
    byte kartIn = lapIn.kart;
    if(lapIn.lapID != lastLap[kartIn].lapID)
    {
        
        if (soloMode) {
            if (kartIn != soloModeKart) {
                return;
            }
        }
        if (notRacing) {
            lastLap[kartIn].lapID = lapIn.lapID;
        }
        else
        {
            if (racer[kartIn].lapCount <= racer[kartIn].lapsInRace)
            {
                racer[kartIn].lastLap = lapIn.lapTime;
                lastLap[kartIn] = lapIn;
                racer[kartIn].lapTimes[racer[kartIn].lapCount] = lastLap[kartIn].lapTime;
                
                if (racer[kartIn].lapCount > 0) {
                    racer[kartIn].totalTime = racer[kartIn].totalTime + lastLap[kartIn].lapTime;
                }
                
                if (racer[kartIn].lapCount == 1)
                {
                    racer[kartIn].bestLap = lastLap[kartIn].lapTime;
                }
                else if (racer[kartIn].lapCount > 1)
                {
                    if (racer[kartIn].bestLap > lastLap[kartIn].lapTime)
                    {
                        racer[kartIn].bestLap = lastLap[kartIn].lapTime;
                    }
                }
                racer[kartIn].lapCount++;
                if (currentScreen == 0 && !firstRun) {
                    MainScreenDraw();
                }
                
                if (racer[0].lapCount >= lapsInCurrentRace && racer[1].lapCount >= lapsInCurrentRace ) {
                    RedYellowGreenChange(1);
                }
                if (soloMode || noPassing) {
                    if (racer[kartIn].lapCount == lapsInCurrentRace ) {
                        RedYellowGreenChange(1);
                    }
                }
                //                if (noPassing) {
                //                    if (racer[kartIn].lapCount >= lapsInCurrentRace ) {
                //                        RedYellowGreenChange(0);
                //                    }
                //                }
            }
            else
            {
                lastLap[kartIn].lapID = lapIn.lapID;
            }
            
            
        }
        
        if (!notRacing) {
            if (racer[kartIn].lapCount > racer[kartIn].lapsInRace)
            {
                racer[kartIn].isFinished = true;
                if (soloMode)
                {
                    notRacing=true;
                    EndRace();
                    
                }
                if (racer[0].isFinished) {
                    if (racer[1].isFinished) {
                        notRacing = true;
                        EndRace();
                    }
                }
            }
            
        }
        OutToTVScores();
    }
    
}



void CompareLastRacePlayerMemory(int whichPlayer)
{
    if (racer[whichPlayer].bestLap < player[whichPlayer].bestLap[whichPlayer])
    {
        player[whichPlayer].bestLap[whichPlayer] = racer[whichPlayer].bestLap;
    }
    player[whichPlayer].kartUsed[whichPlayer]++;
    player[whichPlayer].kartUsedLast = whichPlayer;
    player[whichPlayer].lastRaceMinute = racer[whichPlayer].minute;
    player[whichPlayer].lastRaceHour = racer[whichPlayer].hour;
    player[whichPlayer].lastRaceLaps = racer[whichPlayer].lapsInRace;
    player[whichPlayer].lastBestLap = racer[whichPlayer].bestLap;
    player[whichPlayer].lastTotalTime = racer[whichPlayer].totalTime;
    player[whichPlayer].lastRaceTrack = currentRaceTrack;
    for (int i = 0; i<20; i++) {
        player[whichPlayer].lapTimes[i] = racer[whichPlayer].lapTimes[i];
    }
    player[whichPlayer].lastRaceMonth = currentTimeDate[0];
    player[whichPlayer].lastRaceDay = currentTimeDate[1];
    int playMem = player[whichPlayer].number;
    EEPROM_writeAnything(playerMemoryLocation[playMem], player[whichPlayer]);
}


void PlayerDeleteChangeTopTen(int oldNumber, int newNumber)
{
    
}



void TopTenTimes(int whichKart, int whichList)
{
    boolean refactorTop = false;
    boolean onBoard;
    int locationOfOldScore;
    boolean betterThanLast;
    int i,j,tempPlayNumb,temp;
    
    for  (i = 0; i<10; i++) {
        if (player[whichKart].number == kartTopTen[whichList].player[i]) {
            onBoard = true;
            locationOfOldScore = i;
        }
    }
    if (onBoard)
    {
        if (racer[whichKart].bestLap < kartTopTen[whichList].lapTime[locationOfOldScore]) {
            kartTopTen[whichList].lapTime[locationOfOldScore] = racer[whichKart].bestLap;
            refactorTop = true;
            
        }
    }
    else
        if(racer[whichKart].bestLap < kartTopTen[whichList].lapTime[9])
        {
            kartTopTen[whichList].lapTime[9] = racer[whichKart].bestLap;
            kartTopTen[whichList].player[9] = player[whichKart].number;
            refactorTop = true;
        }
    if (refactorTop) {
        for(i=0;i<10;i++)
        {
            for(j=i;j<10;j++)
            {
                if(kartTopTen[whichList].lapTime[i] > kartTopTen[whichList].lapTime[j])
                {
                    temp = kartTopTen[whichList].lapTime[i];
                    tempPlayNumb = kartTopTen[whichList].player[i];
                    kartTopTen[whichList].lapTime[i]=kartTopTen[whichList].lapTime[j];
                    kartTopTen[whichList].player[i]=kartTopTen[whichList].player[j];
                    kartTopTen[whichList].lapTime[j]=temp;
                    kartTopTen[whichList].player[j]=tempPlayNumb;
                }
            }
        }
    }
}


#pragma mark Printing
void PrintLastRace()
{   ////date formatting
    DrawHeaderBar();
    ChooseLargerFont();
    VariableColor();
    myGLCD.print("Printing", CENTER, screenYSize/2);
    char dateString[11];
    char interim[4];
    char interim2[4];
    itoa(tempPlayer.lastRaceMonth, interim, 10);
    itoa(tempPlayer.lastRaceDay, interim2, 10);
    if (interim[1])
    {
        dateString[0]= interim[0];
        dateString[1]= interim[1];
    }
    else
    {
        dateString[0]= '0';
        dateString[1] = interim[0];
    }
    dateString[2] = '/';
    if (interim2[1])
    {
        dateString[3]= interim2[0];
        dateString[4]= interim2[1];
    }
    else
    {
        dateString[3]= '0';
        dateString[4] = interim2[0];
    }
    dateString[5] = '/';
    dateString[6] = '2';
    dateString[7] = '0';
    dateString[8] = '1';
    dateString[9] = '4';
    dateString[10] = '\0';
    
    //////TIME
    char timeString[6];
    char interim3[4];
    char interim4[4];
    itoa(tempPlayer.lastRaceHour, interim3, 10);
    itoa(tempPlayer.lastRaceMinute, interim4, 10);
    if (interim3[1])
    {
        timeString[0]= interim3[0];
        timeString[1]= interim3[1];
    }
    else
    {
        timeString[0]= '0';
        timeString[1] = interim3[0];
    }
    timeString[2] = ':';
    if (interim4[1])
    {
        timeString[3]= interim4[0];
        timeString[4]= interim4[1];
    }
    else
    {
        timeString[3]= '0';
        timeString[4] = interim4[0];
    }
    timeString[5] = '\0';
    printer.printBitmap(logo_width, logo_height, logo_data);
    myGLCD.print("        ", CENTER, screenYSize/2);
    printer.justify('C');
    printer.setSize('S');
    printer.println(subLine);
    printer.feed(1);
    printer.justify('R');
    printer.setSize('S');
    printer.println(dateString);
    printer.println(timeString);
    printer.feed(1);
    printer.justify('C');
    printer.setSize('L');
    printer.println(tempPlayer.name);
    printer.setSize('S');
    if (tempPlayer.kartUsedLast == 0) {
        printer.println(kartAPrintName);
    }
    else
    {
        printer.println(kartBPrintName);
    }
    if (tempPlayer.lastRaceTrack == 0) {
        printer.println("Full Track");
    }
    else if (tempPlayer.lastRaceTrack == 1)
    {
        printer.println("2/3 Track");
    }
    else
    {
        printer.println("Field Track");
    }
    printer.justify('L');
    myGLCD.print("        ", CENTER, screenYSize/2);
    for (int i=tempPlayer.lastRaceLaps; i>0; i--)
    {
        char tempString[4];
        itoa(i, tempString, 10);
        printer.print("Lap:");
        printer.print(tempString[0]);
        printer.print("   ");
        convertTimeString(tempPlayer.lapTimes[i]);
        printer.println(tempLapTimeString);
    }
    printer.feed(1);
    printer.justify('C');
    printer.println("Total Time");
    convertTimeString(tempPlayer.lastTotalTime);
    printer.println(tempLapTimeString);
    printer.feed(1);
    printer.println("Best Lap");
    convertTimeString(tempPlayer.lastBestLap);
    printer.println(tempLapTimeString);
    printer.feed(1);
    printer.println("Best Lap Ever");
    convertTimeString(tempPlayer.bestLap[tempPlayer.kartUsedLast]);
    printer.println(tempLapTimeString);
    printer.feed(1);
    printer.println(urlLine);
    printer.feed(5);
    
    
}

#pragma mark TimeDate

void GetCurrentTime()
{
    Time t = rtc.getTime();
    currentTimeDate[0] = t.mon;
    currentTimeDate[1] = t.date;
    currentTimeDate[2] = t.year;
    currentTimeDate[3] = t.hour;
    currentTimeDate[4] = t.min;
    
}


#pragma mark Conversions



void restartButton()
{
    player[0] = prevPlayer[0];
    player[1] = prevPlayer[1];
    kartChosen[0] = true;
    kartChosen[1] = true;
    startRace();
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


void CheckIDCard()
{
    byte whichPlayer;
    boolean foundPlayer = false;
    byte uidT[4];
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
        return;
    }
    
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
        return;
    }
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        uidT[i] = mfrc522.uid.uidByte[i];
    }
    if (uidT[3])
    {
        Serial.println(uidT[0]);
        Serial.println(uidT[1]);
        Serial.println(uidT[2]);
        Serial.println(uidT[3]);
        cardLastRead = millis();
        for (int i = 0; i<20; i++) {
            EEPROM_readAnything(playerMemoryLocation[i], tempPlayer);
            if (uidT[0] == tempPlayer.cardID[0] && uidT[1] == tempPlayer.cardID[1] && uidT[2] == tempPlayer.cardID[2]  && uidT[3] == tempPlayer.cardID[3])
            {
                
                whichPlayer = i;
                foundPlayer = true;
            }
        }
        if (foundPlayer) {
            
            if(firstRun)
            {
                EndRace();
                firstRun = false;
            }
            
            delay(20);
            EEPROM_readAnything(playerMemoryLocation[whichPlayer], tempPlayer);
            PrintOrRaceDraw();
            rfidReady = false;
        }
        else
        {
            for (int i = 0; i<4; i++) {
                tempPlayer.cardID[i] = uidT[i];
                tempPlayer.kartUsed[i] = 0;
            }
            for (int i = 0; i<20; i++) {
                tempPlayer.lapTimes[i] = 5*60000;
            }
            tempPlayer.lastRaceLaps = 5;
            tempPlayer.lastBestLap =5*60000;
            tempPlayer.lastTotalTime =40*60000;
            tempPlayer.lastRaceMonth =1;
            tempPlayer.lastRaceDay =1;
            tempPlayer.lastRaceHour = 1;
            tempPlayer.lastRaceMinute = 1;
            tempPlayer.lastRaceTrack = 0;
            tempPlayer.kartUsedLast = 0;
            tempPlayer.nameSize = 0;
            tempPlayer.number = activatedPlayers;
            isNewPlayer = true;
            tempPlayer.bestLap[0] = 5*60000;
            tempPlayer.bestLap[1] = 5*60000;
            CreateNewPlayerDraw();
            
        }
        for (byte i = 0; i < 4; i++) {
            uidT[i] = 0;
        }
        
        
    }
}


void CreateNewPlayerDraw()
{
    FillScreenColor();
    TextColor();
    ChooseSmallerFont();
    myGLCD.print("No Player Found", CENTER, screenYSize/2);
    myGLCD.print("Create New Player?", CENTER, (screenYSize/2) + fontAY);
    int baseline = fontBY+(screenYSize/20)+ (fontBY/3);
    currentScreen = 9;
    BackgroundColor();
    delay(10);
    ChooseLargerFont();
    ButtonColor();
    myGLCD.fillRoundRect(0, rightButtonY1, leftButtonX2, screenYSize-1);
    myGLCD.fillRoundRect(rightButtonX1, rightButtonY1, screenXSize-1, screenYSize-1);
    int rightLetterCount = 3;
    int leftLetterCount = 2;
    rightButtonLabelX = (((screenXSize - rightButtonX1) - (fontAX * rightLetterCount))/2) + rightButtonX1;
    leftButtonLabelX = (leftButtonX2 - (fontAX * leftLetterCount))/2;
    ButtonLabelColor();
    myGLCD.print("No", leftButtonLabelX, rightButtonLabelY);
    myGLCD.print("Yes", rightButtonLabelX, rightButtonLabelY);
}


void CreateNewPlayerTouch() //touch control and variable updating for mainScreen Screen 0
{
    myTouch.read();    // check for touch
    x=myTouch.getX();  // x coordinate of touch
    y=myTouch.getY();  // Y coordinate of touch
    //middle colum y coordinates
    int middleTopLabel = (screenYSize/4) - ((fontBY +fontAY)/2);
    int middleBottomLabel = (screenYSize - (screenYSize/4)) - ((fontBY +fontAY)/2);
    BackgroundColor();
    if((y>=rightButtonY1) && (y<=screenYSize - 3))
    {
        if ((x>=3) && (x<=leftButtonX2)){     //Settings Modes Page
            MarkTouched(0, rightButtonY1, leftButtonX2,  screenYSize);
            MainScreenDraw();
            isNewPlayer = false;
            
        }
        if ((x>=rightButtonX1) && (x<=screenXSize - 3)){  //Run page button
            MarkTouched(rightButtonX1, rightButtonY1, screenXSize-1,  screenYSize-1);
            NameInputDraw();
            
        }
    }
    
    
    
}


//struct queuing
//{
//    char name [15];
//    int playerNumber;
//    byte nameSize;
//};
//typedef struct queuing Queue;
//Queue queue[10]; //current players that have been loaded from memory
//byte queueLength = 0;
//
//void QueueingPlayers()
//{
//    for (int xx = 1 ; xx<11; xx++) {
//        if (tempPlayer.number == queue[xx].playerNumber) {
//            //already in queue screen
//            return;
//        }
//    }
//    if (queueLength < 11) {
//        queue[queueLength].playerNumber = tempPlayer.number;
//        queue[queueLength].nameSize = tempPlayer.nameSize;
//        for (int xx = 0; xx<15; xx++) {
//            queue[queueLength].name[xx] = tempPlayer.name[xx];
//            
//        }
//        queueLength++;
//    }
//    else
//        //Queue is full screen
//        return;
//}


void PauseScreenWithoutDelay()
{
    unsigned long currentTime = millis();
    if (currentTime - delayTimer > 2000)
    {
        MainScreenDraw();
    }
    else
    {
    }
}



//void nextInQueue()
//{
//    int randNumber;
//    if (queueLength) {
//        if (kartChosen[0] || kartChosen[1]) { //checking if a kart has been chosen
//            if (kartChosen[0]) {
//                randNumber = 1;
//            }
//            else
//            {
//                randNumber = 0;
//            }
//        }
//        else
//        {
//            randomSeed(analogRead(3));
//            randNumber = random(1);
//        }
//        player[randNumber].number = queue[1].playerNumber;
//        
//        if (queueLength > 1)
//        {
//            for (int xx = 2; xx<=queueLength; xx++)
//            {
//                queue[xx-1].playerNumber = queue[xx].playerNumber;
//            }
//        }
//        queueLength--;
//        
//    }
//}
//
//void DeleteFromQueue()
//{
//    int numbToDelete;
//    for (int xx = 1; xx<=queueLength; xx++) {
//        if (queue[xx].playerNumber == tempPlayer.number) {
//            numbToDelete = xx;
//        }
//    }
//    for (int xx = numbToDelete; xx<queueLength; xx++)
//    {
//        queue[xx].playerNumber = queue[xx+1].playerNumber;
//    }
//    queueLength--;
//}

void receive(int numBytes) {
}

void setup()
{
    if (lcdSize == 1) {
        fontAY = 16; //largerFont
        fontAX = 16; //largerFont
        
    }
    if (lcdSize == 3){
        fontAY = 32; //largerFont
        fontAX = 24; //largerFont
        fontBY = 16; //smallerFont
        fontBX = 16; //smallerFont
    }
    Serial3.begin(9600);
    Serial2.begin(9600);
    //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc.
    ET.begin(details(myLap), &Serial3);
    ET2.begin(details(outToTV), &Serial2);
    //define handler function on receiving data
    Serial.begin(9600);
    delay(1000);
    printOnBlack = FALSE;
    printUpSideDown = FALSE;
    myGLCD.InitLCD();
    myGLCD.clrScr();
    
    
    myTouch.InitTouch();
    myTouch.setPrecision(PREC_MEDIUM);
    screenSize();
    //Modify the print speed and heat
    rtc.halt(false);
    firstTimeSetup = EEPROM.read(firstTimeSetupLocation);
    SPI.begin();			// Init SPI bus
    mfrc522.PCD_Init();	// Init MFRC522 card
    DrawHeaderBar();
    myGLCD.print("Loading", CENTER, (screenYSize/2));
    for (int x1 = 0; x1<3; x1++) {
        pinMode(redYellowGreenPins[x1], OUTPUT);
        digitalWrite(redYellowGreenPins[x1], LOW);
        switch (x1) {
            case 0:
                myGLCD.print("Loading.", CENTER, (screenYSize/2));
                break;
            case 1:
                myGLCD.print("Loading..", CENTER, (screenYSize/2));
                break;
            case 2:
                myGLCD.print("Loading...", CENTER, (screenYSize/2));
                break;
                
            default:
                break;
        }
        
        delay(1000);
        if (x1 ==2) {
            digitalWrite(redYellowGreenPins[0], HIGH);
            digitalWrite(redYellowGreenPins[1], HIGH);
            digitalWrite(redYellowGreenPins[2], HIGH);
        }
        
    }
    
    if (forceTimeSet) {
        SetTime();
        GetCurrentTime();
        ChooseLargerFont();
        VariableColor();
        myGLCD.print("Time Has Been", CENTER, (screenYSize/2) -(3*fontAY));
        myGLCD.print("SET", CENTER, (screenYSize/2) -(2*fontAY));
        myGLCD.printNumI(currentTimeDate[3], (screenXSize/2) -(2*fontAX), (screenYSize/2) -(1*fontAY));
        myGLCD.print(":", (screenXSize/2), (screenYSize/2) -(1*fontAY));
        myGLCD.printNumI(currentTimeDate[4], (screenXSize/2) +(1*fontAX), (screenYSize/2) -(1*fontAY));
        myGLCD.printNumI(currentTimeDate[0], (screenXSize/2) -(2*fontAX), (screenYSize/2));
        myGLCD.print("/", (screenXSize/2), (screenYSize/2));
        myGLCD.printNumI(currentTimeDate[1], (screenXSize/2) +(1*fontAX), (screenYSize/2));
        myGLCD.print("Reload Software", CENTER, (screenYSize/2)+(1*fontAY));
        while (forceTimeSet) {
            
            delay(10000);
        }
    }
    if (firstTimeSetup != 69 || forceFirst) {
        VariableColor();
        ChooseLargerFont();
        if (forceFirst) {
            myGLCD.print("Erasing Unit", CENTER, (screenYSize/2) -(3*fontAY));
            myGLCD.print("Please Wait", CENTER, (screenYSize/2) - -(1*fontAY));
        }
        else
        {
            SetTime();
            myGLCD.print("Setting Up Unit", CENTER, (screenYSize/2) -(3*fontAY));
            myGLCD.print("Please Wait", CENTER, (screenYSize/2) +(1*fontAY));
        }
        for (int i = 0; i < 4096; i++)
        {
            EEPROM.write(i, 0);
        }
        Setupplayer0();
        
        EEPROM_writeAnything(playerMemoryLocation[0], player[0]);
        activatedPlayers = 1;
        EEPROM.write(playerNumMemLoc, activatedPlayers);
        firstTimeSetup = 69;
        EEPROM.write(firstTimeSetupLocation, firstTimeSetup);
        if (forceFirst) {
            
            myGLCD.print("Erasing Unit", CENTER, (screenYSize/2) -(3*fontAY));
            myGLCD.print("IS Finished", CENTER, (screenYSize/2) - (2*fontAY));
            myGLCD.print("Reload Software", CENTER, (screenYSize/2));
            myGLCD.print("             ", CENTER, (screenYSize/2) +(1*fontAY));
            while (forceFirst) {
                delay(10000);
            }
            
        }
    }
    activatedPlayers = EEPROM.read(playerNumMemLoc);
    EEPROM_readAnything(playerMemoryLocation[0], player[0]);
    EEPROM_readAnything(playerMemoryLocation[0], player[1]);
    tempPlayer = player[0];
    firstRun = false;
    DrawHeaderBar();
    ChooseLargerFont();
    VariableColor();
    firstRun = true;
    printf("OK\n\r");
    GetCurrentTime();
    startRace();
    Serial.println("endOfStart");
    DrawSwipeScreen();
    OutToTVScores();
    
    
}
void loop()
{
    
    unsigned long currentTime = millis();
    GetCurrentTime();
    
    switch (currentScreen)
    {
        case 0:
            AfterRaceScreenTouch();
            break;
        case 1:
            StartScreenTouch();
            break;
        case 2:
            ShowRacersTouch();
            break;
        case 3:
            PrintOrRaceTouch();
            break;
        case 4:
            RaceAorBTouch();
            break;
        case 5:
            SoloModeAorBTouch();
            break;
        case 6:
            PrintRacerAorBTouch();
            break;
        case 7:
            NameInputTouch();
            break;
        case 8:
            PauseScreenWithoutDelay();
            break;
        case 9:
            CreateNewPlayerTouch();
            break;
        case 10:
            AmountOfLapsTouch();
            break;
        case 11:
            DeleteRacerTouch();
            break;
            
    }
    CheckIDCard();
    receiveLapTime();
    
}






