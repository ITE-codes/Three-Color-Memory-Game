// game.h

#ifndef GAME_H_
#define GAME_H_

#include <stdint.h>
// to import game_color_t
#include "leds.h"

// GAME CONFIG. ---------------------------------------

// starting sequence length
#define GAME_START_LENGTH   3

// maximum supported sequence length
#define GAME_MAX_LENGTH     12

// GAME STATES ---------------------------------------

// define all possible states of the game state machine
typedef enum {
    // inital idle state before the game begins
    GAME_IDLE = 0,

    // state where the sequence is shown to the user
    GAME_SHOW_SEQUENCE,

    // state where user inputs the sequence
    GAME_READ_INPUT,

    // state where user input is checked against the sequence
    GAME_CHECK_INPUT,

    // state for handling success or failure of a round
    GAME_ROUND_OUTCOME,
    
    // state for handling end of game
    GAME_GAME_OVER
} game_state_t;

// GAME FUNCTIONS ------------------------------------------

// initialize game state, variables, and sequence
void Game_Init(void);

// run on iteration of the game state machine
void Game_Run(void);

// return the current game state
game_state_t Game_GetState(void);

// return the current round number
uint32_t Game_GetRound(void);

// return the current sequence length
uint32_t Game_GetSequenceLength(void);

#endif /* GAME_H_ */