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
volatile int g_u32encoder_lf=0;
volatile int g_u32encoder_rt=0;

volatile u32 g_u32encoder_lflast=0;
volatile u32 g_u32encoder_rtlast=0;

volatile int motor_deadzone_left,motor_deadzone_right;

volatile u32 balance_centerpoint_set=0;
extern volatile int control_tilt;

u16 motor_test=0;

u8 todis[];//for sprintf usage

/*************************************************************************
Function header
*************************************************************************/
void ccd_interrupts_init(void);
void ccd_all_pin_init(void);
//move this into motor.h later
void motor_init(void);

void main()
{   

  uart_init(UART3, 115200); // For our flashed bluetooth
  //uart_init(UART3, 9600); // For our flashed bluetooth
  
  
  printf("\nWelcome to the SmartCar 2013 Sensor team developement system\n");
  while(1){
    
   printf("==============================================================\n");
   printf("Please select mode:\n---------------------------\n");
   printf("1:Accelerometer&gyro\n");
   printf("2:LinearCCD\n");
   printf("4:Encoder testing\n");
   printf("6:Motor control test\n");
   printf("7:SystemLoop Test\n");
   
   delayms(2500);
   g_char_mode = '7';                 // Hard code mode = system loop
   //g_char_mode = uart_getchar(UART3);
 
     switch (g_char_mode){
      case '0':
        //VR analog input
        adc_init(ADC0,AD14);
        
        while(1){
          delayms(500);
          printf("\n%d",ad_once(ADC0,AD14,ADC_16bit));//vr value
        }
          
      break;
       
      case '1':
        uart_sendStr(UART3,"The mode now is 1: Accelerometer and Gyroscope Test");
        
        //accl_init();
        adc_init(ADC1,AD6b);
        adc_init(ADC1,AD7b);
        adc_init(ADC0,AD14);
        
        balance_centerpoint_set=ad_ave(ADC0,AD14,ADC_12bit,10);

        printf("\nEverything Initialized alright\n");
        
        while(1)
        { 
          //printf("\n\f====================================");
          control_tilt=(ad_ave(ADC1,AD6b,ADC_12bit,8)-3300)+(balance_centerpoint_set/10);
          //printf("\nMain gyro%d",control_tilt);//theta
          printf("\n%d",ad_once(ADC1,AD7b,ADC_12bit)-1940);//omega
          delayms(50);
        }
     break;
     
     case '2':
        uart_sendStr(UART3,"The mode now is 2: Linear CCD");
        ccd_interrupts_init();
        printf("\nEverything Initialized alright\n");
       
        while(1)
        { 
            //ccd_sampling(1); // Longer SI CCD Sampling
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

      case '6':
        uart_sendStr(UART3,"The mode now is 6: Motor Control test");
        //inits
        motor_init();
        
        printf("\nEverything Initialized alright\n");
        delayms(1000);
        
        while(1)
        { 
          //printf("\n\fInput 0-9 Motor Speed, Currently:%d",motor_test);
          //motor_test=100*(uart_getchar(UART3)-48);
          
          FTM_PWM_Duty(FTM1,CH0,2000);
          FTM_PWM_Duty(FTM1,CH1,2000);//left
          
          printf("\n\f Input direction : 0 or 1");
          motor_test = uart_getchar(UART3)-48;
         
          
          
          if (motor_test){
            gpio_set(PORTD,9,1);
            gpio_set(PORTD,7,1);//this is DIR
          }else{
            gpio_set(PORTD,9,0);
            gpio_set(PORTD,1,0);//this is DIR
          }
          
        }  
      break;

      case '7':
        printf("\n The Mode is now 7: SystemLoop Test");
        
        adc_init(ADC1,AD6b);
        adc_init(ADC1,AD7b);
        adc_init(ADC0,AD14);
        
        balance_centerpoint_set=ad_ave(ADC0,AD14,ADC_12bit,10);
        
        motor_init();
        
        gpio_set(PORTD,9,0); //dir
        gpio_set(PORTD,7,0); //dir
        
        FTM_PWM_Duty(FTM1, CH0, 3000); //initital speed
        FTM_PWM_Duty(FTM1, CH1, 3000); //initital speed               
        
        ccd_interrupts_init();
        pit_init_ms(PIT3,1);
        //delayms(4000);
        
        printf("\nEverything inited alright");
        
        
        while(1){
          //system loop runs 
        }
        
     break;
      
     default :
     printf("\n\fYou entered:%c, Please enter a number from 1-7 to select a mode\n\f",g_char_mode);
        
    }
   }
}

void ccd_interrupts_init(void){
    DisableInterrupts;                                
    ccd_all_pin_init(); // init ccd gpio  
    pit_init(PIT0,10);  // Faster Clock, 2.5us period, 50% duty cycle
    EnableInterrupts;			             
}

void ccd_all_pin_init(){
  
 /****************** 1st Gen Main Board ******************/
  //gpio_init(PORTB, 18, GPO, 1);  //PTB18 , Clock / CLK
  //gpio_init(PORTB, 19, GPO, 1);  //PTC19 , SI
  //gpio_init(PORTA, 11, GPI, 1);  //PTA11 , AO
  
 /****************** 2nd Gen Main Board ******************/
   gpio_init(PORTB, 8, GPO, 1);    //PTB8 , SI
   gpio_init(PORTB, 9, GPO, 1);    //PTB9 , Clock / CLK
   gpio_init(PORTB, 10, GPI, 1);   //PTB10, AO(D1)
   
   //gpio_init(PORTD, 10, GPO, 1);   
   //gpio_init(PORTD, 9, GPO, 1);   
   
   
   //LED_init(); // To test ccd sampling function is operating
} 

void motor_init(void){
  
  //motor_deadzone_left=100;
  //motor_deadzone_right=100;
     /*connection config:
     Hardware        DIR             PWM             Physical location
     ---------------+---------------+---------------+-----------------
     Motor right     PTD9           FTM1_CH1        top??
     Motor left      PTD7           FTM1_CH0        top??
     */
  //FTM_PWM_init(FTM0,CH2,10000,0);//motor takes 0-10000 pwm values for duty
  //FTM_PWM_init(FTM0,CH3,10000,0);//motor takes 0-10000 pwm values for duty
  
  //gpio_init(PORTB,22,GPO,0);
  //gpio_init(PORTB,23,GPO,0);
  
  FTM_PWM_init(FTM1,CH0,10000,0);//motor takes 0-10000 pwm values for duty
  FTM_PWM_init(FTM1,CH1,10000,0);//motor takes 0-10000 pwm values for duty
 
  gpio_init(PORTD,9,GPO,0); // Right motor dir
  gpio_init(PORTD,7,GPO,0); // Left motor dir
  
  printf("Before Interrupt");
  
  DisableInterrupts;
  
  printf("Starting exti Interrupt");
  
  exti_init(PORTA,6,rising_up);    //inits left encoder interrupt capture
  exti_init(PORTA,7,rising_up);    //inits right encoder interrupt capture
  
  EnableInterrupts;
  
  printf("Finishing exti Interrupt");
  
}  