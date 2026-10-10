// test_game.c

#include <stdint.h>
#include "game.h"
#include "3140_led.h"
#include "timer.h"
#include "system_init.h"
#include "display.h"

/*
 * This test does two things:
 * 1. Verifies basic initialization through the public getters.
 * 2. Starts the real game FSM so the rest can be tested interactively.
 *
 * PASS pattern before game starts:
 *   FRDM green flashes 3 times
 *
 * FAIL pattern before game starts:
 *   FRDM red flashes forever
 *
 * After the pass pattern, Game_Run() starts and you can test:
 * - idle/start behavior
 * - sequence display
 * - correct input progression
 * - incorrect input -> game over
 * - timeout -> game over
 */

 // display the pass pattern
 // the onboard green LED blinks 3 times
static void pass_pattern(void) {
    int i;
    for (i = 0; i < 3; i++) {
        green_on_frdm();
        Timer_DelayMs(150);
        green_off_frdm();
        Timer_DelayMs(120);
    }
}

// display the fail pattern 
// the onboard RED link blinks
static void fail_pattern(void) {
    while (1) {
        red_on_frdm();
        Timer_DelayMs(200);
        red_off_frdm();
        Timer_DelayMs(150);
    }
}

int main(void) {
    // flag to track whether initialization tests pass
    // 1 = pass
    uint8_t ok = 1;

    // initialize system
    System_Init();
    Display_Clear();

    // initialize game module
    Game_Init();

    // check that initial game state is IDLE
    if (Game_GetState() != GAME_IDLE) {
        ok = 0;
    }

    // check that initial round number is 0
    if (Game_GetRound() != 0) {
        ok = 0;
    }

    // check that initial sequence length is 0
    if (Game_GetSequenceLength() != 0) {
        ok = 0u;
    }

    // if any test failed show fail pattern
    if (!ok) {
        fail_pattern();
    }

    // else all tests passed, show pass pattern
    pass_pattern();

    // delay before the game begins
    Timer_DelayMs(400);

    /*
     * Start the game FSM.
     * From here, behavior must be verified interactively:
     * - sequence display
     * - button input handling
     * - success progression
     * - failure conditions (wrong input / timeout)
     */
    Game_Run();

    while (1) {
    }

    return 0;
}
