// main.c

#include "game.h"
#include "system_init.h"

// main loop/running the game
int main(void) {
	// initialize system
	System_Init();
    // initialize full game and run it forever
    Game_Init();
    Game_Run();
    while (1) {
    }
}
