// rndm.h

#ifndef RNDM_H_
#define RNDM_H_

#include <stdint.h>
#include "leds.h"

/*
 * Seed the pseudo-random number generator.
 */
void Rndm_Init(uint32_t seed);

/*
 * Return the next pseudo-random 32-bit value.
 */
uint32_t Rndm_Next(void);

/*
 * Return a value in [0, max_exclusive - 1].
 * If max_exclusive == 0, returns 0.
 */
uint32_t Rndm_Range(uint32_t max_exclusive);

/*
 * Return one random game color: RED / GREEN / BLUE.
 */
game_color_t Rndm_NextColor(void);

#endif /* RNDM_H_ */