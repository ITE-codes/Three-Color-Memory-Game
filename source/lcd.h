/*
 * lcd.h
 *
 *  Created on: May 6, 2022
 *      Author: Aaron Wilhelm
 */

#ifndef LCD_H_
#define LCD_H_

// the segments that need to be turned on for each of the 0-9 digits
extern const char segments[10][8];

typedef struct LCD_Pin {
	int mcu_pin;
	uint8_t waveForm;
	int isBackPlane;
} LCD_Pin;

extern LCD_Pin pins[13]; // size 13 (with first entry meaningless) so we don't have to convert from pin nums to idx nums confusingly



/*!
 * @brief SLCD time delay.
 * @param count The timedelay counter number.
 */
void LCD_TimeDelay(uint32_t count);

void init_lcd();

void segmentToPinNumber(int dig_num, char seg_char, int* pin_num, uint8_t* waveForm);

// disp_num is the display digit number ranging from 1 to 4
// seg_char is letter corresponding to the desired segment ranging from A-F, P for decimal point and N for colon
void turnOnSegment(int disp_num, char seg_char);

// disp_num is the display digit number ranging from 1 to 4
// seg_char is letter corresponding to the desired segment ranging from A-F, P for decimal point and N for colon
void turnOffSegment(int disp_num, char seg_char);

// clears the specified display
// disp_num is the display digit number ranging from 1 to 4
void clearDigit(int disp_num);

// displays the digit at the specified display.
// Any number currently displayed will be erased before the new one is written
// disp_num is the display digit number ranging from 1 to 4
// digit is the digit to be displayed
void displayDigit(int disp_num, int digit);

// displays the specified decimal point
void displayDecimalPoint(int dp_num);

// clears the specified decimal point
void clearDecimalPoint(int dp_num);

// displays the colon
void displayColon();

// clears the colon
void clearColon();


// clears the entire display
void clearDisplay();


#endif /* LCD_H_ */
