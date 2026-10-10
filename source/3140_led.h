/* led.h
 *
 * Nils Napp
 * Cornell University
 * All right reserved
 *
 * Jan 2024
 * Ithaca NY
 *
 * This file is part of the ECE3140/CS3420 offering for Spring 2024. If you
 * are not part of this class you should not have access to this file. Do not
 * post, share, or otherwise distribute this file. We will consider it an AI
 * violation if you do. If you somehow get this code and you are NOT enrolled
 * the Spring 2024 version of ECE3140 please contact the course staff
 * immediately and describe how you found it.
 */

#ifndef _3140_LED_H_
#define _3140_LED_H_

#include <stdint.h> //include for int types

/*
 *  Struct for passing in RGB settings for LED1 and LED2
 */
typedef struct _grb32 {
	uint8_t blue;
	uint8_t red;
	uint8_t green;
	uint8_t __filler;
} grb32_t;


/**************************************
 *
 * This set up all the pins on the FRDMKL46z board so
 * you can use both the on-board and off board the LEDs.
 *
 * It also sets up the clock(s) to 15MHz to generate the right timings.
 *
 **************************************/
void led_init(void);


/**************************************
 * Functions for LED 0
 **************************************/

void red_on(void);
void red_off(void);
void red_toggle(void);

void blue_on(void);
void blue_off(void);
void blue_toggle(void);

void green_on(void);
void green_off(void);
void green_toggle(void);


/* Turn the red,green,and blue LED of LED0 off
 * Implement these functions in led.c
 */
void led0_off(void);

/******************************************
 *
 * Functions for using the on-board LEDs
 * You can use them even if your external board is broken for some reason
 *
 * For example, test_1.c uses only this functionality.
 *
 ******************************************/

void red_on_frdm(void);
void red_off_frdm(void);
void red_toggle_frdm(void);

void green_on_frdm(void);
void green_off_frdm(void);
void green_toggle_frdm(void);


/**************************************
 *
 * Functions for interacting with the serial LEDs (LED1 and LED2)
 *
 * set_led sends out a bit-stream for one led
 * led_off   sends out a bit-stream for turning off one LED
 *
 *************************************/

void set_led(grb32_t rgb_val);
void led_off(void);

/*
 * This function sets the core and bus clocks to 15MHz.
 * It can only run once, since it assumes the default clock
 * configuration for the board after a reset. If you call
 * it multiple times it will only run execute the first time.
 */
void set15MHz(void);

/*
 * Handy delay function that you are welcome to use.
 * These functions assume a core clock of 15MHz.
 */
void loop_3cycles(int); //implemented in .s file
void delay(uint32_t ms);


#endif /* _3140_LED_H_ */
