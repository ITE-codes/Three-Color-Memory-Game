// test_display.c

#include "display.h"
#include "buttons.h"
#include "timer.h"
#include "3140_led.h"
#include "system_init.h"

int main(void) {
    button_state_t event;

    // initialize system
    System_Init();
    Display_ResetCounter();

    while (1) {
        // update button state
        Buttons_Update();

        // read press event
        event = Buttons_GetPressedEvent();

        // if SW1, SW3, or both are pressed, increment LCD counter
        if (event != BUTTON_NONE) {
            Display_IncrementCounter();

            // small visual confirmation blink of green LED
            green_on_frdm();
            Timer_DelayMs(80u);
            green_off_frdm();
        }

        // small delay
        Timer_DelayMs(20u);
    }

    return 0;
}
