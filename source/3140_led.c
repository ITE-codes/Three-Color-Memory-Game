/* led.c
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

#include <3140_led.h>
#include <MKL46Z4.h>

const int RED_LED_PIN        = 16;
const int GREEN_LED_PIN      = 6;
const int BLUE_LED_PIN       = 2;
const int RED_LED_PIN_FRDM   = 29;
const int GREEN_LED_PIN_FRDM = 5;
const int SERIAL_LED_PIN     = 3;

/*
 *variable to keep track of calls to clock setting routine
 *the clocking transitions in set15MHz() can executed once
 *otherwise subsequent calls result in a hard vault
 *otherwise */
int clock_set_to_15=0;

void set15MHz(void){

	//Make sure this runs only once
	if(clock_set_to_15){
		return;
	}
	clock_set_to_15=1;

	/* The clock generation options for this chip are
	 * somewhat complicated with many options for different
	 * internal/external clock sources that get
	 * multiplied and divided by either a frequency
	 * locked loop (FLL) or a phase locked loop (PLL).
	 *
	 * This configuration is for:
	 * External 8MHz oscillator as we have on our FRDMKL46z board
	 * PLL for generating a 15MHz System core and bus clock
	 * Look at Lab 2 for and the manual for more information
     */

	MCG->C2 = MCG_C2_RANGE0(0b10) | MCG_C2_HGO0(0) | MCG_C2_EREFS0(1);
	MCG->C1 = MCG_C1_CLKS(0b10) | MCG_C1_FRDIV(0b011) | MCG_C1_IREFS(0);

	/* Check that changes went through*/
	while(!((MCG->S & MCG_S_OSCINIT0_MASK) == MCG_S_OSCINIT0(1))){}
	while(!((MCG->S & MCG_S_IREFST_MASK) == 0)){}
	while(!((MCG->S & MCG_S_CLKST_MASK) == MCG_S_CLKST(0b10))){}


	MCG->C5 = MCG_C5_PRDIV0(0b00011);                 // Divide by 4 -> 8MHz/4=2MHz
	MCG->C6 = MCG_C6_VDIV0(0b00110) | MCG_C6_PLLS(1); // Multiply by 30 -> 2MHz * 30=60MHz + set PLL

	 /*Check that changes went through*/
	 while(!((MCG->S & MCG_S_PLLST_MASK) == MCG_S_PLLST(0b1))){} //Wait until PLL is locked on
	 while(!((MCG->S & MCG_S_LOCK0_MASK) == MCG_S_LOCK0(0b1))){} //Wait until PLL is locked on

	 MCG->C1 = MCG_C1_CLKS(0b00);

	 /*Check that changes went through*/
	 while(!((MCG->S & MCG_S_CLKST_MASK) == MCG_S_CLKST(0b11))){} //Wait until PLL is locked on

	 SIM->CLKDIV1= SIM_CLKDIV1_OUTDIV1(0b0011) | SIM_CLKDIV1_OUTDIV4(0); //60MHz/4 = 15MHz

	 delay(1);
}



void led_init(void) {

	//Clock Port E and Port D
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTD_MASK;

	// set up red LED0
	PORTE->PCR[RED_LED_PIN] = PORT_PCR_MUX(0b001);
	PTE->PSOR = 1 << RED_LED_PIN;  // turn off LED
	PTE->PDDR |= 1 << RED_LED_PIN; // make it output

	// set up green LED0
	PORTE->PCR[GREEN_LED_PIN] = PORT_PCR_MUX(0b001);
	PTE->PSOR = 1 << GREEN_LED_PIN;  // turn off LED
	PTE->PDDR |= 1 << GREEN_LED_PIN; // make it output

	// set up blue LED0
	PORTE->PCR[BLUE_LED_PIN] = PORT_PCR_MUX(0b001);
	PTE->PSOR = 1 << BLUE_LED_PIN;  // turn off LED
	PTE->PDDR |= 1 << BLUE_LED_PIN; // make it output

	// set up red LED pin on FRDM board
	PORTE->PCR[29] = PORT_PCR_MUX(001);
	PTE->PDDR |= 1<<29;
	PTE->PSOR = 1<<29;

	//set up green LED on FRDM board
	PORTD->PCR[5] =  PORT_PCR_MUX(001);
	PTD->PSOR = 1<<5;
	PTD->PDDR |= 1<<5;

	//set up the pin to do the serial for LEDs 1 and 2
	PORTE->PCR[SERIAL_LED_PIN] = PORT_PCR_MUX(0b001);
	PTE->PCOR = 1 << SERIAL_LED_PIN; // set output low
	PTE->PDDR |= 1 << SERIAL_LED_PIN; // make it output

	set15MHz();

	led_off();
	led_off();

	delay(1);
}

void delay(uint32_t ms){
	for(int i= ms; i>0; i--){
		/* Each clock cycles is 1/15MHz
		 * So per ms there are: 1*10^-3 * 15^10^6 / 3 = 5000
		 * loops through the small 3-clock cycle helper function. */
		 loop_3cycles(5000);
	}
}


void red_on(void) {
	PTE->PCOR |= GPIO_PCOR_PTCO(1 << RED_LED_PIN);
}

void red_off(void) {
	PTE->PSOR |= GPIO_PSOR_PTSO(1 << RED_LED_PIN);
}
void red_toggle(void) {
	PTE->PTOR |= GPIO_PTOR_PTTO(1 << RED_LED_PIN);
}

void blue_on(void) {
	PTE->PCOR |= GPIO_PCOR_PTCO(1 <<BLUE_LED_PIN);
}

void blue_off(void) {
	PTE->PSOR |= GPIO_PSOR_PTSO(1 << BLUE_LED_PIN);
}
void blue_toggle(void) {
	PTE->PTOR |= GPIO_PTOR_PTTO(1 << BLUE_LED_PIN);
}

void green_on(void) {
	PTE->PCOR |= GPIO_PCOR_PTCO(1 <<GREEN_LED_PIN);
}

void green_off(void) {
	PTE->PSOR |= GPIO_PSOR_PTSO(1 << GREEN_LED_PIN);
}
void green_toggle(void) {
	PTE->PTOR |= GPIO_PTOR_PTTO(1 << GREEN_LED_PIN);
}

void led0_off(void) {
	red_off();
	blue_off();
	green_off();
}


/*
 * Manipulate red FRDM LED pin
 */
void red_on_frdm(void){
	PTE->PCOR = 1<<29;
}
void red_off_frdm(void){
	PTE->PSOR = 1<<29;
}
void red_toggle_frdm(void){
	PTE->PTOR = 1<<29;
}

/*
 * Manipulate green FRDM LED pin
 */
void green_on_frdm(void){
	PTD->PCOR = 1<<5;
}
void green_off_frdm(void){
	PTD->PSOR = 1<<5;
}
void green_toggle_frdm(void){
	PTD->PTOR = 1<<5;
}

