/*
 * File:   main.c
 * Author: rodrigo
 *
 * Created on 13 de Agosto de 2025, 20:55
 */

#include <xc.h>

// --- CONFIGURATION BITS ---
#pragma config FOSC = XT        
#pragma config WDTE = OFF       
#pragma config PWRTE = OFF      
#pragma config BOREN = OFF      
#pragma config LVP = OFF        
#pragma config CPD = OFF        
#pragma config WRT = OFF        
#pragma config CP = OFF         

#define _XTAL_FREQ 4000000      

const unsigned char segment_map[10] = {
    0x3F, // 0 = 0b00111111
    0x06, // 1 = 0b00000110
    0x5B, // 2 = 0b01011011
    0x4F, // 3 = 0b01001111
    0x66, // 4 = 0b01100110
    0x6D, // 5 = 0b01101101
    0x7D, // 6 = 0b01111101
    0x07, // 7 = 0b00000111
    0x7F, // 8 = 0b01111111
    0x6F  // 9 = 0b01101111
};

void adc_init(void) {
    // ADCON1: Configure ADC port and result justification
    // 0x8E = 0b10001110 -> Result is right-justified (ADFM=1), AN0 is analog (PCFG=1110)
    ADCON1 = 0x8E;

    // ADCON0: Configure ADC clock, channel, and enable ADC
    // 0x41 = 0b01000001 -> Fosc/8 clock, Channel 0 (AN0), ADC is ON
    ADCON0 = 0x41;
}

unsigned int adc_read(void) {
    __delay_us(20);      // Acquisition time delay (tACQ)
    GO_nDONE = 1;        // Start ADC conversion
    while(GO_nDONE);     // Wait for conversion to complete
    
    // Combine ADRESH and ADRESL for the 10-bit result
    return ((ADRESH << 8) | ADRESL);
}

void main(void) {
    TRISA = 0xFF;    
    TRISB = 0x00;    
    PORTB = 0x00;    

    adc_init();      

    while(1) {
        // Read the 10-bit value (0-1023) from the potentiometer
        unsigned int adc_value = adc_read();

        // Map the 0-1023 ADC range to a 0-9 digit
        unsigned char digit = (adc_value * 10) / 1024;

        // Safety check to prevent array out-of-bounds access
        if (digit > 9) {
            digit = 9;
        }

        // Send the corresponding segment pattern to the display on PORTB
        PORTB = segment_map[digit];
        __delay_ms(100);
    }
}
