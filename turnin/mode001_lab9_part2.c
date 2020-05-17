/*              Name & E-mail: Michael O'Dea, mode001@ucr.edu
 *              Lab Section: 025
 *              Assignment: Lab #9  Exercise #2
 *              Exercise Description: Using the ATmega1284’s PWM 
 *              functionality, design a system where the notes: C4, D, E, F, 
 *              G, A, B, and C5,  from the table at the top of the lab, can 
 *              be generated on the speaker by scaling up or down the eight 
 *              note scale. Three buttons are used to control the system.
 *              One button toggles sound on/off. The other two buttons scale 
 *              up, or down, the eight note scale. Criteria: The system 
 *              should scale up/down one note per button press. When scaling 
 *              down, the system should not scale below a C. When scaling up, 
 *              the system should not scale above a C.
 *
 *
 *              I acknowledge all content contained herein, excluding 
 *              template or example code, is my own original work.
 */

//DEMO: https://drive.google.com/open?id=1Cp_vWEMcDgnbS9sqkNn3v7rr1tVmij1p

#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include <avr/interrupt.h>
#endif

void set_PWM(double frequency) {	
	// Keeps track of the currently set frequency
	// Will only update the registers when the frequency
	// changes, plays music uninterrupted.
	static double current_frequency;
	if (frequency != current_frequency) {

		if (!frequency) {TCCR3B &= 0x08;} //stops timer/counter
		else {TCCR3B |= 0x03;} // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) {OCR3A = 0xFFFF;}
		
		// prevents OCR3A from underflowing, using prescaler 64				// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) {OCR3A = 0x0000;}
		
		// set OCR3A based on desired frequency
		else {OCR3A = (short)(8000000 / (128 * frequency)) - 1;}

		TCNT3 = 0; // resets counter
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	// COM3A0: Toggle PB6 on compare match between counter and OCR3A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	// WGM32: When counter (TCNT3) matches OCR3A, reset counter
	// CS31 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum States{BEGIN, INIT, ADD, SUBTRACT, SOUND, STOP} state;

int count = 0;
unsigned char none = 0x00;
double a[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};

void Tick(){
	switch(state){
		case BEGIN:
		state = INIT;
		break;

		case INIT:
		if((~PINA & 0x07) == 0x01){
			state = SOUND;
			break;
		}
		else if((~PINA & 0x07) == 0x02){
			state = ADD;
			break;
		}
		else if((~PINA & 0x07) == 0x04){
			state = SUBTRACT;
			break;
		}
		else{
			state = INIT;
			break;
		}
		break;
		
		case SOUND:
		state = STOP;
		break;
		
		case ADD:
		state = STOP;
		break;
	
		case SUBTRACT:
		state = STOP;
		break;

		case STOP: 
		state = ((~PINA & 0x07) == 0x00) ? INIT : STOP;
		break;
		
		default:
		state = BEGIN;
		break;
	}

	switch(state){
		case BEGIN:
		break;

		case INIT:
		break;
		
		case SOUND:
		if(none == 0x00){
			PWM_on();
			none = 0x01;
			break;
		}
		else{
			PWM_off();
			none = 0x00;
			break;
		}
		break;

		case ADD:
		if(count < 7){
			count++;
			break;
		}
		else{
			count = 7;
			break;
		}
		break;

		case SUBTRACT:
		if(count > 0){
                        count--;
			break;
                }
                else{
                        count = 0;
			break;
                }
		break;
		
		case STOP:		
		set_PWM(a[count]);
		break;

		default:
		break;
		
	}
}




int main(void){
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	state = BEGIN;

	while(1){
		Tick();
	}

return 0;

}



