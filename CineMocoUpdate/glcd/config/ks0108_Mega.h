/*
 * ks0108_Mega.h - User specific configuration for Arduino GLCD library
 *
 * Use this file to set io pins
 *
 * The configuration below uses a single port for data 
 * and has the same wiring as the mega config from the previous ks0108 library 
 *
*/

#ifndef GLCD_PIN_CONFIG_H
#define GLCD_PIN_CONFIG_H

/*
 * define name for pin configuration
 */
#define glcd_PinConfigName "ks0108-Mega"

/*********************************************************/
/*  Configuration for assigning LCD bits to Arduino Pins */
/*********************************************************/
/*
 * Pins used for Commands
 */
#define glcdCSEL1     46    // CS1 Bit C 
#define glcdCSEL2     47    // CS2 Bit C
#define glcdRW        29    // R/W Bit C
#define glcdDI        39    // D/I Bit C
#define glcdEN        28    // EN Bit  C

#if NBR_CHIP_SELECT_PINS > 2
#define glcdCSEL3     23   // third chip select if needed
#endif

#if NBR_CHIP_SELECT_PINS > 3
#define glcdCSEL4     25   // fourth chip select if needed
#endif

/*
 * Data pin definitions
 * This version uses pins 22-29 for LCD Data 
 */
#define glcdData0Pin    27 // C
#define glcdData1Pin    26 // C
#define glcdData2Pin    25 // C
#define glcdData3Pin    24 // C
#define glcdData4Pin    23 // C
#define glcdData5Pin    22 // C
#define glcdData6Pin    44 // C
#define glcdData7Pin    45 // C

// Reset Bit  - uncomment the next line if reset is connected to an output pin
//#define glcdRES         30    // Reset Bit

#endif //GLCD_PIN_CONFIG_H
