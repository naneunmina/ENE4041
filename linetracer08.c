#include "msp.h"
#include "Clock.h"
#include <stdio.h>

void (*TimerA2Task)(void);
void TimerA2_init(void(*task)(void), uint16_t period){
    TimerA2Task = task;
    TIMER_A2->CTL = 0x0280;
    TIMER_A2->CCTL[0] = 0x0010;
    TIMER_A2->CCR[0] = period-1;
    TIMER_A2->EX0 = 0x0005;
    NVIC->IP[3] = (NVIC->IP[3]&0xFFFFFF00)|0x00000040;
    NVIC->ISER[0] = 0x00001000;
    TIMER_A2->CTL|=0x0014;
}

void TA2_0_IRQHandler(void){
    TIMER_A2->CCTL[0] &= ~0x0001;
    (*TimerA2Task)();
}

void task(){
    printf("interrupt occurs!\n");
}

void main(void)
{
    Clock_Init48MHz();
    TimerA2_init(&task,5000);
    while(1){};

}
