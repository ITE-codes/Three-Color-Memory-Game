// system_init.c

#include "system_init.h"

#include "display.h"
#include "timer.h"
#include "leds.h"
#include "buttons.h"

/*
 * System-wide initialization function
 * Ensures ALL modules run under the same conditions (48 MHz)
 */
void System_Init(void) {
    /*
     * Initialize LEDs first.
     * This may set the board to 15 MHz.
     */
    Leds_Init();

    /*
     * Initialize LCD second.
     * This changes board clock to 48 MHz.
     */
    Display_Init();

    /*
     * Initialize timer after final clock setup.
     */
    Timer_Init();

    /*
     * Clear LEDs using 48 MHz serial LED routine.
     */
    Leds_AllOff();

    /*
     * Initialize buttons last.
     */
    Buttons_Init();
}
