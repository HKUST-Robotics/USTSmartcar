#include "common.h"
#include "include.h"
#include "linearccd.h"

/*********** system debug mode/flag ************/
int ccd_debug_print_all_message_flag=0;        // 0: off, 1: on
int ccd_print_flag=0;                          // 0: off, 1: on
int ccd_compressed_print_flag=0;               // 0: off, 1: on
int only_balance_pid_mode=0;                   // 0: off, 1: on

/*********** startup PID values ************/
int speed_array[3]              = {300    , 900    , 0};
int balance_kp_array[3]         = {2414746, 2654746, 0};
int balance_kd_array[3]         = {99160  , 120000  , 0};
int balance_offset_array[3]     = {1207   , 1207   , 0};
int speed_kp_array[3]           = {297000 , 297000 , 0};
int speed_ki_array[3]           = {49500  , 49500  , 0};
int turn_kp_array[3]            = {120500 , 91800 , 0};
int turn_kd_array[3]            = {100000 , 15000 , 0}; // mode 1 is orginal 12475
int turn_offset_array[3]        = {1215   , 1215   , 0};
int left_start_length_array[3]  = {25     , 37     , 0}; // mode 0 to be edit to 45
int right_start_length_array[3] = {25     , 37     , 0}; // mode 0 to be edit to 45
int ccd_mid_pos_array[3]        = {128    , 128    , 0};
int run_speed_mode = 1; // vaild input : 0 , 1 , 2

/*********** initialize balance PID ************/
int balance_kp = 0;
#define balance_kp_out_of 10000

int balance_kd = 0;
#define balance_kd_out_of 10000

int balance_offset = 0;

/*********** initialize speed PID ************/
int speed_kp = 0;
#define speed_kp_out_of 10000

int speed_ki = 0;
#define speed_ki_out_of 10000

/*********** initialize turn PID ************/
int turn_kp = 0;
#define turn_kp_out_of 10000

int turn_kd = 0;
#define turn_kd_out_of 10000

int turn_offset = 0;
int gyro_turn = 0;

extern int left_start_length;
extern int right_start_length;
extern int ccd_mid_pos;

/*********** CCD startup variables ************/
volatile int g_int_ccd_si_counter=0;
extern volatile int g_int_ccd_operation_state; // ccd operation state
int pre_set_si_time=3;                         // si active time: e.g. 3 means 3ms * 3 system loop = 9ms

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

/************* Variables for speed/position PID *************/
volatile int speed_p,speed_i;
volatile int car_speed;
volatile int motor_command_speed=0;
volatile int motor_command_speed_delta=0;
volatile int speed_control_integral=0;
volatile int control_car_speed=0; //adjustable value, increase car speed

/************* Variables for direction PID : algorithm 2 *************/
extern int current_dir_arc_value_error;
extern int current_dir_error;
volatile int motor_command_turn_delta=0;
extern int current_edge_middle_distance;
int ccd_distance_value_before_upslope=0;
void temp_ccd_output_debug_message_function(); //temporary

/************* Variables for direction PID : all white encoder hold *************/
volatile int encoder_turn_error=0;

/************* Variables for motor *************/
extern volatile int g_u32encoder_lf;
extern volatile int g_u32encoder_rt;
extern volatile int motor_deadzone_left,motor_deadzone_right;
extern u32 balance_centerpoint_set;
volatile u8 motor_pid_counter=0;  //for the motor command loop

/************* Variables for system *************/
u8 system_mode=0;
u32 system_loop_tick=0;
int end_of_track_wait_flag=0;
int end_of_track_flag=0;
int track_end_time_counter=0;
int mode_selection_start_time_end = 2000;

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
    
    /****** Case 0: get ccd values and calculate turning command from ccd ~410us ******/
    case 0:
      
      gyro_turn=ad_ave(ADC1,AD6b,ADC_12bit,20)-turn_offset;
      
      if( g_int_ccd_si_counter < pre_set_si_time){
        g_int_ccd_si_counter++;
      }else if(g_int_ccd_operation_state == 0){        
        g_int_ccd_si_counter = 0;         
        ccd_trigger_SI();        
        ccd_sampling(g_char_ar_ccd_current_pixel , 1);
        ccd_recongize_left_right_edge_and_return_dir_error(g_char_ar_ccd_current_pixel);        
                
        /****** ccd dubug ******/
        if(ccd_debug_print_all_message_flag == 1){ // print out all ccd message to UART
          temp_ccd_output_debug_message_function();  
        }
        
        if(ccd_print_flag == 1){                   // print out 250 pixle raw data 
          ccd_print(g_char_ar_ccd_current_pixel);
        }
        
        if(ccd_compressed_print_flag == 1){        // print out compressed ccd message to UART
          ccd_compressed_print(g_char_ar_ccd_current_pixel);
        }
        
      }
      
      if(motor_pid_counter<33){
        // do nth
        }else{
        motor_command_turn_delta = ((current_dir_arc_value_error)* turn_kp/turn_kp_out_of + gyro_turn * turn_kd/turn_kd_out_of - motor_turn_left)/33;
      }
      
      motor_turn_left+=motor_command_turn_delta;
      motor_turn_right-=motor_command_turn_delta;
     
    system_mode=1;
    break;
    
    /****** Case 1: get gyro & accl values + balance pid ~140us ******/
    case 1:
                                                
      control_tilt=(ad_ave(ADC1,AD6b,ADC_12bit,20)-balance_offset); // offset
      control_omg=ad_ave(ADC1,AD7b,ADC_12bit,20)-1940;
      motor_command_balance= ((control_tilt)*balance_kp/balance_kp_out_of) - ((control_omg)*balance_kd/balance_kd_out_of);
      
    system_mode=2;
    break;
    
    /****** Case 2: output motor ~3.8-5us ******/
    case 2:
  
        if(motor_pid_counter<33){ 
          motor_pid_counter++;
        }else{
          motor_pid_counter=0;
          
          /****** stuff here happens every 33*3ms=99ms, used for calculating and capturing encoder motor PID ******/          
          car_speed=g_u32encoder_lf+g_u32encoder_rt;
          encoder_turn_error+=g_u32encoder_rt-g_u32encoder_lf;
                    
         /************ clears current encoder ************/
          g_u32encoder_lf=g_u32encoder_rt=0;   
          
          speed_error = speed_offset + control_car_speed - car_speed; //optimal speed offset ~10
                      
          speed_p=speed_error*(speed_kp/speed_kp_out_of);//speed kp ~29.8
          speed_i=speed_error*(speed_ki/speed_ki_out_of);//speed ki ~4.9
          
          speed_control_integral+=speed_i;
          motor_command_speed_delta=((speed_p+speed_control_integral)-motor_command_speed)/33;
        }
       
        motor_command_speed+=motor_command_speed_delta;
        
        if(only_balance_pid_mode == 0){          
          motor_command_left = motor_command_balance - motor_command_speed + motor_turn_left;
          motor_command_right = motor_command_balance - motor_command_speed + motor_turn_right;
        } else if(only_balance_pid_mode == 1){
          motor_command_right = motor_command_balance;        
          motor_command_left = motor_command_balance;
        }
        
        //motor_command_left = motor_turn_left;
        //motor_command_right = motor_turn_right; 
      
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

        /************ saturation & timeout protection ************/   
          if(motor_command_left>8000){
            motor_command_left=8000;
          }
          
          if(motor_command_right>8000){
            motor_command_right=8000;
          }
           
        /************ excute motor pwm with PID ************/  
        if(end_of_track_flag == 0){
          FTM_PWM_Duty(FTM1, CH0, motor_command_left); 
          FTM_PWM_Duty(FTM1, CH1, motor_command_right);          
        } else if (end_of_track_flag == 1){
          FTM_PWM_Duty(FTM1, CH0, 0); 
          FTM_PWM_Duty(FTM1, CH1, 0); 
        }
          
    system_mode=0; //back to the top of pit
    break;
  }
     
   /************ ticks related handling ************/
    system_loop_tick++;
        
    if ( system_loop_tick < mode_selection_start_time_end){ /*** Speed selection , < 2000ms ***/
      
          /* SW physical position
            -----
            |3|4|
            |2|1|
            ----- */    

        if (gpio_get(PORTE, 8) == 0){ // when 3 press
          run_speed_mode = 0;
        }
        else if (gpio_get(PORTE, 9) == 0){ //when 4 press
          run_speed_mode = 1;
        }
        else if (gpio_get(PORTE, 6) == 0){  //when 1 press
          run_speed_mode = 2;
        }
        
        /*** LED notification ***/
        if(run_speed_mode == 0){
          gpio_set(PORTE,24,1);
          gpio_set(PORTE,25,1);
          gpio_set(PORTE,26,0);
        } else if (run_speed_mode == 1){
          gpio_set(PORTE,24,0);
          gpio_set(PORTE,25,1);
          gpio_set(PORTE,26,1);
        } else if (run_speed_mode == 2){
          gpio_set(PORTE,24,1);
          gpio_set(PORTE,25,0);
          gpio_set(PORTE,26,1);          
        }
        
        gpio_set(PORTE,27,0);
    } else if( system_loop_tick == mode_selection_start_time_end){ /*** inital startup time , 2000ms ***/
 
      gpio_set(PORTE,27,1);  /*** initial time end ***/
      
      if(speed_array[run_speed_mode] > 500){
          
          /*** set speed ***/
          control_car_speed = 0;
          
          /*** balance ***/
          balance_kp = balance_kp_array[0];      
          balance_kd = balance_kd_array[0];
          balance_offset = balance_offset_array[0];
          
          /*** speed ***/
          speed_kp =  speed_kp_array[0];    
          speed_ki =  speed_ki_array[0];
           
          /*** turn***/
          turn_kp = turn_kp_array[0]; 
          turn_kd = turn_kd_array[0]; 
          turn_offset = turn_offset_array[0];
          
          /*** vehicle respect to track position ***/
          left_start_length = left_start_length_array[0];
          right_start_length = right_start_length_array[0];
          ccd_mid_pos = ccd_mid_pos_array[0];
      } else {
        
          /*** set speed ***/
          control_car_speed = 0;
          
          /*** balance ***/
          balance_kp = balance_kp_array[run_speed_mode];      
          balance_kd = balance_kd_array[run_speed_mode];
          balance_offset = balance_offset_array[run_speed_mode];
          
          /*** speed ***/
          speed_kp =  speed_kp_array[run_speed_mode];    
          speed_ki =  speed_ki_array[run_speed_mode];
           
          /*** turn***/
          turn_kp = turn_kp_array[run_speed_mode]; 
          turn_kd = turn_kd_array[run_speed_mode];
          turn_offset = turn_offset_array[run_speed_mode];
          
          /*** vehicle respect to track position ***/
          left_start_length = left_start_length_array[run_speed_mode];
          right_start_length = right_start_length_array[run_speed_mode];
          ccd_mid_pos = ccd_mid_pos_array[run_speed_mode];
        }
      }
    
        
    if( system_loop_tick == (mode_selection_start_time_end+6000)){ /*** 8000ms ***/ 
      // to be edit back to 6000 after tuning balance
          
          /*** set speed ***/
          control_car_speed = speed_array[run_speed_mode];
          
          /*** balance ***/
          balance_kp = balance_kp_array[run_speed_mode];      
          balance_kd = balance_kd_array[run_speed_mode];
          balance_offset = balance_offset_array[run_speed_mode];
          
          /*** speed ***/
          speed_kp =  speed_kp_array[run_speed_mode];    
          speed_ki =  speed_ki_array[run_speed_mode];
           
          /*** turn***/
          turn_kp = turn_kp_array[run_speed_mode]; 
          turn_kd = turn_kd_array[run_speed_mode];
          turn_offset = turn_offset_array[run_speed_mode];
          
          /*** vehicle respect to track position ***/
          left_start_length = left_start_length_array[run_speed_mode];
          right_start_length = right_start_length_array[run_speed_mode];
          ccd_mid_pos = ccd_mid_pos_array[run_speed_mode];
    }

    
    if( system_loop_tick >= (mode_selection_start_time_end+18000)){ /*** 20000ms ***/
      if(gpio_get(PORTB, 21) == 1 && gpio_get(PORTB, 22) == 0 && gpio_get(PORTB, 23) == 1){
          end_of_track_wait_flag = 1;
      }
    }
    
    if( end_of_track_wait_flag == 1){
       track_end_time_counter++;
    }
    
    if (track_end_time_counter == 1200){
       end_of_track_flag = 1;
    }
    
    
    PIT_Flag_Clear(PIT3);
    EnableInterrupts;
}

void temp_ccd_output_debug_message_function(){
  output_algorithm_message();
}