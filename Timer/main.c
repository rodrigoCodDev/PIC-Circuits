/*
 * File:   main.c
 * Author: rodrigo
 *
 * Created on 10 de Agosto de 2025, 22:43
 */


#include <xc.h>
#define _XTAL_FREQ 4000000  // 4MHz

#pragma config FOSC = XT, WDTE = OFF, PWRTE = ON, BOREN = OFF, LVP = OFF, CPD = OFF, WRT = OFF, CP = OFF

const unsigned char segmentMap[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

void sendSegment(unsigned char value) {
    PORTD = value;
}

void activateDigit(unsigned char digit) {
    PORTB = ~(1 << digit);
}

void main() {
    TRISD = 0x00;
    TRISB = 0x00;
    
    PORTD = 0x00;
    PORTB = 0xFF;

    unsigned char minutes = 0;
    unsigned char seconds = 0;

    unsigned int multiplexCounter = 0;

    while (1) {
        unsigned char digits[4];
        digits[0] = minutes / 10;
        digits[1] = minutes % 10;
        digits[2] = seconds / 10;
        digits[3] = seconds % 10;

        for (unsigned char i = 0; i < 4; i++) {
            activateDigit(i);
            sendSegment(segmentMap[digits[i]]);
            __delay_ms(5);
            PORTB = 0xFF;
        }


        multiplexCounter++;
        if (multiplexCounter >= 50) {
            multiplexCounter = 0;

            seconds++;
            
            if (seconds >= 60) {
                seconds = 0;
                minutes++;
                if (minutes >= 60) {
                    minutes = 0;
                }
            }
        }
    }
}


