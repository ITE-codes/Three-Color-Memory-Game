// test_timer.c

/*
 * This file tests both the timing helper functions 
 * and the internal software counter logic of the timer module
 *
 * Part 1: Visually verifies timing delays using LEDs 
 * Part 2: Verifies counter behavior and timeout logic 
 *
 * Final result:
 *   - Green LED blinking pattern = all tests passed
 *   - Red LED blinking pattern = at least one test failed
*/

#include <stdbool.h>
#include "timer.h"
#include "3140_led.h"
#include "leds.h"
#include "system_init.h"
#include "display.h"


// show a visible pass pattern using the onboard green LED
static void pass_pattern(void) {
    // loop counter
    int i;
    // blink green onboard LED 3 times
    for (i = 0; i < 3; i++) {
        green_on_frdm();
        Timer_DelayMs(150);
        green_off_frdm();
        Timer_DelayMs(100);
    }
}

// show a visible fail pattern using the onboard red LED
static void fail_pattern(void) {
    // loop counter
    int i;
    // blink red onboard LED 3 times
    for (i = 0; i < 3; i++) {
        red_on_frdm();
        Timer_DelayMs(150);
        red_off_frdm();
        Timer_DelayMs(100);
    }
}

int main(void) {
    // tracks whether all timer tests pass
    bool ok = true;

    // initialize system
    System_Init();
    Display_Clear();

    /* ---------------------------
     * Part 1: visible wait tests
     * ---------------------------
     */

    // show color using color-on timing helper
    Leds_ShowColor(GAME_COLOR_RED);
    // should be a brief flash/blink
    Timer_WaitColorOn();
    Leds_AllOff();

    // show color using wait between colors function
    Leds_ShowColor(GAME_COLOR_BLUE);
    // should be slightly longer wait than previous
    // so that the human eye can register the color
    Timer_WaitBetweenColors();
    Leds_AllOff();

    // show color using wait after success function
    Leds_ShowColor(GAME_COLOR_GREEN);
    // should be longer than previous two functions
    Timer_WaitAfterSuccess();
    Leds_AllOff();

    // turn onboard red LED for wait after failure function
    red_on_frdm();
    Timer_WaitAfterFailure();
    red_off_frdm();

    // turn onboard green LED for next level wait function
    green_on_frdm();
    Timer_WaitNextLevel();
    green_off_frdm();

    // delay between testing parts
    Timer_DelayMs(500);

    /* ---------------------------
     * Part 2: counter logic tests
     * ---------------------------
     */

    // reset timer counter
    Timer_ResetCounter();
    // verify counter reset to zero
    if (Timer_GetCounterMs() != 0) {
        // else mark test as failed
        ok = false;
    }

    // advance time counter by 100 ms
    Timer_UpdateCounter(100);
    // verify time counter = 100 ms
    if (Timer_GetCounterMs() != 100) {
        // mark test as failed
        ok = false;
    }

    // verify timeout has not occurred yet
    if (Timer_HasTimedOut(0, 150)) {
        // should still be false at 100 ms
        ok = false;   
    }

    // advance counter by 60 ms, making total 160 ms
    Timer_UpdateCounter(60); 
    if (!Timer_HasTimedOut(0, 150)) {
        // should now be true (else mark test failed)
        ok = false;   
    }

    // reset software timer counter again
    Timer_ResetCounter();

    // advance counter to input-start timeout amount
    Timer_UpdateCounter(4500);

    // verify input-start timeout is detected
    if (!Timer_HasTimedOut(0, TIMER_INPUT_START_TIMEOUT_MS)) {
        ok = false;
    }

    // repeat final visible result forever
    while (1) {
        // if all checks passed
        if (ok) {
            // show pass blink pattern 
            // onboard green LED blinks 3 times
            pass_pattern();
        } else {
            // otherwise at least one check failed
            // show fail blink pattern = red onboard LEd
            fail_pattern();
        }

        // brief delay
        Timer_DelayMs(800);
    }

    return 0;
}
