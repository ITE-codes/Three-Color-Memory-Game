/* 3140.s
 *
 * Nils Napp
 * Cornell University
 * All right reserved
 *
 * Jan 2024
 * Ithaca NY
 *
 * This file is part of the ECE3140/CS3420 offering for Spring 2024. If you
 * are not part of this class you should not have access to this file. Do not
 * post, share, or otherwise distribute this file. We will consider it an AI
 * violation if you do. If you somehow get this code and you are NOT enrolled
 * the Spring 2024 version of ECE3140 please contact the course staff
 * immediately and describe how you found it.
 */

.section .text

.global set_led
.type set_led %function

.global led_off
.type led_off %function

.global loop_3cycles
.type loop_3cycles %function

// added function
.global set_led_48MHz
.type set_led_48MHz, %function

//Small timing loop to create predictable delays
.balign 4
loop_3cycles:
	SUB R0,#1
	BNE loop_3cycles
	BX LR

.balign 4
led_off:
	PUSH {LR}
	MOV R0, #0
	BL set_led
	POP {PC}

//Binary code for set_led
.balign 4
set_led:
	.word 0x490eb470
	.word 0x4b0f4a0e
	.word 0x25011c04
	.word 0x1c2e05ed
	.word 0xd1044026
	.word 0x46c06011
	.word 0x601946c0
	.word 0x6011e009
	.word 0x1c2d46c0
	.word 0x46c01c2d
	.word 0x1c3f46c0
	.word 0x46c01c3f
	.word 0x086d6019
	.word 0xd1ea2d00
	.word 0x4770bc70
	.word 0x00000008
	.word 0x400ff104
	.word 0x400ff108

// Configure external LEDs 1 and 2 to work under LCD
// Time Configurations

// align the following code to a 4-byte boundary 
.balign 4

// label for the function that sends a 24-bit color to the serial LED at 48 MHz
set_led_48MHz:

    // save registers R4–R7 and return address (LR) onto the stack
    PUSH {R4-R7, LR}

    // disable interrupts to ensure precise timing for LED signal
    CPSID i

    // load address of Port E Set Output Register (PSOR) into R1 (sets pin HIGH)
    LDR R1, =0x400FF104      

    // load address of Port E Clear Output Register (PCOR) into R2 (sets pin LOW)
    LDR R2, =0x400FF108     

    // load bitmask for pin PTE3 (1 << 3) into R3
    MOVS R3, #8             

    // set loop counter to 24 bits (GRB color data)
    MOVS R4, #24           

    // load mask for the most significant bit into R5
    LDR R5, =0x00800000      

// start of bit transmission loop
bit_loop_48:

    // set the serial data pin HIGH to begin transmitting
    STR R3, [R1]            

    // test current bit of input data 
    TST R0, R5              

    // if bit is 0, branch to send_zero_48
    BEQ send_zero_48

// case: sending logic 1
send_one_48:

    // maintain HIGH signal for logic 1 timing (long HIGH pulse)
    .rept 28
    NOP
    .endr

    // set the serial data pin LOW
    STR R3, [R2]           

    // maintain LOW signal for logic 1 timing
    .rept 18
    NOP
    .endr

    // jump to next bit processing
    B next_bit_48

// case: sending logic 0
send_zero_48:

    // maintain HIGH signal for logic 0 timing (short HIGH pulse)
    .rept 7
    NOP
    .endr

    // set the serial data pin LOW
    STR R3, [R2]            

    // maintain LOW signal for logic 0 timing (long LOW pulse)
    .rept 36
    NOP
    .endr

// move to next bit
next_bit_48:

    // shift bit mask right to move to next bit 
    LSR R5, R5, #1

    // decrement bit counter
    SUB R4, R4, #1

    // if more bits remain, repeat loop
    BNE bit_loop_48

    // re-enable interrupts after timing-critical section
    CPSIE i

    // restore registers and return from function
    POP {R4-R7, PC}
