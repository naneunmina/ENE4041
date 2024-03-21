#include "msp.h"
#include "Clock.h"
#include <stdio.h>


/**
 * main.c
 */

void sensor_init(){
    P5->SEL0 &= ~0x08;
    P5->SEL1 &= ~0x08;
    P5->DIR |= 0x08;
    P5->OUT &= ~0x08;

    P9->SEL0 &= ~0x04;
    P9->SEL1 &= ~0x04;
    P9->DIR |= 0x04;
    P9->OUT &= ~0x04;

    P7->SEL0 &= ~0xFF;
    P7->SEL1 &= ~0xFF;
    P7->DIR &= ~0xFF;
}

void led_init(){
    P2->SEL0 &= ~0x07;
    P2->SEL1 &= ~0x07;
    P2->DIR |= 0x07;
    P2->OUT &= ~0x07;
}

void main(void)
{

    Clock_Init48MHz();
    led_init();
    sensor_init();

    int sensor1=0;
    int sensor2=0;
    while (1){
        P5->OUT |= 0x08;
        P9->OUT |= 0x04;

        P7->DIR = 0xFF;
        P7->OUT = 0xFF;

        Clock_Delay1us(10);

        P7->DIR = 0x00;

        int i;
        for (i =0; i<10000; i++){
            sensor1 = P7->IN&0x10;
            sensor2 = P7->IN&0x08;
            if (!(sensor1&&sensor2)){
                break;
            }
            Clock_Delay1us(1);
        }

        if (i>700&&i<1000){
            P2->OUT |= ~0x08;
        }
        else {
            P2->OUT &= ~0x07;
        }

        P5->OUT &= ~0x08;
        P9->OUT &= ~0x04;

        Clock_Delay1ms(10);
    }

}
