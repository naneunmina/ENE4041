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

void moter_init(){
    P3->SEL0 &= ~0xC0;
    P3->SEL1 &= ~0xC0;
    P3->DIR |= 0xC0;
    P3->OUT &= ~0xC0;

    P5->SEL0 &= ~0x30;
    P5->SEL1 &= ~0x30;
    P5->DIR |= 0x30;
    P5->OUT &= ~0x30;

    P2->SEL0 &= ~0xC0;
    P2->SEL1 &= ~0xC0;
    P2->DIR |= 0xC0;
    P2->OUT &= ~0xC0;
}

void main(void)
{

    Clock_Init48MHz();
    led_init();
    sensor_init();
    moter_init();

    int sensor1=0;
    int sensor2=0;
    int sensor3=0;
    int sensor4=0;
    int sensor5=0;
    int sensor6=0;
    while (1){
        P5->OUT |= 0x08;
        P9->OUT |= 0x04;

        P7->DIR = 0xFF;
        P7->OUT = 0xFF;

        Clock_Delay1us(10);

        P7->DIR = 0x00;
        Clock_Delay1us(1000);

        sensor1 = P7->IN&0x10;
        sensor2 = P7->IN&0x08;
        sensor3 = P7->IN&0x02;
        sensor4 = P7->IN&0x04;
        sensor5 = P7->IN&0x20;
        sensor6 = P7->IN&0x40;

        if (sensor1&&sensor2&&sensor3&&sensor4&&sensor5&&sensor6){
            P2->OUT &= ~0xC0;
        }
        else if (sensor1&&sensor2){
            P5->OUT &= ~0x30;
            P2->OUT |= 0xC0;
            P3->OUT |= 0xC0;
            Clock_Delay1us(1000);
            P2->OUT &= ~0xC0;
            Clock_Delay1us(9000);
        }
        else {
            P2->OUT &= ~0xC0;
        }
    }

}
