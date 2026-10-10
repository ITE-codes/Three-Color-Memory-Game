# Final_Project_SP26

## Website

Upload assets folder, index.html, and style.css to VS Code

# Three-Color Memory Game  
ECE 3140 / CS 3420 Final Project  

## Overview
This project is a three-color memory game inspired by Simon Says, implemented on the FRDM-KL46Z board. The board generates a sequence of colors using LEDs, and the user has to repeat the sequence using the onboard buttons. If the user is correct, the sequence gets longer each round. If they make a mistake or take too long, the game ends.

We split the project into different modules (buttons, LEDs, timer, game logic, etc.) so we could build and test things separately before putting everything together.

---

## How to Compile/Run
1. Open the project in MCUXpresso:
   - To run the system you will need the support files found in the directories in slcd SDK demo (under driver_examples).
   - You can find the slcd SDK demo by going to File -> New -> Import SDK Examples.
   - Then choose the frdmkl46z, click next, click the dropdown arrow next to driver_examples, selecting slcd and then hit finish.
2. Delete all the automatically generated files in the "source" folder of the project
3. Import/Upload all the files in the code and tests files of the GitHub Repo into the "source" folder of the project
4. Make sure only one `main()` file is active by excluding all other files from build:
   - `main.c` → runs the full game/system  
   - `test_*.c` → runs individual module tests  
5. Build and flash to the FRDM-KL46Z board.
6. Use SW1 and SW3 to play the game.

---

## File Structure

### Main Game
- `main.c`  
  Entry point. Calls `System_Init()`, `Game_Init()`, and `Game_Run()`.

- `system_init.c / system_init.h`
Handles centralized initialization of all modules.

- `game.c / game.h`  
  Handles the main game logic and state machine (idle, show sequence, input, check, etc.).

### Input
- `buttons.c / buttons.h`  
  Reads SW1 and SW3 and handles debouncing + press detection.

### LEDs
- `leds.c / leds.h`  
  Controls color output, flashing, and sequence playback.

- `3140_led.c / 3140_led.h`  
  Lower-level LED helpers from lab code.

- `led_lowlevel.s`  
  Assembly support for LED timing.

### Timing
- `timer.c / timer.h`  
  Used for delays and basic timing (sequence display, input timing, etc.).

### Random
- `rndm.c / rndm.h`  
  Generates random colors for the sequence.

### Display
- `display.c / display.h`  
  Handles LCD output for displaying the current round number.

---

## Game Flow
1. System starts in idle
2. User presses a button to start
3. LEDs display a sequence
4. User repeats sequence using buttons
5. If correct → next round (longer sequence)  
   If incorrect or too slow → game over → reset

### Input Mapping
We only have two buttons, so:
- SW1 = one color  
- SW3 = one color  
- SW1 + SW3 together = third color  

---

## Testing
We tested each part before integrating everything.

- `test_buttons.c` → checked debouncing + press detection  
- `test_timer.c` → checked delays and timing behavior  
- `test_leds.c` → checked colors and sequence playback  
- `test_rndm.c` → checked randomness
- `test_display.c` → checked LCD counter functionality
- `test_game.c` → checks init, then runs full game

We also just played the game a lot to catch issues.

Main issues we ran into:
- button bounce → fixed with debouncing  
- timing felt off → adjusted delays  
- input sometimes double counted → fixed in button logic  

---

## Notes
We kept everything modular so it was easier to debug. That helped a lot during integration.

Most of the tricky parts were around timing and input handling — if those weren’t right, the whole game felt broken.

---

## Resources
- Lecture slides  
- Lab code / starter files  
- MKL46Z4 reference manual
- LCD Library & files

No external code used beyond class materials.
