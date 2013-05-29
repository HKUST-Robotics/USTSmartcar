#include "common.h"
#include "include.h"
#include "linearccd.h"

#define BALANCEKP 0.1
#define BALANCEKI 0

extern volatile u32 g_u32_systemclock;   // systemclock counter
extern volatile int g_int_ccd_operation_state;

extern volatile u32 g_u32encoder_lf;
extern volatile u32 g_u32encoder_rt;

extern volatile u32 g_u32encoder_lflast;
extern volatile u32 g_u32encoder_rtlast;

extern volatile int motor_deadzone_left,motor_deadzone_right;


//for the motor command loop

/******************************************
 Variables for control PIT
 ******************************************/

volatile int control_omg, control_tilt, control_integrate;
volatile int motor_command_left,motor_command_right;
volatile int motor_turn_left,motor_turn_right;
volatile int motor_command_balance;

volatile int speed_control_out_old;
volatile int speed_control_out_new;

volatile int speed_control_out;
volatile int speed_control_step;
volatile int speed_control_integral=0;
volatile u8 speed_control_counter=0;



volatile int car_speed_set=0;

void motor_execute(void);
void speed_control(void);

extern volatile float accl_tilt16;

u8 system_mode=0;

void PIT0_IRQHandler(void)
{
      //gpio_turn(PORTB, 18);       // CCD Clock Rising and Failing edge    
      g_u32_systemclock++;
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

void FTM1_IRQHandler()
{
    //this triggers on encoder pulse's rising edge, for use on newer mainboard with ftm encoder captures
    /*
    On new board
    Hardware        Port name       Program name    Physical location
    ---------------+---------------+---------------+-----------------
    encoder_left    PTA8            FTM1,CH0        encoder0
    encoder_right   PTA9            FTM1,CH1        encoder1
     */
    
    u8 s=FTM1_STATUS;
    u8 CHn;
    
    FTM1_STATUS=0x00;               //clear the register
    
    CHn=0;
    if( s & (1<<CHn) )
    {
        FTM_IRQ_DIS(FTM1,CHn);      //prevents input capture interrupt
        
        g_u32encoder_lf++; 
        
        //try not to use DisableInterrupt here, do it in main.c
        FTM_IRQ_EN(FTM1,CHn);  //matches with FTM_IRQ_DIS(FTM1,CHn); 

    }
    
    
    CHn=1;
    if( s & (1<<CHn) )
    {
        FTM_IRQ_DIS(FTM1,CHn);
        
        g_u32encoder_rt++;
        
        FTM_IRQ_EN(FTM1,CHn);
    
    }
}

void encoder_counter(void){
  /*connection config:
     
     Hardware        Port name       Program name    Physical location
     ---------------+---------------+---------------+-----------------
     encoder_left    PTC18            exti ptc        servo1
     encoder_right   PTC19            exti ptc        servo2
   */
 
    u8  n=0;
    n=18;
    if(PORTC_ISFR & (1<<n)) 
    {
        PORTC_ISFR  |= (1<<n);
        
        g_u32encoder_lf++;
        
    } 
    
    n=19;
    if(PORTC_ISFR & (1<<n))
    {
        PORTC_ISFR  |= (1<<n);
        g_u32encoder_rt++;
    }
  
}

void pit3_system_loop(void){
  //main system control loop, runs every 1ms, each case runs every 5 ms
  //DisableInterrupts;
  
  switch (system_mode){
    case 0:
      //get gyro & accl values
      control_omg=ad_once(ADC0,AD15,ADC_16bit)-33850;
      control_tilt=ad_once(ADC1,AD6b,ADC_16bit)-28400;
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
      
      if(g_int_ccd_operation_state == 0){
      //g_int_ccd_operation_state = 1;
      ccd_sampling(8,1);
      }
      
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
      
      //current dummy motor response, Yumi please implement PID ~johnc=
      
        if(speed_control_counter<20){
          speed_control_counter++;
          speed_control_out+=speed_control_step;
        }else{
          speed_control_counter=0;
          speed_control();
        }
        
        motor_command_left+=speed_control_out;
        motor_command_right+=speed_control_out;
        
        
      motor_execute();
      
      system_mode=0;//back to the top of pit
    break;
    

      
  }
      //PIT_Flag_Clear(PIT3);
    //EnableInterrupts;
}

void motor_execute(void){
  //set dir pins on both
  if (motor_command_left>0){
    gpio_set(PORTD,7,1);
  }else{
    gpio_set(PORTD,7,0);
    motor_command_left=motor_command_left*-1;
  }
  
  if(motor_command_right>0){
    gpio_set(PORTE,11,0);
  }else{
    gpio_set(PORTE,11,1);
    motor_command_right=motor_command_right*-1;
  }
  
  //deadzone
  motor_command_left+=60;
  motor_command_right+=60;
  
  //saturation & timeout protection
  if(motor_command_left>800){
    motor_command_left=0;
    motor_timeout++;
  }
  else{
    motor_timeout=0;
  }
  
  if(motor_command_right>800){
    motor_command_right=0;
    motor_timeout++;
  }else{
    motor_timeout=0;
  }
  
  
  //excute motor pwm with PID
  printf("\n%d",motor_timeout);
  FTM_PWM_Duty(FTM0,CH6,motor_command_left);
  FTM_PWM_Duty(FTM0,CH5,motor_command_right);
}

void speed_control(void){
  int error=0;
  int car_speed=0;
  u8 lfs,rts=1;
  int speed_p,speed_i=0;
  
  if (motor_command_left<0){
   lfs=-1;
  }
  
  if(motor_command_right<0){
    rts=-1;
  }
  
  car_speed=((g_u32encoder_lf*lfs)+(g_u32encoder_rt*rts))/2;
  g_u32encoder_lf=g_u32encoder_rt=0;
  
  error=car_speed-car_speed_set;
  
  speed_control_integral+=error*0;
  
  speed_control_out_old=speed_control_out_new;
  speed_control_out_new=error*0+speed_control_integral;
  
  speed_control_step=speed_control_out_old-speed_control_out_new;
  speed_control_out=speed_control_out_new;
  
  
}