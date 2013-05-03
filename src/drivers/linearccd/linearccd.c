/*******************************************

linearccd.c

Authored by Louis Mo
for HKUST SmartCar team 2013

Edited by John Ching

*******************************************/

#include  "include.h"
#include  "linearccd.h"


int g_int_SI_state_flag=0;                    // SI flag
int g_int_sampling_state_flag=0;              // sampling state flag
char g_char_ar_ccd_pixel[128];                         // 1-line pixel array
u8 g_u8_ccd_sample_clock=0;

void ccd_print(char array[]){
      
      int i;  
      
      for( i = 0 ; i < 128 ; i++){
           uart_putchar(UART3,array[i]); //print One pixel One loop
      }
}

void ccd_detect_track(){
 
      if(gpio_get(PORTB, 10) == 0) {  // if CCD receive black, the pixel respect to that g_u32_systemclock is 1
        g_char_ar_ccd_pixel[g_u8_ccd_sample_clock] = '1';
      }else {                        // if CCD receive white, the pixel respect to that g_u32_systemclock is 0
        g_char_ar_ccd_pixel[g_u8_ccd_sample_clock] = '0';
      } 
}

void ccd_SI_failing_edge_condition(){
  
      //if(g_u32_systemclock %21 == 20  &&  G_int_SI_state_flag == 1){     // condition for SI failing edge to end 
      if(g_u8_ccd_sample_clock == 20  &&  g_int_SI_state_flag == 1){     // condition for SI failing edge to end 
    
        gpio_set(PORTB, 18, 0); // SI faling edge
        uart_sendStr(UART3,"*.*.*.* SI failing edge happened *.*.*.*");
        uart_sendStr(UART3,"\014");  // New page form feed
      }
}

void ccd_finish_one_sampling(char mode){
  
    if (mode == 2){
        //if(g_u32_systemclock % 129 == 128 && g_int_sampling_state_flag == 1){ // condition for locking SI to end
          if(g_u8_ccd_sample_clock == 127 && g_int_sampling_state_flag == 1){ // condition for locking SI to end
          //g_u32_systemclock = 0;          // No need this after change if into g_u32_systemclock % 129 == 128
          g_int_SI_state_flag = 0;          // SI Flag off
          g_int_sampling_state_flag = 0;    // Sampling flag off
          //gpio_set(PORTD, 1, 1);      // LED D3 ON, 1 means off, notify the SI is unlock
          uart_sendStr(UART3,"*.*.*.* SI locking mode end *.*.*.*");
          uart_sendStr(UART3,"\014"); // New page form feed
       
          // Print the sampling array
          uart_sendStr(UART3,"Just Sampled Array is: ");
          ccd_print(g_char_ar_ccd_pixel);
          uart_sendStr(UART3,"\n\014");     // New page form feed
        }
    } else if(mode == 3){
      
       //if(g_u32_systemclock % 129 == 128 && g_int_sampling_state_flag == 1){ // condition for locking SI to end
         if(g_u8_ccd_sample_clock == 127 && g_int_sampling_state_flag == 1){ // condition for locking SI to end 
          g_int_SI_state_flag = 0;          // SI Flag off
          g_int_sampling_state_flag = 0;    // Sampling flag off
          //gpio_set(PORTD, 1, 1);      // LED D3 ON, 1 means off, notify the SI is unlock
       
          // Print the sampling array
          uart_sendStr(UART3,"Just Sampled Array is: ");
          ccd_print(g_char_ar_ccd_pixel);
          uart_sendStr(UART3,"\n\014");     // New page form feed
       }
    }
}


void ccd_trigger_SI(char mode){
  
    
      if(mode == 2){
           if(g_int_SI_state_flag == 0 ){            // Use this instaed of , if(uart_pendstr(UART3,str) == 1) , can Auto sampling repeatedly
      
           //if(uart_pendstr(UART3,str) == 1){ // Using Bluetooth to trigger , when any key in PC keyboard is pressed
               uart_sendStr(UART3,"*.*.*.* SI Trigger, rising edge generated *.*.*.*");
               uart_sendStr(UART3,"\014");     // New page form feed same as uart_sendStr(UART3,"\n\f");
               g_int_SI_state_flag = 1;              // SI Flag on
               g_int_sampling_state_flag = 1;        // sampling Flag on
               //g_u32_systemclock = 0;                // g_u32_systemclock count from begin,lock the SI status not duplicate
               g_u8_ccd_sample_clock = 0;
               gpio_set(PORTB, 18, 1);         // SI rising edge
           }
      }else if (mode == 3){
           if(g_int_SI_state_flag == 0 ){
               g_int_SI_state_flag = 1;              // SI Flag on
               g_int_sampling_state_flag = 1;        // sampling Flag on
               g_u8_ccd_sample_clock = 0;
               gpio_set(PORTB, 18, 1);         // SI rising edge
           }
      }
}

void ccd_sampling(char mode){
   
       ccd_trigger_SI(mode);
       
       g_u8_ccd_sample_clock++;
       
       ccd_detect_track(); 
       
       ccd_SI_failing_edge_condition();
       
       ccd_finish_one_sampling(mode);
}