#include "common.h"
#include "include.h"
#include "linearccd.h"

#define BALANCEKP 0.1
#define BALANCEKI 0

//extern volatile u32 g_u32_systemclock;   // systemclock counter
extern volatile int g_int_ccd_operation_state;

extern volatile u32 g_u32encoder_lf;
extern volatile u32 g_u32encoder_rt;

extern volatile u32 g_u32encoder_lflast;
extern volatile u32 g_u32encoder_rtlast;

extern volatile int motor_deadzone_left,motor_deadzone_right;

extern u32 balance_centerpoint_set;


//for the motor command loop

/******************************************
 Variables for control PIT
 ******************************************/

volatile int control_omg, control_tilt, control_integrate;
volatile int motor_command_left,motor_command_right;
volatile int motor_turn_left,motor_turn_right;
volatile int motor_command_balance;

extern volatile float accl_tilt16;

u8 system_mode=0;

void PIT0_IRQHandler(void)
{
      //gpio_turn(PORTB, 18);       // CCD Clock Rising and Failing edge   
      //g_u32_systemclock++;
      PIT_Flag_Clear(PIT0);       
}

void PIT1_IRQHandler(void)
{
    //for encoder testing
    DisableInterrupts;
    printf("\n\fg_u32encoder_lf:%d",g_u32encoder_lf);
    printf("\n\fg_u32encoder_rt:%d",g_u32encoder_rt);
    
    g_u32encoder_lf=0;
    g_u32encoder_rt=0;
    
    PIT_Flag_Clear(PIT1);
    EnableInterrupts;
}

void encoder_counter(void){
  /*connection config:
     
     Hardware        Port name       Program name    Physical location
     ---------------+---------------+---------------+-----------------
     encoder_left    PTA8            exti pta        servo1
     encoder_right   PTA9            exti pta        servo2
   */
 
    u8  n=0;
    n=8;
    if(PORTA_ISFR & (1<<n)) 
    {
        PORTA_ISFR  |= (1<<n);
        
        g_u32encoder_lf++;
        
    } 
    
    n=9;
    if(PORTA_ISFR & (1<<n))
    {
        PORTA_ISFR  |= (1<<n);
        g_u32encoder_rt++;
    }
  
}

void pit3_system_loop(void){
  //main system control loop, runs every 1ms, each case runs every 5 ms
  //DisableInterrupts;
  
  switch (system_mode){
    case 0:
      //get gyro & accl values
      control_omg=ad_once(ADC1,AD7b,ADC_16bit)-36050;
      control_tilt=ad_once(ADC1,AD6b,ADC_16bit)-31734+balance_centerpoint_set*5;
      printf("\n%d",(balance_centerpoint_set*5)-31734);
      //proper offest is -26774
      if((control_integrate>0&&control_tilt<0)||(control_integrate<0&&control_tilt>0)){
        control_integrate=0;
      }else{
        control_integrate+=control_tilt;
      }
      system_mode=1;//go to next state on next cycle
    break;
    case 1:
      //get ccd values
      //i.e. sample(2);
      //louis fill this in! ~johnc
      
      
      //if(g_int_ccd_operation_state == 0){
      //g_int_ccd_operation_state = 1;
      ccd_sampling(8);
      //}
      
      //system_mode=1; // hold in this case for testing ccd
      system_mode=2;
    break;
    case 2:
      //calculate balance command with input angle
      //in the end edit motor_command_balance to desired value
      //rounds the code down to an int
      (control_omg<20)?control_omg=0:control_omg;
      motor_command_balance=(control_tilt*100/250)-(control_omg*10/300);
      
      system_mode=3;
    break;
    case 3:
      //calculate turning command from ccd
      //in the end set motor_command_left and motor_command_right to desired values;
      
      system_mode=4;
    break;
    case 4:
     /*connection config:
     
     Hardware        DIR             PWM             Physical location
     ---------------+---------------+---------------+-----------------
     Motor Left      PTD7            ftm0ch6        Motor0
     Motor Right     PTE11           ftm0ch5        Motor1

     */
    //super position for balance + turn
        motor_command_left = motor_command_balance; //+ motor_turn_left;//add this when ccd turn is implemented
        motor_command_right = motor_command_balance;// + motor_turn_right;
      
      //current dummy motor response, Yumi please implement PID ~johnc
        
        //set dir pins on both
          if (motor_command_left>0){
            gpio_set(PORTB,23,1);
          }else{
            gpio_set(PORTB,23,0);
            motor_command_left=motor_command_left*-1;
          }
          
          if(motor_command_right>0){
            gpio_set(PORTB,22,1);
          }else{
            gpio_set(PORTB,22,0);
            motor_command_right=motor_command_right*-1;
          }
          
          //deadzone
          motor_command_left+=50;
          motor_command_right+=50;
          
          //saturation & timeout protection
          if(motor_command_left>800){
            motor_command_left=0;
          }
          
          if(motor_command_right>800){
            motor_command_right=0;
          }
        
          
          //excute motor pwm with PID
          FTM_PWM_Duty(FTM0,CH3,motor_command_left);
          FTM_PWM_Duty(FTM0,CH2,motor_command_right);
      
      //saves current encoder count to last count
      //g_u32encoder_lflast=g_u32encoder_lf;
      //g_u32encoder_rtlast=g_u32encoder_rt;
      
      system_mode=0;//back to the top of pit
    break;
    

      
  }
      //PIT_Flag_Clear(PIT3);
    //EnableInterrupts;
}