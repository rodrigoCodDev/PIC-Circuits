/*
 * File:   main.c
 * Author: rodrigo
 *
 * Created on 10 de Agosto de 2025, 10:19
 */


#include <xc.h>

// CONFIG
#pragma config FOSC = INTRCCLK
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config MCLRE = OFF
#pragma config BOREN = ON
#pragma config CP = OFF
#pragma config CPD = OFF

#define _XTAL_FREQ 4000000  // Frequência do oscilador interno (4 MHz)

void main(void) {   
    CMCON = 0x07;
    
    // Output
    TRISIO0 = 0;
    TRISIO1 = 0;
    TRISIO2 = 0;
    
    // Input
    TRISIO3 = 1;
    
    // Initial level
    int level = 0;
    GP0 = 0;
    GP1 = 0;
    GP2 = 0;
    
    while (1) {
        if (GPIO3 == 1) {    
            // Level configuration
            if (level == 3) {
                level = 0;
            } else {
                level++;
            }

            // DC Motor velocity configuration
            if (level == 0) {
                GP0 = 0;
                GP1 = 0;
                GP2 = 0;
            } else if (level == 1) {
                GP0 = 1;
                GP1 = 0;
                GP2 = 0;

            } else if (level == 2) {
                GP0 = 0;
                GP1 = 1;
                GP2 = 0;

            } else if (level == 3) {
                GP0 = 0;
                GP1 = 0;
                GP2 = 1;

            } else {
                level = 0;
                GP0 = 0;
                GP1 = 0;
                GP2 = 0;
            }
            
            __delay_ms(800);
        }
    }
    
    return;
}
