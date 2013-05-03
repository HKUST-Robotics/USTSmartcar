/*************************************************************************
HKUST Smartcar 2013 Sensor Group

Authoured by:
John Ching
Louis Mo
Yumi Chan

Hardware By:
Zyan Shao
*************************************************************************/
#include "common.h"
#include "include.h"


/*************************************************************************
Global Varaible
*************************************************************************/
char g_char_mode=0;                 // debug and testing mode
volatile u32 g_u32_systemclock=0;   // systemclock counter

//these two increment when pulse is received from encoder, zeroed when timed cycle PIT1 comes around
volatile u32 g_u32encoder_lf=0;
volatile u32 g_u32encoder_rt=0;

u8 todis[];//for sprintf usage


/*************************************************************************
Function header
*************************************************************************/
void ALL_PIN_Init();
void interrupts_init(void);


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
          //temporary case for debuggine encoder libraries, move to encoder.h later
          uart_sendStr(UART3,"The mode now is 4: encoder test");
               
          DisableInterrupts;
          FTM_Input_init(FTM1,CH0,Rising);             //inits left encoder interrupt capture
          FTM_Input_init(FTM1,CH1,Rising);             //inits right encoder interrupt capture
               
          pit_init_ms(PIT0,500);                       //periodic interrupt every second or so
       
          EnableInterrupts;
          printf("\nEverything Initialized alright\n");
       
          while(1){}
            
        break;
        
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
