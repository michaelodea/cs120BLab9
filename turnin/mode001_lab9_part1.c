/*              Name & E-mail: Michael O'Dea, mode001@ucr.edu
 *              Lab Section: 025
 *              Assignment: Lab #9  Exercise #1 
 *              Exercise Description: Using the ATmega1284’s PWM functionality, *              design a system that uses three buttons to select one of three
 *              tones to be generated on the speaker. When a button is pressed,
 *              the tone mapped to it is generated on the speaker. Criteria:
 *              Use the tones C4, D4, and E4 from the table in the introductio
 *              section. When a button is pressed and held, the tone mapped 
 *              to it is generated on the speaker. When more than one button 
 *              is pressed simultaneously, the speaker remains silent. When no  *	       buttons are pressed, the speaker remains silent.
 *
 *
 *              I acknowledge all content contained herein, excluding 
 *              template or example code, is my own original work.
 */

//DEMO: https://drive.google.com/open?id=1B40Qp1HUnuzry4-mhOF2MIgOFfscGZ4h

#ifdef _SIMULATE_
#include <avr/io.h>
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
		if (frequency < 0.954){OCR3A = 0xFFFF;}
		
		// prevents OCR3A from underflowing, using prescaler 64			       // 31250 is largest frequency that will not result in underflow
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

void Tick();
enum States{BEGIN, INIT, OFF, ON} state;

void Tick(){
	switch(state){
		case BEGIN:
		state = INIT;
		break;
		
		case INIT:
		if((~PINA & 0x07) != 0x00){
			state = ON;
			break;
		}
		else{
			state = INIT;
			break;
		}
		break;

		case ON:
		if((~PINA & 0x07) != 0x00){
			state = ON; 
			break;
		}
		else if((~PINA & 0x07) == 0x03){
                        state = OFF;
                        break;
                }
                else if((~PINA & 0x07) == 0x05){
                        state = OFF;
                        break;
                }
                else if((~PINA & 0x07) == 0x06){
                        state = OFF;
                        break;
                }
                else if((~PINA & 0x07) == 0x07){
                        state = OFF;
                        break;
                }
		else{
			state = OFF; 
			break;
		}
		break;

		case OFF:
		state = INIT;
		break;
	
		default:
		break;
	}

	switch(state){
		case BEGIN:
		break;

		case INIT:
		set_PWM(0);
		break;

		case ON:
		if((~PINA & 0x07) == 0x01){
			set_PWM(261.63);
			break;
		}
		else if((~PINA & 0x07) == 0x02){
			set_PWM(293.66);
			break;
		}
		else if((~PINA & 0x07) == 0x04){
			set_PWM(329.63);
			break;
		}

		case OFF:
		set_PWM(0);
		break;
		
		default:
		break;

	}
}

int main(void){
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	state = BEGIN;
	PWM_on();

	while(1){
		Tick();
	}

return 0;

}
