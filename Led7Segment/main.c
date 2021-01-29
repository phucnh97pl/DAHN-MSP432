#include "msp.h"
#include <stdint.h>

unsigned int bits[8] = {BIT0, BIT1, BIT2, BIT3, BIT4, BIT5, BIT6, BIT7};

uint8_t seven_seg_digits[10][7] = { { 1,1,1,1,1,1,0 },  // = 0
        { 0,1,1,0,0,0,0 },  // = 1
        { 1,1,0,1,1,0,1 },  // = 2
        { 1,1,1,1,0,0,1 },  // = 3
        { 0,1,1,0,0,1,1 },  // = 4
        { 1,0,1,1,0,1,1 },  // = 5
        { 1,0,1,1,1,1,1 },  // = 6
        { 1,1,1,0,0,0,0 },  // = 7
        { 1,1,1,1,1,1,1 },  // = 8
        { 1,1,1,0,0,1,1 }   // = 9
        };


void sevenSegWrite(unsigned int digit){
	unsigned int segCount;
	for (segCount = 0; segCount < 7; ++segCount){
		if(seven_seg_digits[digit][segCount] == 1){
			// turn on the segment
			P4OUT |= bits[segCount];
		}
		else{
			// turn off the segment
			P4OUT &= ~bits[segCount];
		}
	}
}

int main(void) {
    volatile uint32_t i;

    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT
    P4DIR |= 0x00FF;                          // set pins as outputs

    unsigned int digit = 0;
    while (1)                               // continuous loop
    {
    	sevenSegWrite(digit);
    	digit++;
    	if(digit == 10){
    		digit = 0;
    	}
        for (i = 100000; i > 0; i--);        // Delay
    }
}