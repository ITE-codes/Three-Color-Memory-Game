/*
 * lcd.c
 *
 *  Created on: May 6, 2022
 *      Author: Aaron Wilhelm.
 */

#include "board.h"
#include "fsl_slcd.h"
#include "pin_mux.h"
#include "lcd.h"


const char segments[10][8] = {"ABCDEF", "BC", "ABEDG", "ABCDG", "BCFG", "ACDDFG", "ACDEFG", "ABC", "ABCDEFG", "ABCDFG"};
LCD_Pin pins[13];

void LCD_TimeDelay(uint32_t count)
{
    while (count--)
    {
        __NOP();
    }
}


void init_lcd() {

	slcd_config_t config;
	slcd_clock_config_t clkConfig = {kSLCD_AlternateClk1, kSLCD_AltClkDivFactor1, kSLCD_ClkPrescaler01, false};

	/* Hardware initialize. */
	BOARD_InitPins();
	BOARD_BootClockRUN();

	/* SLCD get default configure. */
	/*
	 * config.displayMode = kSLCD_NormalMode;
	 * config.powerSupply = kSLCD_InternalVll3UseChargePump;
	 * config.voltageTrim = kSLCD_RegulatedVolatgeTrim08;
	 * config.lowPowerBehavior = kSLCD_EnabledInWaitStop;
	 * config.frameFreqIntEnable = false;
	 * config.faultConfig = NULL;
	 */
	SLCD_GetDefaultConfig(&config);

	/* Verify and Complete the configuration structure. */
	config.clkConfig = &clkConfig;
	config.loadAdjust = kSLCD_HighLoadOrSlowestClkSrc;
	config.dutyCycle = kSLCD_1Div4DutyCycle;
	config.slcdLowPinEnabled = 0x000e0d80U;  /* LCD_P19/P18/P17/P11/P10/P8/P7. */
	config.slcdHighPinEnabled = 0x00300160U; /* LCD_P53/P52/P40/P38/P37. */
	config.backPlaneLowPin = 0x000c0000U;    /* LCD_P19/P18 --> b19/b18. */
	config.backPlaneHighPin = 0x00100100U;   /* LCD_P52/P40 --> b20/b8. */

	config.faultConfig = NULL;
	/* SLCD Initialize. */
	SLCD_Init(LCD, &config);

	// initialize the p
	pins[0] = (LCD_Pin) {0};
	pins[1] = (LCD_Pin) { .mcu_pin=40, .waveForm=kSLCD_PhaseAActivate, .isBackPlane=1 }; /* SLCD COM1 --- LCD_P40. */
	pins[2] = (LCD_Pin) { .mcu_pin=52, .waveForm=kSLCD_PhaseBActivate, .isBackPlane=1 }; /* SLCD COM2 --- LCD_P52. */
	pins[3] = (LCD_Pin) { .mcu_pin=19, .waveForm=kSLCD_PhaseCActivate, .isBackPlane=1 }; /* SLCD COM3 --- LCD_P19. */
	pins[4] = (LCD_Pin) { .mcu_pin=18, .waveForm=kSLCD_PhaseDActivate, .isBackPlane=1 }; /* SLCD COM4 --- LCD_P18. */
	pins[5] = (LCD_Pin) { .mcu_pin=37, .waveForm=0, .isBackPlane=0 }; /* SLCD P05 --- LCD_P37. */
	pins[6] = (LCD_Pin) { .mcu_pin=17, .waveForm=0, .isBackPlane=0 }; /* SLCD P06 --- LCD_P17. */
	pins[7] = (LCD_Pin) { .mcu_pin=7, .waveForm=0, .isBackPlane=0 }; /* SLCD P07 --- LCD_P7. */
	pins[8] = (LCD_Pin) { .mcu_pin=8, .waveForm=0, .isBackPlane=0 }; /* SLCD P08 --- LCD_P8. */
	pins[9] = (LCD_Pin) { .mcu_pin=53, .waveForm=0, .isBackPlane=0 }; /* SLCD P09 --- LCD_P53. */
	pins[10] = (LCD_Pin) { .mcu_pin=38, .waveForm=0, .isBackPlane=0 }; /* SLCD P10 --- LCD_P38. */
	pins[11] = (LCD_Pin) { .mcu_pin=10, .waveForm=0, .isBackPlane=0 }; /* SLCD P11 --- LCD_P10. */
	pins[12] = (LCD_Pin) { .mcu_pin=11, .waveForm=0, .isBackPlane=0 }; /* SLCD P12 --- LCD_P11. */

	/* Set SLCD back plane phase. */
	SLCD_SetBackPlanePhase(LCD, 40, kSLCD_PhaseAActivate); /* SLCD COM1 --- LCD_P40. */
	SLCD_SetBackPlanePhase(LCD, 52, kSLCD_PhaseBActivate); /* SLCD COM2 --- LCD_P52. */
	SLCD_SetBackPlanePhase(LCD, 19, kSLCD_PhaseCActivate); /* SLCD COM3 --- LCD_P19. */
	SLCD_SetBackPlanePhase(LCD, 18, kSLCD_PhaseDActivate); /* SLCD COM4 --- LCD_P18. */
    SLCD_StartDisplay(LCD);
}


// dig_num is the digit number ranging from 1 to 4
// seg_char is letter corresponding to the desired segment ranging from A-F, P for decimal point and N for colon
void segmentToPinNumber(int dig_num, char seg_char, int* pin_num, uint8_t* waveForm) {
	*pin_num = 5 + 2*(dig_num-1);  // See LCD-S401M16KR datasheet for segment to pin conversions
	if(seg_char == 'A' || seg_char == 'B' || seg_char == 'C' || seg_char == 'P') {
		*pin_num += 1;
	}
	if(seg_char == 'N'){
		*pin_num = 12;
	}

	if(seg_char == 'A' || seg_char == 'F') {
		*waveForm = kSLCD_PhaseDActivate;
	} else if(seg_char == 'B' || seg_char == 'G') {
		*waveForm = kSLCD_PhaseCActivate;
	} else if(seg_char == 'C' || seg_char == 'E') {
		*waveForm = kSLCD_PhaseBActivate;
	} else {
		*waveForm = kSLCD_PhaseAActivate;
	}
}


// disp_num is the display digit number ranging from 1 to 4
// seg_char is letter corresponding to the desired segment ranging from A-F, P for decimal point and N for colon
void turnOnSegment(int disp_num, char seg_char) {
	uint8_t waveForm;
	int pin_num;
	segmentToPinNumber(disp_num, seg_char, &pin_num, &waveForm);
	pins[pin_num].waveForm |= waveForm;
	SLCD_SetFrontPlaneSegments(LCD, pins[pin_num].mcu_pin, pins[pin_num].waveForm);
}

// disp_num is the display digit number ranging from 1 to 4
// seg_char is letter corresponding to the desired segment ranging from A-F, P for decimal point and N for colon
void turnOffSegment(int disp_num, char seg_char) {
	uint8_t waveForm;
	int pin_num;
	segmentToPinNumber(disp_num, seg_char, &pin_num, &waveForm);
	pins[pin_num].waveForm &= ~waveForm;
	SLCD_SetFrontPlaneSegments(LCD, pins[pin_num].mcu_pin, pins[pin_num].waveForm);
}


// clears the specified display
// disp_num is the display digit number ranging from 1 to 4
void clearDigit(int disp_num) {
	int clear_val = 8;  // as 8 uses all segments, turn off all segments in 8 to clear the digit
	int idx = 0;
	while(segments[clear_val][idx] != 0) {
		turnOffSegment(disp_num, segments[clear_val][idx]);
		idx++;
	}
}


// displays the digit at the specified display.
// Any number currently displayed will be erased before the new one is written
// disp_num is the display digit number ranging from 1 to 4
// digit is the digit to be displayed
void displayDigit(int disp_num, int digit) {
	clearDigit(disp_num);
	int idx = 0;
	while(segments[digit][idx] != 0) {
		turnOnSegment(disp_num, segments[digit][idx]);
		idx++;
	}
}


// displays the specified decimal point
void displayDecimalPoint(int dp_num) {
	turnOnSegment(dp_num, 'P');
}


// clears the specified decimal
void clearDecimalPoint(int dp_num) {
	turnOffSegment(dp_num, 'P');
}


// displays the colon
void displayColon() {
	turnOnSegment(4, 'N');
}


// clears the colon
void clearColon() {
	turnOffSegment(4, 'N');
}


// clears the entire display
void clearDisplay() {
	for(int disp_num = 1; disp_num <= 4; disp_num++) {
		clearDigit(disp_num);
	}
	for(int dp_num = 1; dp_num <= 3; dp_num++) {
		clearDecimalPoint(dp_num);
	}
	clearColon();
}

