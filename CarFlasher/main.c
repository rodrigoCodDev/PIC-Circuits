/*
 * File:   main.c
 * Author: rodrigo
 *
 * Created on 11 de Agosto de 2025, 21:02
 */


#include <xc.h>
#include <stdbool.h>

#pragma config FOSC = INTOSCIO
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config MCLRE = OFF
#pragma config CP = OFF
#pragma config BOREN = OFF
#pragma config IESO = OFF
#pragma config FCMEN = OFF

#define _XTAL_FREQ 4000000

bool blink_right = false;
bool blink_left = false;
bool blink_both = false;

unsigned int timer = 0;
bool led_state = false;

void main(void) {
    TRISIO = 0b00110100; // GP2, GP4, GP5 as input
    GPIO = 0x00;
    ANSEL = 0x00;
    CMCON0 = 0x07;

    while (1) {
        // Check buttons
        if (GP4 == 1) {
            blink_right = !blink_right;
            __delay_ms(200);
            while (GP4 == 1);
        }

        if (GP5 == 1) {
            blink_left = !blink_left;
            __delay_ms(200);
            while (GP5 == 1);
        }

        if (GP2 == 1) {
            blink_both = !blink_both;
            __delay_ms(200);
            while (GP2 == 1);
        }

        // Timer logic
        __delay_ms(10); // Small delay to simulate time slice
        timer += 10;

        if (timer >= 500) {
            timer = 0;
            led_state = !led_state;

            // Update LEDs
            GP0 = (blink_right || blink_both) ? led_state : 0;
            GP1 = (blink_left  || blink_both) ? led_state : 0;
        }
    }
}

