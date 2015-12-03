/*

 (c) 2015 Airic Lenz, C.A. Church
 
 The 1st version of this code, dealing with core functionalities, 
 was heavily inspired by the OpenMoCo Engine by C.A. Church
 and is basically based on it. Thank you for your great work! 
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 This software is based on miniEngine 1.0 (please see www.airiclenz.com)
 
 */


// include the library header

#include <MoCoM.h>
#include <EEPROM.h>

#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>

#include <fonts/SystemFont5x7.h>
#include <fonts/cineMoco_splash.h>

#include <bitmaps/batteryIcon.h>
#include <bitmaps/plugIcon.h>
#include <bitmaps/messageIcon.h>
#include <bitmaps/cinemocosplash.h>

#include <glcd.h>               // See http://code.google.com/p/glcd-arduino/downloads/list 

//#include <MemoryFree.h>       // See http://www.arduino.cc/playground/Code/AvailableMemory






#define DEBUG                           false 

// --------------------------
// Version of the software

#define DEF_VERSION                     2
#define DEF_SUBVERSION                  5




// --------------------------
// Keys and input interrupts


#define DEF_KEY_SELECT_INTERRUPT        1    // Pin 3
#define DEF_KEY_MENU_INTERRUPT          4    // Pin 19

#define DEF_KEY_LEFT_INTERRUPT          5    // Pin 18
#define DEF_KEY_LEFT_PIN                18

#define DEF_KEY_UP_INTERRUPT            3    // Pin 20
#define DEF_KEY_UP_PIN                  20

#define DEF_KEY_DOWN_INTERRUPT          2    // Pin 21
#define DEF_KEY_DOWN_PIN                21

#define DEF_KEY_RIGHT_INTERRUPT         0    // Pin 2
#define DEF_KEY_RIGHT_PIN               2


// --------------------------
// power supply measurements

#define DEF_POWER_BATTERY_PIN           A1
#define DEF_POWER_EXT_SUPPLY_PIN        A2



// --------------------------
// Arduino pins

// pin to tell the motor drivers to go to sleep
#define DEF_MOTOR_ENABLE_PIN            34
#define DEF_MOTOR_SLEEP_PIN             30         
#define DEF_MOTOR_STEP_PIN              33
#define DEF_MOTOR_DIR_PIN               35
#define DEF_MOTOR_MS1_PIN               31
#define DEF_MOTOR_MS2_PIN               32

// shutter and focus connections
#define DEF_CORE_CAMERA_PIN             9
#define DEF_CORE_FOCUS_PIN              8 

// pin for LCD backlight
#define DEF_UI_LCD_BACKLIGHT_PIN        4

// Communication pins
#define DEF_COM_DIRECTION_PIN           6



// internal key codes
#define  KEY_NONE                       255
#define  KEY_SELECT                     0
#define  KEY_MENU                       1
#define  KEY_UP                         2
#define  KEY_DOWN                       3
#define  KEY_LEFT                       4  
#define  KEY_RIGHT                      5



////////////////////////////////////////////////
//                                            //
//  B I T    O P E R A T I O N   M A C R O S  //
//                                            //
////////////////////////////////////////////////


#define isBit(v, b)     ((v & b) != 0) 
#define setBit(v, b)    (v |= b)
#define toggleBit(v, b) (v ^= b)
#define deleteBit(v, b) (v &= ~b)

// BITS FOR USAGE IN BITOPERATION MACROS
const uint8_t  BIT_0  = 1<<0;
const uint8_t  BIT_1  = 1<<1;
const uint8_t  BIT_2  = 1<<2;
const uint8_t  BIT_3  = 1<<3;
const uint8_t  BIT_4  = 1<<4;
const uint8_t  BIT_5  = 1<<5;
const uint8_t  BIT_6  = 1<<6;
const uint8_t  BIT_7  = 1<<7;

const uint16_t BIT_8  = 1<<8;
const uint16_t BIT_9  = 1<<9;
const uint16_t BIT_10 = 1<<10;
const uint16_t BIT_11 = 1<<11;
const uint16_t BIT_12 = 1<<12;
const uint16_t BIT_13 = 1<<13;
const uint16_t BIT_14 = 1<<14;
const uint16_t BIT_15 = 1<<15;




// ---------------------------
//   S Y S T E M   S E T U P
// ---------------------------

#define DEF_SYSTEM_SLEEP_DELAY          60000UL      // time in ms after the system falls asleep to save some energy (1 min)
#define DEF_SYSTEM_START_DELAY          1500L        // time delay pressing the start button to enable the system to settle down after pressing a button

#define DEF_STOP_MOTION_DELAY           1500UL       // delay in ms to user interactions between the single shots
#define DEF_RUNTIME_STANDARD            600          // standard time to record in run-based setup in seconds (10 min)

#define DEF_CAMERA_POST_STANDARD        200UL        // camera post delay in ms
#define DEF_CAMERA_CYCLE_STANDARD       30           // 30 * 0.1 sec = 3 sec standard cycle time 
#define DEF_CAMERA_MIN_EXPOSURE         50UL         // minimal working exposure in ms (shorter trigger signals are not recognized by the camera)
#define DEF_CAMERA_MAX_EXPOSURE         900000L      // 900k ms = 15min  

#define DEF_MOTOR_RAMP_STANDARD         400          // all motor-ramps shorter than this (in steps) will be reduced in speed
#define DEF_MOTOR_RAMP_DELAY            311          // a delay needed for the continuous ramping
#define DEF_MOTOR_POST_STANDARD         300UL        // motor post delay in ms
#define DEF_MOTOR_MAX_DELAY_STANDARD    16383        // used for the minimum speed of the motor (in microseconds)
#define DEF_MOTOR_MIN_DELAY_STANDARD    20 //40      // used for the maximum speed of the motor (in microseconds)
#define DEF_MOTOR_BREAK_DELAY           50           // milliseconds needed to let the motor settle and break after 
// action (before sleep is activated)
#define DEF_MOTOR_DISTANCE_STANDARD     1.0          // 1cm standard move between the cycles 
#define DEF_MOTOR_VIDEO_SPEED_STANDARD  80           // 80% speed (between zero delay and max delay) 
#define DEF_MOTOR_VIDEO_DIST_STANDARD   100          // 100cm standard distance


#define	MOTOR_TYPE_LINEAR		0
#define	MOTOR_TYPE_RADIAL		1


// --------------------------
// some static values

#define DEF_ULONG_MAX                   4294967295
#define DEF_UINT_MAX                    65535
#define DEF_UBYTE_MAX                   255

#define DEF_LONG_MAX                    2147483647


// --------------------------
// some mode definitions

#define DEF_RAMPING_LINEAR              0
#define DEF_RAMPING_ATANGENS            1
#define DEF_RAMPING_SINUS               2







// UI status flags
// 
// B0 = backlight on / off
// B1 = ui repaint flag
// B2 = save settings now flag
// B3 = settings were changed flag
// B4 = 
// B5 = long key press
// B6 = message on screen
// B7 = message to be painted

// B8 = full repaint needed
// B9 = 
// B10 = program done screen
// B11 = paint shoot count
// B12 = paint battery status
uint16_t ui_status = B11000000;






// UI / system settings
// 
// B7 = 
// B6 = settings autosave flag
// B5 = system sleep
// B4 = 
// B3 =
// B2 = 
// B1 = 
// B0 = 

// set default values:
byte ui_settings = BIT_6;




#define DEF_MODE_COUNT                  2
#define DEF_MODE_TIMELAPSE              1 
#define DEF_MODE_VIDEO                  0

#define DEF_MOVE_STYLE_COUNT            2
#define DEF_MOVE_STYLE_SMS              1
#define DEF_MOVE_STYLE_CONTINUOUS       0

#define DEF_SETUP_STYLE_COUNT           2
#define DEF_SETUP_STYLE_RUN             1
#define DEF_SETUP_STYLE_KEYFRAMES       0


// setup style
byte ui_setup_style =    DEF_SETUP_STYLE_KEYFRAMES;

byte system_mode =       DEF_MODE_VIDEO;
byte system_move_style = DEF_MOVE_STYLE_SMS;


// --------------------------
// General action status flags:
// 
// B7 = start phase is active
// B6 = camera focussing
// B5 = start cycle immediately
// B4 = camera is in the exposure phase (does not mean it is exporing in any case!)
// B3 = program running
// B2 = camera post exposure
// B1 = motor slot open
// B0 = motor post delay is active

byte action_status = 0;

// the time we started with an action (focussing, exposure, post delay) 
unsigned long action_start_time;

// user- or system interaction time (used for sleep and backlight timing)
unsigned long interaction_time;



// some variables for the run-based setup
byte          run_fps               = 25;                     // fps of the final clip  
unsigned long run_record_time       = DEF_RUNTIME_STANDARD;   // in seconds (default = 10 min)
unsigned long run_play_time         = 10;                     // seconds
float         run_dist              = 100.0;                  // cm



// --------------------------
// Camera settings

// cycle length of the whole cycle in milliseconds
unsigned long cycle_length; //           = DEF_CAMERA_CYCLE_STANDARD;

// camera shoot counter
unsigned long camera_shoot_count     = 0;
// the focus line will be risen before the exposure is done (in milliseconds)
unsigned long camera_focus_time      = 0;
// the time we want to expose our pictures (in milliseconds)
unsigned long camera_exp_time        = 1000;
// time after exposure before the motor is moved (in milliseconds)
unsigned long camera_post            = DEF_CAMERA_POST_STANDARD;
// the maximum amount of shots to do (0 = unlimited)
unsigned long camera_shot_max        = 0;

// array for the exposure stop-values used for exposure settings shorter than 1 sec
//                                      0   1   2   3    4    5    6    7    8    9    10   11   12    13    14    15    16    17    18    
int           exposure_values[]      = { 
  66, 77, 100, 125, 166, 200, 250, 300, 400, 500, 600, 800, 1000, 1300, 1600, 2000, 2500, 3200, 4000 };        
byte          exposure_value_count   = 16; // use the arry only until 2000


// --------------------------
// HDR & exposure settings

// B7 = HDR enabled or disabled
// B6 = bulp ramping enabled or disabled
// B5 = camera is exposing rigth now
// B4 = camera is between HDR shots
// B3 =
// B2 =
// B1 =
// B0 = 
byte exposure_status = 0;




#define CAMERA_TYPE_COUNT  5

// the motor calibration structure
typedef struct cameraType {
  const char    *name;
  boolean       focusHighWithShutter;
};


// the existing motor calibrations 
const struct cameraType camera_types[CAMERA_TYPE_COUNT] = {
  { 
    "Canon",     false   }  
  , { 
    "Nikon",     true    }  
  , { 
    "Panasonic", true    }
  , { 
    "Sony",      true    }  
  , { 
    "Other",     false   }  
};

// index of the used motor calibration
byte camera_type_index = 0;  // Canon





// the number of shots per HDR frame (minimum = 2, maximum = 7)
byte         HDR_shot_number     = 3;
// HDR current shot in the HDR row;
byte         HDR_current_shot    = 0;
// the first exposure value during bulp ramping 
unsigned long HDR_exposure_first  = 100;
// the last esposure value during bulp rmaping
unsigned long HDR_exposure_last   = 100; 
// the actual HDR bracketing value in 1/3 steps (6 * 1/3 = 2 ---> 6 = +/- 2)
byte         HDR_bracket         = 6;     
// bracketing factor (calculated by the program) to one side
// (staring with 100 ms center exposure and a bracketing of +/-2, the factor will
// be 4 --> 100ms / 4 = minimum exposure value of 25ms)
float        HDR_bracket_factor;  
// bracketing step-factor between the single HDR shots;
float        HDR_bracket_step_factor;  



// --------------------------
// Motor settings

// acceleration and decelleration variabled
byte          motor_ramp_percent    = 50;
unsigned long motor_ramp            = DEF_MOTOR_RAMP_STANDARD;   
unsigned long motor_ramp_jog        = DEF_MOTOR_RAMP_STANDARD;   


// the distance to be traveled during one cycle (in cm) - only used in S-M-S and stop motion mode
float motor_cycle_distance          = DEF_MOTOR_DISTANCE_STANDARD; 
// the speed, the motor is moving (in cm/s) - only used in continuous mode
float motor_speed                   = 0.5;
// jogging speed in percent (calculated up from the minimum speed)
byte motor_speed_jog                = 95;


// delay after motor movement (fixed)
unsigned long motor_post            = DEF_MOTOR_POST_STANDARD; 
// maximum delay in microsecond (delay between steps at minimum 
// speed of the motor (start and endphase)). 
unsigned long motor_step_delay_max  = DEF_MOTOR_MAX_DELAY_STANDARD;   
// minimum delay in microseconds (delay between steps at maximum speed of the motor) 
unsigned long motor_step_delay_min  = DEF_MOTOR_MIN_DELAY_STANDARD;    
// speed of the video mode (percentage between motor_step_delay_max = 1% and 100% with a delay of zero)
byte motor_video_speed              = DEF_MOTOR_VIDEO_SPEED_STANDARD;
// distance that the motor shound go in video mode
float motor_video_dist              = DEF_MOTOR_VIDEO_DIST_STANDARD;
// acceleration and decelleration (in % of motor_video_dist)
byte motor_video_ramp_in   = 5;
// acceleration and decelleration (in % motor_video_dist)
byte motor_video_ramp_out  = 5;






// the amount of steps done away from the defined home position
// clockwise = direction bit low (positive count)
// anti-clockwise = direction bit high (negative count)
long motor_home_steps               = 0;
// keyframe - start point
long motor_key_start                = 0;
// keyframe - end point
long motor_key_end                  = 0;



#define MOTOR_CALIBRATION_COUNT     4

// the motor calibration structure
typedef struct motorCalibration {
  const char    *name;
  float         stepsPerCm;
};

// the existing motor calibrations 
const struct motorCalibration motor_calibrations[MOTOR_CALIBRATION_COUNT] = {

  { 
    "Pan/Tilt",   89.0   }  // 1.8° Motor with 1/20th redudction 
  , { 
    "Slider",   4267.0   }  // 1.8° Motor with 1/20th redudction  
  , { 
    "S60",        94.3   }  // CineMoco 1.8° Motor  
  , { 
    "M60",       188.6   }  // CineMoco 0.9° Motor

};

// index of the used motor calibration
byte motor_calibration_index = 0;  // 0.9° Motor





#define MOTOR_STEP_SIZE_COUNT     3

// the motor calibration structure
typedef struct motorStepSize {
  const char    *name;
  byte          pinStates;
};


// the existing motor step sizes 
const struct motorStepSize motor_step_sizes[MOTOR_STEP_SIZE_COUNT] = {
  //   MS2 MS1
  { 
    "Low",     3   }  // B  1   1    1/8 Step
  , { 
    "Medium",  2   }  // B  1   0    1/4 Step
  , { 
    "High",    1   }  // B  0   1    1/2 Step

};

// index of the used motor calibration
byte motor_step_sizes_index = 0;  // 1/8 step





// Keyframe status flags:

// B0 = start key set
// B1 = end key set
// B2 = bouncing
// B3 = 
// B4 = 
// B5 = 
// B6 = 
// B7 = 

byte motor_key_status = 0;




// General Motor status and settings flags:
//
// B7 = sleep on / off            // default off
// B6 = limit switches on / off   // default off
// B5 = 
// B4 = keep motor powered        // default off
// B3 = motor direction           
// B2 = start from home           // default on 
// B1 = program starting          // used for preparing motor values for the run      
// B0 = set time for cont-mode    // re-sets the program start time for a smooth start

byte motor_status  = B00000000;


// the time we started the program
unsigned long program_start_time;


// --------------------------
// Menu 

#define MENU_MAX_LINES 20

// content of the menus (we initialize the array with some items - 
// fitting the longest possible menu.
char lines[32][MENU_MAX_LINES]; 

// help array for the lines to know which value corresponds to which
// line. this is filled by the item_codes when loading the menu content 
byte line_codes[MENU_MAX_LINES]; 





// --------------------------
// User interface 

// --------------------------
// Strings of our menu. 
// This is done to store the strings in in the program flash.
// If given like this: "some string", the strings would be placed in
// the RAM.

prog_char string_0  [] PROGMEM = "ON";    
prog_char string_1  [] PROGMEM = "OFF";
prog_char string_2  [] PROGMEM = "CW";   
prog_char string_3  [] PROGMEM = "CCW";
prog_char string_4  [] PROGMEM = "On";
prog_char string_5  [] PROGMEM = "Off";
prog_char string_6  [] PROGMEM = "";
prog_char string_7  [] PROGMEM = "Move Type";   
prog_char string_8  [] PROGMEM = "Continuous";
prog_char string_9  [] PROGMEM = "S-M-S";
prog_char string_10 [] PROGMEM = "Timelapse";
prog_char string_11 [] PROGMEM = "Cycle";
prog_char string_12 [] PROGMEM = "Run";
prog_char string_13 [] PROGMEM = "~";       // ~ = ->
prog_char string_14 [] PROGMEM = "Show";
prog_char string_15 [] PROGMEM = "Settings";
prog_char string_16 [] PROGMEM = "GEN";
prog_char string_17 [] PROGMEM = "CON";
prog_char string_18 [] PROGMEM = "MTR";
prog_char string_19 [] PROGMEM = "CAM";
prog_char string_20 [] PROGMEM = "deg";
prog_char string_21 [] PROGMEM = "#";
prog_char string_22 [] PROGMEM = "Speed";
prog_char string_23 [] PROGMEM = "Direction";
prog_char string_24 [] PROGMEM = "Motor Sleep";
prog_char string_25 [] PROGMEM = "Record";
prog_char string_26 [] PROGMEM = "Cam Type";
prog_char string_27 [] PROGMEM = "Fire Test Shot";
prog_char string_28 [] PROGMEM = "All";
prog_char string_29 [] PROGMEM = "Information";
prog_char string_30 [] PROGMEM = "Distance";
prog_char string_31 [] PROGMEM = "fps";
prog_char string_32 [] PROGMEM = "Rec time";
prog_char string_33 [] PROGMEM = "PlayB Time";
prog_char string_34 [] PROGMEM = "PlayB FPS";
prog_char string_35 [] PROGMEM = "Start Home";       
prog_char string_36 [] PROGMEM = "Exposure";
prog_char string_37 [] PROGMEM = "shot #";
prog_char string_38 [] PROGMEM = "no limit";
prog_char string_39 [] PROGMEM = ":";
prog_char string_40 [] PROGMEM = "Brightness";
prog_char string_41 [] PROGMEM = "b-light tm";
prog_char string_42 [] PROGMEM = "Motor Delay";    // motor post                
prog_char string_43 [] PROGMEM = "Cam Delay";      // camera post            
prog_char string_44 [] PROGMEM = "Scrl Ramp";               
prog_char string_45 [] PROGMEM = "Motor Type";                
prog_char string_46 [] PROGMEM = "Go Home";  
prog_char string_47 [] PROGMEM = "Set Home";  
prog_char string_48 [] PROGMEM = "HDR";  
prog_char string_49 [] PROGMEM = "Bracket";             
prog_char string_50 [] PROGMEM = "Bulb Ramp";  
prog_char string_51 [] PROGMEM = "First Exp";  
prog_char string_52 [] PROGMEM = "Last Exp";
prog_char string_53 [] PROGMEM = "Video";  
prog_char string_54 [] PROGMEM = "Setup";
prog_char string_55 [] PROGMEM = "-----";         
prog_char string_56 [] PROGMEM = "Ramp";        
prog_char string_57 [] PROGMEM = "Reset Settings";
prog_char string_58 [] PROGMEM = "Scrl Ramp";      
prog_char string_59 [] PROGMEM = "Scrl Speed";
prog_char string_60 [] PROGMEM = "ms";
prog_char string_61 [] PROGMEM = "Keyframes";
prog_char string_62 [] PROGMEM = "%";
prog_char string_63 [] PROGMEM = "cm";
prog_char string_64 [] PROGMEM = "cm/s";
prog_char string_65 [] PROGMEM = "rev";
prog_char string_66 [] PROGMEM = "Motor";
prog_char string_67 [] PROGMEM = "min";
prog_char string_68 [] PROGMEM = "s";             // s
prog_char string_69 [] PROGMEM = "/";             // / 

prog_char string_70 [] PROGMEM = "Information";
prog_char string_71 [] PROGMEM = "miniEngine";
prog_char string_72 [] PROGMEM = "CineMoco";
prog_char string_73 [] PROGMEM = "Developers (in";
prog_char string_74 [] PROGMEM = "alphabet. order):";
prog_char string_75 [] PROGMEM = "Airic Lenz,";
prog_char string_76 [] PROGMEM = "Alvarocalvo,";
prog_char string_77 [] PROGMEM = "C.A. Church,";
prog_char string_78 [] PROGMEM = "Eric Pak,";
prog_char string_79 [] PROGMEM = "Justin Jensen,";
prog_char string_80 [] PROGMEM = "Karl McDonald,";
prog_char string_81 [] PROGMEM = "Marc Lane.";
prog_char string_82 [] PROGMEM = "Check out";
prog_char string_83 [] PROGMEM = "cinetics.com or";
prog_char string_84 [] PROGMEM = "airiclenz.com";


prog_char string_85 [] PROGMEM = "USB Mode";
prog_char string_86 [] PROGMEM = "Press [SELECT] to";
prog_char string_87 [] PROGMEM = "restart the CineMoco";
prog_char string_88 [] PROGMEM = "and get back to the";
prog_char string_89 [] PROGMEM = "usual CineMoco mode.";
prog_char string_90 [] PROGMEM = "";
prog_char string_91 [] PROGMEM = "Motor home";
prog_char string_92 [] PROGMEM = "position is now set.";
prog_char string_93 [] PROGMEM = "Please wait while"; 
prog_char string_94 [] PROGMEM = "motor is moved to";
prog_char string_95 [] PROGMEM = "home position...";
prog_char string_96 [] PROGMEM = "Motor home position";
prog_char string_97 [] PROGMEM = "reached.";

prog_char string_98 [] PROGMEM = "Reset settings";
prog_char string_99 [] PROGMEM = "All custom settings";
prog_char string_100[] PROGMEM = "deleted. System will";
prog_char string_101[] PROGMEM = "reboot now...";

prog_char string_102[] PROGMEM = "Chain ID";
prog_char string_103[] PROGMEM = "Master";
prog_char string_104[] PROGMEM = "Device ";
prog_char string_105[] PROGMEM = "Daisy-chaining";
prog_char string_106[] PROGMEM = "Update Chain";
prog_char string_107[] PROGMEM = "~ update!";
prog_char string_108[] PROGMEM = "# Of Shots";
prog_char string_109[] PROGMEM = "+/- ";
prog_char string_110[] PROGMEM = "Canon";
prog_char string_111[] PROGMEM = "Nikon";
prog_char string_112[] PROGMEM = "System";
prog_char string_113[] PROGMEM = "";
prog_char string_114[] PROGMEM = "Show supply";
prog_char string_115[] PROGMEM = "<= 1/15s";
prog_char string_116[] PROGMEM = "1/13s";
prog_char string_117[] PROGMEM = "1/10s";
prog_char string_118[] PROGMEM = "1/8s";
prog_char string_119[] PROGMEM = "1/6s";
prog_char string_120[] PROGMEM = "1/5s";
prog_char string_121[] PROGMEM = "1/4s";
prog_char string_122[] PROGMEM = "";
prog_char string_123[] PROGMEM = "shw extpwr"; 
prog_char string_124[] PROGMEM = "Speed"; 
prog_char string_125[] PROGMEM = "Ramp In"; 
prog_char string_126[] PROGMEM = "Ramp Out"; 
prog_char string_127[] PROGMEM = "Distance"; 
prog_char string_128[] PROGMEM = "VIDEO";
prog_char string_129[] PROGMEM = "Move";
prog_char string_130[] PROGMEM = { 
  char(130), char(130), char(0) }; // < + null termination
prog_char string_131[] PROGMEM = { 
  char(129), char(129), char(0) }; // > + null termination
prog_char string_132[] PROGMEM = "Set Start Point";
prog_char string_133[] PROGMEM = "Set End Point";
prog_char string_134[] PROGMEM = "Go to Start";
prog_char string_135[] PROGMEM = "Keyframes";
prog_char string_136[] PROGMEM = "Position is now set.";
prog_char string_137[] PROGMEM = "Please wait while";
prog_char string_138[] PROGMEM = "motor is moved to";
prog_char string_139[] PROGMEM = "key position...";
prog_char string_140[] PROGMEM = "Key position";
prog_char string_141[] PROGMEM = "reached.";
prog_char string_142[] PROGMEM = "";
prog_char string_143[] PROGMEM = { 
  char(128), ' ' , '~', char(0) }; // v -> + null termination
prog_char string_144[] PROGMEM = "This key was not";
prog_char string_145[] PROGMEM = "set yet!";
prog_char string_146[] PROGMEM = "Not all keyframes";
prog_char string_147[] PROGMEM = "were set!";
prog_char string_148[] PROGMEM = "Syst Sleep";
prog_char string_149[] PROGMEM = "Keep Powered";
prog_char string_150[] PROGMEM = "Go to End";
prog_char string_151[] PROGMEM = "Updating chaining";
prog_char string_152[] PROGMEM = "data. This might";
prog_char string_153[] PROGMEM = "take some seconds...";
prog_char string_154[] PROGMEM = "Dev";
prog_char string_155[] PROGMEM = "--";
prog_char string_156[] PROGMEM = "Loop";
prog_char string_157[] PROGMEM = "All";         
prog_char string_158[] PROGMEM = "Registered in";
prog_char string_159[] PROGMEM = "the chain...";
prog_char string_160[] PROGMEM = "Engine preparing...";        
prog_char string_161[] PROGMEM = "While waiting for";        
prog_char string_162[] PROGMEM = "the central sync";
prog_char string_163[] PROGMEM = "signal, press Play";
prog_char string_164[] PROGMEM = "to start manually.";

prog_char string_165 [] PROGMEM = "Stop-motion";
prog_char string_166 [] PROGMEM = "Please PRESS PLAY";
prog_char string_167 [] PROGMEM = "to continue with";
prog_char string_168 [] PROGMEM = "the next shot.";
prog_char string_169 [] PROGMEM = "Doubble-press to";
prog_char string_170 [] PROGMEM = "stop recording.";

prog_char string_171 [] PROGMEM = "Start USB Mode";
prog_char string_172 [] PROGMEM = "";   
prog_char string_173 [] PROGMEM = "";
prog_char string_174 [] PROGMEM = "";



// Now set up a table to refer to the strings 
// (a simple list of all the strings we defined)
PROGMEM const char *string_table[] = {   
  string_0,   string_1,   string_2,   string_3,   string_4, 
  string_5,   string_6,   string_7,   string_8,   string_9,   
  string_10,  string_11,  string_12,  string_13,  string_14,  
  string_15,  string_16,  string_17,  string_18,  string_19, 
  string_20,  string_21,  string_22,  string_23,  string_24,
  string_25,  string_26,  string_27,  string_28,  string_29,
  string_30,  string_31,  string_32,  string_33,  string_34,
  string_35,  string_36,  string_37,  string_38,  string_39,
  string_40,  string_41,  string_42,  string_43,  string_44,
  string_45,  string_46,  string_47,  string_48,  string_49,
  string_50,  string_51,  string_52,  string_53,  string_54,
  string_55,  string_56,  string_57,  string_58,  string_59,
  string_60,  string_61,  string_62,  string_63,  string_64,
  string_65,  string_66,  string_67,  string_68,  string_69,
  string_70,  string_71,  string_72,  string_73,  string_74,
  string_75,  string_76,  string_77,  string_78,  string_79,
  string_80,  string_81,  string_82,  string_83,  string_84,
  string_85,  string_86,  string_87,  string_88,  string_89,
  string_90,  string_91,  string_92,  string_93,  string_94,
  string_95,  string_96,  string_97,  string_98,  string_99,
  string_100, string_101, string_102, string_103, string_104,
  string_105, string_106, string_107, string_108, string_109,
  string_110, string_111, string_112, string_113, string_114,
  string_115, string_116, string_117, string_118, string_119,
  string_120, string_121, string_122, string_123, string_124,
  string_125, string_126, string_127, string_128, string_129, 
  string_130, string_131, string_132, string_133, string_134,
  string_135, string_136, string_137, string_138, string_139,
  string_140, string_141, string_142, string_143, string_144,
  string_145, string_146, string_147, string_148, string_149,
  string_150, string_151, string_152, string_153, string_154,
  string_155, string_156, string_157, string_158, string_159,
  string_160, string_161, string_162, string_163, string_164,
  string_165, string_166, string_167, string_168, string_169,
  string_170, string_171, string_172, string_173, string_174

}; 






// the global variable holding the key-pressed buffer
byte key_pressed;

// wait time until backlight turns off in milliseconds
unsigned int backlight_wait = 15;

// backlight level in percent
byte backlight_level = 100;

// a variable to remeber when a message started
unsigned long message_start_time; 

// a variable to set how long the message will be displayed (-1 = eternal)
byte message_duration;



// the byte that stores the battery and external supply values
// B0 = External supply connected
// B1-7 = battery level (decimal value)  
byte power_status = B00000000; 

#define BATTERY_AVG_BUF_LEN   10               // length of the average-value buffer (longer = slower changes)
#define BATTERY_DELAY         3000             // measure every X milliseconds

// array for storing the last battery values and calculating an average value  
// to filter out voltage drops created by short heavy loads
byte battery_avg_buffer[BATTERY_AVG_BUF_LEN]; 
// current position in the array
byte battery_avg_pos = 0;
// the time we measured the power supply the last time
unsigned long power_measure_time;
unsigned int  battery_val_empty = 390;                 // initial values - get changed by self-calibration
unsigned int  battery_val_full  = 440;                 // initial values - get changed by self-calibration
boolean       battery_show_external_supply = true;     // show if the external supply is connected 

// the time
unsigned long now;



// the communication object
MoCoM com(DEF_COM_DIRECTION_PIN, Serial2);
// the id of our communication entity - initially 0
byte  com_id = MOCOM_MASTER_ID; 


uint8_t old = system_mode;

// varibale storing if the next cycle started
boolean timeOk;




// --------------------------
// User interface 


// this variable is used to detect if the 
// user chose to start the USB mode. It is needed as
// the unit resets after connecting to the computer 
boolean  dragonframe_startUSBMode = false;





// ===================================================================================
// Arduino Setup Procedure
// ===================================================================================
void setup() {

  if (DEBUG)
  {
    Serial.begin(57600);
    Serial.println("DEBUG mode enabled");
    Serial.println();
  }
  
  // Initialize the GLCD 
  GLCD.Init(NON_INVERTED); 


  pinMode(DEF_MOTOR_DIR_PIN, OUTPUT);
  pinMode(DEF_MOTOR_STEP_PIN, OUTPUT);
  pinMode(DEF_MOTOR_SLEEP_PIN, OUTPUT);
  pinMode(DEF_MOTOR_MS1_PIN, OUTPUT);
  pinMode(DEF_MOTOR_MS2_PIN, OUTPUT);


  // set motor to sleep 
  digitalWrite(DEF_MOTOR_SLEEP_PIN, LOW);


  pinMode(DEF_CORE_CAMERA_PIN, OUTPUT);
  pinMode(DEF_CORE_FOCUS_PIN, OUTPUT);

  pinMode(DEF_UI_LCD_BACKLIGHT_PIN, OUTPUT);

  // set the pinModes for the key input pins (needed for long presses)
  //pinMode(DEF_KEY_SELECT_PIN, INPUT);
  //pinMode(DEF_KEY_MENU_PIN , INPUT);
  pinMode(DEF_KEY_LEFT_PIN, INPUT);
  pinMode(DEF_KEY_UP_PIN, INPUT);
  pinMode(DEF_KEY_DOWN_PIN, INPUT);
  pinMode(DEF_KEY_RIGHT_PIN, INPUT);


  // register the interrupt events for the key pins
  attachInterrupt(DEF_KEY_SELECT_INTERRUPT, keys_select_key_event, RISING);
  attachInterrupt(DEF_KEY_MENU_INTERRUPT,   keys_menu_key_event,   RISING);
  attachInterrupt(DEF_KEY_UP_INTERRUPT,     keys_up_key_event,     RISING);
  attachInterrupt(DEF_KEY_DOWN_INTERRUPT,   keys_down_key_event,   RISING);
  attachInterrupt(DEF_KEY_LEFT_INTERRUPT,   keys_left_key_event,   RISING);
  attachInterrupt(DEF_KEY_RIGHT_INTERRUPT,  keys_right_key_event,  RISING);


  // did we previously save settings to eeprom?
  // is config saved and has the config has the correct version?
  if ((eeprom_is_config_saved()) && 
    (eeprom_is_version_ok())) {

    eeprom_load_battery_calibration();    
    eeprom_load_config();

    // delete some bits that might have been save
    // but which we don't want to be set
    deleteBit(motor_key_status, BIT_0);    // start keyframe is set
    deleteBit(motor_key_status, BIT_1);    // end keyframe is set   

  } 
  else {  

    eeprom_write_battery_calibration();
    eeprom_write_config();
    eeprom_write_version();
  } 


  // set the motor step-size
  motor_setStepSize();

  // init the battery functionalities  
  pinMode(DEF_POWER_BATTERY_PIN, INPUT);
  pinMode(DEF_POWER_EXT_SUPPLY_PIN, INPUT);
  power_init_battery_measurement();



  // print welcome screen
  GLCD.DrawBitmap(cinemocosplash, 0, 8, BLACK);
  GLCD.SelectFont(cineMoco_splash, BLACK);
  GLCD.CursorToXY(89, 28);

  GLCD.print(DEF_VERSION);
  GLCD.print(F("."));
  GLCD.print(DEF_SUBVERSION);

  // enable backlight
  ui_set_backlight_level();


  delay(2300);
  GLCD.ClearScreen();
  GLCD.SelectFont(System5x7, BLACK);

  // initialize the communication
  com_init();

  // delete the key buffer to have no unwanted actions
  keys_delete_key_buffer();


  /*
  #ifdef DEBUG
   
   Serial.print(F("mem free "));
   Serial.println(freeMemory(), DEC);
   
   #endif
   */


  // we we started the USB-mode and the computer connected
  // (which caused the reboot), the start the USB mode again
  if (dragonframe_startUSBMode) {

    // initialize the dragonframe module
    // and send false for deleting the reboot flag     
    dragonframe_init(false);

    // print the message for the dragonframe 
    ui_init_dragonframe_message();
    ui_print_message();

    // jump to the dragonframe module
    dragonframe_process();  
  }


}





// ===================================================================================
// Arduinos eternal loop procedure
// ===================================================================================
void loop() {

  // if the programm is running and we are in contiuous mode
  motor_continuous_check(); 

  // process the communication and everything is is related
  com_process();

  // if the programm is running and we are in contiuous mode
  motor_continuous_check(); 

  // do the screen stuff
  ui_do_screen();

  // if the programm is running and we are in contiuous mode
  motor_continuous_check(); 



  // programm is running
  if (core_is_program_running()) {

    // is one cycle over?
    timeOk = core_is_next_cycle();


    // =================================================
    // C O N T I N U O U S   m o d e
    // =================================================
    if (core_is_mode_timelapse() && core_is_style_continuous()) {

      // in case we started the program (start immediatelly flag is set)
      if (core_is_start_immediately()) {

        // should we ramp into continuous mode?
        if (motor_status & B00100000) {

          // do some continuous steps    
          motor_move_continuous();

        }

      }  

      // do some continuous steps    
      motor_move_continuous();


      if (
      ((!com.isMaster()) && 
        (
      (com_is_sync_flag()) ||
        (com_is_manual_override_start_flag() && timeOk)
        )
        ) ||
        ((com.isMaster()) && 
        (
      core_is_start_immediately() ||
        timeOk
        )
        )
        ) {


        // do some continuous steps    
        motor_move_continuous();

        // if not exposing right now and not in camera post delay       
        if (!(action_status & B01000000) && 
          !(action_status & B00010000) && 
          !(action_status & B00000100))  { 

          // do some continuous steps    
          motor_move_continuous();    

          // should we do focussing?    
          if (camera_focus_time > 0) {
            camera_start_focus_phase();
          } 
          else {

            // send the shoot command to the clients
            if (com.isMaster()) {
              com.sendCommand(MOCOM_BROADCAST, MOCOM_COMMAND_CAMERA_SHOOT);

              // wait 1ms (time needed to reach the slaves) to have a perfectly
              // synced shot with all clients
              core_delay(1);
            } 

            // shoot camera 
            camera_start_exposure_phase();
          }

        }

        // do some continuous steps    
        motor_move_continuous();

        // delete the start immediately flag if set
        if (core_is_start_immediately()) {

          // delete the start immediately flag
          bitClear(action_status, 5); // B11011111
        }

        // do some continuous steps    
        motor_move_continuous();

        // delete the daisy chaining sync flag if it was set
        if (com_is_sync_flag()) com_delete_sync_flag();

      } // end: camera cycle started


      // do some continuous steps    
      motor_move_continuous();


      // is camera focussing right now?
      if (camera_is_in_focus_phase()) {

        // did we focus the time we wanted to focus?
        if((action_start_time + (unsigned long) camera_focus_time) <= millis()) {

          // stop focussing (this function automatically starts the exposure)
          camera_stop_focus_phase();

          // now shoot the camera
          camera_start_exposure_phase();

        } 
      }

      // do some continuous steps    
      motor_move_continuous();


      // is camera in exposure phase right now?
      if (camera_is_in_exposure_phase()) {

        // do some continuous steps    
        motor_move_continuous();    

        // if camera exposure is done
        if (camera_check_exposure_state()) {

          // do some continuous steps    
          motor_move_continuous(); 

          // stop the camera phase
          camera_stop_exposure_phase();

          // do some continuous steps    
          motor_move_continuous(); 

          camera_start_post_phase();

        }

      }

      // do some continuous steps    
      motor_move_continuous();


      // camera post delay
      if (camera_is_in_post_phase()) {

        // did we waited the time we wanted to wait?
        if((action_start_time + (unsigned long) camera_post) <= millis()) {

          // stop the motor post phase
          camera_stop_post_phase();

          // check if we need to stop the program
          core_check_if_timelapse_is_done();

        }

      }

    }


    // =================================================
    // S H O O T - M O V E - S H O O T   m o d e
    // =================================================
    else if (core_is_mode_timelapse() && core_is_style_SMS()) { 


      // only start if we are not in stop-motion wait
      //if (!core_is_in_stop_motion_wait()) {


      // start the next cycle now if:
      // we are in stop-motion mode (for not waiting until the cycle is over)
      // it is time (cycle time has come) or
      // the "start immediately" flag is set

      if (
      ((!com.isMaster()) && 
        (
      (com_is_sync_flag()) ||
        (com_is_manual_override_start_flag() && timeOk)
        )
        ) ||
        ((com.isMaster()) && 
        (
      //(core_is_mode_stop_motion()) ||   
      (core_is_start_immediately()) ||
        timeOk
        )
        ) 
        ) {


        // if not focussing or in exposure phaser right now and not in camera or motor post delay        
        if (!camera_is_in_focus_phase()    && 
          !camera_is_in_exposure_phase() && 
          !camera_is_in_post_phase()     && 
          !motor_is_in_post_phase() )  { 

          // should we do focussing?    
          if (camera_focus_time > 0) {

            camera_start_focus_phase();

          } 
          else {

            // send the shoot command to the clients
            if (com.isMaster()) {
              com.sendCommand(MOCOM_BROADCAST, MOCOM_COMMAND_CAMERA_SHOOT);

              // wait 1ms (time needed to reach the slaves) to have a perfectly
              // synced shot with all clients (accuracy within 200 microseconds)
              core_delay(1);
            }

            // shoot camera 
            camera_start_exposure_phase();

          }
        }

        // delete the start immediately flag if set
        if (core_is_start_immediately()) {

          // toggle the start immediately flag to off
          bitClear(action_status, 5);
        }

        // delete the daisy chaingin sync flag when it was set
        if (com_is_sync_flag()) com_delete_sync_flag();

      } // camera cycle started


      // is camera focussing right now?
      if (camera_is_in_focus_phase()) {

        // did we focus the time we wanted to focus?
        if((action_start_time + (unsigned long) camera_focus_time) <= millis()) {

          // stop focussing (this function automatically starts the exposure)
          camera_stop_focus_phase();

          // now shoot the camera
          camera_start_exposure_phase();

        } 
      }


      // is camera exposing right now?
      if (camera_is_in_exposure_phase()) {

        // if camera exposure is done (result = true) then start the motor
        if (camera_check_exposure_state()) {

          camera_stop_exposure_phase();


          // if program is still running (it might be stopped by
          // max shot number reached) then start the post or the motor phase
          if (core_is_program_running()) {

            if (camera_post > 0) {

              // start the camera post phase
              camera_start_post_phase(); 

            }
            // no camera post
            else {

              // check if we need to stop the program
              core_check_if_timelapse_is_done();

              // now start the motor phase
              motor_start_phase();  

            }

          }

        } // end: camera exposure done

      } // end: camera exposing


      // camera post delay
      if (camera_is_in_post_phase()) {

        // did we exposed the time we wanted to expose?
        if((action_start_time + (unsigned long) camera_post) <= millis()) {

          // stop the motor post phase
          camera_stop_post_phase();

          // check if we need to stop the program
          core_check_if_timelapse_is_done();

          // now start the motor phase 
          motor_start_phase();  

        } // end: camera post done

      } // end: camera post phase







        // time for the motor
      if (motor_is_in_motor_phase()) {

        // do the motor phase with error check
        if (!motor_move(DEF_RAMPING_ATANGENS)) {

          // if an error occured (return = false) stop the program and do not continue
          core_stop_program(com.isMaster(), false);

        } 
        // successfully finnished the motor phase:
        else {

          // end the motor phase
          motor_stop_phase();

          // start the motor post delay
          motor_start_post_phase();

        }

      } 


      // motor post delay
      if (motor_is_in_post_phase()) {

        // did we waited the time we wanted to wait?
        if((action_start_time + (unsigned long) motor_post) <= millis()) {

          motor_stop_post_phase();

        }

      }

    } // operation mode (SMS)



    // =================================================
    // V I D E O   M O D E
    // =================================================
    else if (core_is_mode_video()) {

      // send the shoot command to the clients
      if (com.isMaster()) {
        com.sendCommand(MOCOM_BROADCAST, MOCOM_COMMAND_START);

        // wait 1ms (time needed for the com to reach the slaves) to have a synced start
        core_delay(1);
      }


      boolean motorMoveResult;
      boolean keysFlipped = false;
      long temp;
      boolean motorDir = motor_status & B00001000;


      // no bounce bounce 
      if (!motor_is_looping()) {
        
        // do the video move with error check
        motorMoveResult = motor_video_move(true, DEF_RAMPING_ATANGENS, motor_video_ramp_in,  motor_video_ramp_out);
        
      } 
      else { // bounce mode

        boolean stopBounce = false;

        while(!stopBounce && core_is_program_running()) {

          // do the video move with error check
          motorMoveResult = motor_video_move(true, DEF_RAMPING_ATANGENS, motor_video_ramp_in, motor_video_ramp_out);

          if (motorMoveResult == false) {

            stopBounce = true;

          } 
          else {
            
            // switch start- and end-keyframe 
            temp = motor_key_start;
            motor_key_start = motor_key_end;
            motor_key_end = temp;

            // track the status of the keyframe change
            keysFlipped = !keysFlipped;

            // wait some time to allow the motor to come to a full stop
            core_delay(50);

            // change the motor direction
            motorDir = !motorDir;
            motor_set_direction(motorDir);

            // prepare everything for the motor
            motor_prepare_values(true, false);

          }


          // check the key input
          if (keys_get_key_pressed() == KEY_SELECT) {
            core_stop_program(com.isMaster(), false);  
          }


        } // end: loop

      } // end: bounce mode


        // reset the keyframe & motor values if needed
      if (keysFlipped) {

        // switch start- and end-keyframe back to normal
        temp = motor_key_start;
        motor_key_start = motor_key_end;
        motor_key_end = temp;  

        // change the motor direction
        motorDir = !motorDir;
        motor_set_direction(motorDir);

      } // reset keyframes


      // reset the max speed value to default
      motor_step_delay_min = DEF_MOTOR_MIN_DELAY_STANDARD;

      // stop the program and send a stop signal to all clients if master
      core_stop_program(false, true);  

    } // video mode



      // if the programm is running and we are in contiuous mode
    motor_continuous_check();

  }  // end: program is running

  // no program running
  else {

    // set the motor to sleep if it is supposed to sleep
    if ((!motor_is_keep_powered()) &&
      (digitalRead(DEF_MOTOR_SLEEP_PIN) == HIGH)) {

      digitalWrite(DEF_MOTOR_SLEEP_PIN, LOW);
    }

  } 


  // check if we received any new com-package
  com.executeCommunication();

  // if the programm is running and we are in contiuous mode
  motor_continuous_check(); 

  // check the power supply (if external is connected, battery level, ...)
  power_check();

  // if the programm is running and we are in contiuous mode
  motor_continuous_check(); 

  // check if the system needs to sleep...
  core_check_sleep();  

  // if the programm is running and we are in contiuous mode
  motor_continuous_check(); 


}



