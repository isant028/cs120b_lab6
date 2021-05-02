/*	Author: Isabella Santiago
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 6  Exercise 3
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


enum Count_STATES {Count_Start, Count_Inc, Count_Dec, Count_Res, Count_Wait, Count_WaitInc, Count_WaitDec} Count_STATE;
unsigned char count = 0x00; //checks to make sure when button is pressed it dec or inc once per sec
void Count_Tick() {
    
	switch(Count_STATE) {
	
    case Count_Start:
        Count_STATE = Count_Wait;
        break; 

    case Count_Wait:
        //PORTC ==0x07
        if ((~PINA & 0x01)==0x01){ //A0 pushed and add 1 to b up to 9
                Count_STATE = Count_Inc;
        }
        else  if ((~PINA & 0x02) ==0x02 ){ //A1 pushed and subs 1 to b till 0
                Count_STATE =  Count_Dec;
            }   
         else if ((~PINA & 0x03)==0x03){ //if both buttons pushed b = 0
            //PORTC = 0;
            Count_STATE = Count_Res;
        }
        else{
            Count_STATE = Count_Wait; 
        }
        break; 

        case Count_Inc:
        Count_STATE = Count_WaitInc;
        break; 

    case Count_WaitInc:
        if ((~PINA & 0x01) == 0x01){//is Ao is still pushed stay here
            Count_STATE = Count_WaitInc;
            count++; 
        }
        
        //else if ((~PINA & 0x00) ==0x00){ //if neother are pushed go to beginning 
           // Count_STATE = Count_Start;
        //}
        else if ((~PINA & 0x03)==0x03){ //if both are pushed, b =0 reset
           // PORTC = 0;
            Count_STATE = Count_Res;
        }
        else  if ((~PINA & 0x02) ==0x02 ){ //A1 pushed and subs 1 to b till 0
                Count_STATE =  Count_Dec;

        }   
        else
            Count_STATE = Count_Wait; 
        
        break;
    
    case Count_Dec:
        Count_STATE = Count_WaitDec;
        break; 
    
    case  Count_WaitDec:
        if ((~PINA & 0x02)==0x02){//if A1 is still pushed stay here
            Count_STATE =  Count_WaitDec;
            count++; }
        
        else if ((~PINA & 0x01) == 0x01){ //is Ao is still pushed stay here
            Count_STATE = Count_WaitInc;
        }
        //else if ((~PINA & 0x00)==0x00){ //if neither is pushed go to start
           // Count_STATE = Count_Start;
       // }
        else if ((~PINA & 0x03)==0x03) //if both are pushed b =0 restart
            //PORTC = 0;
            Count_STATE = Count_Res;
        
        else
            Count_STATE = Count_Wait; 
        
        break;


    case Count_Res:
        //PORTC =0x00; 
        //if ((~PINA & 0x03)==0x00)
            //Count_STATE = Count_Wait;
            //PORTC = 0x00; 
            //PORTC = 0;
    
       // else 
             //Count_STATE = Count_Res;
  
        //else if ((~PINA & 0x02)==0x02){ 
            //Count_STATE = Count_Start;
            //PORTC = 0x00; 
            //PORTC = 0;
        //}
        Count_STATE = Count_Wait;

        break;
	    
        default:
			Count_STATE = Count_Wait;
			break;
    }

    switch (Count_STATE)
    {
       case Count_Start:
            //tmpB = 0x07;
            break; 

        case Count_Wait:
            PORTB = 0x07;
            count = 0; 
            break; 

        case Count_Inc:
            if(PORTB < 9 && !(count%10) ){
                PORTB = PORTB + 0x01; //increments port B 
            }
            count++; 

            break;
    
        case  Count_Dec:
            if(PORTB > 0  && !(count%10) ){
                PORTB = PORTB- 0x01; //decrements port B 
            }
            count++; 
            break;
    
        case Count_Res:
            PORTB = 0x00; //both buttons are pushed and resets to 0
            count = 0x00; 
            break;
	    
        default:
			break;
    }
     //PORTB = tmpB;
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00; 
    TimerSet(100);
    TimerOn();
    PORTB = 0x07; //starts at 0x07
    Count_STATE = Count_Start;
    
	while (1) {
	    Count_Tick();
        while(!TimerFlag){
            TimerFlag = 0; 
    
        }
	}
    return 1; 
}	


