#include "common.h"
#include "include.h"

/*************************************************************************
HKUST Smartcar 2013 Sensor Group
*************************************************************************/


void ALL_PIN_Init();
volatile unsigned int systemclock=0;             // systemclock
volatile int SI_state_flag=0;                    // SI flag mode
volatile int sampling_state_flag=0;              // Sample flag mode
char Pixel[128];
char str[1];
char mode=0;
int i;

//these two increment when pulse is received from encoder, zeroed when timed cycle PIT1 comes around
volatile u32 g_u32encoder_lf=0;
volatile u32 g_u32encoder_rt=0;


void interrupts_init(void);
void sampling(char mode);
u8 todis[];//for sprintf usage

void CCD_print(char []);

void main()
{   
   uart_init(UART3, 115200); // For our flashed bluetooth
   //uart_init(UART3, 9600); // For un-flash bluetooth
    printf("Please select mode:\n\f1:Accelerometer&gyro\n\f2:LinearCCD\n\f3:Tuning CCD\n\f4:Encoder testing\n\f");
   mode = uart_getchar(UART3);
   delayms(500); 
  
       switch (mode){
       case '1':
          uart_sendStr(UART3,"The mode now is 1: Accelerometer and Gyroscope");
           
          //interrupts_init();
          //uart_init(UART3, 115200); // Alreday init before switch into mode
          accl_init();
          printf("\nEverything Initialized alright\n");
          
          while(1)
          { 
              accl_print();
              //trigger_si(); // trigger SI to sample
              delayms(500);
          }
       break;
       
       case '2':
          uart_sendStr(UART3,"The mode now is 2: Linear CCD");
          interrupts_init();
          //uart_init(UART3, 115200);
          //accl_init();
          printf("\nEverything Initialized alright\n");
          
          while(1)
          { 
              //accl_print();
              sampling(2); // sampling
              //delayms(500);
          }  
        break;
        
        case '3':
          uart_sendStr(UART3,"The mode now is 3: Tuning CCD");
          interrupts_init();
          //uart_init(UART3, 115200);
          //accl_init();
          printf("\nEverything Initialized alright\n");
          
          while(1)
          { 
              //accl_print();
              sampling(3); // Tuning CCD
              //delayms(500);
          }  
        break;
               
       case '4':
       uart_sendStr(UART3,"The mode now is 4: encoder test");
               
       FTM_Input_init(FTM1,CH0,Rising);             //inits left encoder interrupt capture
       FTM_Input_init(FTM1,CH1,Rising);             //inits right encoder interrupt capture
               
       pit_init_ms(PIT0,500);                      //periodic interrupt every second or so

        
       printf("\nEverything Initialized alright\n");
       
       while(1)
       {}  
       break;
        
   }
}

void CCD_print(char array[]){
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
    // Maximum clock is 8us cycle by using PIT_init_ms
    
    //pit_init(PIT0,10);   // Faster Clock, 2us period, 50% duty cycle
    
    EnableInterrupts;			              //Enable Interrupts
}

void CCD_detect_track(){
 
      if(gpio_get(PORTA, 11) == 0) {  // if CCD receive black, the pixle respect to that systemclock is 1
        Pixel[systemclock] = '1';
      }else {                        // if CCD receive white, the pixle respect to that systemclock is 0
        Pixel[systemclock] = '0';
      } 
}

void SI_failing_edge_condition(int local_clock){
  
      //if(systemclock %21 == 20  &&  SI_state_flag == 1){     // condition for SI failing edge to end 
      if(local_clock == 20  &&  SI_state_flag == 1){     // condition for SI failing edge to end 
    
        gpio_set(PORTC, 19, 0); // SI faling edge
        uart_sendStr(UART3,"*.*.*.* SI failing edge happened *.*.*.*");
        uart_sendStr(UART3,"\014");  // New page form feed
      }
}


void CCD_finish_one_sampling(int mode){
    if (mode == 2){
        if(systemclock % 129 == 128 && sampling_state_flag == 1){ // condition for locking SI to end
          
          //systemclock = 0;          // No need this after change if into systemclock % 129 == 128
          SI_state_flag = 0;          // SI Flag off
          sampling_state_flag = 0;    // Sampling flag off
          gpio_set(PORTD, 1, 1);      // LED D3 ON, 1 means off, notify the SI is unlock
          uart_sendStr(UART3,"*.*.*.* SI locking mode end *.*.*.*");
          uart_sendStr(UART3,"\014"); // New page form feed
       
          // Print the sampling array
          uart_sendStr(UART3,"Just Sampled Array is: ");
          CCD_print(Pixel);
          uart_sendStr(UART3,"\n\014");     // New page form feed
        }
    } else if(mode == 3){
      
       if(systemclock % 129 == 128 && sampling_state_flag == 1){ // condition for locking SI to end
          
          SI_state_flag = 0;          // SI Flag off
          sampling_state_flag = 0;    // Sampling flag off
          gpio_set(PORTD, 1, 1);      // LED D3 ON, 1 means off, notify the SI is unlock
       
          // Print the sampling array
          uart_sendStr(UART3,"Just Sampled Array is: ");
          CCD_print(Pixel);
          uart_sendStr(UART3,"\n\014");     // New page form feed
       }
    }
}

void trigger_SI(int local_clock, int mode){
      if(mode == 2){
           if(SI_state_flag == 0 ){            // Use this instaed of , if(uart_pendstr(UART3,str) == 1) , can Auto sampling repeatedly
      
           //if(uart_pendstr(UART3,str) == 1){ // Using Bluetooth to trigger , when any key in PC keyboard is pressed
               uart_sendStr(UART3,"*.*.*.* SI Trigger, rising edge generated *.*.*.*");
               uart_sendStr(UART3,"\014");     // New page form feed same as uart_sendStr(UART3,"\n\f");
               SI_state_flag = 1;              // SI Flag on
               sampling_state_flag = 1;        // sampling Flag on
               //systemclock = 0;                // systemclock count from begin,lock the SI status not duplicate
               local_clock = 0;
               gpio_set(PORTC, 19, 1);         // SI rising edge
           }
      }else if (mode == 3){
           if(SI_state_flag == 0 ){            // Use this instaed of , if(uart_pendstr(UART3,str) == 1) , can Auto sampling repeatedly
               SI_state_flag = 1;              // SI Flag on
               sampling_state_flag = 1;        // sampling Flag on
               local_clock = 0;
               gpio_set(PORTC, 19, 1);         // SI rising edge
           }
     }
}

void sampling(char mode){
   
       int local_clock = 0;
       
       trigger_SI(local_clock,mode);
       
       local_clock++;
       
       CCD_detect_track(); 
       
       SI_failing_edge_condition(local_clock);
       
       CCD_finish_one_sampling(mode);
}

void ALL_PIN_Init(){
    
    gpio_init(PORTD, 0, GPO, 1); //PTD0, D2 LED
    gpio_init(PORTD, 1, GPO, 1); //PTD1, D3 LED
    gpio_init(PORTD, 2, GPO, 1); //PTD2, D4 LED
    gpio_init(PORTD, 3, GPO, 1); //PTD3, D5 LED
        
    gpio_init(PORTB, 18, GPO, 1);  //PTB18 , Clock / CLK
    gpio_init(PORTC, 19, GPO, 1);  //PTC19 , SI
    gpio_init(PORTA, 11, GPI, 1);  //PTA11 , AO
  
    //uart_init(UART3, 115200); // BlueTooth UART init
}