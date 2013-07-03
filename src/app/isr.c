#include "common.h"
#include "include.h"
#include "linearccd.h"

/*********** CCD startup variables ************/
extern volatile int g_int_ccd_operation_state; // ccd operation state
volatile int g_int_ccd_si_counter=0;
int pre_set_si_time = 3;

/*********** CCD related sample result & array ************/
extern char g_char_ar_ccd_current_pixel[256];        // current sample
extern char g_char_ar_ccd_previous_pixel[256];       // previous sample
extern char g_char_ar_ccd_benchmark_one[256];        // benchmark 1
extern char g_char_ar_ccd_benchmark_reuse[256];      // reuseable benchmark

/*********** CCD track decision related variables ***********/
int g_int_ccd_benchmark_counter=0;                   // benchmark counter  
int g_int_ccd_preset_benchmark_time=100;             // adjustable start benchmark time
int g_int_ccd_benchmark_state=1;                     // default active 

/************* Variables for control PID *************/
volatile int control_omg=0, control_tilt=0;
volatile int motor_command_left=0,motor_command_right=0;
volatile int motor_turn_left=0,motor_turn_right=0;
volatile int motor_command_balance=0;
volatile int speed_error=0;
volatile int speed_offset=10; //adjustable value
volatile int leftDir,rightDir=0;
volatile int offset_speed=500;

/************* Variables for speed/position PID *************/
volatile int speed_p,speed_i;
volatile int car_speed;
volatile int motor_command_speed=0;
volatile int motor_command_speed_delta=0;
volatile int speed_control_integral=0;
u8  omgready_flag=0; //position control 
volatile int control_car_speed=0; //adjustable value, increase car speed

/************* Variables for direction PID : algorthm 1 *************/
extern char dir_pid_mode;
volatile int dir_error=0;
extern volatile int previous_dir_error;
u16 turn_kp=0;
extern u16 g_u16_ccd_middle_pos;
volatile int motor_command_turn_delta=0;
int left_bias_flag=0;
int right_bias_flag=0;
void temp_ccd_output_debug_message_function(); //temporary
extern char ccd_left_mode; //temporary
extern char ccd_right_mode; //temporary


/************* Variables for direction PID : algorthm 2 *************/
extern int current_dir_arc_value_error;
extern int current_dir_error;

/*********** CCD edge decision related variable ************/
extern volatile u16 g_u16_ccd_left_pos;                // dynamic left edge scan
extern volatile u16 g_u16_ccd_right_pos;             // dynamic right edge scan

/************* Variables for motor *************/
extern volatile int g_u32encoder_lf;
extern volatile int g_u32encoder_rt;
extern volatile int motor_deadzone_left,motor_deadzone_right;
extern u32 balance_centerpoint_set;
volatile u8 motor_pid_counter=0;  //for the motor command loop

/************* system loop counter*************/
u8 system_mode=0;
int system_flag=0;
u32 system_loop_tick=0;

void PIT0_IRQHandler(void){
  PIT_Flag_Clear(PIT0);       
}

void PIT1_IRQHandler(void) //for encoder testing
{   DisableInterrupts;
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
    n=6;
    if(PORTA_ISFR & (1<<n)) 
    {
      PORTA_ISFR  |= (1<<n);
      if(leftDir==1){
        g_u32encoder_lf++;
      }else{
        g_u32encoder_lf--;
      }
    } 
    
    n=7;
    if(PORTA_ISFR & (1<<n))
    {
      PORTA_ISFR  |= (1<<n);
    
      if(rightDir==1){
        g_u32encoder_rt++; 
      }else{
       g_u32encoder_rt--; 
     }
    }
}

//main system control loop, runs every 1ms, each case runs every 3 ms
void pit3_system_loop(void){
  DisableInterrupts;   
  switch (system_mode){
    
    /****** Case 0: get ccd values and calculate turning command from ccd ~410us******/
    case 0:
      
      if( g_int_ccd_si_counter < pre_set_si_time){
      g_int_ccd_si_counter++;
      }else if(g_int_ccd_operation_state == 0){
        g_int_ccd_si_counter = 0;
        ccd_trigger_SI();
        
        ccd_sampling(g_char_ar_ccd_current_pixel , 1);
     
        ccd_recongize_left_right_edge_and_return_dir_error(g_char_ar_ccd_current_pixel);
        
        turn_kp = (160000/10000);    //dir kp
        
        motor_command_turn_delta = ((current_dir_arc_value_error * turn_kp ) - motor_turn_left)/pre_set_si_time;
        
        //motor_command_turn_delta = ((current_dir_error * turn_kp ) - motor_turn_left)/pre_set_si_time;
        //ccd_detect_current_left_right_edge_and_filter_middle_noise(g_char_ar_ccd_current_pixel);
        //temp_ccd_output_debug_message_function();
        
      }
      
      motor_turn_left+=motor_command_turn_delta;
      motor_turn_right-=motor_command_turn_delta;
      
      
    system_mode=1;
    break;
    
    /****** Case 1: get gyro & accl values + balance pid ~140us ******/
    case 1:
                                                // offset
      control_tilt=(ad_ave(ADC1,AD6b,ADC_12bit,20)-1045)+(balance_centerpoint_set/10);
      control_omg=ad_ave(ADC1,AD7b,ADC_12bit,20)-1940;
      printf("\ncontrol tilt:%d",control_tilt);
      //printf("\n%d",control_tilt);
      
                               // angle kp ~ 121.9811      //angle kd ~10.644
      // fine-tune kp start from : 142.6046         //fine-tune angle kd start from :9.595
      
      // when speed = 150 -- kp: 179.6046 && kd: 9.595
      // when speed = 250 -- kp: 224.8946 && kd: 11.999
      // when speed = 300 -- kp: 247.5396 && kd: 9.916 (7.81V) 
      // when speed = 600 -- kp: 597.1783 && kd: 10.309 (___V) 
      
      motor_command_balance= ((control_tilt)*1796046/10000) - (control_omg*9595/1000);
        
    system_mode=2;
    break;
    
    /****** Case 2: output motor ~3.8-5us******/
    case 2:
  
        if(motor_pid_counter<33){ //updated from 20 to 33
          motor_pid_counter++;
        }else{
          motor_pid_counter=0;
          //stuff here happens every 20*5ms=100ms, used for calculating
          //and capturing encoder motor PID
          
          car_speed=g_u32encoder_lf+g_u32encoder_rt;

          //clears current encoder
          g_u32encoder_lf=g_u32encoder_rt=0;
          
          /*
          printf("car_speed is : ");
          printf("%d",car_speed);
          printf("\n");
          
          if(car_speed>900){
            //going downhill
            control_car_speed=-100;
          }else{
            control_car_speed=275;
          }
           */ 
          
          //control_car_speed = 40;
          speed_error = speed_offset + control_car_speed-car_speed; //optimal speed offset ~10
                      
          speed_p=speed_error*298000/10000;//speed kp ~29.8
          speed_i=speed_error*49500/10000;//speed ki ~4.9
          
          speed_control_integral+=speed_i;
          motor_command_speed_delta=((speed_p+speed_control_integral)-motor_command_speed)/33; //updated from 20 to 33
          
          //printf("\n%d",speed_control_integral);
        }
       
        motor_command_speed+=motor_command_speed_delta;
        
        motor_command_left = motor_command_balance;
        //motor_command_left = motor_command_balance - motor_command_speed;
        //motor_command_left = motor_command_balance - motor_command_speed + motor_turn_left;
        
        motor_command_right = motor_command_balance;
        //motor_command_right = motor_command_balance - motor_command_speed;
        //motor_command_right = motor_command_balance - motor_command_speed + motor_turn_right;
        
        
        //set dir pins on both
          if (motor_command_left>0){
            gpio_set(PORTD,7,0);
            leftDir=1;
          }else{
            gpio_set(PORTD,7,1);
            leftDir=-1;
            motor_command_left=motor_command_left*-1;
          }
          
          if(motor_command_right>0){
            gpio_set(PORTD,9,0);
            rightDir=1;
          }else{
            gpio_set(PORTD,9,1);
            rightDir=-1;
            motor_command_right=motor_command_right*-1;
          }
          
          //deadzone
          //motor_command_left+=150;
          //motor_command_right+=150;
          
          //saturation & timeout protection
          
          if(motor_command_left>8000){
            motor_command_left=8000;
          }
          
          if(motor_command_right>8000){
            motor_command_right=8000;
          }
          
          printf("\nmotor command left:%d",motor_command_left);
          printf("\nmotor command right:%d",motor_command_right);
          
          //excute motor pwm with PID
          FTM_PWM_Duty(FTM1, CH0, motor_command_left); //speed down
          FTM_PWM_Duty(FTM1, CH1, motor_command_right); //speed down          
          
    
          //FTM_PWM_Duty(FTM1, CH0, 2000); 
          //FTM_PWM_Duty(FTM1, CH1, 2000); 
          
         
          
          
          
      //saves current encoder count to last count
      //g_u32encoder_lflast=g_u32encoder_lf;
      //g_u32encoder_rtlast=g_u32encoder_rt; 
          
    system_mode=0;//back to the top of pit
    break;
  }
    system_loop_tick++;
    if( system_loop_tick == 3000){ //3000ms
      control_car_speed = 150;   
    }
    PIT_Flag_Clear(PIT3);
    EnableInterrupts;
}

void temp_ccd_output_debug_message_function(){
  
  ccd_output_sample_to_UART(g_char_ar_ccd_current_pixel);
  //ccd_output_edge_to_UART();
}