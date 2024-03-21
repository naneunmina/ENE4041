#include "msp.h"
#include "Clock.h"
#include <stdio.h>

void systick_init(void){
    SysTick->LOAD=0x00FFFFFF;
    SysTick->CTRL=0x00000005;
}

void systick_wait1ms(){
    SysTick->LOAD=48000;
    SysTick->VAL=0;
    while((SysTick->CTRL & 0x00010000)==0){};
}

void systick_wait1s(){
    int i;
    int cnt=1000;

    for(i=0;i<cnt;i++){systick_wait1ms();}
}

void sensor_init(){
    //0,2,4,6 IR Emitter
    P5->SEL0 &= ~0x08;
    P5->SEL1 &= ~0x08;  //GPIO
    P5->DIR |= 0x08;    //OUTPUT
    P5->OUT &= ~0x08;   //turn off 4 even IR LEDs

    //1,3,5,7 IR Emitter
    P9->SEL0 &= ~0x04;
    P9->SEL1 &= ~0x04;  //GPIO
    P9->DIR |= 0x04;    //OUTPUT
    P9->OUT &= ~0x04;   //turn off 4 odd IR LEDs

    //0~7 IR Sensor
    P7->SEL0 &= ~0xFF;
    P7->SEL1 &= ~0xFF;  //GPIO
    P7->DIR &= ~0xFF;   //INPUT
}

void pwm_init34(uint16_t period, uint16_t duty3, uint16_t duty4){
    //CCR0 Period
    TIMER_A0->CCR[0]=period;

    //divide by 1
    TIMER_A0->EX0=0x0000;

    //toggle/reset
    TIMER_A0->CCTL[3]=0x0040;
    TIMER_A0->CCR[3]=duty3;
    TIMER_A0->CCTL[4]=0x0040;
    TIMER_A0->CCR[4]=duty4;

    TIMER_A0->CTL=0x02F0;

    //set alternative
    P2->DIR|=0xC0;
    P2->SEL0|=0xC0;
    P2->SEL1&=~0xC0;
}

void motor_init(void){
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

    pwm_init34(7500, 0, 0);
}

void move(uint16_t leftDuty, uint16_t rightDuty){
    P3->OUT |= 0xC0;
    TIMER_A0->CCR[3]=leftDuty;
    TIMER_A0->CCR[4]=rightDuty;
}

void left_forward(){
    P5->OUT &= ~0x10;
}

void left_backward(){
    P5->OUT |= 0x10;
}

void right_forward(){
    P5->OUT &= ~0x20;
}

void right_backward(){
    P5->OUT |= 0x20;
}

void go_straight(int s){
    left_forward();
    right_forward();
    move(s+40,s);
}

void go_back(int l_s, int r_s){
    left_backward();
    right_backward();
    move(l_s,r_s+30);
}

void turn_lr(int dir, int left_s, int right_s){
    if(dir){
        left_backward();
        right_forward();
    }
    else {
        left_forward();
        right_backward();
    }
    move(left_s,right_s);
}

void rotate(int dir, int s){
    move(0,0);
    turn_lr(dir,s,s);
    systick_wait1s();
}

void stay_rotate(uint8_t end){
    uint8_t sensorValues = SensorIn();
    while (sensorValues!=end){
        sensorValues = SensorIn();
    }
}
void stop1s(){
    move(0,0);
    systick_wait1s();
}

void trace(int speed, uint8_t sensorValues){
    if ((sensorValues & 0x03) == 0x03) { //1,2
        turn_lr(0,speed,speed+700);
    }
    else if ((sensorValues & 0xc0) == 0xc0) { //7,8
        turn_lr(1,speed+700,speed);
    }
    else if ((sensorValues & 0x06) == 0x06) { //2,3
        turn_lr(0,speed,speed+600);
        }
    else if ((sensorValues & 0x60) == 0x60) { //6,7
        turn_lr(1,speed+600,speed);
    }
    else if ((sensorValues & 0x0c) == 0x0c) { //3,4
        turn_lr(0,speed-500,speed+500);
    }
    else if ((sensorValues & 0x30) == 0x30) {  //5,6
        turn_lr(1,speed+500,speed-500);
    }
    else if ((sensorValues & 0x18) == 0x18) { //4,5
       go_straight(speed);
    }
    else {
        go_back(400,500);
    }
}

uint8_t SensorIn(){
    P7->DIR = 0xFF;
    P7->OUT = 0xFF;
    Clock_Delay1us(10);

    P7->DIR = 0x00;
    Clock_Delay1us(1000);
    return P7->IN;
}

void main(void)
{
    Clock_Init48MHz();
    systick_init();
    sensor_init();
    motor_init();
    int flag=0;
    int ls=4000;
    int fs=1000;
    int i;

    while(1){
        P5->OUT |= 0x08;
        P9->OUT |= 0x04;

        uint8_t sensorValues = SensorIn();

        if (sensorValues == 0x7e && flag==0x7f){ //7th - STOP
            move(0, 0);
        }
        else if (sensorValues == 0x99 && flag==0x00){ //1st - STOP AND WAIT
            for (i=0; i<3; i++) stop1s();
            flag|=0x01;
        }
        else if (sensorValues==0x5a && flag==0x01){ //2nd - REPEAT
            turn_lr(1,1000,1000);
            for (i=0; i<100; i++) systick_wait1ms();
            int a=0;
            for (; a<3; a++){
                stop1s();
                go_straight(1000);
                systick_wait1s();
                stop1s();
                go_back(1000,1000);
                systick_wait1s();
            }
            flag|=0x02;
        }
        else if (sensorValues==0x19 && (flag&0x02)==0x02){ //3rd - DECELERATION
            if (flag==0x03){
                stop1s();
            }
            int dc=(--ls)/4;
            trace(dc, sensorValues);
            if (ls<2000) ls++;
            flag|=0x04;
        }
        else if (sensorValues==0x98 && flag==0x07){ //4th - ACCELERATION
            turn_lr(0,1000,1000);
            for (i=0; i<60; i++) systick_wait1ms();
            stop1s();
            for (i=0; i<1100; i++){
                fs+=2;
                go_straight(fs);
                systick_wait1ms();
            }
            flag|=0x08;
        }
        else if (sensorValues == 0x58 && flag==0x0f){ //5th - ROTATE
            stop1s();
            rotate(1,1500); //left
            stay_rotate(0x58);
            for (i=0; i<1000; i++){
                trace(1000, sensorValues);
                sensorValues = SensorIn();
            }
            stop1s();
            rotate(0,1500); //right
            stay_rotate(0x58);
            stop1s();
            rotate(1,1500); //left
            stay_rotate(0x58);
            flag|=0x10;
        }
        else if (sensorValues  == 0x1a && flag==0x1f){ //6th - ROUND
            turn_lr(1,1000,1000);
            for (i=0; i<150; i++) systick_wait1ms();
            while (sensorValues != 0x18){
                trace(1000,sensorValues);
                sensorValues = SensorIn();
            }
            stop1s();
            rotate(1,1500);
            stay_rotate(0x58);
            move(0,0);

            flag|=0x20;
        }
        else if (sensorValues == 0x58 && flag==0x3f){
            while (sensorValues != 0x18){
                 trace(1000,sensorValues);
                 sensorValues = SensorIn();
            }
            stop1s();
            rotate(1,1500);
            stay_rotate(0x1a);
            move(0,0);
            flag|=0x40;
            turn_lr(1,1000,1000);
            for (i=0; i<150; i++) systick_wait1ms();
        }
        else trace (1000,sensorValues);
        systick_wait1ms();
    }
}
