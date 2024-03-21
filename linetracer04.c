#include "msp.h"
#include "Clock.h"
#include <stdio.h>


/**
 * main.c
 */

void sistick_init(){
    SysTick->LOAD = 0x00FFFFFF;
    SysTick->CTRL= 0x00000005;
}

void systick_wait1ms(){
    SysTick->LOAD=0x0BB80;
    SysTick->VAL=0;
    while((SysTick->CTRL & 0x00010000)==0) {};
}
void systick_wait1s(){
    int i;
    int cnt=1000;
    for (i=0; i<cnt; i++) {
        systick_wait1ms();
    }
}

void switch_init(){
    //Setup switch as GPIO
    P1->SEL0 &= ~0x12;
    P1->SEL1 &= ~0x12;

    P1->DIR &= ~0x12;

    P1->REN |= 0x12;

    P1->OUT |= 0x12;
}

void main(void)
{

    Clock_Init48MHz();

    P2->SEL0 &= ~0x07;
    P2->SEL1 &= ~0x07;
    P2->DIR |= 0x07;
    P2->OUT &= ~0x07;

    switch_init();
    sistick_init();

    int count=0;
    int sw1;
    while (1){
        systick_wait1ms();
        count++;
        sw1=P1->IN & 0x02;
        if(!sw1){
            printf("Current Time : %dm %ds %dms\n",(count/1000)/60,(count/1000)%60,count%1000);
        }
    }

}
