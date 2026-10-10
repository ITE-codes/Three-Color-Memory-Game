// timer.c

#include "timer.h"
#include "3140_led.h"

// store a simple software counter in milliseconds
static uint32_t g_timer_counter_ms = 0;

// TIMER SET-UP
void Timer_Init(void){
    // start software timer at 0
    g_timer_counter_ms = 0;
}

// TIMING DURING GAME -----------------------------------------------

void Timer_DelayMs(uint32_t ms){
    // use a scaled version of the class-provided 
    // delay function so that the system time is aligned
    // with the time configurations set by the imported LCD files
    uint32_t scaled_ms = (ms * 16u) / 5u;
	delay(scaled_ms);
}

void Timer_WaitColorOn(void){
    // keep a displayed color on long enough for user to see it
    Timer_DelayMs(TIMER_COLOR_ON_MS);
}

void Timer_WaitBetweenColors(void){
    // wait 1 second between consecutive colors in sequence playback
    Timer_DelayMs(TIMER_BETWEEN_COLORS_MS);
}

void Timer_WaitAfterSuccess(void){
    // give user time to recognize success before next level starts
    Timer_DelayMs(TIMER_SUCCESS_DELAY_MS);
}

void Timer_WaitAfterFailure(void){
    // short pause before failure/game-over feedback
    Timer_DelayMs(TIMER_FAILURE_DELAY_MS);
}

void Timer_WaitNextLevel(void){
    // pause before showing the next level
    Timer_DelayMs(TIMER_NEXT_LEVEL_DELAY_MS);
}

// HELPER FUNCTIONS ----------------------------------------------

void Timer_ResetCounter(void){
    // reset software counter back to 0
    g_timer_counter_ms = 0;
}

void Timer_UpdateCounter(uint32_t elapsed_ms){
    // advance internal software time by elapsed amount
    g_timer_counter_ms += elapsed_ms;
}

uint32_t Timer_GetCounterMs(void){
    // return current software timer count
    return g_timer_counter_ms;
}

bool Timer_HasTimedOut(uint32_t start_time_ms, uint32_t timeout_ms){
    // return true if enough time has passed since start_time_ms
    return ((g_timer_counter_ms - start_time_ms) >= timeout_ms);
}