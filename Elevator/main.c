/*
 * File:   main.c
 * Author: rodrigo
 *
 * Created on 18 de Agosto de 2025, 22:47
 */
    
#include <xc.h>

// --- CONFIGURATION BITS ---
#pragma config FOSC = XT        // XT oscillator
#pragma config WDTE = OFF       // Watchdog Timer disabled
#pragma config PWRTE = OFF      // Power-up Timer disabled
#pragma config BOREN = OFF      // Brown-out Reset disabled
#pragma config LVP = OFF        // Low-Voltage Programming disabled
#pragma config CP = OFF         // Code protection disabled

#define _XTAL_FREQ 4000000      // 4MHz Crystal Oscillator Frequency

// --- PIN DEFINITIONS ---
#define BTN_FLOOR_0      PORTBbits.RB0
#define BTN_FLOOR_1      PORTBbits.RB1
#define BTN_FLOOR_2      PORTBbits.RB2
#define BTN_FLOOR_3      PORTBbits.RB3
#define BTN_CLOSE_DOOR   PORTBbits.RB4
#define BTN_ALERT        PORTBbits.RB5

#define LED_DOOR_OPEN    PORTCbits.RC0
#define LED_ALERT        PORTCbits.RC1
#define BUZZER           PORTCbits.RC2

// --- CONSTANTS ---
#define FLOOR_TRAVEL_TIME_MS  2000
#define DOOR_CLOSING_TIME_MS  1000
#define ARRIVAL_BEEP_DURATION_MS 500
#define ALERT_BEEP_ON_MS      200
#define ALERT_BEEP_OFF_MS     800

// --- NEW: Constants for the automatic door closing timer ---
#define MAIN_LOOP_DELAY_MS    50     // Each loop cycle takes 50ms (our "tick")
#define AUTO_CLOSE_TIMEOUT_S  10     // 10 seconds timeout
#define AUTO_CLOSE_TICKS      (AUTO_CLOSE_TIMEOUT_S * 1000 / MAIN_LOOP_DELAY_MS) // -> 200 ticks

// Segment map for a common-cathode 7-segment display
const unsigned char segment_map[4] = {0x3F, 0x06, 0x5B, 0x4F};

// --- GLOBAL VARIABLES ---
enum {IDLE_DOOR_OPEN, DOOR_CLOSING, MOVING_UP, MOVING_DOWN, ARRIVED_AT_FLOOR} elevator_state;

unsigned char current_floor = 0;
unsigned char requested_floor = 0;
char alert_active = 0;
// --- NEW: Timer counter variable ---
unsigned int idle_timer_ticks = 0;

// --- HELPER FUNCTIONS ---
void update_display(unsigned char floor) {
    if (floor < 4) PORTD = segment_map[floor];
}

void arrival_beep() {
    BUZZER = 1;
    __delay_ms(ARRIVAL_BEEP_DURATION_MS);
    BUZZER = 0;
}

// --- MAIN FUNCTION ---
void main(void) {
    // --- INITIAL CONFIGURATION ---
    TRISB = 0xFF; 
    TRISC = 0x00;
    TRISD = 0x00;
    OPTION_REGbits.nRBPU = 0; 
    PORTC = 0x00;
    PORTD = 0x00;

    // --- INITIAL STATE ---
    current_floor = 0;
    requested_floor = 0;
    elevator_state = IDLE_DOOR_OPEN;
    LED_DOOR_OPEN = 1;
    update_display(current_floor);

    // --- MAIN LOOP ---
    while(1) {
        // --- ALERT BUTTON LOGIC (highest priority) ---
        if (!BTN_ALERT) {
            __delay_ms(50);
            if (!BTN_ALERT) {
                alert_active = !alert_active;
                LED_ALERT = alert_active;
                while(!BTN_ALERT);
            }
        }

        // --- EXECUTION LOGIC BASED ON ALERT STATE ---
        if (alert_active) {
            BUZZER = 1;
            __delay_ms(ALERT_BEEP_ON_MS);
            BUZZER = 0;
            __delay_ms(ALERT_BEEP_OFF_MS);
        } else {
            // --- NORMAL ELEVATOR LOGIC ---
            if (elevator_state == IDLE_DOOR_OPEN) {
                LED_DOOR_OPEN = 1;
                
                // Check for floor requests
                if (!BTN_FLOOR_0) requested_floor = 0;
                if (!BTN_FLOOR_1) requested_floor = 1;
                if (!BTN_FLOOR_2) requested_floor = 2;
                if (!BTN_FLOOR_3) requested_floor = 3;

                // --- NEW: Auto-close timer logic ---
                if (requested_floor != current_floor) {
                    // A request is pending, start the timer
                    idle_timer_ticks++;
                    
                    // Check if timer expired OR if close button is pressed
                    if (idle_timer_ticks >= AUTO_CLOSE_TICKS || !BTN_CLOSE_DOOR) {
                        elevator_state = DOOR_CLOSING;
                    }
                } else {
                    // No request pending, so keep the timer reset
                    idle_timer_ticks = 0;
                }
            } 
            else if (elevator_state == DOOR_CLOSING) {
                LED_DOOR_OPEN = 0;
                __delay_ms(DOOR_CLOSING_TIME_MS);
                
                if (requested_floor > current_floor) elevator_state = MOVING_UP;
                else if (requested_floor < current_floor) elevator_state = MOVING_DOWN;
                else elevator_state = IDLE_DOOR_OPEN;
            } 
            else if (elevator_state == MOVING_UP) {
                if (current_floor < requested_floor) {
                    __delay_ms(FLOOR_TRAVEL_TIME_MS);
                    current_floor++;
                    update_display(current_floor);
                } else {
                    elevator_state = ARRIVED_AT_FLOOR;
                }
            } 
            else if (elevator_state == MOVING_DOWN) {
                if (current_floor > requested_floor) {
                    __delay_ms(FLOOR_TRAVEL_TIME_MS);
                    current_floor--;
                    update_display(current_floor);
                } else {
                    elevator_state = ARRIVED_AT_FLOOR;
                }
            } 
            else if (elevator_state == ARRIVED_AT_FLOOR) {
                arrival_beep();
                elevator_state = IDLE_DOOR_OPEN;
                // --- NEW: Reset the timer upon arrival ---
                idle_timer_ticks = 0;
            }
        }
        
        // --- NEW: Main loop delay that defines our software timer "tick" ---
        __delay_ms(MAIN_LOOP_DELAY_MS);
    }
}