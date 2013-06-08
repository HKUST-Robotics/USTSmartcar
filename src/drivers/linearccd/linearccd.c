/*******************************************

linearccd.c

Authored by Louis Mo
for HKUST SmartCar team 2013

Edited by John Ching

*******************************************/

#include "include.h"
#include "linearccd.h"

char g_ccd_sample_array[128]; // the sample result array
int g_int_SI_state_flag = 0;    // SI flag
u16 g_u16_SI_locking_counter = 0; // tuen SI duration
int g_int_SI_locking_flag = 0; // to lock and unlock SI
int g_int_ccd_sample_finish_flag = 0; // to check one sampling is finished

u16 g_u16_ccd_clock = 0; 
u16 g_u16_ccd_pixel_pos = 0;


void ccd_sampling(char mode){

    ccd_clock_turn();
    
    ccd_trigger_SI();

    ccd_detect_track();
    
    ccd_array_position_handler();
    
    ccd_monitor_SI_faling_edge();
    
    ccd_adjust_SI_length();
    
    ccd_finish_one_sample();
    
    ccd_output_sample();
}

void ccd_clock_turn(){
   gpio_turn(PORTB, 9); // CCD Clock Rising and Failing edge
   g_u16_ccd_clock++;
   g_u16_SI_locking_counter++;
}


void ccd_trigger_SI(){
  
  if(g_int_SI_locking_flag == 0){
     gpio_set(PORTB, 8, 1); // SI rising edge
     g_int_SI_state_flag = 1;
     g_int_SI_locking_flag = 1; // lock SI, once trigger, until condition to unlock
     
     uart_sendStr(UART3,"*.*.*.* SI Trigger, rising edge generated , SI is locked*.*.*.*");
     uart_sendStr(UART3,"\n\014");
  }
}

void ccd_detect_track(){
  
  if(gpio_get(PORTB, 10) == 1) { // if CCD receive black, the pixel respect to that g_u16_ccd_sample_clock is 1 (old board)
       g_ccd_sample_array[g_u16_ccd_pixel_pos] = '|';
    }else { // if CCD receive white, the pixel respect to that g_u16_ccd_sample_clock is 0
       g_ccd_sample_array[g_u16_ccd_pixel_pos] = '_';
  }
  
}


void ccd_array_position_handler(){
  
    g_u16_ccd_pixel_pos++;
}

void ccd_monitor_SI_faling_edge(){
  
  if(g_u16_ccd_clock == 20 && g_int_SI_state_flag == 1){
     gpio_set(PORTB, 8, 0); // SI Falling edge  
     g_int_SI_state_flag = 0;  
     uart_sendStr(UART3,"*.*.*.* SI Falling edge happened *.*.*.*");
     uart_sendStr(UART3,"\n\014");
  }
}

void ccd_adjust_SI_length(){
  
  if(g_u16_SI_locking_counter == 12500){
  g_int_SI_locking_flag = 0;
  
  uart_sendStr(UART3,"*.*.*.* SI unlocked *.*.*.*");
  uart_sendStr(UART3,"\n\014");
  
  }
  
  // when PIT0 clock = 200us
  // to set 25ms/100us = 250 
  //        50ms/100us = 500
  //        100ms/100us = 1000
  
  // when PIT0 clock = 2us
  // to set 25ms/2us = 12500 
  //        50ms/2us = 25000
  //        100ms/2us = 50000
  
}

void ccd_finish_one_sample(){
  
  //if(g_int_SI_locking_flag == 0){
    g_u16_ccd_pixel_pos = 0; // back to 1st position and prepare for next sample 
    g_int_ccd_sample_finish_flag = 1; // allow ccd to print the result in UART
  //}
}

void ccd_output_sample(){
  
  if(g_int_ccd_sample_finish_flag == 1){
     uart_sendStr(UART3,"The Sample is: ");
     ccd_print(g_ccd_sample_array);
     uart_sendStr(UART3,"\n\014"); 
  }
}

void ccd_print(char array[]){
      
      int i;      
      for( i = 0 ; i < 128 ; i++){
           uart_putchar(UART3,array[i]); // print One pixel One loop
      }
}

