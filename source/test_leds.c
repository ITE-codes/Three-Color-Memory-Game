/* test_leds.c
 *
 * HOW THIS TEST FILE OPERATES:
 *
 * 1. This file contains its own main() function.
 *    That means it should be the ONLY active file with main() in the project
 *    when you want to run LED tests on the board.
 *
 * 2. The test file runs a sequence of numbered tests, one after another.
 *    Before each test begins, the onboard FRDM green LED blinks a number of
 *    times equal to the test number.
 *    For Example:
 *      - 1 green blink = Test 1 is starting
 *      - 2 green blinks = Test 2 is starting
 *
 * 3. After the test runs, the file shows a pass/fail result:
 *      - 2 green onboard blinks = PASS
 *      - 2 red onboard blinks = FAIL
 *
 * 4. At the end:
 *      - 5 green onboard blinks = all tests passed
 *      - 5 red onboard blinks = at least one test failed
 *
 * TEST CASE SUMMARY:
 *
 * Test 1:
 *   Name: Init and All Off
 *   Purpose: verify Leds_Init() and Leds_AllOff() can run and leave LEDs off
 *   Expected behavior: No LEDs should remain visibly on
 *
 * Test 2:
 *   Name: Show Red
 *   Purpose: verify Leds_ShowColor(GAME_COLOR_RED)
 *   Expected behavior: External LED shows red
 *
 * Test 3:
 *   Name: Show Green
 *   Purpose: verify Leds_ShowColor(GAME_COLOR_GREEN)
 *   Expected behavior: External LED shows green
 *
 * Test 4:
 *   Name: Show Blue
 *   Purpose: verify Leds_ShowColor(GAME_COLOR_BLUE)
 *   Expected behavior: External LED shows blue
 *
 * Test 5:
 *   Name: Show None Turns Off
 *   Purpose: verify Leds_ShowColor(GAME_COLOR_NONE) turns display off
 *   Expected behavior: External LED turns off after being on
 *
 * Test 6:
 *   Name: Flash Color Red
 *   Purpose: verify Leds_FlashColor() turns a color on, waits, then turns it off
 *   Expected behavior: Red appears briefly, then turns off.
 *
 * Test 7:
 *   Name: Play Short Sequence
 *   Purpose: verify Leds_PlaySequence() displays multiple colors in order
 *   Expected behavior: External LED shows red, green, then blue, with timing
 *
 * Test 8:
 *   Name: Show User Input Red
 *   Purpose: verify Leds_ShowUserInput() for red
 *   Expected behavior: Short red flash
 *
 * Test 9:
 *   Name: Show User Input Green
 *   Purpose: verify Leds_ShowUserInput() for green.
 *   Expected behavior: Short green flash
 *
 * Test 10:
 *   Name: Show User Input Blue
 *   Purpose: verify Leds_ShowUserInput() for blue
 *   Expected behavior: Short blue flash
 *
 * Test 11:
 *   Name: Show Success Pattern
 *   Purpose: verify Leds_ShowSuccess().
 *   Expected behavior: Onboard FRDM green LED and external green LED blink 3 times
 *
 * Test 12:
 *   Name: Show Failure Pattern
 *   Purpose: verify Leds_ShowFailure().
 *   Expected behavior: Onboard FRDM red LED and external red LED blink 3 times
 *
 * Test 13:
 *   Name: Button To Color Mapping
 *   Purpose: Verify the button-to-color conversion logic.
 *   Expected behavior:
 *      BUTTON_SW1  -> GAME_COLOR_RED
 *      BUTTON_SW3  -> GAME_COLOR_GREEN
 *      BUTTON_BOTH -> GAME_COLOR_BLUE
 *      BUTTON_NONE -> GAME_COLOR_NONE
 *
 * HOW TO USE THIS FILE:
 * - Build and flash this file to the FRDM board
 * - Watch the onboard FRDM LEDs and the external LEDs
 * - Follow the numbered test sequence
 * - Confirm whether each test's expected LED behavior occurs
 */

#include "leds.h"

// include timer helpers used for visible delays
#include "timer.h"

#include "3140_led.h"

// include system.h
#include "system_init.h"

// include display.h
#include "display.h"

// include button definitions; leds.h depends on button_state_t
#include "buttons.h"

// include integer types
#include <stdint.h>

// include boolean type
#include <stdbool.h>

//----------------------------------------------------------
// timing constants for test numbering / pass-fail
//----------------------------------------------------------

// LED on-time used when blinking the test number
#define TEST_NUM_ON_MS 300

// LED off-time used between test-number blinks
#define TEST_NUM_OFF_MS 250

// LED on-time used for pass/fail result blinks
#define RESULT_ON_MS 400

// LED off-time used between pass/fail result blinks
#define RESULT_OFF_MS 300

// pause between finishing one test and starting the next
#define BETWEEN_TESTS_MS 900

// how long to hold a color visibly on screen for human eye
#define VISUAL_HOLD_MS 1000

// HELPER FUNCTIONS --------------------------------------------------------------

// blink the onboard FRDM green LED X number of times
static void BlinkGreenFRDM(uint32_t times, uint32_t on_ms, uint32_t off_ms);

// blink the onboard FRDM red LED X number of times
static void BlinkRedFRDM(uint32_t times, uint32_t on_ms, uint32_t off_ms);

// display the current test number using green onboard LED blinks
static void ShowTestNumber(uint32_t test_num);

// display a pass result using 2 green onboard blinks
static void ShowPass(void);

// display a fail result using 2 red onboard blinks
static void ShowFail(void);

// turn all LEDs off and pause briefly
static void ResetAllLEDs(void);

// TEST CASES ----------------------------------------------------------------------

// Test 1: verify init + all-off behavior
static bool Test01_InitAndAllOff(void);

// Test 2: verify showing red on external LED
static bool Test02_ShowRed(void);

// Test 3: verify showing green on external LED
static bool Test03_ShowGreen(void);

// Test 4: verify showing blue on external LED
static bool Test04_ShowBlue(void);

// Test 5: verify showing none/off state
static bool Test05_ShowNoneTurnsOff(void);

// Test 6: verify flash-color behavior
static bool Test06_FlashColorRed(void);

// Test 7: verify short sequence playback
static bool Test07_PlayShortSequence(void);

// Test 8: verify short user-input red display
static bool Test08_ShowUserInputRed(void);

// Test 9: verify short user-input green display
static bool Test09_ShowUserInputGreen(void);

// Test 10: verify short user-input blue display
static bool Test10_ShowUserInputBlue(void);

// Test 11: verify success pattern
static bool Test11_ShowSuccessPattern(void);

// Test 12: verify failure pattern
static bool Test12_ShowFailurePattern(void);

// Test 13: verify button-to-color mapping logic
static bool Test13_ButtonToColorMapping(void);

//----------------------------------------------------------
// Helper implementations
//----------------------------------------------------------

// blink the onboard FRDM green LED X times
static void BlinkGreenFRDM(uint32_t times, uint32_t on_ms, uint32_t off_ms){
    // loop the requested number of blinks
    uint32_t i;

    // repeat blink pattern X=times
    for (i = 0; i < times; i++) {
        // turn green onboard LED on
        green_on_frdm();

        // keep it on for requested on-time
        Timer_DelayMs(on_ms);

        // turn green onboard LED off
        green_off_frdm();

        // keep it off for requested off-time
        Timer_DelayMs(off_ms);
    }
}

// blink the onboard FRDM red LED X times
static void BlinkRedFRDM(uint32_t times, uint32_t on_ms, uint32_t off_ms){
    // loop variable for repeated blinks
    uint32_t i;

    // Repeat blink pattern X=times
    for (i = 0u; i < times; i++) {
        // turn red onboard LED on
        red_on_frdm();

        // keep it on for requested on-time
        Timer_DelayMs(on_ms);

        // turn red onboard LED off
        red_off_frdm();

        // keep it off for requested off-time
        Timer_DelayMs(off_ms);
    }
}

// show the test number before each test begins
static void ShowTestNumber(uint32_t test_num) {
    // blink green onboard LED the same number as the test number
    BlinkGreenFRDM(test_num, TEST_NUM_ON_MS, TEST_NUM_OFF_MS);

    // pause before running the actual test
    Timer_DelayMs(600);
}

// show the PASS indication
static void ShowPass(void) {
    // blink green onboard LED twice
    BlinkGreenFRDM(2, RESULT_ON_MS, RESULT_OFF_MS);
    // pause before next test
    Timer_DelayMs(BETWEEN_TESTS_MS);
}

// show the sFAIL indication
static void ShowFail(void) {
    // blink red onboard LED twice
    BlinkRedFRDM(2, RESULT_ON_MS, RESULT_OFF_MS);
    // pause before next test
    Timer_DelayMs(BETWEEN_TESTS_MS);
}

// turn all LEDs off and pause briefly
static void ResetAllLEDs(void) {
    // turn off all project-controlled LEDs
    Leds_AllOff();
    // small pause after reset
    Timer_DelayMs(250);
}

// TEST IMPLEMENTATIONS --------------------------------------------------

// Test 1: initialize LED system and verify everything can be turned off
static bool Test01_InitAndAllOff(void){
    // show test number 1
    ShowTestNumber(1);

    // turn all LEDs off
    Leds_AllOff();

    // delay
    Timer_DelayMs(600);

    return true;
}

// Test 2: show red on the external RGB LED
static bool Test02_ShowRed(void){
    // show test number 2
    ShowTestNumber(2);

    // display red
    Leds_ShowColor(GAME_COLOR_RED);

    // hold long enough to see it
    Timer_DelayMs(VISUAL_HOLD_MS);

    // turn LEDs off afterward
    Leds_AllOff();

    return true;
}

// Test 3: show green on the external RGB LED
static bool Test03_ShowGreen(void){
    ShowTestNumber(3);
    Leds_ShowColor(GAME_COLOR_GREEN);
    Timer_DelayMs(VISUAL_HOLD_MS);
    Leds_AllOff();
    return true;
}

// Test 4: show blue on the external RGB LED
static bool Test04_ShowBlue(void){
    ShowTestNumber(4);
    Leds_ShowColor(GAME_COLOR_BLUE);
    Timer_DelayMs(VISUAL_HOLD_MS);
    Leds_AllOff();
    return true;
}

// Test 5: verify the NONE color turns LEDs off
static bool Test05_ShowNoneTurnsOff(void){
    ShowTestNumber(5);
    Leds_ShowColor(GAME_COLOR_RED);
    Timer_DelayMs(500);
    Leds_ShowColor(GAME_COLOR_NONE);
    Timer_DelayMs(800);
    return true;
}

// Test 6: verify flash-color behavior
static bool Test06_FlashColorRed(void){
    ShowTestNumber(6);
    Leds_FlashColor(GAME_COLOR_RED, 700, 500);
    return true;
}

// Test 7: verify short sequence playback
static bool Test07_PlayShortSequence(void){
    // create a short test sequence
    game_color_t seq[3];

    ShowTestNumber(7);

    // first color in sequence is red
    seq[0] = GAME_COLOR_RED;

    // second color in sequence is green
    seq[1] = GAME_COLOR_GREEN;

    // third color in sequence is blue
    seq[2] = GAME_COLOR_BLUE;

    // play the full sequence
    Leds_PlaySequence(seq, 3);

    // check order is red, green, blue
    return true;
}

// Test 8: verify brief user-input display for red
static bool Test08_ShowUserInputRed(void) {
    ShowTestNumber(8);

    // display short red user-input flash
    Leds_ShowUserInput(GAME_COLOR_RED);

    return true;
}

// Test 9: verify brief user-input display for green
static bool Test09_ShowUserInputGreen(void){
    ShowTestNumber(9);

    // display short green user-input flash
    Leds_ShowUserInput(GAME_COLOR_GREEN);

    return true;
}

// Test 10: verify brief user-input display for blue
static bool Test10_ShowUserInputBlue(void){
    ShowTestNumber(10);
    Leds_ShowUserInput(GAME_COLOR_BLUE);
    return true;
}

// Test 11: verify success pattern
static bool Test11_ShowSuccessPattern(void){
    ShowTestNumber(11);
    // run success pattern
    Leds_ShowSuccess();
    // checks onboard green + external green blink 3 times
    return true;
}

// Test 12: verify failure pattern
static bool Test12_ShowFailurePattern(void){
    ShowTestNumber(12);
    // run failure pattern
    Leds_ShowFailure();
    // checks onboard red + external red blink 3 times
    return true;
}

// Test 13: verify button-to-color mapping logic directly in code
static bool Test13_ButtonToColorMapping(void){
    ShowTestNumber(13);

    // verify SW1 maps to red
    if (Leds_ButtonToColor(BUTTON_SW1) != GAME_COLOR_RED) {
        return false;
    }

    // verify SW3 maps to green
    if (Leds_ButtonToColor(BUTTON_SW3) != GAME_COLOR_GREEN) {
        return false;
    }

    // verify BOTH maps to blue
    if (Leds_ButtonToColor(BUTTON_BOTH) != GAME_COLOR_BLUE) {
        return false;
    }

    // verify NONE maps to no color
    if (Leds_ButtonToColor(BUTTON_NONE) != GAME_COLOR_NONE) {
        return false;
    }

    // all mapping checks passed
    return true;
}

// MAIN ----------------------------------------------------------------

int main(void) {
    // count how many tests passed
    uint32_t passed = 0;

    // total number of tests in this file
    uint32_t total = 13;

    // set up system
    System_Init();
    ResetAllLEDs();
    Display_Clear();

    // run Test # and record it as a pass or fail
    if (Test01_InitAndAllOff()) { ShowPass(); passed++; } else { ShowFail(); }

    if (Test02_ShowRed()) { ShowPass(); passed++; } else { ShowFail(); }

    if (Test03_ShowGreen()) { ShowPass(); passed++; } else { ShowFail(); }

    if (Test04_ShowBlue()) { ShowPass(); passed++; } else { ShowFail(); }

    if (Test05_ShowNoneTurnsOff()) { ShowPass(); passed++; } else { ShowFail(); }

    if (Test06_FlashColorRed()) { ShowPass(); passed++; } else { ShowFail(); }

    if (Test07_PlayShortSequence()) { ShowPass(); passed++; } else { ShowFail(); }

    if (Test08_ShowUserInputRed()) { ShowPass(); passed++; } else { ShowFail(); }

    if (Test09_ShowUserInputGreen()) { ShowPass(); passed++; } else { ShowFail(); }

    if (Test10_ShowUserInputBlue()) { ShowPass(); passed++; } else { ShowFail(); }

    if (Test11_ShowSuccessPattern()) { ShowPass(); passed++; } else { ShowFail(); }

    if (Test12_ShowFailurePattern()) { ShowPass(); passed++; } else { ShowFail(); }

    if (Test13_ButtonToColorMapping()) { ShowPass(); passed++; } else { ShowFail(); }

    // if all tests passed
    if (passed == total) {
        // blink green onboard LED 5 times
        BlinkGreenFRDM(5, 450, 350);
    } else {
        // otherwise blink red onboard LED 5 times
        BlinkRedFRDM(5, 450, 350);
    }

    // infinite loop
    while (1) {
        // keep LEDs off while idle
        ResetAllLEDs();
        // idle delay
        Timer_DelayMs(700);
    }
}
