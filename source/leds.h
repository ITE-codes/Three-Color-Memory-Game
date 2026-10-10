// leds.h

#ifndef LEDS_H_
#define LEDS_H_

#include <stdint.h>
#include <stdbool.h>
#include "buttons.h"
#include "3140_led.h"

/*
 * Three game colors used by the memory game
 */
typedef enum {
    GAME_COLOR_NONE = 0,
    GAME_COLOR_RED, // external LED0
    GAME_COLOR_GREEN, // external LED1
    GAME_COLOR_BLUE // external LED2
} game_color_t;

// set LED for 48MHz timing (to work with LCD clock config)
void set_led_48MHz(grb32_t rgb_val);

/*
 * Initialize LED module / underlying board LEDs
 */
void Leds_Init(void);

/*
 * Turn all game LEDs off
 */
void Leds_AllOff(void);

/*
 * Show exactly one game color
 */
void Leds_ShowColor(game_color_t color);

/*
 * Turn the given color on, wait, then turn everything off
 */
void Leds_FlashColor(game_color_t color, uint32_t on_ms, uint32_t off_ms);

/*
 * Show a whole sequence of colors
 */
void Leds_PlaySequence(const game_color_t *sequence, uint32_t length);

/*
 * Success/Failure Feedback Patterns
 */
void Leds_ShowSuccess(void);
void Leds_ShowFailure(void);

/*
 * Convert a button event/state into a game color
 * Assumed mapping:
 *   SW1       = RED
 *   SW3       = GREEN
 *   SW1+SW3   = BLUE
 */
game_color_t Leds_ButtonToColor(button_state_t button_state);

/*
 * Optional helper for displaying a user's pressed color briefly
 */
void Leds_ShowUserInput(game_color_t color);

#endif /* LEDS_H_ */