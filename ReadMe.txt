  ___              _      ___         _           
 | _ \__ _ _ _  __| |_   | _ \__ _ __(_)_ _  __ _ 
 |   / _` | ' \/ _| ' \  |   / _` / _| | ' \/ _` |
 |_|_\__,_|_||_\__|_||_| |_|_\__,_\__|_|_||_\__, |
 | |   __ _ _ __  |_   _(_)_ __  ___ _ _    |___/ 
 | |__/ _` | '_ \   | | | | '  \/ -_) '_|         
 |____\__,_| .__/   |_| |_|_|_|_\___|_|           
           |_| 
/// @author		Nicholas Germon
/// @author		Nicholas Germon
/// @date		12/6/14
/// @version	.1
/// 
/// @copyright	(c) Nicholas Germon, 2014
/// @copyright	CC = BY SA NC


Version History

.1  Initial Release Working Supports 26 racers, 2 vehicles, Race Ticket Printing,
	TV Out, 20 lap maximum, RFID Identification


Included Files

Fritzing Files - Schematics and PCB Layouts
xCode -  All embedXcode projects. Can also be opened using arduino IDE
libraries - All libraries needed for compiling. The thermal library has been modified from
            The original thermal library make sure you use the one included


How it works

On each vehicle is a IR beacon that transmits a unique irCode. 
Currently it supports two vehicles
When the vehicle drives by the receiver, the receiver is triggered and the vehicles lap
time is calculated. The receiver sends the Main Unit the lap time, a unique id for the lap
, and the vehicle that did the lap. This communication is done through the serial port.
The Main unit then acts as the brain of the race.
Each racer is identified by a rfid card. Currently it supports 20 racers.
To start a race the racer needs to swipe his id card.
If the card is not recognized the user will then be able to add the card to the timer, by
entering their race name and confirming. If the id card is recognized the 
racers name will apear and he will be asked if he would like to race, edit his name,
or print the last race.
The machine will only print THE LAST RACE not the racer's last race.
If the racer chooses race he can then choose a kart or solo mode.
Race starts after both or solo racer(s) have chosen vehicle and amount of laps.
When the racer passes the receiver his first lap starts.
The stoplight will turn yellow when the racer(s) are on their last lap.
It will turn red when they are finished.
A new race can begin with new racers, and the finished racers can print their lap times.
Printing receipts is allowed even after the next race has started.


Notes on construction of each unit

All fritzing files are included
All Circuit boards are single sided so they can be printed at home

Main Board
Must be powered by a 7-9v   at least 2amp supply to power the printer and board
This board will have a tft shield stacked on it so make sure to use stackable headers
They are on most arduino shields so they can be stacked. This board will essentially be
a shield

TVOut
Housed in the main units case is the 
Main Board and the tvout board although tv can be separated into its own case.
Powered by the main units power supply.
Although Sound is wired on the board, There are no sounds in the program the audio cable
out is therefore optional

IR Receiver
Make sure the irReceiver is housed 1 inch behind the case so it is not exposed to direct
Sunilght 
This Unit can be installed inside the same housing as the main board if you like.
Or it can be in its own housing attached using a length of cable. This allows the start 
line to be away from the printer and main unit. This unit is powered off of the main unit.
The Reason for a separate arduino in the receiver is to make sure no reading is delayed
due to the actions of the main brain, especially printing a receipt. 

IR Beacons 
Power source can be from 7-14v
12v lead acid (car) batteries work great
(can easily convert to an attiny for rc cars)

The Stoplight 
The outputs of the main board RYGVCC
Go to the stop light red yellow green power. Inside the stoplight is a 4 channel relay 
board.
When you create your stoplight use those relays to turn power on and off to your lights
You will need external 5v power for your relay board unless you hook up a ground wire to the 
mainboard

Parts List (for Circuit Boards)

Coin Cell Holder
DS1302
32.768 kHz crystal
2 1.5kOhm resistors
female headers or choice of cennectors
stackable headers for main board
1 IR TSOP1738 or other 38khz reciever
2 BD237 NPN Transistor
2 IR Leds high Power
5v regulator
2 4.7 kOhm resistors 1/2watt - 1watt
Arduino Mega
4 arduino pro mini 5v
1 mini thermal printer
1 RFID board MFRC522
1 4.3 TFT Screen
1 TFT Shield


 
