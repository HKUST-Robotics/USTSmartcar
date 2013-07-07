#include "common.h"
#include "include.h"
#include "linearccd.h"

/*********** CCD startup variables ************/
volatile int g_int_ccd_si_counter=0;
extern volatile int g_int_ccd_operation_state; // ccd operation state
int pre_set_si_time=3;                         // si active time: e.g. 3 means 3ms * 3 system loop = 9ms
int ccd_debug_flag=0;                          // 0: off, 1: on

/*********** CCD related sample result & array ************/
extern char g_char_ar_ccd_current_pixel[256];  // current sample

/************* Variables for control PID *************/
volatile int control_omg=0, control_tilt=0;
volatile int motor_command_left=0,motor_command_right=0;
volatile int motor_turn_left=0,motor_turn_right=0;
volatile int motor_command_balance=0;
volatile int speed_error=0;
volatile int speed_offset=10; //adjustable value
volatile int leftDir,rightDir=0;

/****** reliable balance data point *****
speed = 150 -- kp: 179.6046 && kd: 9.595  (8.10V)
speed = 300 -- kp: 262.5396 && kd: 9.916  (7.76 ~ 8.00V )
speed = 450 -- kp: 315.4746 && kd: 10.237 (7.967V)
speed = 600 -- kp: 368.4096 && kd: 10.558 (7.936V)
speed = 700 -- kp: 403.6996 && kd: 10.94  (7.948V) // not reliable
*/

#define balance_kp 3684096
#define balance_kp_out_of 10000

#define balance_kd 105580
#define balance_kd_out_of 10000

#define speed_kp 298000
#define speed_kp_out_of 10000

#define speed_ki 49500
#define speed_ki_out_of 10000

/************* Variables for speed/position PID *************/
volatile int speed_p,speed_i;
volatile int car_speed;
volatile int motor_command_speed=0;
volatile int motor_command_speed_delta=0;
volatile int speed_control_integral=0;
volatile int control_car_speed=0; //adjustable value, increase car speed

/************* Variables for direction PID : algorthm 2 *************/
extern int current_dir_arc_value_error;
extern int current_dir_error;
volatile int motor_command_turn_delta=0;
extern int current_edge_middle_distance;
int ccd_distance_value_before_upslope=0;
void temp_ccd_output_debug_message_function(); //temporary
#define turn_kp 130500
#define turn_kp_out_of 10000

/************* Variables for motor *************/
extern volatile int g_u32encoder_lf;
extern volatile int g_u32encoder_rt;
extern volatile int motor_deadzone_left,motor_deadzone_right;
extern u32 balance_centerpoint_set;
volatile u8 motor_pid_counter=0;  //for the motor command loop

/************* system loop counter *************/
u8 system_mode=0;
u32 system_loop_tick=0;

/************* Variables for slope case *************/
int slope_state=0;
int slope_startup_flag=0;
int state_1_middle_distance=0;
int state_2_similarity_counter=0;
int state_2_reday_happen_flag=0;

void PIT0_IRQHandler(void){
  PIT_Flag_Clear(PIT0);       
}

/****** for encoder testing ******/
void PIT1_IRQHandler(void) 
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
     encoder_left    PTA6            exti pta        servo1
     encoder_right   PTA7            exti pta        servo2    */
    u8  n=0;
    n=6;
    if(PORTA_ISFR & (1<<n)) 
    {
      PORTA_ISFR  |= (1<<n);
      
      if(GPIO_GET_1bit(PORTC,5)==1){
        g_u32encoder_lf++;
      }else{
        g_u32encoder_lf--;
      }
    } 
    
    n=7;
    if(PORTA_ISFR & (1<<n))
    {
      PORTA_ISFR  |= (1<<n);
      if(GPIO_GET_1bit(PORTC,4)==0){
        g_u32encoder_rt++; 
      }else{
       g_u32encoder_rt--; 
      }
    }
}


/****** main system control loop, runs every 1ms, each case runs every 3 ms ******/
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
                
        if(ccd_debug_flag == 1){
          temp_ccd_output_debug_message_function();  
        }
      }
      
      
      if(motor_pid_counter<33){
        // do nth
        }else{
        motor_command_turn_delta = ((current_dir_arc_value_error)* turn_kp/turn_kp_out_of  - motor_turn_left)/33;
      }
      
      motor_turn_left+=motor_command_turn_delta;
      motor_turn_right-=motor_command_turn_delta;      
    system_mode=1;
    break;
    
    /****** Case 1: get gyro & accl values + balance pid ~140us ******/
    case 1:
                                                // offset
      control_tilt=(ad_ave(ADC1,AD6b,ADC_12bit,20)-1225)+(balance_centerpoint_set/10);
      control_omg=ad_ave(ADC1,AD7b,ADC_12bit,20)-1940;
      //printf("\ncontrol tilt:%d",control_tilt);
      //printf("\n%d",control_tilt);
                                     
      motor_command_balance= ((control_tilt)*balance_kp/balance_kp_out_of) - ((control_omg)*balance_kd/balance_kd_out_of);
        
    system_mode=2;
    break;
    
    /****** Case 2: output motor ~3.8-5us ******/
    case 2:
  
        if(motor_pid_counter<33){ //updated from 20 to 33
          motor_pid_counter++;
        }else{
          motor_pid_counter=0;
          
          //stuff here happens every 33*3ms=99ms, used for calculating and capturing encoder motor PID          
          car_speed=g_u32encoder_lf+g_u32encoder_rt;
          printf("\nCarspeed:%d",car_speed);
        /************ clears current encoder ************/
          g_u32encoder_lf=g_u32encoder_rt=0;          
          
          /*
          if(car_speed>850){            //going downhill
            speed_control_integral=-17500;
            gpio_set(PORTE,24,0);
            gpio_set(PORTE,25,0);
            gpio_set(PORTE,26,0);
          }*/
          
       //printf("\nslope state is : %d",slope_state);
       //printf("\nspeed_control_integral is : %d",speed_control_integral);
       
       //printf("\nccd_distance_value_before_upslope is : %d", ccd_distance_value_before_upslope);
       //printf("\nstate_1_middle_distance is : %d",state_1_middle_distance);
       
       //printf("\ncurrent_edge_middle_distance is: %d", current_edge_middle_distance ); 
       
     
     /************ slope case handling ************/     
     if( slope_startup_flag == 1){ //5000ms
        gpio_set(PORTE,25,0);
            if(slope_state == 0){
              if( speed_control_integral > 50000){
                ccd_distance_value_before_upslope = current_edge_middle_distance;
                slope_state = 1;
                gpio_set(PORTE,26,0);
              }
            } else if(slope_state == 1){
              
              // by detecting continuous 0 to check state 2
              if(current_edge_middle_distance == 0){
                state_2_similarity_counter++;
              }
              
              if(state_2_similarity_counter > 10){
                state_2_reday_happen_flag = 1;
              }
              
              if(state_2_reday_happen_flag == 1){
                if(current_edge_middle_distance > 0){
                  slope_state = 2; 
                  gpio_set(PORTE,24,0);
                  gpio_set(PORTE,26,1);
                }
              }              
               
              //printf("\n%d",car_speed); 
              
              /*state_1_middle_distance = (current_edge_middle_distance - ccd_distance_value_before_upslope);
              if(state_1_middle_distance >= 20){
               slope_state = 2; 
               gpio_set(PORTE,24,0);
               gpio_set(PORTE,26,1);
              } 
              */
              
            }else if(slope_state == 2){
               //printf("\n%d",car_speed);  
            }      
    }
          
          //printf("\n%d",speed_control_integral);
          
          speed_error = speed_offset + control_car_speed - car_speed; //optimal speed offset ~10
                      
          speed_p=speed_error*(speed_kp/speed_kp_out_of);//speed kp ~29.8
          speed_i=speed_error*(speed_ki/speed_ki_out_of);//speed ki ~4.9
          
          speed_control_integral+=speed_i;
          motor_command_speed_delta=((speed_p+speed_control_integral)-motor_command_speed)/33;
        }
       
        motor_command_speed+=motor_command_speed_delta;
        
        motor_command_left = motor_command_balance - motor_command_speed + motor_turn_left;
        motor_command_right = motor_command_balance - motor_command_speed + motor_turn_right;
        
        /************ set dir pins on both ************/
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
          
          
        /************ saturation & timeout protection ************/   
          if(motor_command_left>8000){
            motor_command_left=8000;
          }
          
          if(motor_command_right>8000){
            motor_command_right=8000;
          }
           
        /************ excute motor pwm with PID ************/   
          FTM_PWM_Duty(FTM1, CH0, motor_command_left); 
          FTM_PWM_Duty(FTM1, CH1, motor_command_right);          
          
          //saves current encoder count to last count
          //g_u32encoder_lflast=g_u32encoder_lf;
          //g_u32encoder_rtlast=g_u32encoder_rt; 
          
    system_mode=0;//back to the top of pit
    break;
  }
     
   /************ ticks related handling ************/
    system_loop_tick++;
    if( system_loop_tick == 2000){ //2000ms
      control_car_speed = 600;   
    }
    
    if( system_loop_tick == 5000){ //5000ms
      slope_startup_flag = 1;
    }
    
    PIT_Flag_Clear(PIT3);
    EnableInterrupts;
}

void temp_ccd_output_debug_message_function(){
  ccd_output_sample_to_UART(g_char_ar_ccd_current_pixel);
}