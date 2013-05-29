/*************************************************************************
   
  main.c
  HKUST Smartcar 2013 Sensor Group

  Authoured by:
  John Ching
  Louis Mo
  Yumi Chan

  Hardware By:
  Zyan Shao
*************************************************************************/
#include "common.h"
#include "include.h"
#include "stdlib.h"

/*************************************************************************
Global Varaible
*************************************************************************/
char g_char_mode=0;                 // debug and testing mode
volatile u32 g_u32_systemclock=0;   // systemclock counter

//these two increment when pulse is received from encoder, zeroed when timed cycle PIT1 comes around
volatile u32 g_u32encoder_lf=0;
volatile u32 g_u32encoder_rt=0;

volatile u32 g_u32encoder_lflast=0;
volatile u32 g_u32encoder_rtlast=0;

volatile int motor_deadzone_left,motor_deadzone_right;

u16 motor_test=0;

u8 todis[];//for sprintf usage

/*************************************************************************
Function header
*************************************************************************/
void motor_init(void){
  
  motor_deadzone_left=100;
  motor_deadzone_right=100;
     /*connection config:
     
     Hardware        DIR             PWM             Physical location
     ---------------+---------------+---------------+-----------------
     Motor Left      PTD7            ftm0ch6        Motor0
     Motor Right     PTE11           ftm0ch5        Motor1

     */
  gpio_init(PORTC,3,GPO,1);
  gpio_init(PORTC,4,GPO,0);//this is DIR
  
  FTM_PWM_init(FTM0,CH2,10000,0);//motor takes 0-1000 pwm values for duty
  FTM_PWM_init(FTM0,CH3,10000,0);//motor takes 0-1000 pwm values for duty
  
  DisableInterrupts;
  exti_init(PORTC,18,rising_up);             //inits left encoder interrupt capture
  exti_init(PORTC,19,rising_up);            //inits right encoder interrupt capture
  
  EnableInterrupts;
  
//  while(g_u32encoder_lf<5&&motor_deadzone_left<200){
//    motor_deadzone_left++;
//    FTM_PWM_Duty(FTM0,CH6,motor_deadzone_left);
//    delayms(100);
//  }
//  while(g_u32encoder_rt<5&&motor_deadzone_right<200){
//    motor_deadzone_right++;
//    FTM_PWM_Duty(FTM0,CH6,motor_deadzone_right);
//    delayms(100);
//  }
    
  
    
  
}
void main(void){
  gpio_init(PORTE, 24, GPO, 0);
  uart_init(UART3, 115200);
  uart_init(UART0, 115200);
  
  gpio_turn(PORTE, 24);
        uart_sendStr(UART3,"The mode now is 6: Motor Control test");
  gpio_turn(PORTE, 24);
  
        motor_init();
        printf("\nEverything Initialized alright\n");
        delayms(3000);
        
         
          FTM_PWM_Duty(FTM0,CH2,200); // left motor PWM
          FTM_PWM_Duty(FTM0,CH3,200); // right motor PWM
   /*
        while(1)
        { 
          //printf("\n\fInput 0-9 Motor Speed, Currently:%d",motor_test);
          //motor_test=100*(uart_getchar(UART3)-48);
          
          printf("\n\f Input direction : 0 or 1");
          motor_test = uart_getchar(UART3)-48;
         
          FTM_PWM_Duty(FTM0,CH2,200); // left motor PWM
          FTM_PWM_Duty(FTM0,CH3,200); // right motor PWM

          if (motor_test){
            gpio_init(PORTC,3,GPO,1); // left motot DIR
            gpio_init(PORTC,4,GPO,0); // right motot DIR
          }else{
            gpio_init(PORTC,3,GPO,0); // left motot DIR
            gpio_init(PORTC,4,GPO,1); // right motot DIR
          }
          
        }  
          */
  /*
  gpio_init(PORTE,24,GPO,1);
  gpio_init(PORTE,25,GPO,1);
  gpio_init(PORTE,26,GPO,1);
  gpio_init(PORTE,27,GPO,1);
  
  uart_init(UART3, 115200);
  uart_init(UART0, 115200);
   printf("\n\fWelcome to the SmartCar 2013 Sensor team developement system\n\f");
  
  while(1){
   uart_sendStr(UART0,"\n\fWelcome to the SmartCar 2013 Sensor team developement system\n\f");
   printf("\n\ftesing\n\f");
    for(int i=24;i<28;i++){
      gpio_turn(PORTE,i);
      delayms(300);
    }
  }
  */
}
  