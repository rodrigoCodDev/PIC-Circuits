/*
 * File:   main.c
 * Author: rodrigo
 *
 * Created on 12 de Agosto de 2025, 22:17
 */

#include <xc.h>

// CONFIG
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config MCLRE = OFF
#pragma config BOREN = OFF
#pragma config CP = OFF
#pragma config CPD = OFF

#define _XTAL_FREQ 4000000

void main() {
    TRISIO = 0b00001000; // GP3 input, GP0 output
    GPIO = 0x00;

    unsigned char state = 0;
    unsigned char lastButton = 0;

    while (1) {
        // Button press detection with debounce
        if (GP3 == 1 && lastButton == 0) {
            state++;
            if (state > 3) state = 0;
            
            __delay_ms(50);
        }
        lastButton = GP3;

        // LED behavior using if statements
        if (state == 0) {
            GP0 = 0; // OFF
        }
        else if (state == 1) {
            GP0 = 1; // ON
        }
        else if (state == 2) {
            GP0 = 1;
            __delay_ms(1000);
            GP0 = 0;
            __delay_ms(1000);
        }
        else if (state == 3) {
            GP0 = 1;
            __delay_ms(600);
            GP0 = 0;
            __delay_ms(600);
        }
    }
}
