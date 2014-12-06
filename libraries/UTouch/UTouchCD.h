// UTouchCD.h
// ----------
//
// Since there are slight deviations in all touch screens you should run a
// calibration on your display module. Run the UTouch_Calibration sketch
// that came with this library and follow the on-screen instructions to
// update this file.
//
// Remember that is you have multiple display modules they will probably 
// require different calibration data so you should run the calibration
// every time you switch to another module.
// You can, of course, store calibration data for all your modules here
// and comment out the ones you dont need at the moment.
//

// These calibration settings works with my ITDB02-3.2S.
// They MIGHT work on your display module, but you should run the
// calibration sketch anyway.
//for 3.2 screen
// #define CAL_X 0x00380F4CUL
// #define CAL_Y 0x03C38174UL
// #define CAL_S 0x000EF13FUL

//for 3.2 screen Red
// #define CAL_X 0x01D7C755UL
// #define CAL_Y 0x0000C001UL
// #define CAL_S 0x8013F0EFUL



//for 4.3 screen
//  #define CAL_X 0x01CF8773UL
//  #define CAL_Y 0x039D01A9UL
//  #define CAL_S 0x801DF10FUL
 
 
 //for 4.3 screen B
 #define CAL_X 0x03E7004DUL
 #define CAL_Y 0x03ACC1D2UL
 #define CAL_S 0x801DF10FUL

//for 7.0 screen
// #define CAL_X 0x000E4FA9UL
// #define CAL_Y 0x003F8F0AUL
// #define CAL_S 0x8031F1DFUL
