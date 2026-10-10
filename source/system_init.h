// system_init.h

#ifndef SYSTEM_INIT_H_
#define SYSTEM_INIT_H_

#include <stdint.h>

/*
 * Initializes the entire system in a consistent way for:
 * - LCD (sets system clock to 48 MHz)
 * - Timer (uses scaled delays)
 * - LEDs (uses correct serial timing)
 * - Buttons
 */
void System_Init(void);

#endif /* SYSTEM_INIT_H_ */
