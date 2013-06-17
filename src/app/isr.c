#include "common.h"
#include "include.h"
#include "linearccd.h"

#define BALANCEKP 0.1
#define BALANCEKI 0

extern volatile int g_int_ccd_operation_state; // ccd operation state

extern volatile u32 g_u32encoder_lf;
extern volatile u32 g_u32encoder_rt;

extern volatile u32 g_u32encoder_lflast;
extern volatile u32 g_u32encoder_rtlast;

extern volatile int motor_deadzone_left,motor_deadzone_right;

extern u32 balance_centerpoint_set;

volatile u8 motor_pid_counter;

//position control 

volatile int car_speed;
volatile int control_car_speed=0;
volatile int motor_command_speed=0;
volatile int motor_command_speed_delta=0;
volatile int speed_control_integral=0;

//for the motor command loop

/******************************************
 Variables for control PIT
 ******************************************/

volatile int control_omg, control_tilt, control_tilt_last;
volatile int motor_command_left,motor_command_right;
volatile int motor_turn_left,motor_turn_right;
volatile int motor_command_balance;
volatile int speed_error=0;
volatile int leftDir,rightDir=0;

int testcommand=0;

extern volatile float accl_tilt16;

u8 system_mode=0;

void PIT0_IRQHandler(void)
{     //gpio_turn(PORTB, 9); // Gen 2 main board Clock  
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
  int speed_p,speed_i;
  //main system control loop, runs every 1ms, each case runs every 5 ms
  DisableInterrupts;

  switch (system_mode){
    case 0:
      //get gyro & accl values
      control_tilt_last=control_tilt;
      control_tilt=ad_once(ADC1,AD6b,ADC_16bit)-26650+balance_centerpoint_set*5;
      control_omg=control_tilt_last-control_tilt;
      
      //control_omg=ad_once(ADC1,AD7b,ADC_16bit)-36050;
      printf("\ncontrol tilt:%d",control_tilt);
      //proper offest is -26774
      system_mode=1;//go to next state on next cycle
    break;
    case 1:
      //get ccd values      
      if(g_int_ccd_operation_state == 0){
        ccd_sampling(1);
      }
      
      system_mode=2;
    break;
    case 2:
      //calculate balance command with input angle
      //in the end edit motor_command_balance to desired value
      //rounds the code down to an int
      motor_command_balance=(control_tilt*8/2)+(control_omg*1);
      //printf("\nMotorBalance:%d",motor_command_balance);
      
      system_mode=3;
    break;
    case 3:
      //calculate turning command from ccd
      //in the end set motor_command_left and motor_command_right to desired values;
      
      system_mode=4;
    break;
    case 4:
      
      //grab encoder functions
        if(motor_pid_counter<20){
          motor_pid_counter++;
          
          
        }else{
          motor_pid_counter=0;
          //stuff here happens every 20*5ms=100ms, used for calculating
          //and capturing encoder motor PID
          
          car_speed=g_u32encoder_lf*leftDir+g_u32encoder_rt*rightDir;

          //clears current encoder
          g_u32encoder_lf=g_u32encoder_rt=0;
          
          speed_error=control_car_speed-car_speed;
          
          speed_p=speed_error*1;//speed kp
          speed_i=speed_error*3;//speed ki
          
          speed_control_integral+=speed_i;
          motor_command_speed_delta=((speed_p+speed_control_integral)-motor_command_speed)/20;
          
        }
        
        motor_command_speed+=motor_command_speed_delta;
    //super position for balance + turn
        motor_command_left = motor_command_balance;//-motor_command_speed; 
        //+ motor_turn_left;//add this when ccd turn is implemented
        motor_command_right = motor_command_balance;//-motor_command_speed;
        // + motor_turn_right;
        
      //current dummy motor response, Yumi please implement PID ~johnc
        
        //set dir pins on both
          if (motor_command_left>0){
            gpio_set(PORTB,23,0);
            leftDir=1;
          }else{
            gpio_set(PORTB,23,1);
            leftDir=-1;
            motor_command_left=motor_command_left*-1;
          }
          
          if(motor_command_right>0){
            gpio_set(PORTB,22,0);
            rightDir=1;
          }else{
            gpio_set(PORTB,22,1);
            rightDir=-1;
            motor_command_right=motor_command_right*-1;
          }
          
          //deadzone
          //motor_command_left+=100;
          //motor_command_right+=100;
          
          //saturation & timeout protection
          if(motor_command_left>8000){
            motor_command_left=8000;
          }
          
          if(motor_command_right>8000){
            motor_command_right=8000;
          }
          
          printf("\nmotor command left:%d");
          
          //excute motor pwm with PID
          FTM_PWM_Duty(FTM0,CH3,motor_command_left);
          FTM_PWM_Duty(FTM0,CH2,motor_command_right);
          
      //saves current encoder count to last count
      //g_u32encoder_lflast=g_u32encoder_lf;
      //g_u32encoder_rtlast=g_u32encoder_rt;
      
      system_mode=0;//back to the top of pit
    break;
      
  }
    PIT_Flag_Clear(PIT3);
    EnableInterrupts;
}