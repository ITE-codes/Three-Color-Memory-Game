// leds.c

#include "leds.h"
#include "3140_led.h"
#include "timer.h"

// LED1 and LED2 are serial LEDs.
// LED0 is the direct GPIO RGB LED.

// LED/Color mapping:
//   RED   = LED0 
//   GREEN = LED1 
//   BLUE  = LED2 

// define number of serial LEDs in the LED chain
#define SERIAL_LED_COUNT 2

// define brightness value used for serial RGB LEDs
#define LED_BRIGHTNESS   40

// Serial LED Color Helper Functions

// define an off color packet for serial LEDs
static grb32_t COLOR_OFF = {0, 0, 0, 0};

// create a GRB color packet for LED1/LED2
static grb32_t MakeColor(uint8_t red, uint8_t green, uint8_t blue) {
    // local color packet
    grb32_t color;
    
    // stores blue brightness value
    color.blue = blue;
    // stores red brightness value
    color.red = red;
    // stores green brightness value
    color.green = green;
    
    // clear unused filler ield
    color.__filler = 0;

    // return color packet
    return color;
}

/*
 * Set LED1 and LED2 using the serial LED chain
 */
static void Leds_SetSerialLEDs(grb32_t led1_color, grb32_t led2_color) {
    // send color packet to LED1
    set_led_48MHz(led1_color);
    // send color packet to LED2
    set_led_48MHz(led2_color);
    // delay to allow serial LED update to finish
    Timer_DelayMs(1u);
}

// turn off all external RGB LEDs
static void Leds_ExternalAllOff(void) {
    // turn off external LED0 color channels
    red_off();
    green_off();
    blue_off();
    // turn of serial LED1
    set_led_48MHz(COLOR_OFF);
    // turn of serial LED2
    set_led_48MHz(COLOR_OFF);
    // delay to allow update
    Timer_DelayMs(1u);
}

// turn off all external and onboard LEDs
void Leds_AllOff(void) {
    Leds_ExternalAllOff();
    // turn of red and green onboard LEDs
    red_off_frdm();
    green_off_frdm();
}

// initialize LED hardware and clear all LEDs
void Leds_Init(void) {
    led_init();
    Leds_AllOff();
}

// show exactly one game color on the external LEDs
void Leds_ShowColor(game_color_t color) {
    // clear LED outputs
    Leds_AllOff();

    // choose which LED/color to display
    switch (color) {
        // show red on external LED 0
        case GAME_COLOR_RED:
            // turn on external LED 0 red channel
            red_on();
            break;

        // show green using external LED 1
        case GAME_COLOR_GREEN:
            // set LED 1 = green
            Leds_SetSerialLEDs(
                // set green
                MakeColor(0, LED_BRIGHTNESS, 0),
                // turn off LED 2
                COLOR_OFF
            );
            break;

        // show blue using external LED 2
        case GAME_COLOR_BLUE:
            Leds_SetSerialLEDs(
                // turn off LED 1
                COLOR_OFF,
                // set blue
                MakeColor(0, 0, LED_BRIGHTNESS)
            );
            break;

        // if in a no color state
        case GAME_COLOR_NONE:
        default:
            // do nothing (leave LEDs off)
            break;
    }
}

/*
 * Flash a color ON for on_ms milliseconds, 
 * then OFF for off_ms milliseconds
 */
void Leds_FlashColor(game_color_t color, uint32_t on_ms, uint32_t off_ms) {
    // show color
    Leds_ShowColor(color);
    // keep color on for a time
    Timer_DelayMs(on_ms);
    // turn all leds off
    Leds_AllOff();
    // wait/delay
    Timer_DelayMs(off_ms);
}

/*
 * Play a full sequence of colors (used during "show sequence" phase)
 *
 * For each color:
 * 1. turn it on
 * 2. wait (visible duration)
 * 3. turn it off
 * 4. wait between colors
 */
void Leds_PlaySequence(const game_color_t *sequence, uint32_t length) {
    // loop counter/index for sequence position
    uint32_t i;

    // guard against null pointer
    if (sequence == 0) {
        return;
    }

    // loop through each color in the sequence
    for (i = 0; i < length; i++) {
        // show current sequence color
        Leds_ShowColor(sequence[i]);
        // keep the color on/visible
        Timer_WaitColorOn();     
        // turn LEDs off   
        Leds_AllOff();
        // time gap between colors
        Timer_WaitBetweenColors();  
    }
}

/*
 * Show success feedback:
 * flash green LEDs (both FRDM + external) multiple times
 */
void Leds_ShowSuccess(void) {
    // loop counter/index
    uint32_t i;

    // blink success pattern three times
    for (i = 0; i < 3; i++) {
        // turn on green onboard LED
        green_on_frdm();

        // turn on external LED 1 = green
        Leds_ShowColor(GAME_COLOR_GREEN);

        // keep LEDs on briefly
        Timer_DelayMs(200);

        // turn off onboard green LED
        green_off_frdm();
        
        // turn off LEDs
        Leds_AllOff();
        
        // short delay for LEDs to update
        Timer_DelayMs(1u);
        
        // pause between blinks
        Timer_DelayMs(150);
    }
}

/*
 * Show failure feedback:
 * flash red LEDs (both FRDM + external) multiple times
 */
void Leds_ShowFailure(void) {
    // loop counter
    uint32_t i;

    // blink failure pattern three times
    for (i = 0; i < 3; i++) {
        // turn on red onboard LED
        red_on_frdm();
        // turn on external LED 0 =  red
        red_on();

        // keep LEDs on briefly
        Timer_DelayMs(250);

        // turn off onboard and external red LEDs
        red_off_frdm();
        red_off();

        // delay between blinks
        Timer_DelayMs(150);
    }
}

/*
 * Convert button input into a game color
 *
 * Mapping:
 * SW1 = RED
 * SW3 = GREEN
 * BOTH = BLUE
 * NONE = no input
 *
 * This is how we get 3 colors using only 2 buttons
 */
game_color_t Leds_ButtonToColor(button_state_t button_state) {
    // choose color based on button input
    switch (button_state) {
        // map SW1 to red
        case BUTTON_SW1:
            return GAME_COLOR_RED;

        // map SW3 to green
        case BUTTON_SW3:
            return GAME_COLOR_GREEN;

        // map both buttons to blue
        case BUTTON_BOTH:
            return GAME_COLOR_BLUE;

        // no buttons pressed maps to no color
        case BUTTON_NONE:
        default:
            return GAME_COLOR_NONE;
    }
}

/*
 * Briefly show the color corresponding to user input
 * Used as immediate visual feedback when user presses buttons
 */
void Leds_ShowUserInput(game_color_t color) {
    // ignore invalid/no input
    if (color == GAME_COLOR_NONE) {
        return;
    }

    // show selected color
    Leds_ShowColor(color);

    // keep selected color on briefly
    Timer_DelayMs(200);

    // turn LEDs off
    Leds_AllOff();
}
