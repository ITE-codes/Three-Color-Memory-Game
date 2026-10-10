.section .data
//global variable in assembly
OrigStackPointer:
.word 0x00


PIT1Stack:
.skip PIT1StackSize

/*
These can be global variables to keep track of how many times
each channel was triggered. They are defined here as weak in case you
don't want to use them in your code. If you do want to use them define
the following  two golbal variables in your process.c file and they
will be automatically incremented in 3140.s

extern volatile int pit1_cnt = 0;
extern volatile int pit0_cnt = 0;
*/

pit1_cnt:
.word 0x00

pit0_cnt:
.word 0x00



.syntax unified

		
.section .text
.syntax unified
//export assembly functions
.global process_terminated
.global process_begin
.global process_blocked
.global PIT_IRQHandler
.global SVC_Handler

//import C functions
.weak process_select
.type process_select, %function

//Function to service PIT Chanel 1 Routine
//Export this so you can write C-fucntion
.weak PIT1_Service
.type PIT1_Service, %function

				
.equ TFLG0, 0x4003710C // TFLG address
.equ CTRL0, 0x40037108 // Ctrl address
.equ SHCSR, 0xE000ED20

.equ TFLG1, 0x4003711C // TFLG Chanel 1 address
.equ CTRL1, 0x40037118 // Ctrl Chann1l address

.equ 	PIT1StackSize, 	32

/*
General note: you need to declare actual function labels as functions.
Otherwise, the linker will not set the thumb bit correctly when branching,
and the processor will HardFault with INV_STATE.

Lab0 and Lab1 for ECE 3140 work fine because nothing is actually called
as a function. Main is called properly.
*/
.type  SVC_Handler, %function
SVC_Handler:
	LDR  R1, [SP,#24]	// Read PC from SVC exception
	MOVS R2,#2
	SUBS R1, R2
	LDRB R0, [R1] 		// Get #N from SVC instruction (PC is SVC addr + 2)
	ADR  R1, SVC_Table
	LSLS R0, #2 		//Multiply by 4 to (left shift by 2) to get offset from table
	LDR  R2, [R1,R0] 	// Get Branch Address
	MOV  PC, R2		// Branch to Appropriate SVC Routine


SVC_Table:
.word SVC0_begin
.word SVC1_terminate
.word PIT_IRQHandler 		// Use system tick as SVC2 handler

.type  SVC0_begin, %function
SVC0_begin:
			PUSH {LR}    // First put return code on stack
			PUSH {R4-R7} // These are original values of R4-R7
			MOV R4, R8
			MOV R5, R9
			MOV R6, R10
			MOV R7, R11
			PUSH {R4-R7} // These now hold the values of R8-R11


			//******* Store Original Stack Pointer ********
			LDR R1, =OrigStackPointer
			MOV R2, SP
			STR R2, [R1]
			//********************************************

.type  SVC1_terminate, %function
SVC1_terminate:
			CPSID i   //Disable interrupts so that terminated process cannot mess up stack
			MOVS R0, #0
			B do_process_select


.type  process_terminated, %function
process_terminated:
			CPSIE i // Enable global interrupts, just in case
			SVC #1 // SVC1 = process terminated
			// This SVC shouldn't ever return, as it would mean the process was scheduled again

.type  process_begin, %function
process_begin:
			CPSIE i // Enable global interrupts (for SVC)
			SVC #0 // Syscall into scheduler
			BX LR

.type  process_blocked, %function
process_blocked:
			CPSIE i // Enable global interrupts, just in case
			SVC #2 // SVC2 = process blocked
			BX LR

.type  PIT_IRQHandler, %function
PIT_IRQHandler:		// Timer Interrupt
			CPSID i   	  // Disable all interrupts

			/*
			Check interrupt source and service accordingly
			Check Channel 1 first and make call to c-function
			*/


			LDR R1, =TFLG1
			LDR R0, [R1]
			CMP R0, #1
			BNE PIT_Channel_0  //Go to "regular" service for Channel 0 ISR

			//increment pit1 counter
			LDR R1, =pit1_cnt
			LDR R2, [R1]
			ADDS R2, #1
			STR R2, [R1]

			//---- Move process's SP to R0 and switch to PIT1 stack ----
			MOV R0,  SP
			LDR R1, =PIT1Stack
			LDR R2, =PIT1StackSize
			ADDS R1, R2		//make R1 point to the word just above the stack (the stack grows down)
			MOV SP, R1

			PUSH  {R0,LR}     // Save current SP and return onto scheduler stack
			BL PIT1_Service	  // Call Service Rourtine written in C (on scheduler stack)
			POP   {R0,R1}     // R1 now has return code

			//---- Restore SP, enable interrupts, and return ----
			MOV SP, R0
			CPSIE I
			BX R1


PIT_Channel_0:          // This is the start of the "normal" ISR from prior labs
			PUSH {LR}     // First put return code on process's stack
			PUSH {R4-R7}  // These are the original values of R4-R7
			MOV R4, R8
			MOV R5, R9
			MOV R6, R10
			MOV R7, R11
			PUSH {R4-R7} // These now hold the values of R8-R11

			LDR R1, =pit0_cnt //increment pit0_counter
			LDR R2, [R1]
			ADDS R2, #1
			STR R2, [R1]

			//---- store scheduling timer state and
			//     turn off context switching timer ----
			LDR R1, =CTRL0
			LDR R0, [R1]
			PUSH {R0}
			MOVS R0, #0
			STR R0, [R1]


			//---clear the interrupt flag----
			LDR  R4, =TFLG0
			MOVS R1, #1
			STR  R1, [R4]
			//-------------------------------
				
			//move sp to r0 to prepare for process_select
			MOV R0, SP
				
do_process_select:
			CPSID i   	  // Disable all interrupts (again!) Can get here from terminate
						  // With interrupts still on

			//******* Load Original Stack Pointer ********
			// We want the process select function to run on the "main" stack
			// This helps reduce funkiness when a process stack is small
			// and process_select overwrites other memory
			LDR R1, =OrigStackPointer
			LDR R2, [R1]
			MOV SP, R2
			//********************************************

			BL process_select	//Process_select returns 0 if there are no processes left
			CMP R0, #0
			BNE resume_process	//take branch if there are more processes


			// Disable scheduling timer before returning to initial caller
			LDR R1, =CTRL0
			MOVS R0, #0
			STR R0, [R1]

			// Disable real time timer before returning to initial caller
			LDR R1, =CTRL1
			MOVS R0, #0
			STR R0, [R1]

			POP {R4-R7} // Grab the high registers and restore them

			MOV R8, R4
			MOV R9, R5
			MOV R10, R6
			MOV R11, R7

			POP {R4-R7,PC} // These are now the original values of R4-R7 and the return code

				
resume_process:
			MOV SP, R0    //switch stacks
			//---- restore scheduling timer state
			POP {R0}
		    	LDR R1, =CTRL0
			STR R0, [R1]
				
			CPSIE I // Enable global interrupts before returning from handler

			POP {R4-R7} // Grab the high registers and restore them

			MOV R8, R4
			MOV R9, R5
			MOV R10, R6
			MOV R11, R7

			POP {R4-R7,PC} // These are now the original values of R4-R7 and the return code

