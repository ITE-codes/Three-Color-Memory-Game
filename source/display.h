// display.h

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

/*
 * Initialize the LCD display and start counter at 1
 */
void Display_Init(void);

/*
 * Clear the LCD display
 */
void Display_Clear(void);

/*
 * Show a number on the LCD
 */
void Display_ShowNumber(uint32_t number);

/*
 * Increment the display counter by 1 and update LCD
 */
void Display_IncrementCounter(void);

/*
 * Return current counter value
 */
uint32_t Display_GetCounter(void);

/*
 * Reset counter back to 1 and update LCD
 */
void Display_ResetCounter(void);

#endif /* DISPLAY_H_ */
