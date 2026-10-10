// rndm.c

#include "rndm.h"

/*
 * Simple linear congruential generator (LCG)
 * This is sufficient for generating random game sequences.
 */
static uint32_t g_rndm_state = 1u;

/*
 * Initialize the random generator with a seed.
 * If seed = 0, we replace it with 1 to avoid getting stuck at 0.
 */
void Rndm_Init(uint32_t seed) {
    // check if seed is zero
    if (seed == 0u) {
        // replace with 1 to ensure generator progresses
        seed = 1u;
    }

    // set internal generator state to seed value
    g_rndm_state = seed;
}

/*
 * Generate the next pseudo-random number.
 * Uses a standard LCG formula:
 *   next = (a * state + c)
 */
uint32_t Rndm_Next(void) {
    g_rndm_state = (1664525u * g_rndm_state) + 1013904223u;
    return g_rndm_state;
}

/*
 * Generate a number in range [0, max_exclusive)
 * Uses modulo to limit the range.
 *
 * If max_exclusive = 0, just return 0 to avoid divide-by-zero.
 */
uint32_t Rndm_Range(uint32_t max_exclusive) {
    // check for invalid range (division by zero case)
    if (max_exclusive == 0u) {
        // return 0 if no valid range exists
        return 0u;
    }

    // return value limited to desired range using modulo
    return (Rndm_Next() % max_exclusive);
}

/*
 * Generate a random game color.
 * Maps values 0,1,2 → RED, GREEN, BLUE.
 */
game_color_t Rndm_NextColor(void) {
    // choose color based on random value in range 0-2
    switch (Rndm_Range(3u)) {
        // map 0 to red
        case 0u:
            return GAME_COLOR_RED;

        // map 1 to green
        case 1u:
            return GAME_COLOR_GREEN;

        // map 2 to blue
        case 2u:
        default:
            return GAME_COLOR_BLUE;
    }
}