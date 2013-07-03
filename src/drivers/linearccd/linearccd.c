/*******************************************

linearccd.c

Authored by Louis Mo
for HKUST SmartCar team 2013

Edited by John Ching

*******************************************/

#include  "include.h"
#include  "linearccd.h"
#include  "math.h"

/*********** CCD related counter ************/
u16 g_u16_ccd_sample_clock=0;

/*********** CCD related status flag ************/
int g_int_SI_state_flag=0;                    // SI flag
int g_int_sampling_state_flag=0;              // sampling state flag
int g_int_trash_sample_flag=0;                // notice dummy sample
int g_int_ccd_operation_state=0;

/*********** CCD related sample result & array ************/
char g_char_ar_ccd_current_pixel[256];        // 1-line pixel array
char g_char_ar_ccd_previous_pixel[256];       // previous pixel array
char g_char_ar_ccd_benchmark_one[256];        // benchmark
char g_char_ar_ccd_benchmark_reuse[256];      // reuseable benchmark

/*********** CCD edge decision related variable ************/
u16 g_u16_ccd_left_pos=0;                // dynamic left edge scan
u16 g_u16_ccd_right_pos=250;             // dynamic right edge scan
u16 g_u16_ccd_middle_pos=0;              // dynamic middle point 

/************* Variables for direction PID : algorthm 2 *************/
int current_mid_error_pos=0;
int last_sample_error_pos=0;

int first_start_up_flag=1;
int left_start_length=100;
int right_start_length=100;

int current_dir_error=0;
int last_sample_dir_error=125;

int current_dir_arc_value_error=0;

int detect_left_flag = 0;
int detect_right_flag = 0;

void output_algorithm_message(); //temp
void output_last_sample_error_pos_message(); //temp

/*********** CCD basic library ************/
void ccd_sampling(char array[], int state){  
  g_int_ccd_operation_state = state;  
  while(g_int_ccd_operation_state == 1){        
       ccd_clock_turn();      
       ccd_detect_track(array);       
       ccd_SI_failing_edge_condition();       
       ccd_finish_one_sampling(array);
       g_u16_ccd_sample_clock++;
  }   
}

void ccd_clock_turn(){
    gpio_turn(PORTB, 9); // Gen 2 main board Clock
}

void ccd_trigger_SI(){
    if(g_int_SI_state_flag == 0 ){
               g_int_SI_state_flag = 1;              // SI Flag on
               g_int_sampling_state_flag = 1;        // sampling Flag on
               g_u16_ccd_sample_clock = 0;
               gpio_set(PORTB, 8, 1);                // Gen 2 main board SI rising edge
    }
}

void ccd_detect_track(char array[]){
   if(gpio_get(PORTB, 10) == 1) {  
   // if CCD receives black (2nd gen)
        array[g_u16_ccd_sample_clock] = 'W';
   } else {                        
   // if CCD receives white (2nd gen)
        array[g_u16_ccd_sample_clock] = 'o';
   }
}

void ccd_SI_failing_edge_condition(){
  if(g_u16_ccd_sample_clock == 1 && g_int_SI_state_flag == 1){ // condition for Longer SI failing edge to end
        gpio_set(PORTB, 8, 0); // Gen 2 SI faling edge
  }
}

void ccd_finish_one_sampling(char array[]){  
     if(g_u16_ccd_sample_clock == 255){
          g_int_SI_state_flag = 0;          // SI Flag off
          g_int_sampling_state_flag = 0;    // Sampling flag off
        
          ccd_shift_sample_to_manageable_position(array);
          ccd_scan_dummy_sample_result(array);     
          
          if(g_int_trash_sample_flag == 1){
          // do nth
          }else{
          //ccd_output_sample_to_UART(array);
          g_int_ccd_operation_state = 0;
          }
     }       
}

void ccd_output_sample_to_UART(char array[]){
     uart_sendStr(UART3,"\n");
     uart_sendStr(UART3,"CCD Sample: ");
     ccd_print(array);
}
  
void ccd_shift_sample_to_manageable_position(char array[]){
      u16 i; 
      for( i = 0 ; i < 252 ; i++){
      array[i] = array[i+2];
      }
      
      array[252] = 'X';
      array[253] = 'X';
      array[254] = 'X';
      array[255] = 'X';
}

void ccd_scan_dummy_sample_result(char array[]){
      u16 i; 
      u16 dummy = 0;
      g_int_trash_sample_flag = 0; // reset trash flag
      
      for( i = 0 ; i < 256 ; i++){
        if(array[i] == 'o'){
          dummy++; }
      }
      
      if(dummy == 255){              // if dummy sample detect
        g_int_trash_sample_flag = 1; // trash flag ON
      }
}

void ccd_print(char array[]){
      u16 i;  
      for( i = 0 ; i < 252 ; i++){
        uart_putchar(UART3,array[i]); // print sample to UART
      }
}

/*********** CCD Direction PID decision ************/

void ccd_recongize_left_right_edge_and_return_dir_error(char array[]){
    
  volatile int i;
  
  int current_1st_left_edge=250;
  int current_1st_right_edge=0;
  
  int detect_left_flag = 0;
  int detect_right_flag = 0;
  
  for( i = last_sample_error_pos ; i > 0 ; i--){ // scan from last_sample_error_pos to left edge
    if(array[i] == 'W'){
      current_1st_left_edge = i;
      detect_left_flag = 1;
      i = 1;
    }
  }
  
  for( i = last_sample_error_pos ; i < 251 ; i++){  // scan from last_sample_error_pos to right edge
    if(array[i] == 'W'){
      current_1st_right_edge = i;
      detect_right_flag = 1;
      i = 250;
    }
  }
  
  /*
  if(first_start_up_flag == 0){
      left_start_length = 125 - current_1st_left_edge;
      right_start_length = current_1st_right_edge - 125;
      first_start_up_flag = 1;
     
      printf("left_start_length is: ");
      printf("%d",left_start_length);
      printf("\n");
      
      printf("left_start_length is: ");
      printf("%d",left_start_length);
      printf("\n");
  }
  */
  
  /* -|||--------------------------------|||- */
  if(detect_left_flag == 1 && detect_right_flag == 1){
    current_mid_error_pos = (current_1st_left_edge + current_1st_right_edge) / 2;
  }
  
    
  /* -|||--------------------------------|||-  
     ------------|||-------------------------
     ----------------------|||--------------- */
  else if(detect_left_flag == 1 && detect_right_flag == 0){
    current_mid_error_pos = current_1st_left_edge + right_start_length;
    if( current_1st_left_edge == 251){
      current_mid_error_pos = 125;
    }
  }
  
     /* -|||-------------------------------|||- 
      --------------------------|||---------- 
      -----------------|||------------------- */
  else if(detect_left_flag == 0 && detect_right_flag == 1){
    current_mid_error_pos = current_1st_right_edge - left_start_length;
    if(current_1st_right_edge == 0){
      current_mid_error_pos = 125;
    }
  }
    /* ---------------------------------------- */
  else if(detect_left_flag == 0 && detect_right_flag == 0){
    current_mid_error_pos = 125;
  }
  
  current_dir_error = (current_mid_error_pos - 125);
  current_dir_arc_value_error = atan(current_dir_error*(0.005291005))*1000;
  
  //output_last_sample_error_pos_message();
  
  if(first_start_up_flag == 1){
    last_sample_error_pos = current_mid_error_pos;
  }
  
  //output_algorithm_message();
}


void output_last_sample_error_pos_message(){ //temp
  printf("\n");
  printf("*** ***\n");
  printf("last_sample_error_pos is: ");
  printf("%d",last_sample_error_pos);
  printf("\n");
}

void output_algorithm_message(){ //temp
  
  ccd_output_sample_to_UART(g_char_ar_ccd_current_pixel);
  
  if(detect_left_flag == 1 && detect_right_flag == 0){
    printf("Both side detected : STRAIGHT line");
  } else if(detect_left_flag == 1 && detect_right_flag == 0){ 
    printf("Left side detected : ONE-EDGE");
  } else if(detect_left_flag == 0 && detect_right_flag == 1){
    printf("Right side detected : ONE-EDGE");
  } else if(detect_left_flag == 0 && detect_right_flag == 0){
    printf("NO EDGE detected");
  }
    

  printf("****** ******\n");
  /*
  printf("current_1st_left_edge is: ");
  printf("%d",current_1st_left_edge);
  printf("\n");
  
  printf("current_1st_right_edge is: ");
  printf("%d",current_1st_right_edge);
  printf("\n");
  */
  printf("****** ******\n");
  
  printf("current_mid_error_pos is: ");
  printf("%d",current_mid_error_pos);
  printf("\n");
  
  printf("****** ******\n");
  
  printf("current_dir_error is: ");
  printf("%d",current_dir_error);
  printf("\n");
  
  printf("current_dir_arc_value_error is: ");
  printf("%d",current_dir_arc_value_error);
  printf("\n");
  
  
  printf("****** ******\n");

}

void ccd_output_edge_to_UART(){
     uart_sendStr(UART3,"\n");
     uart_sendStr(UART3,"\n");
     uart_sendStr(UART3,"Left edge detect position: ");
     printf("%d", g_u16_ccd_left_pos);
     uart_sendStr(UART3,"\n");
     
     uart_sendStr(UART3,"Right edge detect position: ");
     printf("%d", g_u16_ccd_right_pos);
     uart_sendStr(UART3,"\n");
}

  