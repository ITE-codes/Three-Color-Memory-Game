// buttons.h

#ifndef BUTTONS_H_
#define BUTTONS_H_

// include boolean type and integer types
#include <stdbool.h>
#include <stdint.h>

// define all button states
typedef enum {
    // no buttons pressed
    BUTTON_NONE = 0,
    // only SW1 pressed
    BUTTON_SW1  = 1,
    // only SW3 pressed
    BUTTON_SW3  = 2,
    // both SW1 and SW3 pressed at same time
    BUTTON_BOTH = 3
} button_state_t;

// initialize GPIO for SW1 and SW3
void Buttons_Init(void);

// call periodically to update debounced button state
// updates button state
// detects stable state changes and press events
void Buttons_Update(void);

// raw physical button reads
bool Buttons_ReadSW1(void); // true if SW1 pressed
bool Buttons_ReadSW3(void); // true if SW3 pressed

// returns combined raw state of both buttons' state
button_state_t Buttons_ReadRaw(void);

// returns current stable button state
button_state_t Buttons_GetStableState(void);

// returns a button pressed event
// triggers when button transitions from not pressed to pressed
// automatically clears after read
// return BUTTON_NONE if no event
button_state_t Buttons_GetPressedEvent(void);

// helper functions on stable state
bool Buttons_IsSW1Pressed(void);
bool Buttons_IsSW3Pressed(void);
bool Buttons_IsBothPressed(void);

// clear any pending press event
void Buttons_ClearPressedEvent(void);

#endif /* BUTTONS_H_ */
