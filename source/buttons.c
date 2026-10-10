// buttons.c

#include "buttons.h"
#include "MKL46Z4.h"

// FRDM-KL46Z user buttons from MKL46Z4.h
// SW1 = PTC3
// SW3 = PTC12
// they are active-low inputs

// define which PORT register controls SW1
#define SW1_PORT     PORTC
// define which GPIO register controls SW1 pin data
#define SW1_GPIO     PTC
// define SW1 pin number on PORTC
#define SW1_PIN      3

// define which PORT register controls SW3
#define SW3_PORT     PORTC
// define which GPIO register controls SW3
#define SW3_GPIO     PTC
// define SW3 pin number on PORTC
#define SW3_PIN      12

// number of consecutive identical readings required
// before accepting a stable button state
#define BUTTONS_DEBOUNCE_TICKS  4

// INTERNAL STATE VARIABLES --------------------------------

// stores last raw (immediate) button reading
static button_state_t g_last_raw_state = BUTTON_NONE;

// stores debounced, stable button state
static button_state_t g_stable_state = BUTTON_NONE;

// stores a one-time "press-event"
static button_state_t g_pressed_event = BUTTON_NONE;

// counter used for debounce timing
static uint8_t g_debounce_count = 0;

// LOW-LEVEL HARDWARE FUNCTIONS ----------------------------

// read SW1 directly from hardware (ACTIVE-LOW)
// uses PDIR register from MKL46Z4.h
static bool Buttons_HW_ReadSW1_Active(void){
    // read pin state:
    // (1 << SW1_PIN) creates bitmask for pin
    // PDIR holds current input value
    // 0 means pressed (active-low)
    return ((SW1_GPIO->PDIR & (1u << SW1_PIN)) == 0u);
}

// same logic as for SW1
static bool Buttons_HW_ReadSW3_Active(void){
    return ((SW3_GPIO->PDIR & (1u << SW3_PIN)) == 0u);
}

// Initialize Buttons
void Buttons_Init(void){ 
    // enable clock to PORTC
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

    // configure SW1 pin: select GPIO mode, enable internal pull register, select pull-up
    SW1_PORT->PCR[SW1_PIN] = PORT_PCR_MUX(1u) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
   
    // configure SW3 pin
    SW3_PORT->PCR[SW3_PIN] = PORT_PCR_MUX(1u) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;

    // set SW1 & SW3 pin as input (0=input)
    SW1_GPIO->PDDR &= ~(1u << SW1_PIN);
    SW3_GPIO->PDDR &= ~(1u << SW3_PIN);

    // initialize raw and stable states to current hardware state
    g_last_raw_state = Buttons_ReadRaw();
   
    // start stable state as current state
    g_stable_state   = g_last_raw_state;
    
    // no press event yet
    g_pressed_event  = BUTTON_NONE;
   
    // reset debounce counter
    g_debounce_count = 0;
}

// return current physical state of SW1
bool Buttons_ReadSW1(void){
    return Buttons_HW_ReadSW1_Active();
}

// return current physical state of SW3
bool Buttons_ReadSW3(void){
    return Buttons_HW_ReadSW3_Active();
}

// combine both button states into one
button_state_t Buttons_ReadRaw(void){
    
    // read hardware states
    bool sw1 = Buttons_HW_ReadSW1_Active();
    bool sw3 = Buttons_HW_ReadSW3_Active();

    // determine combined state
    // both pressed
    if (sw1 && sw3) return BUTTON_BOTH;
    // only SW1 pressed
    if (sw1) return BUTTON_SW1;
    // only SW3 pressed
    if (sw3) return BUTTON_SW3;
    // no buttons pressed
    return BUTTON_NONE;
}

void Buttons_Update(void){
    // get current raw reading
    button_state_t raw = Buttons_ReadRaw();

    // if raw reading is same as previous reading
    if (raw == g_last_raw_state) {
        
        // increase debounce counter (until max)
        if (g_debounce_count < BUTTONS_DEBOUNCE_TICKS) {
            g_debounce_count++;
        }

        // if stable long enough and different from current stable state
        if ((g_debounce_count >= BUTTONS_DEBOUNCE_TICKS) && (raw != g_stable_state)) {
            
            // update stable state
            g_stable_state = raw;

            // generate event only when entering a pressed state
            if (g_stable_state != BUTTON_NONE) {
                g_pressed_event = g_stable_state;
            }
        }

    } else {
        // raw state changed - restart debounce process
        g_last_raw_state = raw;
        
        // reset counter
        g_debounce_count = 0;
    }
}

// STATE ACCESS FUNCTIONS ------------------------------------

// return current stable button state
button_state_t Buttons_GetStableState(void){
    return g_stable_state;
}

// return one-time press event and clear it
button_state_t Buttons_GetPressedEvent(void){
    // same event value
    button_state_t ev = g_pressed_event;
    
    // clear event
    g_pressed_event = BUTTON_NONE;
    
    return ev;
}

// manually clear event
void Buttons_ClearPressedEvent(void){
    g_pressed_event = BUTTON_NONE;
}

// HELPER FUNCTIONS ------------------------------------------

// check if stable state is SW1
bool Buttons_IsSW1Pressed(void){
    return (g_stable_state == BUTTON_SW1);
}

// check if stable state is SW3
bool Buttons_IsSW3Pressed(void){
    return (g_stable_state == BUTTON_SW3);
}

// check if both buttons are pressed
bool Buttons_IsBothPressed(void){
    return (g_stable_state == BUTTON_BOTH);
}
