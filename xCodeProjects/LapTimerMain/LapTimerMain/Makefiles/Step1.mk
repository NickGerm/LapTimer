#
# embedXcode
# ----------------------------------
# Embedded Computing on Xcode
#
# Copyright © Rei VILO, 2010-2014
# http://embedxcode.weebly.com
# All rights reserved
#
#
# Last update: Nov 12, 2014 release 233



# Sketch unicity test and extension
# ----------------------------------
#
ifndef SKETCH_EXTENSION
    ifeq ($(words $(wildcard *.pde) $(wildcard *.ino)), 0)
        $(error No pde or ino sketch)
    endif

    ifneq ($(words $(wildcard *.pde) $(wildcard *.ino)), 1)
        $(error More than 1 pde or ino sketch)
    endif

    ifneq ($(wildcard *.pde),)
        SKETCH_EXTENSION := pde
    else ifneq ($(wildcard *.ino),)
        SKETCH_EXTENSION := ino
    else
        $(error Extension error)
    endif
endif


ifneq ($(SKETCH_EXTENSION),__main_cpp_only__)
    ifneq ($(SKETCH_EXTENSION),_main_cpp_only_)
        ifneq ($(SKETCH_EXTENSION),cpp)
            ifeq ($(words $(wildcard *.$(SKETCH_EXTENSION))), 0)
                $(error No $(SKETCH_EXTENSION) sketch)
            endif

            ifneq ($(words $(wildcard *.$(SKETCH_EXTENSION))), 1)
                $(error More than one $(SKETCH_EXTENSION) sketch)
            endif
        endif
    endif
endif


# Board selection
# ----------------------------------
# Board specifics defined in .xconfig file
# BOARD_TAG and AVRDUDE_PORT 
#
ifneq ($(MAKECMDGOALS),boards)
    ifneq ($(MAKECMDGOALS),clean)
        ifndef BOARD_TAG
            $(error BOARD_TAG not defined)
        endif
    endif
endif

ifndef BOARD_PORT
    BOARD_PORT = /dev/tty.usb*
endif


# Path to applications folder
#
USER_PATH      := $(wildcard ~)
EMBEDXCODE_APP  = $(USER_PATH)/Library/embedXcode
PARAMETERS_TXT  = $(EMBEDXCODE_APP)/parameters.txt

ifndef APPLICATIONS_PATH
    ifneq ($(wildcard $(PARAMETERS_TXT)),)
        ap1 = $(shell grep ^applications.path $(PARAMETERS_TXT) | cut -d = -f 2-;)
        ifneq ($(ap1),)
            APPLICATIONS_PATH = $(ap1)
        endif
    endif
endif

ifndef APPLICATIONS_PATH
    APPLICATIONS_PATH = /Applications
endif

# APPlications full paths
#
ARDUINO_APP   = $(APPLICATIONS_PATH)/Arduino.app
MPIDE_APP     = $(APPLICATIONS_PATH)/Mpide.app
WIRING_APP    = $(APPLICATIONS_PATH)/Wiring.app
ENERGIA_APP   = $(APPLICATIONS_PATH)/Energia.app
MAPLE_APP     = $(APPLICATIONS_PATH)/MapleIDE.app
ADAFRUIT_APP  = $(APPLICATIONS_PATH)/Adafruit.app
# ~
ROBOTIS_APP   = $(APPLICATIONS_PATH)/ROBOTIS_OpenCM.app
RFDUINO_APP   = $(APPLICATIONS_PATH)/RFduino.app

# Teensyduino.app path
#
TEENSY_0    = $(APPLICATIONS_PATH)/Teensyduino.app
ifneq ($(wildcard $(TEENSY_0)),)
    TEENSY_APP    = $(TEENSY_0)
else
    TEENSY_APP    = $(APPLICATIONS_PATH)/Arduino.app
endif

# DigisparkArduino.app path
#
DIGISPARK_0 = $(APPLICATIONS_PATH)/DigisparkArduino.app
ifneq ($(wildcard $(DIGISPARK_0)),)
    DIGISPARK_APP = $(DIGISPARK_0)
else
    DIGISPARK_APP = $(APPLICATIONS_PATH)/Arduino.app
endif

# Microduino.app path
#
MICRODUINO_0 = $(APPLICATIONS_PATH)/Microduino.app
ifneq ($(wildcard $(MICRODUINO_0)),)
    MICRODUINO_APP = $(MICRODUINO_0)
else
    MICRODUINO_APP = $(APPLICATIONS_PATH)/Arduino.app
endif

# ~
# LightBlueIDE.app path
#
LIGHTBLUE_0 = $(APPLICATIONS_PATH)/LightBlueIDE.app
ifneq ($(wildcard $(LIGHTBLUE_0)),)
    LIGHTBLUE_APP = $(LIGHTBLUE_0)
else
    LIGHTBLUE_APP = $(APPLICATIONS_PATH)/Arduino.app
endif

# GalileoIDE.app path
#
GALILEO_0    = $(APPLICATIONS_PATH)/GalileoIDE.app
ifneq ($(wildcard $(GALILEO_0)),)
    GALILEO_APP    = $(GALILEO_0)
else
    GALILEO_APP    = $(APPLICATIONS_PATH)/Arduino.app
endif

# RedBearLab.app path
#
REDBEARLAB_0    = $(APPLICATIONS_PATH)/RedBearLab.app
ifneq ($(wildcard $(REDBEARLAB_0)),)
    REDBEARLAB_APP    = $(REDBEARLAB_0)
else
    REDBEARLAB_APP    = $(APPLICATIONS_PATH)/Arduino.app
endif
# LittleRobotFriends.app path
#
LITTLEROBOTFRIENDS_0 = $(APPLICATIONS_PATH)/LittleRobotFriends.app
ifneq ($(wildcard $(LITTLEROBOTFRIENDS_0)),)
    LITTLEROBOTFRIENDS_APP    = $(LITTLEROBOTFRIENDS_0)
else
    LITTLEROBOTFRIENDS_APP    = $(APPLICATIONS_PATH)/Arduino.app
endif

ifeq ($(wildcard $(ARDUINO_APP)),)
    ifeq ($(wildcard $(MPIDE_APP)),)
        ifeq ($(wildcard $(WIRING_APP)),)
            ifeq ($(wildcard $(ENERGIA_APP)),)
                ifeq ($(wildcard $(MAPLE_APP)),)
                    ifeq ($(wildcard $(TEENSY_APP)),)
                        ifeq ($(wildcard $(DIGISPARK_APP)),)
                            ifeq ($(wildcard $(MICRODUINO_APP)),)
                                ifeq ($(wildcard $(LIGHTBLUE_APP)),)
                                    ifeq ($(wildcard $(GALILEO_APP)),)
                                        ifeq ($(wildcard $(ROBOTIS_APP)),)
                                            ifeq ($(wildcard $(RFDUINO_APP)),)
                                                ifeq ($(wildcard $(REDBEARLAB_APP)),)
                                                    ifeq ($(wildcard $(LITTLEROBOTFRIENDS_APP)),)
                                                        ifeq ($(wildcard $(MBED_APP)/*),) # */
                                                            ifeq ($(wildcard $(ADAFRUIT_APP)),)
                                                                $(error Error: no application found)
                                                            endif
                                                        endif
                                                    endif
                                                endif
                                            endif
                                        endif
                                    endif
                                endif
                            endif
                        endif
                    endif
                endif
            endif
        endif
    endif
endif


# Arduino-related nightmares
# ----------------------------------
#
# Arduino 1.0, 1.5 Java 6 and 1.5 Java 7 triple release nightmare
# Will 1.5 release go out of beta one day or shall we wait for 2.0?
#
ifneq ($(wildcard $(ARDUINO_APP)),) # */
    s102 = $(ARDUINO_APP)/Contents/Resources/Java/lib/version.txt
    s103 = $(ARDUINO_APP)/Contents/Java/lib/version.txt
    ifneq ($(wildcard $(s102)),)
        ARDUINO_RELEASE := $(shell cat $(s102) | sed -e "s/\.//g")
    else
        ARDUINO_RELEASE := $(shell cat $(s103) | sed -e "s/\.//g")
    endif
    ARDUINO_MAJOR := $(shell echo $(ARDUINO_RELEASE) | cut -d. -f 1-2)
else
    ARDUINO_RELEASE := 0
    ARDUINO_MAJOR   := 0
endif

# Arduino 1.5.7 nightmare with Java folder locations
# Another example of Arduino's quick and dirty job
#
ifeq ($(wildcard $(ARDUINO_APP)/Contents/Resources/Java),)
    ARDUINO_PATH   := $(ARDUINO_APP)/Contents/Java
else
    ARDUINO_PATH   := $(ARDUINO_APP)/Contents/Resources/Java
endif

# Same for RedBearLab plug-in for Arduino 1.5.x
#
ifeq ($(wildcard $(REDBEARLAB_APP)/Contents/Resources/Java),)
    REDBEARLAB_PATH   := $(REDBEARLAB_APP)/Contents/Java
else
    REDBEARLAB_PATH   := $(REDBEARLAB_APP)/Contents/Resources/Java
endif

# Same for LittleRobotFriends plug-in for Arduino 1.0.x
#
ifneq ($(findstring LITTLEROBOTFRIENDS,$(GCC_PREPROCESSOR_DEFINITIONS)),)

    LITTLEROBOTFRIENDS_RELEASE := $(shell find $(LITTLEROBOTFRIENDS_APP) -name version.txt -exec cat {} \; | sed -e "s/\.//g")
    LITTLEROBOTFRIENDS_MAJOR := $(shell find $(LITTLEROBOTFRIENDS_APP) -name version.txt -exec cat {} \; | cut -d. -f 1-2 | sed -e "s/\.//g")

    ifeq ($(wildcard $(LITTLEROBOTFRIENDS_APP)),)
        $(error Error: no application found)
    else
        ifeq ($(LITTLEROBOTFRIENDS_MAJOR),10)
            s101 = $(shell grep sketchbook.path $(USER_PATH)/Library/Arduino/preferences.txt | cut -d = -f 2)
            LITTLEROBOTFRIENDS_BOARDS = $(wildcard $(s101)/?ardware/LittleRobotFriends/boards.txt)
        else
#            s101 = $(shell grep sketchbook.path $(USER_PATH)/Library/Arduino15/preferences.txt | cut -d = -f 2)
#            LITTLEROBOTFRIENDS_PATH = $(wildcard $(s101)/?ardware/LittleRobotFriends/avr)
            $(error Little Robot Friends requires Arduino 1.0)
        endif
    endif
endif

# Paths list
#
MPIDE_PATH      = $(MPIDE_APP)/Contents/Resources/Java
WIRING_PATH     = $(WIRING_APP)/Contents/Resources/Java
ENERGIA_PATH    = $(ENERGIA_APP)/Contents/Resources/Java
MAPLE_PATH      = $(MAPLE_APP)/Contents/Resources/Java
TEENSY_PATH     = $(TEENSY_APP)/Contents/Resources/Java
DIGISPARK_PATH  = $(DIGISPARK_APP)/Contents/Resources/Java
MICRODUINO_PATH = $(MICRODUINO_APP)/Contents/Resources/Java
GALILEO_PATH    = $(GALILEO_APP)/Contents/Resources/Java
MBED_PATH       = $(EMBEDXCODE_APP)/mbed
ADAFRUIT_PATH   = $(ADAFRUIT_APP)/Contents/Resources/Java
LITTLEROBOTFRIENDS_PATH = $(LITTLEROBOTFRIENDS_APP)/Contents/Resources/Java
# ~
LIGHTBLUE_PATH  = $(LIGHTBLUE_APP)/Contents/Resources/Java
RFDUINO_PATH    = $(RFDUINO_APP)/Contents/Java
ROBOTIS_PATH    = $(ROBOTIS_APP)/Contents/Resources/Java


# Miscellaneous
# ----------------------------------
# Variables
#
TARGET      := embeddedcomputing
USER_FLAG   := false
TEMPLATE    := NmiFVEpTZkMqAfrNXJh7pgGKXf6GJLwz

# Builds directory
#
OBJDIR  = Builds

# Function PARSE_BOARD data retrieval from boards.txt
# result = $(call PARSE_BOARD 'boardname','parameter')
#
PARSE_BOARD = $(shell if [ -f $(BOARDS_TXT) ]; then grep ^$(1).$(2)= $(BOARDS_TXT) | cut -d = -f 2-; fi; )

# Function PARSE_FILE data retrieval from specified file
# result = $(call PARSE_FILE 'boardname','parameter','filename')
#
PARSE_FILE = $(shell if [ -f $(3) ]; then grep ^$(1).$(2) $(3) | cut -d = -f 2-; fi; )


# Clean if new BOARD_TAG
# ----------------------------------
#
NEW_TAG := $(strip $(OBJDIR)/$(BOARD_TAG)-TAG) #
OLD_TAG := $(strip $(wildcard $(OBJDIR)/*-TAG)) # */

ifneq ($(OLD_TAG),$(NEW_TAG))
    CHANGE_FLAG := 1
else
    CHANGE_FLAG := 0
endif


# Identification and switch
# ----------------------------------
# Look if BOARD_TAG is listed as a Arduino/Arduino board
# Look if BOARD_TAG is listed as a Arduino/arduino/avr board *1.5
# Look if BOARD_TAG is listed as a Arduino/arduino/sam board *1.5
# Look if BOARD_TAG is listed as a Mpide/PIC32 board
# Look if BOARD_TAG is listed as a Wiring/Wiring board
# Look if BOARD_TAG is listed as a Energia/MPS430 board
# Look if BOARD_TAG is listed as a MapleIDE/LeafLabs board
# Look if BOARD_TAG is listed as a Teensy/Teensy board
# Look if BOARD_TAG is listed as a Microduino/Microduino board
# Look if BOARD_TAG is listed as a Digispark/Digispark board
# Look if BOARD_TAG is listed as a IntelGalileo/arduino/x86 board
# Look if BOARD_TAG is listed as a Adafruit/Arduino board
# Look if BOARD_TAG is listed as a LittleRobotFriends board
# Look if BOARD_TAG is listed as a RedBearLab/arduino/RBL_nRF51822 board
# ~
# Look if BOARD_TAG is listed as a LightBlueIDE/LightBlue-Bean board
# Look if BOARD_TAG is listed as a Robotis/robotis board
# Look if BOARD_TAG is listed as a RFduino/RFduino board
#
# Order matters!
#
ifneq ($(MAKECMDGOALS),boards)
    ifneq ($(MAKECMDGOALS),clean)

        ifneq ($(findstring COSA,$(GCC_PREPROCESSOR_DEFINITIONS)),)
            include $(MAKEFILE_PATH)/Cosa.mk

        else
            ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(ARDUINO_PATH)/hardware/arduino/boards.txt),)
                include $(MAKEFILE_PATH)/Arduino.mk
            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(ARDUINO_PATH)/hardware/arduino/avr/boards.txt),)
                include $(MAKEFILE_PATH)/Arduino15avr.mk
            else ifneq ($(call PARSE_FILE,$(BOARD_TAG1),name,$(ARDUINO_PATH)/hardware/arduino/avr/boards.txt),)
                include $(MAKEFILE_PATH)/Arduino15avr.mk
            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(ARDUINO_PATH)/hardware/arduino/sam/boards.txt),)
                include $(MAKEFILE_PATH)/Arduino15sam.mk
            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(ARDUINO_PATH)/hardware/arduino/boards.txt),)
                include $(MAKEFILE_PATH)/Arduino.mk
            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(GALILEO_PATH)/hardware/arduino/x86/boards.txt),)
                include $(MAKEFILE_PATH)/Galileo.mk
            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(ADAFRUIT_PATH)/hardware/arduino/boards.txt),)
                ARDUINO_PATH := $(ADAFRUIT_PATH)
                include $(MAKEFILE_PATH)/Arduino1.mk

            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(LITTLEROBOTFRIENDS_BOARDS)),)
                include $(MAKEFILE_PATH)/Arduino1.mk
            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(REDBEARLAB_PATH)/hardware/arduino/RBL_nRF51822/boards.txt),)
                include $(MAKEFILE_PATH)/RedBearLab.mk

            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(MPIDE_PATH)/hardware/pic32/boards.txt),)
                include $(MAKEFILE_PATH)/Mpide.mk
            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(MPIDE_PATH)/hardware/pic32/variants/picadillo_35t/boards.txt),)
                include $(MAKEFILE_PATH)/Mpide.mk

            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(ENERGIA_PATH)/hardware/msp430/boards.txt),)
                include $(MAKEFILE_PATH)/EnergiaMSP430.mk
            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(ENERGIA_PATH)/hardware/c2000/boards.txt),)
                include $(MAKEFILE_PATH)/EnergiaC2000.mk
            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(ENERGIA_PATH)/hardware/lm4f/boards.txt),)
                include $(MAKEFILE_PATH)/EnergiaLM4F.mk
            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(ENERGIA_PATH)/hardware/cc3200/boards.txt),)
                include $(MAKEFILE_PATH)/EnergiaCC3200.mk

            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(MAPLE_PATH)/hardware/leaflabs/boards.txt),)
                include $(MAKEFILE_PATH)/MapleIDE.mk
            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(WIRING_PATH)/hardware/Wiring/boards.txt),)
                include $(MAKEFILE_PATH)/Wiring.mk
            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(TEENSY_PATH)/hardware/teensy/boards.txt),)
                include $(MAKEFILE_PATH)/Teensy.mk
            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(MICRODUINO_PATH)/hardware/Microduino/boards.txt),)
                include $(MAKEFILE_PATH)/Microduino.mk
            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(DIGISPARK_PATH)/hardware/digispark/boards.txt),)
                include $(MAKEFILE_PATH)/Digispark.mk

#            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(MBED_PATH)/boards.txt),)
#                include $(MAKEFILE_PATH)/mbed.mk
            else ifeq ($(filter MBED,$(GCC_PREPROCESSOR_DEFINITIONS)),MBED)
                include $(MAKEFILE_PATH)/mbed.mk

            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(REDBEARLAB_PATH)/hardware/arduino/RBL_nRF51822/boards.txt),)
                include $(MAKEFILE_PATH)/RedBearLab.mk

# ~
            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(LIGHTBLUE_PATH)/hardware/LightBlue-Bean/boards.txt),)
                include $(MAKEFILE_PATH)/LightBlue.mk

            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(ROBOTIS_PATH)/hardware/robotis/boards.txt),)
                include $(MAKEFILE_PATH)/Robotis.mk

            else ifneq ($(call PARSE_FILE,$(BOARD_TAG),name,$(RFDUINO_PATH)/hardware/arduino/RFduino/boards.txt),)
                include $(MAKEFILE_PATH)/RFduino.mk

            else
                $(error $(BOARD_TAG) board is unknown)
            endif
        endif
    endif
endif


# List of sub-paths to be excluded
#
EXCLUDE_NAMES  = Example example Examples examples Archive archive Archives archives Documentation documentation Reference reference
EXCLUDE_NAMES += ArduinoTestSuite
EXCLUDE_NAMES += $(EXCLUDE_LIBS)
EXCLUDE_LIST   = $(addprefix %,$(EXCLUDE_NAMES))

# Step 2
#
include $(MAKEFILE_PATH)/Step2.mk
