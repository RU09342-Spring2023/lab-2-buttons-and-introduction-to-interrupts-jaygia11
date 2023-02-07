/*
 * OccupancyDetector.c
 *
 *  Created on: Jan 31, 2023
 *      Author: Jay Gianduso
 */

#include <msp430.h>

void gpioInit();


int main(void){
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    #define ARMED_STATE 0
    #define WARNING_STATE 1
    #define ALERT_STATE 2

    gpioInit();                             // Initialize all of the gpio pins

    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                               // to activate previously configured port settings

    char state = ARMED_STATE;

    while(1){

        switch(state){

        case ARMED_STATE:{
            P1OUT &= ~BIT0;                      // Turns off red LED
            P6OUT |= BIT6;                       // Turns on green LED
            __delay_cycles(100000);              // Delay for .1 seconds
            P6OUT &= ~BIT6;                      // Turns off green LED
            __delay_cycles(2900000);             // Delay for 3 seconds

            if(!(P2IN & BIT3)){
                state = WARNING_STATE;
            }
            break;

            }

        case WARNING_STATE:{
            P6OUT &= ~BIT6;                      // Turns off green LED
            int i;

            for(i = 1; i <= 10; i++){        // Runs the warning blinking LED for 10 seconds
                P1OUT |= BIT0;                   // Turns on red LED
                __delay_cycles(500000);          // Delay for .5 seconds
                P1OUT &= ~BIT0;                  // Turns off red LED
                __delay_cycles(500000);          // Delay for .5 seconds
                if(P2IN & BIT3){               // Sends back to ARMED if stops detecting intruder
                    state = ARMED_STATE;
                    break;
                }
                }
            if(P2IN & BIT3){               // Sends back to ARMED if stops detecting intruder
                                state = ARMED_STATE;
                                break;
                            }
            state = ALERT_STATE;
            break;
            }
        case ALERT_STATE:{

            while(1){
                P1OUT |= BIT0;
                if(!(P4IN & BIT1)){
                    state = ARMED_STATE;
                    P1OUT &= ~BIT0;
                break;
                }
            }


            }

        }



    }
}

void gpioInit(){

    P1OUT &= ~BIT0;             // Sets P1.0 to 0
    P6OUT &= ~BIT6;             // Sets P6.6 to 0
    P1DIR |= BIT0;              // Configure P1.0 to an Output
    P6DIR |= BIT6;              // Configure P6.6 to an Output
    P2DIR &= ~BIT3;             // Configure P2.3 to an Input
    P4DIR &= ~BIT1;             // Configure P4.1 to an Input

    P2REN |= BIT3;              // Enable Resistor on P2.3
    P2OUT |= BIT3;              // Configure Resistor on P2.3 to be Pullup
    P4REN |= BIT1;              // Enable Resistor on P4.1
    P4OUT |= BIT1;              // Configure Resistor on P4.1 to be Pullup
}

