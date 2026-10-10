// timer.h

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>
#include <stdbool.h>

// GAME TIMING CONSTANTS ------------------------------------------

// amount of time each color stays on during sequence playback
#define TIMER_COLOR_ON_MS              700

// pause between colors in the shown sequence
#define TIMER_BETWEEN_COLORS_MS        1000

// delay after a successful round before next level starts
#define TIMER_SUCCESS_DELAY_MS         2500

// very short delay before failure/game over feedback
#define TIMER_FAILURE_DELAY_MS         200

// maximum time allowed before user starts entering input
#define TIMER_INPUT_START_TIMEOUT_MS   4500

// maximum time allowed between user inputs
#define TIMER_BETWEEN_INPUTS_TIMEOUT_MS 4500

// delay between finishing one level and starting the next
#define TIMER_NEXT_LEVEL_DELAY_MS      2000

// FUNCTIONS --------------------------------------------------------

// initialize timer module state
void Timer_Init(void);

// general-purpose millisecond delay
void Timer_DelayMs(uint32_t ms);

// hold one color on-screen long enough to be clearly visible
void Timer_WaitColorOn(void);

// pause between colors in the sequence
void Timer_WaitBetweenColors(void);

// delay after user successfully completes a sequence
void Timer_WaitAfterSuccess(void);

// short delay before failure/game-over 
void Timer_WaitAfterFailure(void);

// delay before next level begins
void Timer_WaitNextLevel(void);

// reset the internal software timer to 0
void Timer_ResetCounter(void);

// advance the internal software timer by elapsed_ms
void Timer_UpdateCounter(uint32_t elapsed_ms);

// get current software timer count in milliseconds
uint32_t Timer_GetCounterMs(void);

// return true if the timeout interval has passed
bool Timer_HasTimedOut(uint32_t start_time_ms, uint32_t timeout_ms);

#endif /* TIMER_H_ */