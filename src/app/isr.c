#include "common.h"
#include "include.h"
#include "linearccd.h"

/*********** CCD startup variables ************/
extern volatile int g_int_ccd_operation_state; // ccd operation state

/*********** CCD related sample result & array ************/
extern char g_char_ar_ccd_current_pixel[256];        // current sample
extern char g_char_ar_ccd_previous_pixel[256];       // previous sample
extern char g_char_ar_ccd_benchmark_one[256];        // benchmark 1
extern char g_char_ar_ccd_benchmark_two[256];        // benchmark 2
extern char g_char_ar_ccd_benchmark_reuse[256];      // reuseable benchmark

/*********** CCD track decision related variables ***********/
int g_int_ccd_benchmark_counter=0;                   // benchmark counter  
int g_int_ccd_preset_benchmark_time=100;             // adjustable startbenchmark time
int g_int_ccd_benchmark_state=1;                     // default active 

/************* Variables for control PID *************/
volatile int control_omg=0, control_tilt=0, control_tilt_last;
volatile int motor_command_left=0,motor_command_right=0;
volatile int motor_turn_left,motor_turn_right;
volatile int motor_command_balance;
volatile int speed_error=0;
volatile int leftDir,rightDir=0;

/************* Variables for speed/position PID *************/
volatile int speed_p,speed_i;
volatile int car_speed;
volatile int control_car_speed=0;
volatile int motor_command_speed=0;
volatile int motor_command_speed_delta=0;
volatile int speed_control_integral=0;
u8  omgready_flag=0; //position control 

/************* Variables for direction PID *************/
volatile int dir_error=0;
u16 turn_kp=0;
extern u16 g_u16_ccd_middle_pos;

/************* Variables for motor *************/
extern volatile u32 g_u32encoder_lf;
extern volatile u32 g_u32encoder_rt;
extern volatile int motor_deadzone_left,motor_deadzone_right;
extern u32 balance_centerpoint_set;
volatile u8 motor_pid_counter;  //for the motor command loop

/************* system loop counter*************/
u8 system_mode=0;

void PIT0_IRQHandler(void){
  PIT_Flag_Clear(PIT0);       
}

void PIT1_IRQHandler(void)
{   //for encoder testing
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
     encoder_right   PTA9            exti pta        servo2    */
    u8  n=0;
    n=8;
    if(PORTA_ISFR & (1<<n)) 
    {PORTA_ISFR  |= (1<<n);
     g_u32encoder_lf++;
    } 
    
    n=9;
    if(PORTA_ISFR & (1<<n))
    {PORTA_ISFR  |= (1<<n);
    g_u32encoder_rt++;        
    }
}

//main system control loop, runs every 1ms, each case runs every 5 ms
void pit3_system_loop(void){
  DisableInterrupts;   
  switch (system_mode){
    
    /****** Case 0: get ccd values  ******/
    case 0:
      if(g_int_ccd_operation_state == 0){
        ccd_trigger_SI();
        ccd_sampling(g_char_ar_ccd_current_pixel , 1);
      }
    system_mode=1;
    break;
    
    /****** Case 1: calculate turning command from ccd******/
    case 1:
      ccd_detect_current_left_right_edge_and_filter_middle_noise(g_char_ar_ccd_current_pixel);
      dir_error = g_u16_ccd_middle_pos - 128;  
      
      //printf("g_u16_ccd_middle_pos:");
      //printf("%d",g_u16_ccd_middle_pos);
      //printf("\n");
      
      //printf("Direction error:");
      //printf("%d",dir_error);
      //printf("\n");
      
      turn_kp = (1000000/10000);    //dir kp
      motor_turn_left = dir_error * turn_kp;
      motor_turn_right = dir_error * (-turn_kp);
      
      //printf("motor_turn_left");
      //printf("%d",motor_turn_left);
      //printf("\n");
      
      //printf("motor_turn_right");
      //printf("%d", motor_turn_right);
      //printf("\n");
      
      //in the end set motor_command_left and motor_command_right to desired values;   
    system_mode=2;
    break;
    
    /****** Case 2: get gyro & accl values******/
    case 2:
      control_tilt_last=control_tilt;              // offset
      control_tilt=(ad_ave(ADC1,AD6b,ADC_12bit,20)-1150)+(balance_centerpoint_set/10);
      control_omg=ad_ave(ADC1,AD7b,ADC_12bit,20)-1940;
      //printf("\ncontrol tilt:%d",control_tilt);
      printf("\n%d",control_tilt);
      /*if(omgready_flag==0){
        control_omg=0;
        omgready_flag=1;
      }else{
        control_omg=control_tilt-control_tilt_last;
      }*/
    system_mode=3;
    break;
   
    /****** Case 3: balance pid ******/
    case 3:                               
      motor_command_balance= ((control_tilt)*1219811/10000) - (control_omg*10644/1000);
    //motor_command_balance= ((control_tilt)*0/10000) - (control_omg*0/1000);
    system_mode=4;                    // angle kp ~ 121.9811      //angle kd ~10.644
    break;
    
    /****** Case 4: output motor ******/
    case 4:
      
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
          
          speed_p=speed_error*200563/10000;//speed kp ~20.0563
          speed_i=speed_error*40093/10000;//speed ki ~4.0093
          
          //speed_p=speed_error*0/10000;//speed kp 
          //speed_i=speed_error*0/10000;//speed ki
          
          speed_control_integral+=speed_i;
          motor_command_speed_delta=((speed_p+speed_control_integral)-motor_command_speed)/20;
        }
       
        motor_command_speed+=motor_command_speed_delta;
        
        motor_command_left = motor_command_balance - motor_command_speed;
        //+ motor_turn_left;
        motor_command_right = motor_command_balance - motor_command_speed;
        //+ motor_turn_right;
        
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
          
          //printf("\nmotor command left:%d",motor_command_left);
          
          /* Safety
          if(control_tilt>450){
          motor_command_left = 0;
          motor_command_right = 0;
          } else if (control_tilt<(-400)){
          motor_command_left = 0;
          motor_command_right = 0;
          }*/
          
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