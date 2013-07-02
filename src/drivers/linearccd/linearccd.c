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

/************* Variables for direction PID : algorthm 1 *************/
char dir_pid_mode='U';
volatile int previous_dir_error=0;
char ccd_left_mode='0'; 
char ccd_right_mode='0';

/************* Variables for direction PID : algorthm 2 *************/
int current_mid_error_pos=0;
int last_sample_error_pos=0;

int first_start_up_flag=1;
int left_start_length=20;
int right_start_length=20;

int current_dir_error=0;
int last_sample_dir_error=125;


int current_dir_arc_value_error=0;


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
  
  if(detect_left_flag == 1 && detect_right_flag == 1){
    current_mid_error_pos = (current_1st_left_edge + current_1st_right_edge) / 2;
    //printf("Both side detected : STRAIGHT line");
  }
  
  else if(detect_left_flag == 1 && detect_right_flag == 0){
    current_mid_error_pos = current_1st_left_edge + right_start_length;
    if( current_1st_left_edge == 251){
      current_mid_error_pos = 125;
    }
    //printf("Left side detected : ONE-EDGE");
  }
  
  
  else if(detect_left_flag == 0 && detect_right_flag == 1){
    current_mid_error_pos = current_1st_right_edge - left_start_length;
    if(current_1st_right_edge == 0){
      current_mid_error_pos = 125;
    }
    //printf("Right side detected : ONE-EDGE");
  }
  
  else if(detect_left_flag == 0 && detect_right_flag == 0){
    current_mid_error_pos = 125;
  }
  
  current_dir_error = (current_mid_error_pos - 125);
  current_dir_arc_value_error = atan(current_dir_error*(0.005291005))*1000;
  
  /*
  printf("\n");
  printf("*** ***\n");
  printf("last_sample_error_pos is: ");
  printf("%d",last_sample_error_pos);
  printf("\n");
  */
  
  if(first_start_up_flag == 1){
    last_sample_error_pos = current_mid_error_pos;
  }
  
  /*
  printf("*** ***\n");
  
  printf("current_1st_left_edge is: ");
  printf("%d",current_1st_left_edge);
  printf("\n");
  
  printf("current_1st_right_edge is: ");
  printf("%d",current_1st_right_edge);
  printf("\n");
 
  printf("*** ***\n");
  
  printf("current_mid_error_pos is: ");
  printf("%d",current_mid_error_pos);
  printf("\n");
  
  printf("*** ***\n");
  
  
  printf("current_dir_error is: ");
  printf("%d",current_dir_error);
  printf("\n");
  
  printf("current_dir_arc_value_error is: ");
  printf("%d",current_dir_arc_value_error);
  printf("\n");
  
  printf("*** ***\n");
  */
  
}

void ccd_detect_current_left_right_edge_and_filter_middle_noise(char array[]){
  
  g_u16_ccd_left_pos=0; //reset to initial value
  g_u16_ccd_right_pos=250; //reset to initial value
  

  char ccd_abnormal_mode='0';
  u16 i;
    
  /***** Case 1 Variables *****/  
  u16 black_cont_left=0;
  u16 white_cout_left=0;
  //u16 left_break_pos=0;
  int first_time_left_white_break=0;
  u16 first_left_edge_lenght=0;  
  
  /***** Case 2 Variables *****/
  int first_time_left_black=0;  
  u16 first_left_black_pos=0;  
  
  /***** Case 3 Variables *****/    
  u16 black_cont_right=0;
  u16 white_cout_right=0;
  //u16 right_break_pos=0;
  int first_time_right_white_break=0;
  u16 first_right_edge_lenght=0;
  
  /***** Case 4 Variables *****/
  int first_time_right_black=0;  
  u16 first_right_black_pos=0;    
  
  /***** Abnormal case Variables ****
  u8 middle_black_counter=0;
  u16 all_white_counter=0;*/
  
  printf("\n");
  
  /***** Left mode *****/ 
  if(array[0] == 'W'){
    ccd_left_mode = '1';
    dir_pid_mode = 'S';
  }else if(array[0] == 'o'){
    ccd_left_mode = '2';
    dir_pid_mode = 'S';
  }
  
  /***** Right mode *****/   
  if(array[250] == 'W'){
    ccd_right_mode = '3';
    dir_pid_mode = 'S';
  }else if(array[250] == 'o'){
    ccd_right_mode = '4';
    dir_pid_mode = 'S';
  }

  if(array[0] == 'o' && array[250] == 'o'){ // one-side line case
    ccd_handle_one_side_black_edge(array);
    dir_pid_mode = 'E';
    ccd_left_mode ='0'; //reset
    ccd_right_mode ='0'; //reset
    printf("One-side track case\n");
  }

    
  /***** Left *****/ 
  switch(ccd_left_mode){  
  
  case '1': //Case 1: Left, start form Black edge  
    
  for( i = 0 ; i < 128 ; i++){ // scan 1st left half array
      if(array[i] == 'W'){
          if( first_time_left_white_break == 0){ // 1st part black is still continous 
              g_u16_ccd_left_pos = i; // update left edge
              first_left_edge_lenght = g_u16_ccd_left_pos+1;
          }else if( first_time_left_white_break == 1){ // black edge not continues alreday
              if(white_cout_left/2 > (first_left_edge_lenght)){ //  continues white/2 cannot > 1st left black part lenght
              i = 128; // filter case, keep previous left pos
              } else if(black_cont_left >= first_left_edge_lenght/3){ // if 2nd part black length > 1st part black length/3
              g_u16_ccd_left_pos = i; // update left edge 
              white_cout_left = 0; // erase previous, update left pos
              }
           }
          black_cont_left++;
      }else if (array[i] == 'o'){
          black_cont_left = 0;
            if( first_time_left_white_break == 0){
            //left_break_pos = i;
            first_time_left_white_break = 1;
            }
          white_cout_left++;
       }
  }
  break;
       
  case '2':  //Case 2: Left, start form White edge  
  
  for( i = 0 ; i < 128 ; i++){ // scan 1st left half array    
      if(array[i] == 'W'){     
        
          if( first_time_left_black == 0){ // if 1st part black edge happened, after a chain of white
            white_cout_left=0;
            first_time_left_black = 1;
            first_left_black_pos=i; // set 1st part edge, 1st black position
          }                      
           
          if( first_time_left_white_break == 0){ // 1st part black is still continous 
            g_u16_ccd_left_pos = i; // update left edge
            first_left_edge_lenght = (g_u16_ccd_left_pos - first_left_black_pos)+1;            
          }else if( first_time_left_white_break == 1){ // 2nd discontinues black part
              if(white_cout_left/2 > (first_left_edge_lenght)){ // continues white/2 cannot > 1st left black part lenght 
              i = 128; // filter case, keep previous left pos            
              }else if(black_cont_left >= first_left_edge_lenght/3){ // if 2nd part black length > 1st part black length/3
              g_u16_ccd_left_pos = i; // update left edge 
              white_cout_left = 0; // erase previous, update left pos
              }
          }
          black_cont_left++;    
      }else if (array[i] == 'o'){  
          black_cont_left = 0;           
            if( first_time_left_black == 0){ // beginning edge, this case must be white 
            // do nth            
            } else if( first_time_left_black == 1){ // 1st part black edge alreday dected           
                if( first_time_left_white_break == 0){
                //left_break_pos = i;
                first_time_left_white_break = 1;
                }            
            }          
          white_cout_left++;  
       }
  }  
  
  if( first_time_left_black == 0){
    dir_pid_mode = 'E';
    ccd_handle_one_side_black_edge(array);
  }
  
  break; 
  }
  
  
  /***** Right *****/    
  switch(ccd_right_mode){ 
    
  case '3': //Case 3: Right, start form Black edge   
  for( i = 250 ; i > 128 ; i--){ // scan right half array
      if(array[i] == 'W'){
          if( first_time_right_white_break == 0){ 
              g_u16_ccd_right_pos = i; // update right edge
              first_right_edge_lenght = (250 - g_u16_ccd_right_pos)+1;
          }else if( first_time_right_white_break == 1){ // black edge not continues alreday
              if(white_cout_right/2 > (first_right_edge_lenght)){ // continues white cannot > continues black/2
              i = 128; // filter case, keep previous right pos
              } else if(black_cont_right >= first_right_edge_lenght/3){ // if 2nd part black length > 1st part black length/2
              g_u16_ccd_right_pos = i ;// erase previous, update right pos 
              white_cout_right = 0;
              }
           }
          black_cont_right++;
      }else if (array[i] == 'o'){
          black_cont_right = 0;
            if( first_time_right_white_break == 0){
            //right_break_pos = i;
            first_time_right_white_break = 1;
            }
          white_cout_right++;
        }

  }
  break;
    
  case '4': //Case 4: Right, start form White edge
    
  for( i = 250 ; i > 128 ; i--){ // scan right half array    
      if(array[i] == 'W'){     
        
          if( first_time_right_black == 0){ // if 1st part black edge happened, after a chain of white
            white_cout_right=0;
            first_time_right_black = 1;
            first_right_black_pos=i; // set 1st part edge, 1st black position
          }                      
           
          if( first_time_right_white_break == 0){ // 1st part black is still continous 
            g_u16_ccd_right_pos = i; // update right edge
            first_right_edge_lenght = (first_right_black_pos - g_u16_ccd_right_pos)+1;            
          }else if( first_time_right_white_break == 1){ // 2nd discontinues black part
              if(white_cout_right/2 > (first_right_edge_lenght)){ // continues white/2 cannot > 1st left black part lenght 
              i = 128; // filter case, keep previous right pos            
              }else if(black_cont_right >= first_right_edge_lenght/3){ // if 2nd part black length > 1st part black length/3
              g_u16_ccd_right_pos = i; // update right edge 
              white_cout_right = 0; // erase previous, update right pos
              }
          }
          black_cont_right++;    
      }else if (array[i] == 'o'){  
          black_cont_right = 0;           
            if( first_time_right_black == 0){ // beginning edge, this case must be white 
            // do nth            
            } else if( first_time_right_black == 1){ // 1st part black edge alreday dected           
                if( first_time_right_white_break == 0){
                //right_break_pos = i;
                first_time_right_white_break = 1;
                }            
            }          
          white_cout_right++;  
       }
  }
  
  if( first_time_right_black == 0){
    dir_pid_mode = 'E';
    ccd_handle_one_side_black_edge(array);
  }
    
  break;
  
  }  
 
  ccd_calculate_current_mid_error(array);
}

void ccd_handle_one_side_black_edge(char array[]){
 
  u16 i;
  int first_time_left_black=0;
  int first_time_right_black=0;
  
  int left_cont_black_break=0;
  int right_cont_black_break=0;
  
  if(previous_dir_error > 0){
    
    printf("\n");
    printf("Previous dir error is positive \n");
  
    for( i = 0 ; i < 251 ; i++){ // scan from left to right
        if(array[i] == 'W'){
          
            if(first_time_left_black == 0){
              first_time_left_black = 1;
            } 
            
  
            if(left_cont_black_break == 0){
              g_u16_ccd_left_pos = i;  
            }
            
          } 
          else if(array[i] == 'o'){
          
            if( first_time_left_black == 1){ // means 1st part black alreday happen, discontinuous
              left_cont_black_break = 1;
              i = 250;
            }
          
          
        }
    }
  } else if (previous_dir_error < 0){
      
      printf("\n");
      printf("Previous dir error is negative \n");
    
    for( i = 250 ; i > 0 ; i--){ // scan from right to left
        if(array[i] == 'W'){
          if(first_time_right_black == 0){
            first_time_right_black = 1;
          }
          
          if(right_cont_black_break == 0){
            g_u16_ccd_right_pos = i;
          }
          
        }else if(array[i] == 'o'){
          
          if( right_cont_black_break == 1){
            left_cont_black_break = 1;
            i = 1;
          }
          
        }
    }
  }
  
}

void ccd_calculate_current_mid_error(char array[]){
  g_u16_ccd_middle_pos = ( g_u16_ccd_left_pos + g_u16_ccd_right_pos)/2;
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

/*********** CCD sample filtering ************/
void ccd_sample_filtering(char array[]){
  
  // compare currect sample with previos sample  
  int ccd_same_pixel_count = 0;
  char ccd_similar_count_digit[3];
  int x;
  int y;
  
  // eveulate the similarity between previous sample and currenct sample
  // by int value 0 to 127, higher is better
  for( x = 0 ; x < 128 ; x++){
          if ( g_char_ar_ccd_previous_pixel[x] == g_char_ar_ccd_current_pixel[x]){
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
        g_char_ar_ccd_current_pixel[y] = g_char_ar_ccd_previous_pixel[y]; 
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


/*********** Supplementary function************/

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

void ccd_save_previous_sampling(char input_array[], char output_stored_array[]){ 
      u16 i;  
      for( i = 0 ; i < 256 ; i++){
        output_stored_array[i] = input_array[i]; // copy previous array, before next sampling
      }
}


/************ dummy code 

void ccd_analyze_track_from_sample(char array[]){  
  u16 i;  
  u16 straight_line_similarity=0;
  u16 straight_line_positive_range=0;
  u16 straight_line_negative_range=0;
  
  for(i = 0 ; i < g_u16_ccd_right_pos ; i++){ // scan 1st half left array
    if(array[i] == '|'){
    straight_line_similarity++;
    }
  }
  
  for(i = g_u16_ccd_left_pos; i < 256 ; i++){ // scan 2nd half right array
    if(array[i] == '|'){
    straight_line_similarity++;
    }
  }  
  
  g_u16_ccd_valid_range = (g_u16_ccd_left_pos + (256 - g_u16_ccd_right_pos)); // reference value from 0 to 256
  
  printf("g_u16_ccd_valid_range reference is :");
  printf("%d", g_u16_ccd_valid_range);
  printf("\n");
 
  straight_line_positive_range = g_u16_ccd_valid_range - straight_line_similarity; // in valid range > similarity case 
  straight_line_negative_range = straight_line_similarity -  g_u16_ccd_valid_range; // in similarity > valid range case 
  
  printf("straight_line_possitive_range is :");
  printf("%d", straight_line_positive_range);
  printf("\n");
  
  printf("straight_line_negative_range :");
  printf("%d", straight_line_negative_range );
  printf("\n");
  
  if(straight_line_positive_range <= 20){ // straight line case 1
    //printf("\n");
    //printf("Valid reference range > similarity : Straight Line Case");  
    //printf("\n");
    gpio_set(PORTE,24,0); // Straight line case
  }
  else{
    gpio_set(PORTE,24,1);
  }
  
  if(straight_line_negative_range <= 20){  // straight line case 2
    //printf("\n");
    //printf("Similarity > valid reference range : Straight Line Case");  
    //printf("\n");
    gpio_set(PORTE,24,0);  // Straight line case
  }else{
    gpio_set(PORTE,24,1);
  }
 
}



void ccd_decide_range_for_detection(char array[]){
      u16 i;  
      u16 update_left;
      u16 update_right;
      for( i = 0 ; i < 128 ; i++){ // scan 1st half left array
        if(array[i] == '|'){
        g_u16_ccd_left_pos = i; // set left edge
        }
      }
      for( i = 256 ; i > 128 ; i--){ // scan 2nd half right array
        if(array[i] == '|'){
        g_u16_ccd_right_pos = i; // set right edge 
        }
      }

     //Left
     uart_sendStr(UART3,"This Left edge position is:"); 
     printf("%d", g_u16_ccd_left_pos);
     uart_sendStr(UART3,"\n");
     
     uart_sendStr(UART3,"Previous left edge position is:"); 
     printf("%d", g_u16_ccd_previous_left_pos);
     uart_sendStr(UART3,"\n");
     
     update_left = (g_u16_ccd_left_pos+g_u16_ccd_previous_left_pos)/2; // factor : 50% from previous average, 50% from current effect
     
     uart_sendStr(UART3,"Final left edge position for decision is:"); 
     printf("%d", update_left);
     uart_sendStr(UART3,"\n");
     uart_sendStr(UART3,"\n");

     g_u16_ccd_previous_left_pos = update_left;        //update previous pos to current pos 
     g_u16_ccd_left_pos = update_left;                 //final pos before this function end
     
     //Right
     uart_sendStr(UART3,"This Right edge position is:"); 
     printf("%d", g_u16_ccd_right_pos);
     uart_sendStr(UART3,"\n");
     
     uart_sendStr(UART3,"Previous right edge position is:"); 
     printf("%d", g_u16_ccd_previous_right_pos);
     uart_sendStr(UART3,"\n");
     
     update_right = (g_u16_ccd_right_pos+g_u16_ccd_previous_right_pos)/2; // factor : 50% from previous average, 50% from current effect
     
     uart_sendStr(UART3,"Final right edge position for decision is:"); 
     printf("%d", update_right);
     uart_sendStr(UART3,"\n");
     
     g_u16_ccd_previous_right_pos = update_right;         //update previous pos to current pos 
     g_u16_ccd_right_pos = update_right;                 //final pos before this function end
}


  isr.c
  if(g_int_ccd_benchmark_state == 1){
    
     FTM_PWM_Duty(FTM0,CH3,0);
     FTM_PWM_Duty(FTM0,CH2,0);
     gpio_set(PORTE,25,0);
     ccd_trigger_SI();
     ccd_sampling(g_char_ar_ccd_benchmark_reuse,1); 
     ccd_decide_range_for_detection(g_char_ar_ccd_benchmark_reuse); // caculate valid black range
     g_int_ccd_benchmark_counter++;
  
    if(g_int_ccd_benchmark_counter == g_int_ccd_preset_benchmark_time){
       g_int_ccd_benchmark_state = 0;
       gpio_set(PORTE,25,1);
    }
    gpio_set(PORTE,25,1);
  }
  

  
  // Abnormal mode
  if(array[0] == 'o' && array[250] == 'o'){ // dectected one-side black track or cross cases 
    
      for( i = 120; i < 130; i++){  // check one-side black similarity
        if(array[i] == 'o'){
          middle_black_counter++;
        }
        if(middle_black_counter >=5){ // similar fall into one-side black
          ccd_abnormal_mode ='7';
        }
      }
    
      for( i = 0; i < 256; i++){
        if(array[i] == 'o'){
          all_white_counter++;
        }
        if( all_white_counter >= 250){ // similar fall into cross
          ccd_abnormal_mode ='8';
        }
      }
    }
  
*/