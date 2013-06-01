/*************************************************************************
  main.c
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
#include "stdlib.h"

/*************************************************************************
Global Varaible
*************************************************************************/
char g_char_mode=0;                 // debug and testing mode
volatile u32 g_u32_systemclock=0;   // systemclock counter

//these two increment when pulse is received from encoder, zeroed when timed cycle PIT1 comes around
volatile u32 g_u32encoder_lf=0;
volatile u32 g_u32encoder_rt=0;

volatile u32 g_u32encoder_lflast=0;
volatile u32 g_u32encoder_rtlast=0;

volatile int motor_deadzone_left,motor_deadzone_right;

volatile u32 balance_centerpoint_set=0;

u16 motor_test=0;

u8 todis[];//for sprintf usage

/*************************************************************************
Function header
*************************************************************************/
void all_pin_init(void);
void ccd_interrupts_init(void);
//move this into motor.h later
void motor_init(void);

void main()
{   
  uart_init(UART3, 115200); // For our flashed bluetooth
  //uart_init(UART3, 9600); // For un-flash bluetooth
   
  printf("\nWelcome to the SmartCar 2013 Sensor team developement system\n");
  while(1){
   printf("==============================================================\n");
   
   printf("Please select mode:\n---------------------------\n");
   printf("1:Accelerometer&gyro\n");
   printf("2:LinearCCD\n");
   printf("3:Tuning CCD\n");
   printf("4:Encoder testing\n");
   printf("5:CCDSample Filter Algorithm\n");
   printf("6:Motor control test\n");
   printf("7:SystemLoop Test\n");
   printf("8:Longer SI Sampling\n");
   
   g_char_mode = '7';                 // Hard code mode = system loop
   //g_char_mode = uart_getchar(UART3);
   
   delayms(500); 

     switch (g_char_mode){
      case '0':
        //VR analog input
        adc_init(ADC0,AD14);
        
        while(1){
          delayms(500);
          printf("\n%d",ad_ave(ADC0,AD14,ADC_12bit,10));//vr value
        }
          
      break;
       
      case '1':
        uart_sendStr(UART3,"The mode now is 1: Accelerometer and Gyroscope");
        
        //accl_init();
        adc_init(ADC1,AD6b);
        adc_init(ADC1,AD7b);

        printf("\nEverything Initialized alright\n");
        
        while(1)
        { 
          //printf("\n\f====================================");

          printf("\n%d",ad_once(ADC1,AD6b,ADC_16bit)-31834);//theta
          //printf("\n%d",ad_once(ADC1,AD7b,ADC_16bit)-36050);//omega
          delayms(50);

        }
     break;
     
     case '2':
        uart_sendStr(UART3,"The mode now is 2: Linear CCD");
        ccd_interrupts_init();
        printf("\nEverything Initialized alright\n");
        
        while(1)
        {  
           ccd_sampling(2,1); // sampling, with more notice message
        }  
     break;
      
      case '3':
        uart_sendStr(UART3,"The mode now is 3: Tuning CCD");
        ccd_interrupts_init();
        printf("\nEverything Initialized alright\n");
        
        while(1)
        {   
           
            ccd_sampling(3,1); // Tuning CCD, with less notice message
        }  
      break;
             
      case '4':
        //temporary case for debuggine encoder libraries, move to encoder.h later
        uart_sendStr(UART3,"The mode now is 4: encoder test");
             
        DisableInterrupts;
        exti_init(PORTA,8,rising_up);    //inits left encoder interrupt capture
        exti_init(PORTA,9,rising_up);    //inits right encoder interrupt capture
             
        pit_init_ms(PIT1,500);                 //periodic interrupt every 500ms
     
        EnableInterrupts;
        printf("\nEverything Initialized alright\n");
     
        while(1){
        
        }
          
      break;
      
        case '5':
        uart_sendStr(UART3,"The mode now is 5: CCD Sample Filtering");
        ccd_interrupts_init();
        printf("\nEverything Initialized alright\n");
        
        while(1)
        {  
            
            ccd_sampling(5,1); // CCD Sampling with filter algorithm
        }  
      break;
      case '6':
        uart_sendStr(UART3,"The mode now is 6: Motor Control test");
        //inits
        motor_init();
        
        printf("\nEverything Initialized alright\n");
        delayms(3000);
        
        while(1)
        { 
          //printf("\n\fInput 0-9 Motor Speed, Currently:%d",motor_test);
          //motor_test=100*(uart_getchar(UART3)-48);
          
          printf("\n\f Input direction : 0 or 1");
          motor_test = uart_getchar(UART3)-48;
         
          FTM_PWM_Duty(FTM0,CH2,200);//right 
          FTM_PWM_Duty(FTM0,CH3,200);//left
          
          if (motor_test){
            gpio_set(PORTB,22,1);
            gpio_set(PORTB,23,1);//this is left DIR
          }else{
            gpio_set(PORTB,22,0);
            gpio_set(PORTB,23,0);//this is DIR
          }
          
        }  
      break;

      case '7':
        printf("\n\f The Mode is now 7: SystemLoop Test");
        
        adc_init(ADC1,AD6b);
        adc_init(ADC1,AD7b);
        adc_init(ADC0,AD14);
        
        balance_centerpoint_set=ad_ave(ADC0,AD14,ADC_12bit,10);
        
        pit_init_ms(PIT3,1);
        ccd_interrupts_init(); // Louis added to text ccd code integration
        motor_init();
        delayms(4000);
        
        printf("\nEverything inited alright");
        while(1){
          //system loop runs
        }
      
     break;
      
        case '8':
        uart_sendStr(UART3,"The mode now is 8: Longer SI Sampling");
        ccd_interrupts_init();
        printf("\nEverything Initialized alright\n");
        
        while(1)
        { 
            ccd_sampling(8,1); // Longer SI CCD Sampling
        }  
      break;
      
      default :
        printf("\n\fYou entered:%c, Please enter a number from 1-7 to select a mode\n\f",g_char_mode);
        
    }
   }
}

void ccd_interrupts_init(void){
    
    DisableInterrupts;                                //Disable Interrupts
    all_pin_init();
    //pit_init_ms(PIT0,5);                            // Clock, 10ms period, 50% duty cycle
    //pit_init_ms(PIT0,10);                           // Clock, 20ms period, 50% duty cycle
    //pit_init_ms(PIT0,0.01);                         // Clock, 20us period, 50% duty cycle
 
    //Maximum clock is 8us cycle by using PIT_init_ms
    
    pit_init(PIT0,10);   // Faster Clock, 2us period, 50% duty cycle
    //pit_init(PIT0,100);
    
    //EnableInterrupts;			              //Enable Interrupts
}

void all_pin_init(){

 /*************************************************************************  
  Old Main Board
  
  Hardware        Port name       Program name    Physical location
  ---------------+---------------+---------------+-----------------
  ccd_3v-5v                                     JP5
  ccd_GND    					JP5/JP3
  ccd_SI	PTB19			        JP5
  ccd_clock	PTB18		                SPI-2b
  ccd_AO        PTA11	        	        JP8
 *************************************************************************/
  
    gpio_init(PORTB, 18, GPO, 1);  //PTB18 , Clock / CLK
    gpio_init(PORTB, 19, GPO, 1);  //PTC19 , SI
    gpio_init(PORTA, 11, GPI, 1);  //PTA11 , AO
    
    gpio_init(PORTA, 8, GPO, 1);   //PTA8  , Trigger Oscilloscope
   

 /*************************************************************************  
  New Main Board
  
     gpio_init(PORTE, 24, GPO, 0); //PTE24, D2 LED
     gpio_init(PORTE, 25, GPO, 0); //PTE25, D3 LED
     gpio_init(PORTE, 27, GPO, 0); //PTE27, D4 LED
     gpio_init(PORTE, 28, GPO, 0); //PTE28, D5 LED 
  
     gpio_init(PORTB, 22, GPO, 1);  //PTB22 , Clock / CLK
     gpio_init(PORTB, 18, GPO, 1);  //PTB18 , SI
     gpio_init(PORTB, 10, GPI, 1);  //PTB10 , AO
  *************************************************************************/
 
    //uart_init(UART3, 115200); // BlueTooth UART init
}

void motor_init(void){
  
  motor_deadzone_left=100;
  motor_deadzone_right=100;
     /*connection config:
     
     Hardware        DIR             PWM             Physical location
     ---------------+---------------+---------------+-----------------
     Motor right     PTB22           ftm0ch2        top??
     Motor left      PTB23           ftm0ch3        top??

     */
  FTM_PWM_init(FTM0,CH2,10000,0);//motor takes 0-1000 pwm values for duty
  FTM_PWM_init(FTM0,CH3,10000,0);//motor takes 0-1000 pwm values for duty
  
  gpio_init(PORTB,22,GPO,0);
  gpio_init(PORTB,23,GPO,0);
  
  DisableInterrupts;
  
  exti_init(PORTA,8,rising_up);    //inits left encoder interrupt capture
  exti_init(PORTA,9,rising_up);    //inits right encoder interrupt capture
             
             
  
  
  EnableInterrupts;
  

  
}