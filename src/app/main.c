#include "common.h"
#include "include.h"

/*************************************************************************
HKUST Smartcar 2013 Sensor Group
*************************************************************************/

volatile u32 g_u32_systemclock=0;                      // systemclock
volatile int g_int_SI_state_flag=0;                    // SI flag
volatile int g_int_sampling_state_flag=0;              // sampling state flag
char g_char_ar_ccd_pixel[128];                         // 1-line pixel array
u8 g_u8_ccd_sample_clock=0;
char g_char_mode=0;                                    // program mode

/*************************************************************************
Global Varaible
*************************************************************************/

//these two increment when pulse is received from encoder, zeroed when timed cycle PIT1 comes around
volatile u32 g_u32encoder_lf=0;
volatile u32 g_u32encoder_rt=0;

void interrupts_init(void);
void ccd_sampling(char g_char_mode);
u8 todis[];//for sprintf usage

/*************************************************************************
Global Varaible
*************************************************************************/
                                              
u8 todis[];                                           //for sprintf usage

void ALL_PIN_Init();
void interrupts_init(void);
void ccd_sampling(char g_char_mode);
void ccd_print(char []);

/*************************************************************************
Function header
*************************************************************************/

void main()
{   
   uart_init(UART3, 115200); // For our flashed bluetooth
   //uart_init(UART3, 9600); // For un-flash bluetooth
   
   printf("Please select mode:\n\f1:Accelerometer&gyro\n\f2:LinearCCD\n\f3:Tuning CCD\n\f4:Encoder testing\n\f");
   g_char_mode = uart_getchar(UART3);
   delayms(500); 
  
       switch (g_char_mode){
       case '1':
          uart_sendStr(UART3,"The mode now is 1: Accelerometer and Gyroscope");
           
          //interrupts_init();
          //uart_init(UART3, 115200); // Alreday init before switch into mode
          accl_init();
          printf("\nEverything Initialized alright\n");
          
          while(1)
          { 
              accl_print();
              //ccd_sampling(3); // sampling
              delayms(500);
          }
       break;
       
       case '2':
          uart_sendStr(UART3,"The mode now is 2: Linear CCD");
          interrupts_init();
          printf("\nEverything Initialized alright\n");
          
          while(1)
          { 
              ccd_sampling(2); // sampling, with more notice message
          }  
       break;
        
        case '3':
          uart_sendStr(UART3,"The mode now is 3: Tuning CCD");
          interrupts_init();
          printf("\nEverything Initialized alright\n");
          
          while(1)
          { 
              ccd_sampling(3); // Tuning CCD, with less notice message
          }  
       break;
               
       case '4':
       uart_sendStr(UART3,"The mode now is 4: encoder test");
               
       FTM_Input_init(FTM1,CH0,Rising);             //inits left encoder interrupt capture
       FTM_Input_init(FTM1,CH1,Rising);             //inits right encoder interrupt capture
               
       pit_init_ms(PIT0,500);                       //periodic interrupt every second or so
       
       printf("\nEverything Initialized alright\n");
       
       while(1)
       {}  
       break;
        
   }
}

void ccd_print(char array[]){
      
      int i;  
      
      for( i = 0 ; i < 128 ; i++){
           uart_putchar(UART3,array[i]); //print One pixel One loop
      }
}

void interrupts_init(void){
    
    DisableInterrupts;                                //Disable Interrupts
    ALL_PIN_Init();
    //pit_init_ms(PIT0,5);                            // Clock, 10ms period, 50% duty cycle
    pit_init_ms(PIT0,10);                           // Clock, 20ms period, 50% duty cycle
    //pit_init_ms(PIT0,0.01);                           // Clock, 20us period, 50% duty cycle
 
/*************************************************************************
Maximum clock is 8us cycle by using PIT_init_ms
*************************************************************************/
    
    //pit_init(PIT0,10);   // Faster Clock, 2us period, 50% duty cycle
    
    EnableInterrupts;			              //Enable Interrupts
}

void ccd_detect_track(){
 
      if(gpio_get(PORTB, 10) == 0) {  // if CCD receive black, the pixle respect to that g_u32_systemclock is 1
        g_char_ar_ccd_pixel[g_u8_ccd_sample_clock] = '1';
      }else {                        // if CCD receive white, the pixle respect to that g_u32_systemclock is 0
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

void ccd_finish_one_sampling(char g_char_mode){
  
    if (g_char_mode == 2){
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
    } else if(g_char_mode == 3){
      
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

//void trigger_SI(int local_clock, int mode){

void ccd_trigger_SI(char g_char_mode){
  
      //char str[1]; 
    
      if(g_char_mode == 2){
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
      }else if (g_char_mode == 3){
           if(g_int_SI_state_flag == 0 ){            // Use this instaed of , if(uart_pendstr(UART3,str) == 1) , can Auto sampling repeatedly
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

void ALL_PIN_Init(){

/************************************************************************* 
  Old Main Board
  
    gpio_init(PORTD, 0, GPO, 1); //PTD0, D2 LED
    gpio_init(PORTD, 1, GPO, 1); //PTD1, D3 LED
    gpio_init(PORTD, 2, GPO, 1); //PTD2, D4 LED
    gpio_init(PORTD, 3, GPO, 1); //PTD3, D5 LED
        
    gpio_init(PORTB, 18, GPO, 1);  //PTB18 , Clock / CLK
    gpio_init(PORTC, 19, GPO, 1);  //PTC19 , SI
    gpio_init(PORTA, 11, GPI, 1);  //PTA11 , AO
*************************************************************************/
  
     gpio_init(PORTE, 24, GPO, 0); //PTE24, D2 LED
     gpio_init(PORTE, 25, GPO, 0); //PTE25, D3 LED
     gpio_init(PORTE, 27, GPO, 0); //PTE27, D4 LED
     gpio_init(PORTE, 28, GPO, 0); //PTE28, D5 LED 
  
     gpio_init(PORTB, 22, GPO, 1);  //PTB22 , Clock / CLK
     gpio_init(PORTB, 18, GPO, 1);  //PTB18 , SI
     gpio_init(PORTB, 10, GPI, 1);  //PTB10 , AO
 
    //uart_init(UART3, 115200); // BlueTooth UART init
}
