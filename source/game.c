// game.c

#include "game.h"
#include "display.h"

// include all files
#include "buttons.h"
#include "leds.h"
#include "rndm.h"
#include "timer.h"

// GAME STATE VARIABLES -------------------------------

// store current FSM state
static game_state_t g_state = GAME_IDLE;

// store current round number
static uint32_t g_round = 0u;

// store number of elements in the active sequence
static uint32_t g_sequence_length = 0u;

// track which index of the sequence 
// the user is currently inputting
static uint32_t g_input_index = 0u;

// store most recent user input 
// converted into a game color
static game_color_t g_last_input_color = GAME_COLOR_NONE;

// store the randomly generated color sequence
static game_color_t g_sequence[GAME_MAX_LENGTH];

// store time when user input phase begins
static uint32_t g_input_start_time = 0u;

// store time of most recent user input
static uint32_t g_last_input_time = 0u;

// HELP FUNCTIONS ----------------------------------------

// reset sequence array and clear length
static void Game_ClearSequence(void) {
    // loop counter variable
    uint32_t i;
    // loop through entire sequence
    for (i = 0u; i < GAME_MAX_LENGTH; i++) {
        // set every entry to "no color"
        g_sequence[i] = GAME_COLOR_NONE;
    }
    // reset sequence length to 0
    g_sequence_length = 0u;
}

// generate initial sequence of starting length
static void Game_GenerateStartSequence(void) {
    // loop variable
    uint32_t i;
    // clear any previous sequence
    Game_ClearSequence();
    // fill sequence with random colors
    for (i = 0u; i < GAME_START_LENGTH; i++) {
        // generate next random color
        g_sequence[i] = Rndm_NextColor();
    }
    // set current sequence length to start length
    g_sequence_length = GAME_START_LENGTH;
    // set round equal to sequence level
    g_round = g_sequence_length;
}

// add one more random color to sequence (next level)
static void Game_AppendColor(void){
    // only append if max size not reached
    if (g_sequence_length < GAME_MAX_LENGTH) {
        // add new random color at the end of sequence
        g_sequence[g_sequence_length] = Rndm_NextColor();
        // increase sequence length
        g_sequence_length++;
        // update round number
        g_round = g_sequence_length;
    }
}

// prepare variables before user input
static void Game_PrepareForInput(void) {
    // start checking from first element in sequence
    g_input_index = 0u;
    // reset last input color
    g_last_input_color = GAME_COLOR_NONE;

    // clear any button press events
    Buttons_ClearPressedEvent();
    // reset timer counter
    Timer_ResetCounter();

    // record time when input phase starts
    g_input_start_time = Timer_GetCounterMs();
    // initialize last input time = start time
    g_last_input_time = g_input_start_time;
}

// return 1 if user completed sequence correctly
static uint8_t Game_IsRoundComplete(void) {
    // checks if user matched entire sequence
    return (g_input_index >= g_sequence_length) ? 1u : 0u;
}

// return 1 if sequence is at max length
static uint8_t Game_IsAtMaxLength(void) {
    return (g_sequence_length >= GAME_MAX_LENGTH) ? 1u : 0u;
}

// GAME FUNCTIONS ---------------------------------------------

// initialize all modules and reset game state
void Game_Init(void) {
    Buttons_Init();
    Timer_Init();
    Leds_Init();

    Display_Init();
    Display_ResetCounter();

    // initialize random generation
    Rndm_Init(1u);

    // clear sequence
    Game_ClearSequence();

    // set initial game state to idle
    g_state = GAME_IDLE;
    // reset round number
    g_round = 0u;
    // reset input tracker
    g_input_index = 0u;
    // reset last input color
    g_last_input_color = GAME_COLOR_NONE;
    // reset timer varaibles
    g_input_start_time = 0u;
    g_last_input_time = 0u;

    // ensure all LEDS are off
    Leds_AllOff();
}

// return current FSM state
game_state_t Game_GetState(void) { return g_state; }

// return current round number
uint32_t Game_GetRound(void) { return g_round; }

// return current sequence length
uint32_t Game_GetSequenceLength(void) { return g_sequence_length; }

// MAIN GAME FUNCTION / LOOP (FSM) --------------------------------------
void Game_Run(void) {
    // infinite loop
    while (1) {

        // IDLE STATE -------------------------------
        if (g_state == GAME_IDLE) {

            // show ready signal (green flash)
            Leds_FlashColor(GAME_COLOR_GREEN, 250u, 150u);

            // wait for any valid press to start the game
            while (g_state == GAME_IDLE) {
                Buttons_Update();

                // check if a button was pressed
                if (Buttons_GetPressedEvent() != BUTTON_NONE) {
                    // generate starting sequence
                    Game_GenerateStartSequence();

                    // when game starts, LCD show round 1
                    Display_ResetCounter();

                    // move to display sequence state
                    g_state = GAME_SHOW_SEQUENCE;
                }

                // small delay
                Timer_DelayMs(5u);
            }
        }

        // SHOW SEQUENCE STATE ---------------------------------
        else if (g_state == GAME_SHOW_SEQUENCE) {

            // play sequence on external LED
            Leds_PlaySequence(g_sequence, g_sequence_length);

            // prepare input tracker
            Game_PrepareForInput();

            // move to input state
            g_state = GAME_READ_INPUT;
        }

        // READ USER INPUT STATE ------------------------------
        else if (g_state == GAME_READ_INPUT) {

            // loop until input or timeout
            while (g_state == GAME_READ_INPUT) {
                // temp variable to store button event
                button_state_t button_event;

                // update button logic
                Buttons_Update();
                // increase timer
                Timer_UpdateCounter(5u);

                // timeout if user hasn't started input (user takes too long)
                if ((g_input_index == 0u) &&
                    Timer_HasTimedOut(g_input_start_time, TIMER_INPUT_START_TIMEOUT_MS)) {
                    g_state = GAME_GAME_OVER;
                    break;
                }

                // timeout between later inputs (user takes too long)
                if ((g_input_index > 0u) &&
                    Timer_HasTimedOut(g_last_input_time, TIMER_BETWEEN_INPUTS_TIMEOUT_MS)) {
                    g_state = GAME_GAME_OVER;
                    break;
                }

                // read button press event
                button_event = Buttons_GetPressedEvent();

                // if user pressed a button
                if (button_event != BUTTON_NONE) {
                    // convert button input to game color
                    g_last_input_color = Leds_ButtonToColor(button_event);

                    // show feedback for pressed color
                    Leds_ShowUserInput(g_last_input_color);

                    // update last input time
                    g_last_input_time = Timer_GetCounterMs();
                    
                    // move to checking state
                    g_state = GAME_CHECK_INPUT;
                    break;
                }

                // small delay
                Timer_DelayMs(5u);
            }
        }

        // CHECK USER INPUT STATE ------------------------------------
        else if (g_state == GAME_CHECK_INPUT) {

            // compare user input with expected sequence color
            if (g_last_input_color == g_sequence[g_input_index]) {
                // move to next index
                g_input_index++;
                // if entire sequence matches
                if (Game_IsRoundComplete()) {
                    // move to outcome state
                    g_state = GAME_ROUND_OUTCOME;
                }
                else {
                    // else keep reading input
                    g_state = GAME_READ_INPUT;
                }
            }
            else {
                // else wrong input, then game over
                g_state = GAME_GAME_OVER;
            }
        }

        // ROUND/LEVEL OUTCOME STATE -----------------------------------
        else if (g_state == GAME_ROUND_OUTCOME) {

            // show success patter
            Leds_ShowSuccess();

            // pause after success
            Timer_WaitAfterSuccess();

            // if at max difficulty, reset game
            if (Game_IsAtMaxLength()) {
                g_state = GAME_IDLE;
            }
            // else, extend sequence
            else {
                Game_AppendColor();
                // increase LCD round counter by 1
                Display_IncrementCounter();
                // wait before next level
                Timer_WaitNextLevel();
                // then show next sequence
                g_state = GAME_SHOW_SEQUENCE;
            }
        }

        // GAME OVER STATE ----------------------------------
        else if (g_state == GAME_GAME_OVER) {

            // show failure pattern
            Leds_ShowFailure();

            // reset LCD round counter back to 1
            Display_ResetCounter();

            // short delay before reset
            Timer_WaitAfterFailure();

            // return to idle state
            g_state = GAME_IDLE;
        }

        // IDLE FALLBACK ------------------------------
        else {
            // in case there is an error/invalid state
            // reset to idle state
            g_state = GAME_IDLE;
        }
    }
}
