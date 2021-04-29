/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 6  Exercise 2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
//dont change above 


enum ledBlink {start, led1, led1wait, led2, led2wait, led3, led3wait, res } blinkState;
void Timer_Tick(){

    switch (blinkState)
    {
    case start:
        blinkState = led1;
        break;

    case led1:
       if (~PINA & 01){
           blinkState = led1wait; 
       }
        else{
            blinkState = led2;
        }
        
        break;

    case led2: 
        if (~PINA & 01){
           blinkState = led2wait; 
       }
       else{
           blinkState = led3;
       }
        
        break;
    
    case led3:
        if (~PINA & 01){
           blinkState = led3wait; 
       }
       else{
           blinkState = led1;
       }
        
        break; 
    
     case led1wait:
       if (~PINA & 01){
           blinkState = led1; 
        }
        break;  
    
    case led2wait:
       if (~PINA & 01){
           blinkState = led2; 
        }
        break;

    case led3wait:
       if (~PINA & 01){
           blinkState = led3; 
        }
        break;

    default:
        blinkState = start; 
       break;
    }
    
    switch (blinkState)
    {

    case led1:
        PORTB = 0x01;
        break;

    case led2: 
        PORTB = 0x02;
        break;
    
    case led3:
        PORTB = 0x04;
        break; 
    
    case led1wait:
        PORTB = 0x01;
        break; 
    
     case led2wait:
        PORTB = 0x02;
        break; 

     case led3wait:
        PORTB = 0x04;
        break; 

    default:
        break;
    }

}
int main(void){
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    TimerSet(300);
    TimerOn();
    blinkState = start; 
    while(1){
        Timer_Tick(); 
        while(!TimerFlag);
            TimerFlag = 0; 
        }
        return 1; 
    }


