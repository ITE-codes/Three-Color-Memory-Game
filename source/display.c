// display.c

#include "display.h"
#include "lcd.h"
#include "timer.h"
#include "leds.h"

/*
 * Store current LCD counter value.
 */
static uint32_t g_display_counter = 1u;

/*
 * Initialize LCD and show starting value 1.
 */
void Display_Init(void) {
    init_lcd();

    Leds_AllOff();
    Timer_DelayMs(10u);
    Leds_AllOff();
    Timer_DelayMs(10u);
    Leds_AllOff();

    g_display_counter = 1u;

    Display_ShowNumber(g_display_counter);
}

/*
 * Clear entire LCD.
 */
void Display_Clear(void) {
    clearDisplay();
}

/*
 * Show number on the 4-digit LCD.
 *
 * This supports 0 to 9999.
 */
void Display_ShowNumber(uint32_t number) {
    uint32_t thousands;
    uint32_t hundreds;
    uint32_t tens;
    uint32_t ones;

    clearDisplay();

    if (number > 9999u) {
        number = 9999u;
    }

    thousands = number / 1000u;
    hundreds  = (number / 100u) % 10u;
    tens      = (number / 10u) % 10u;
    ones      = number % 10u;

    if (number >= 1000u) {
        displayDigit(1, (int)thousands);
    }

    if (number >= 100u) {
        displayDigit(2, (int)hundreds);
    }

    if (number >= 10u) {
        displayDigit(3, (int)tens);
    }

    displayDigit(4, (int)ones);
}

/*
 * Increment counter and update LCD.
 */
void Display_IncrementCounter(void) {
    g_display_counter++;

    Display_ShowNumber(g_display_counter);
}

/*
 * Return current counter value.
 */
uint32_t Display_GetCounter(void) {
    return g_display_counter;
}

/*
 * Reset counter to 1 and update LCD.
 */
void Display_ResetCounter(void) {
    g_display_counter = 1u;

    Display_ShowNumber(g_display_counter);
}