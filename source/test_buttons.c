// test_buttons.c

/* Expected behavior:
 * The current stable button state is displayed 
 *  using external LEDs:
 *     SW1  = RED = LED 0 
 *     SW3  = GREEN = LED 1
 *     BOTH = BLUE = LED 2
 *     NONE = LEDs OFF
 *
 * a button pressed event triggers a short 
 *  flash of the onboard FRDM green LED
*/

#include "buttons.h"
#include "3140_led.h"
#include "timer.h"
#include "leds.h"
#include "system_init.h"
#include "display.h"

int main(void){
    // initialize LEDs, timer system, buttons
	System_Init();
	Display_Clear();

	Leds_AllOff();
	Timer_DelayMs(500);
	Leds_AllOff();

    while (1){
        // variable to store current debounced (stable) button state
        button_state_t stable;

        // variable to store button press event
        button_state_t event;

        // update button state
        Buttons_Update();

        // read current stable button state
        stable = Buttons_GetStableState();

        // read press event
        event = Buttons_GetPressedEvent();

        // turn off all LEDs
        Leds_AllOff();

        // if only SW1 is pressed, show red
        if (stable == BUTTON_SW1) {
        	Leds_ShowColor(GAME_COLOR_RED);

        // if only SW3 pressed, show green
        } else if (stable == BUTTON_SW3) {
        	Leds_ShowColor(GAME_COLOR_GREEN);

        // if both buttons pressed, show blue
        } else if (stable == BUTTON_BOTH) {
        	Leds_ShowColor(GAME_COLOR_BLUE);
        }

        // if a button is pressed
        if (event != BUTTON_NONE) {
            // the green onboard LED should blink
        	green_on_frdm();
            Timer_DelayMs(150);
            green_off_frdm();
        }

        // delay to control sampling rate of buttons
        Timer_DelayMs(20);
    }

    return 0;
}
