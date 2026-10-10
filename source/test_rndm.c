/* test_rndm.c
 *
 * HOW THIS TEST FILE OPERATES:
 *
 * 1. This file contains its own main() function.
 *    That means it should be the ONLY active file with main() in the project
 *    when you want to run random-generator tests on the board.
 *
 * 2. The file runs a sequence of numbered tests one at a time.
 *    Before each test begins, the onboard FRDM green LED blinks a number of
 *    times equal to the test number.
 *    Example:
 *      - 1 green blink = Test 1 is starting
 *
 * 3. After each test finishes:
 *      - 2 green onboard blinks = PASS
 *      - 2 red onboard blinks = FAIL
 *
 * 4. At the very end:
 *      - 5 green onboard blinks = all tests passed
 *      - 5 red onboard blinks = at least one test failed
 *
 * TEST CASES SUMMARY:
 *
 * Test 1:
 *   Name: Seed Zero Becomes Valid State
 *   Purpose: verify Rndm_Init(0) does not leave the generator stuck at 0
 *   Expected behavior: After seeding with 0, calling Rndm_Next() should return
 *   a nonzero changing value
 *
 * Test 2:
 *   Name: Same Seed Repeats Same Sequence
 *   Purpose: verify deterministic behavior for a fixed seed
 *   Expected behavior: Using the same seed twice should produce the same first
 *   random value
 *
 * Test 3:
 *   Name: Next Changes State
 *   Purpose: verify consecutive calls to Rndm_Next() update internal state
 *   Expected behavior: Two consecutive Rndm_Next() results should differ
 *
 * Test 4:
 *   Name: Range Zero Returns Zero
 *   Purpose: verify safe handling of max_exclusive == 0.
 *   Expected behavior: Rndm_Range(0) returns 0
 *
 * Test 5:
 *   Name: Range One Always Returns Zero
 *   Purpose: verify modulo behavior for max_exclusive == 1
 *   Expected behavior: Rndm_Range(1) always returns 0
 *
 * Test 6:
 *   Name: Range Bound Check
 *   Purpose: verify Rndm_Range(max) always stays within valid range.
 *   Expected behavior: Repeated values are always less than max_exclusive
 *
 * Test 7:
 *   Name: NextColor Returns Valid Color
 *   Purpose: verify Rndm_NextColor() only returns red, green, or blue
 *   Expected behavior: No invalid color values are returned
 *
 * Test 8:
 *   Name: Visual Random Color Display
 *   Purpose: display several random colors on the external LEDs
 *   Expected behavior: External LED should show only red, green, or blue
 *
 * Test 9:
 *   Name: Same Seed Repeats Same Color
 *   Purpose: verify deterministic color sequence for same seed
 *   Expected behavior: Resetting to same seed should give same first color
 */


#include "rndm.h"
#include "leds.h"
#include "timer.h"
#include "3140_led.h"
#include "system_init.h"
#include "display.h"

#include <stdint.h>
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

// amount of time to hold a generated color visibly on the external LED
#define VISUAL_HOLD_MS 700

// HELPER FUNCTIONS ----------------------------------------------------

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

// turn all LEDs off and pause
static void ResetAllLEDs(void);

// check if a color is one of the valid game colors
static bool IsValidGameColor(game_color_t color);

// TEST CASES -------------------------------------------------

// Test 1: verify seed 0 is corrected to a usable internal state
static bool Test01_SeedZeroBecomesValidState(void);

// Test 2: verify same seed gives same first raw random value
static bool Test02_SameSeedRepeatsSameSequence(void);

// Test 3: verify consecutive random values are not identical
static bool Test03_NextChangesState(void);

// Test 4: verify range(0) safely returns 0
static bool Test04_RangeZeroReturnsZero(void);

// Test 5: verify range(1) always returns 0
static bool Test05_RangeOneAlwaysReturnsZero(void);

// Test 6: verify range(max) always stays below max
static bool Test06_RangeBoundCheck(void);

// Test 7: verify NextColor only returns valid game colors
static bool Test07_NextColorReturnsValidColor(void);

// Test 8: visually display several random colors on external LED
static bool Test08_VisualRandomColorDisplay(void);

// Test 9: verify same seed gives same first random color
static bool Test09_SameSeedRepeatsSameColor(void);

//----------------------------------------------------------
// Helper implementations
//----------------------------------------------------------

// blink the onboard FRDM green LED X times
static void BlinkGreenFRDM(uint32_t times, uint32_t on_ms, uint32_t off_ms){
    // loop variable for repeated blinks
    uint32_t i;
    // repeat blink pattern X=times
    for (i = 0; i < times; i++) {
        // turn green onboard LED on
        green_on_frdm();
        // keep it on
        Timer_DelayMs(on_ms);
        // turn green onboard LED off
        green_off_frdm();
        // keep it off
        Timer_DelayMs(off_ms);
    }
}

// blink the onboard FRDM red LED X times
static void BlinkRedFRDM(uint32_t times, uint32_t on_ms, uint32_t off_ms){
    uint32_t i;
    for (i = 0; i < times; i++) {
        // turn red onboard LED on
        red_on_frdm();
        // keep it on
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

// show PASS indication
static void ShowPass(void){
    // blink green onboard LED twice
    BlinkGreenFRDM(2, RESULT_ON_MS, RESULT_OFF_MS);
    // pause before next test
    Timer_DelayMs(BETWEEN_TESTS_MS);
}

// show FAIL indication
static void ShowFail(void){
    // blink red onboard LED twice
    BlinkRedFRDM(2, RESULT_ON_MS, RESULT_OFF_MS);
    // pause before next test
    Timer_DelayMs(BETWEEN_TESTS_MS);
}

// turn all LEDs off and pause briefly
static void ResetAllLEDs(void){
    // turn off all LEDs
    Leds_AllOff();
    // small pause
    Timer_DelayMs(250);
}

// return true only if color is one of the valid random game colors
static bool IsValidGameColor(game_color_t color) {
    // accept red as valid
    if (color == GAME_COLOR_RED) {
        return true;
    }
    // accept green as valid
    if (color == GAME_COLOR_GREEN) {
        return true;
    }
    // accept blue as valid
    if (color == GAME_COLOR_BLUE) {
        return true;
    }
    // reject anything else
    return false;
}

// TEST IMPLEMENTATIONS --------------------------------------------------

// Test 1: verify that seed 0 does not leave generator unusable
static bool Test01_SeedZeroBecomesValidState(void){
    // show test number 1
    ShowTestNumber(1);
    // seed generator with 0
    Rndm_Init(0);
    // get first generated value
    uint32_t value = Rndm_Next();
    // pass if generated value is nonzero
    return (value != 0);
}

// Test 2: verify that same seed gives same first random value
static bool Test02_SameSeedRepeatsSameSequence(void){
    ShowTestNumber(2);
    Rndm_Init(1234);
    // record first generated value
    uint32_t first = Rndm_Next();
    // re-seed with the same fixed value
    Rndm_Init(1234);
    // record first generated value again
    uint32_t second = Rndm_Next();
    // pass if both values match
    return (first == second);
}

// Test 3: verify consecutive random values change internal state
static bool Test03_NextChangesState(void){
    ShowTestNumber(3);
    Rndm_Init(555);
    // get first random value
    uint32_t first = Rndm_Next();
    // get second random value
    uint32_t second = Rndm_Next();
    // pass if values are different
    return (first != second);
}

// Test 4: verify range(0) safely returns 0
static bool Test04_RangeZeroReturnsZero(void){
    ShowTestNumber(4);
    Rndm_Init(999);
    // request a value in range(0)
    // should return 0
    uint32_t value = Rndm_Range(0u);
    // pass if function returned 0
    return (value == 0);
}

// Test 5: verify range(1) always returns 0
static bool Test05_RangeOneAlwaysReturnsZero(void){
    ShowTestNumber(5);
    Rndm_Init(42);
    // loop through several calls to confirm result is always 0
    uint32_t i;
    for (i = 0; i < 10; i++) {
        // if any result is not 0, test fails
        if (Rndm_Range(1u) != 0) {
            return false;
        }
    }
    // all values 0, so test passes
    return true;
}

// Test 6: verify range(max) stays below max
static bool Test06_RangeBoundCheck(void){
    ShowTestNumber(6);
    Rndm_Init(777);
    // choose max exclusive value for testing
    uint32_t max = 7;
    // loop through multiple samples
    uint32_t i;
    for (i = 0; i < 20; i++) {
        // get ranged value
        uint32_t value = Rndm_Range(max);
        // fail if value is outside valid range
        if (value >= max) {
            return false;
        }
    }
    // all sampled values stayed within range
    return true;
}

// Test 7: verify NextColor only returns valid colors
static bool Test07_NextColorReturnsValidColor(void){
    ShowTestNumber(7);
    Rndm_Init(2468);
    // check multiple generated colors
    uint32_t i;
    for (i = 0; i < 20; i++) {
        // get next random color
        game_color_t color = Rndm_NextColor();

        // fail if color is not red, green, or blue
        if (!IsValidGameColor(color)) {
            return false;
        }
    }
    // all generated colors were valid
    return true;
}

// Test 8: visually display several random colors on the external LED
static bool Test08_VisualRandomColorDisplay(void){
    ShowTestNumber(8);
    Rndm_Init(1357);

    // loop through several random colors
    uint32_t i;
    for (i = 0; i < 6; i++) {
        // get next random color
        game_color_t color = Rndm_NextColor();
        // display that color on the external LED
        Leds_ShowColor(color);
        // hold it long enough to see
        Timer_DelayMs(VISUAL_HOLD_MS);
        // turn LEDs off between displayed colors
        Leds_AllOff();
        // brief pause between displays
        Timer_DelayMs(250);
    }
    return true;
}

// Test 9: verify same seed gives same first random color
static bool Test09_SameSeedRepeatsSameColor(void){
    ShowTestNumber(9);
    Rndm_Init(2024);

    // record first generated color
    game_color_t first = Rndm_NextColor();
    // re-seed with same value
    Rndm_Init(2024);
    // record first generated color again
    game_color_t second = Rndm_NextColor();
    // pass if both colors match
    return (first == second);
}

// MAIN ------------------------------------------------------

int main(void){
    // count how many tests passed
    uint32_t passed = 0;
    // total number of tests in this file
    uint32_t total = 9;

    // initialize system
    System_Init();
    ResetAllLEDs();
    Display_Clear();

    // ensure LEDs start off
    ResetAllLEDs();

    // run Test # and record pass or fail
    if (Test01_SeedZeroBecomesValidState()) { ShowPass(); passed++; } else { ShowFail(); }

    if (Test02_SameSeedRepeatsSameSequence()) { ShowPass(); passed++; } else { ShowFail(); }

    if (Test03_NextChangesState()) { ShowPass(); passed++; } else { ShowFail(); }

    if (Test04_RangeZeroReturnsZero()) { ShowPass(); passed++; } else { ShowFail(); }

    if (Test05_RangeOneAlwaysReturnsZero()) { ShowPass(); passed++; } else { ShowFail(); }

    if (Test06_RangeBoundCheck()) { ShowPass(); passed++; } else { ShowFail(); }

    if (Test07_NextColorReturnsValidColor()) { ShowPass(); passed++; } else { ShowFail(); }

    if (Test08_VisualRandomColorDisplay()) { ShowPass(); passed++; } else { ShowFail(); }

    if (Test09_SameSeedRepeatsSameColor()) { ShowPass(); passed++; } else { ShowFail(); }

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
