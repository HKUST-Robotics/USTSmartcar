/*******************************************

linearccd.c

Authored by Louis Mo
for HKUST SmartCar team 2013

Edited by John Ching

*******************************************/

#include  "include.h"
#include  "linearccd.h"

/*********** CCD related sample result & array ************/

char g_char_ar_ccd_pixel[128];                // 1-line pixel array
//char g_char_ar_ccd_previous_pixel[128];       // previous pixel array
//char g_char_ar_ccd_benchmark_one[128];        // benchmark 1
//char g_char_ar_ccd_benchmark_two[128];        // benchmark 2


/*********** CCD related status flag ************/
int g_int_SI_state_flag = 0;    // SI status flag
//int g_int_SI_locking_flag = 0;  // lock and unlock SI flag
int g_int_sampling_state_flag = 0; // overall sample status flag

//int g_int_SI_state_flag=0;                    // SI flag
//int g_int_sampling_state_flag=0;              // sampling state flag


/*********** CCD related counter ************/
u16 g_u16_ccd_clock = 0; // ccd local clock
u16 g_u16_ccd_long_SI_counter=12500;
//u16 g_u16_SI_locking_counter = 0; // tuen SI duration
u16 g_u16_ccd_pixel_pos = 0; // handle array position

//u16 g_u16_ccd_sample_clock=0;
//u16 g_u16_ccd_long_SI_counter=12500;
//volatile int g_int_ccd_operation_state=0;


void ccd_sampling(char mode){
    

   if(g_u16_ccd_long_SI_counter == 12500)
   {  
    ccd_trigger_SI();
    g_u16_ccd_pixel_pos = 0 ;
   }
   
    ccd_detect_track();
    
    //ccd_array_position_handler();
    
    ccd_monitor_SI_faling_edge();
    
    ccd_finish_one_sampling();
    
    ccd_output_sample_to_UART();
    
    g_u16_ccd_long_SI_counter++;
    
    g_u16_ccd_clock++;
    
    //g_u16_ccd_pixel_pos++;
    
    
}
  

  /*
  
       //g_int_ccd_operation_state = state;
  
       //while(g_int_ccd_operation_state == 1){
         
       //gpio_turn(PORTB, 18);       // CCD Clock Rising and Failing edge
       //gpio_turn(PORTB, 9); // Gen 2 main board Clock
       //<- This syntax might be wrong due to logic and operation of ccd ) if execute in system loop mode ->
   
       //ccd_hard_code_benchmark();
          
       //water_lights();
       
       if(mode == 3 || mode == 5){
          ccd_trigger_SI(mode);
       }else if(mode == 8 && g_u16_ccd_long_SI_counter == 12500){  // when PIT1 clock = 200us , 25ms/100us =250 50ms/100us = 500 , 100ms/100us = 1000
          ccd_trigger_SI(mode);                                     // when PIT1 clock = 2us , 25ms/2us = 12500 50ms/2us = 25000 , 100ms/2us = 50000
        }
          
       //ccd_save_previous_sampling();
       
       ccd_detect_track(); 
       
       //g_u16_ccd_sample_clock++;
       
       ccd_SI_failing_edge_condition(mode);
       
       ccd_finish_one_sampling(mode);
       
       
       g_u16_ccd_long_SI_counter++;
       g_u16_ccd_sample_clock++;
       
       
       //}
     
}
*/


void ccd_trigger_SI(){
  /*
   else if ( (mode == 3 || mode == 5 || mode == 8) ){
           if(g_int_SI_state_flag == 0 ){
               g_int_SI_state_flag = 1;              // SI Flag on
               g_int_sampling_state_flag = 1;        // sampling Flag on
               g_u16_ccd_sample_clock = 0;
               //gpio_set(PORTB, 19, 1);               // SI rising edge
               gpio_set(PORTB, 8, 1);               // Gen 2 main board SI rising edge
           
           }
      }
  */
  
    if(g_int_SI_state_flag == 0){

      g_int_SI_state_flag = 1;              // SI Flag on
      g_int_sampling_state_flag = 1;        // sampling Flag on
      g_u16_ccd_clock = 0;
      //g_u16_ccd_pixel_pos = 0; // position back to 0, prepare for next sample
     
      gpio_set(PORTB, 8, 1); // Gen 2 mainboard SI rising edge
    
    }
}

void ccd_detect_track(){
    
   if(gpio_get(PORTB, 10) == 1) {  
   // if CCD receives black (2nd gen)
     
   //if(gpio_get(PORTA, 11) == 1) {  (1st gen)
        g_char_ar_ccd_pixel[g_u16_ccd_clock] = '|';
   } else {                        
   // if CCD receives white
        g_char_ar_ccd_pixel[g_u16_ccd_clock] = '_';
   }
   
   g_u16_ccd_pixel_pos++;
}

void ccd_array_position_handler(){
  //if(g_int_SI_state_flag == 1){
   g_u16_ccd_pixel_pos++;
  //}
}

void ccd_monitor_SI_faling_edge(){
  /*
  else if(g_u16_ccd_sample_clock == 1 &&  g_int_SI_state_flag == 1 && mode == 8){ // condition for Longer SI failing edge to end                                                                            // condition for Longer SI failing edge to end
        //gpio_set(PORTB, 19, 0); // SI faling edge
        gpio_set(PORTB, 8, 0); // Gen 2 SI faling edge
        g_u16_ccd_long_SI_counter = 0;
  }*/
  
  if(g_u16_ccd_clock == 1 && g_int_SI_state_flag == 1){
     gpio_set(PORTB, 8, 0); // SI Falling edge
     g_u16_ccd_long_SI_counter = 0;
     
     //g_u16_SI_locking_counter = 0;
     //uart_sendStr(UART3,"*.*.*.* SI Falling edge happened *.*.*.*");
     //uart_sendStr(UART3,"\n");
  }
  
  
}

void ccd_finish_one_sampling(){
    
    /*
    int p;
  
    if (mode == 2){ // Debug mode
          if(g_u16_ccd_sample_clock == 128 && g_int_sampling_state_flag == 1){ // condition for ccd finish one sampleing
          g_int_SI_state_flag = 0;          // SI Flag off
          g_int_sampling_state_flag = 0;    // Sampling flag off
          uart_sendStr(UART3,"*.*.*.* SI locking mode end *.*.*.*");
          uart_sendStr(UART3,"\n");       
       
          // Print the sampling array
          uart_sendStr(UART3,"Just Sampled Array is: ");
          ccd_print(g_char_ar_ccd_pixel);
          uart_sendStr(UART3,"\n");     
          
          gpio_set(PORTA, 8, 1);            // Trigger Oscilloscope          
          uart_sendStr(UART3,"*.*.*.* Trigger Oscilloscop *.*.*.*");
          uart_sendStr(UART3,"\n");     
        }
    }else if(mode == 5){ // Benchmark and Sample Filter
      
         if(g_u16_ccd_sample_clock == 128 && g_int_sampling_state_flag == 1){ 
         
          g_int_SI_state_flag = 0;          // SI Flag off
          g_int_sampling_state_flag = 0;    // Sampling flag off
       
          // Print (filtering) results
          uart_sendStr(UART3,"The 1st Benchmark sample is: ");
          ccd_print(g_char_ar_ccd_benchmark_one);
          uart_sendStr(UART3,"\n");    
          
          uart_sendStr(UART3,"The 2nd Benchmark sample is: ");
          ccd_print(g_char_ar_ccd_benchmark_two);
          uart_sendStr(UART3,"\n");     
          
          uart_sendStr(UART3,"         Previous sample is: ");
          ccd_print(g_char_ar_ccd_previous_pixel);
          uart_sendStr(UART3,"\n");    
          
          uart_sendStr(UART3,"      Just sampled Array is: ");
          ccd_print(g_char_ar_ccd_pixel);
          uart_sendStr(UART3,"\n");    
          
          ccd_sample_filtering();
          
          uart_sendStr(UART3,"   Sample used for decision: ");
          ccd_print(g_char_ar_ccd_pixel);
          uart_sendStr(UART3,"\n");   
          
          //print bottom line to separate each run
          for( p = 0 ; p < 150 ; p++){
             uart_putchar(UART3,'='); 
          }
          uart_sendStr(UART3,"\n");    
          
          gpio_set(PORTA, 8, 1);            // Trigger Oscilloscope          
          uart_sendStr(UART3,"*.*.*.* Trigger Oscilloscop *.*.*.*");
          uart_sendStr(UART3,"\n");    
          
       }
    }
    
    */
    
    //else if( (mode == 3 || mode == 8) ){ // Contiune Sampling & adjustable SI time
      
         if(g_u16_ccd_clock == 128 && g_int_sampling_state_flag == 1){
          g_int_SI_state_flag = 0;          // SI Flag off
          g_int_sampling_state_flag = 0;    // Sampling flag off
       
          // Print the sampling
          ccd_output_sample_to_UART();
          //uart_sendStr(UART3,"Just Sampled Array is: ");
          //ccd_print(g_char_ar_ccd_pixel);
          //uart_sendStr(UART3,"\n");     // New page form feed
          
          //gpio_set(PORTA, 8, 1);            // Trigger Oscilloscope          
          //uart_sendStr(UART3,"*.*.*.* Trigger Oscilloscop *.*.*.*");
          //uart_sendStr(UART3,"\n");     // New page form feed
          
          //g_int_ccd_operation_state = 0;
          
               
       }       
    //}
}

void ccd_output_sample_to_UART(){
  
  //if(g_int_ccd_sample_finish_flag == 1){
     uart_sendStr(UART3,"The Sample is:");
     ccd_print(g_char_ar_ccd_pixel);
     uart_sendStr(UART3,"\n"); 
  //}
}

void ccd_print(char array[]){
      
      int i;  
      for( i = 0 ; i < 128 ; i++){
           uart_putchar(UART3,array[i]);      // print One pixel One loop
      }
}


/*****************************************

Only useful when basic track detection is finished

void convert_char_to_readable_integer(int input_int, char output_char[]){
  
  if(input_int >= 0 && input_int < 10){
    
    output_char[0] = input_int + '0';             // ones 
    output_char[1] = '0';                         // tens
    output_char[2] = '0';                         // hundreds
    
  } else if (input_int >= 10 && input_int < 100){
    
    output_char[0] = (input_int%10) + '0';        // ones
    output_char[1] = (input_int/10)%10 + '0';     // tens
    output_char[2] = '0';                         // hundreds
    
  } else if (input_int >= 100 && input_int < 1000){
    
    output_char[0] = (input_int%10) + '0';        // ones
    output_char[1] = (input_int/10)%10 + '0';     // tens
    output_char[2] = (input_int/100)%10 + '0';    // hundreds
  }
  
}

void ccd_sample_filtering(){
  
  // compare currect sample with previos sample
  
  int ccd_same_pixel_count = 0;
  char ccd_similar_count_digit[3];
  int x;
  int y;
  
  // eveulate the similarity between previous sample and currenct sample
  // by int value 0 to 127, higher is better
  for( x = 0 ; x < 128 ; x++){
          if ( g_char_ar_ccd_previous_pixel[x] == g_char_ar_ccd_pixel[x]){
          ccd_same_pixel_count++;
          }
  }
      
  convert_char_to_readable_integer(ccd_same_pixel_count,ccd_similar_count_digit);
   
  for( x = 2 ; x >= 0 ; x--){
  uart_putchar(UART3,ccd_similar_count_digit[x]);
  }
  
  uart_sendStr(UART3," number of current pixels are same as pervious sample");
  uart_sendStr(UART3,"\n");     
  
  // if the current sample is unsatisfactory, replace the previous sample
  if(ccd_same_pixel_count > 1 && ccd_same_pixel_count < 60){
    
     for( y = 0 ; y < 128 ; y++){
     g_char_ar_ccd_pixel[y] = g_char_ar_ccd_previous_pixel[y]; 
     } 
     
      uart_sendStr(UART3,"\t\t\t\t");    
      uart_sendStr(UART3,"**************************************************************************");   
      uart_sendStr(UART3,"\n");      
      
      uart_sendStr(UART3,"\t\t\t\t");    
      uart_sendStr(UART3,"** The current sample has been filtered and replaced by previous sample **");
      uart_sendStr(UART3,"\n");     
      
      uart_sendStr(UART3,"\t\t\t\t");    
      uart_sendStr(UART3,"**************************************************************************");   
      uart_sendStr(UART3,"\n");     
  }
  
  // instead of using previous sample, use the benchmark sample
  // ... to be edit
  
}

void ccd_save_previous_sampling(){
  
      g_char_ar_ccd_previous_pixel[g_u16_ccd_sample_clock] = g_char_ar_ccd_pixel[g_u16_ccd_sample_clock]; 
      // copy previous array, before next sampling
      // ... to be test and edit
}

Only useful when basic sampling is correct


*****************************************/









/*

void ccd_hard_code_benchmark(){
 
      int j;
     
      for( j = 0 ; j < 128 ; j++){
       
            // condition for benchmark one array to be high
           if(j > 15 && j < 30 || (j > 85 && j < 100) ){ 
              g_char_ar_ccd_benchmark_one[j] = '1';
            }
           else{
              g_char_ar_ccd_benchmark_one[j] = '0';
            }
           
            // condition for benchmark two array to be high
           if(j > 13 && j < 19 || (j > 21 && j < 26) || (j > 28 && j < 30) || (j > 84 && j < 89)|| (j > 90 && j < 92)|| (j > 93 && j < 99)){ 
              g_char_ar_ccd_benchmark_two[j] = '1';
            }
           else{
              g_char_ar_ccd_benchmark_two[j] = '0';
           }
      }
}
*/