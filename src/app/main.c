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

u16 motor_test=0;

u8 todis[];//for sprintf usage

/*************************************************************************
Function header
*************************************************************************/
void ALL_PIN_Init(void);
void interrupts_init(void);
//move this into motor.h later
void motor_init(void);

  

void main()
{   
   uart_init(UART3, 115200); // For our flashed bluetooth
   //uart_init(UART3, 9600); // For un-flash bluetooth
   
   printf("\n\fWelcome to the SmartCar 2013 Sensor team developement system\n\f");
   while(1){
   printf("========================================================\n\f");
   
   printf("Please select mode:\n\f---------------------------\n\f");
   printf("1:Accelerometer&gyro\n\f");
   printf("2:LinearCCD\n\f");
   printf("3:Tuning CCD\n\f");
   printf("4:Encoder testing\n\f");
   printf("5:CCDSample Filter Algorithm\n\f");
   printf("6:Motor control test\n\f");
   printf("7:SystemLoop Test\n\f");
   printf("8: Longer SI Sampling\n\f");
   
   //g_char_mode = '7';//uart_getchar(UART3); 
   g_char_mode = uart_getchar(UART3);
   delayms(500); 

     switch (g_char_mode){
      case '1':
        uart_sendStr(UART3,"The mode now is 1: Accelerometer and Gyroscope");
        
        //accl_init();
        adc_init(ADC1,AD6b);
        adc_init(ADC0,AD15);

        printf("\nEverything Initialized alright\n");
        
        while(1)
        { 
          //printf("\n\f====================================");

          printf("\n%d",ad_once(ADC1,AD6b,ADC_16bit)-29300);//theta
          printf("\n%d",ad_once(ADC0,AD15,ADC_16bit)-33850);//omega
            delayms(50);

          printf("\n\f%d",ad_once(ADC1,AD6b,ADC_16bit));
          //printf("\n\ftheta is: %d",ad_once(ADC1,AD7b,ADC_16bit));
            delayms(100);

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
        exti_init(PORTC,18,rising_up);             //inits left encoder interrupt capture
        exti_init(PORTC,19,rising_up);            //inits right encoder interrupt capture
             //FTM_Input_init(FTM1,CH0,Rising); for new board
             
        pit_init_ms(PIT1,500);                 //periodic interrupt every 500ms
     
        EnableInterrupts;
        printf("\nEverything Initialized alright\n");
     
        while(1){}
          
      break;
      
        case '5':
        uart_sendStr(UART3,"The mode now is 5: CCD Sample Filtering");
        interrupts_init();
        printf("\nEverything Initialized alright\n");
        
        while(1)
        { 
            ccd_sampling(5); // CCD Sampling with filter algorithm
        }  
      break;
      case '6':
        uart_sendStr(UART3,"The mode now is 6: Motor Control test");
        motor_init();
        printf("\nEverything Initialized alright\n");
        delayms(3000);
        
        while(1)
        { 
          //printf("\n\fInput 0-9 Motor Speed, Currently:%d",motor_test);
          //motor_test=100*(uart_getchar(UART3)-48);
          
          printf("\n\f Input direction : 0 or 1");
          motor_test = uart_getchar(UART3)-48;
         
          FTM_PWM_Duty(FTM0,CH6,200);
          
          
          FTM_PWM_Duty(FTM0,CH5,200);
          
          if (motor_test){
            gpio_init(PORTD,7,GPO,1);
            gpio_init(PORTE,11,GPO,0);//this is DIR
          }else{
            gpio_init(PORTD,7,GPO,0);
            gpio_init(PORTE,11,GPO,1);//this is DIR
          }
          
        }  
      break;

      case '7':
        printf("\n\f The Mode is now 7: SystemLoop Test");
        
        adc_init(ADC1,AD6b);
        adc_init(ADC0,AD15);
        pit_init_ms(PIT3,1);
        motor_init();
        delayms(4000);
        
        printf("\nEverything inited alright");
        while(1){
          //system loop runs
        }
      
     break;
      
        case '8':
        uart_sendStr(UART3,"The mode now is 8: Longer SI Sampling");
        interrupts_init();
        printf("\nEverything Initialized alright\n");
        
        while(1)
        { 
            ccd_sampling(8); // Longer SI CCD Sampling
        }  
      break;
     
      
      default :
        printf("\n\fYou entered:%c, Please enter a number from 1-7 to select a mode\n\f",g_char_mode);
        
    }
   }
}

void interrupts_init(void){
    
    DisableInterrupts;                                //Disable Interrupts
    ALL_PIN_Init();
    //pit_init_ms(PIT0,5);                            // Clock, 10ms period, 50% duty cycle
    //pit_init_ms(PIT0,10);                           // Clock, 20ms period, 50% duty cycle
    //pit_init_ms(PIT0,0.01);                         // Clock, 20us period, 50% duty cycle
 
/*************************************************************************
Maximum clock is 8us cycle by using PIT_init_ms
*************************************************************************/
    
    pit_init(PIT0,10);   // Faster Clock, 2us period, 50% duty cycle
    //pit_init(PIT0,100);
    
    EnableInterrupts;			              //Enable Interrupts
}

void ALL_PIN_Init(){

 /*************************************************************************  
  Old Main Board
 *************************************************************************/
    //gpio_init(PORTD, 0, GPO, 1); //PTD0, D2 LED
    //gpio_init(PORTD, 1, GPO, 1); //PTD1, D3 LED
    //gpio_init(PORTD, 2, GPO, 1); //PTD2, D4 LED
    //gpio_init(PORTD, 3, GPO, 1); //PTD3, D5 LED
        
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
     Motor Left      PTD7            ftm0ch6        Motor0
     Motor Right     PTE11           ftm0ch5        Motor1

     */
  gpio_init(PORTD,7,GPO,1);
  gpio_init(PORTE,11,GPO,0);//this is DIR
  
  FTM_PWM_init(FTM0,CH6,10000,0);//motor takes 0-1000 pwm values for duty
  FTM_PWM_init(FTM0,CH5,10000,0);//motor takes 0-1000 pwm values for duty
  
  DisableInterrupts;
  exti_init(PORTC,18,rising_up);             //inits left encoder interrupt capture
  exti_init(PORTC,19,rising_up);            //inits right encoder interrupt capture
  
  EnableInterrupts;
  
//  while(g_u32encoder_lf<5&&motor_deadzone_left<200){
//    motor_deadzone_left++;
//    FTM_PWM_Duty(FTM0,CH6,motor_deadzone_left);
//    delayms(100);
//  }
//  while(g_u32encoder_rt<5&&motor_deadzone_right<200){
//    motor_deadzone_right++;
//    FTM_PWM_Duty(FTM0,CH6,motor_deadzone_right);
//    delayms(100);
//  }
    
  
    
  
}