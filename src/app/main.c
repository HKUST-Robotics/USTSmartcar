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

/************ Global Varaible ************/
char g_char_mode=0;                 // debug and testing mode

//these two increment when pulse is received from encoder, zeroed when timed cycle PIT1 comes around
volatile int g_u32encoder_lf=0;
volatile int g_u32encoder_rt=0;

volatile u32 g_u32encoder_lflast=0;
volatile u32 g_u32encoder_rtlast=0;

u32 balance_gyro_offset=0;
u32 turn_gyro_offset=0;

volatile int motor_deadzone_left,motor_deadzone_right;

volatile u32 balance_centerpoint_set=0;
extern volatile int control_tilt;

u16 motor_test=0;

/************ Function header ************/
void ccd_interrupts_init(void);
void ccd_all_pin_init(void);
void light_sensor_init();
void motor_init(void); 

void main()
{   

  uart_init(UART3, 115200); // For our flashed bluetooth
  //uart_init(UART3, 9600); // For our non-flashed bluetooth
  gpio_init(PORTE,6,GPI,0); // SW2 goes into gyro calibration mode
  
  
  printf("\nWelcome to the SmartCar 2013 Sensor team developement system\n");
  while(1){
    
   printf("==============================================================\n");
   printf("Please select mode:\n---------------------------\n");
   printf("1:Accelerometer&gyro\n");
   printf("2:LinearCCD\n");
   printf("3:Flash Memory\n");
   printf("4:Encoder testing\n");
   printf("6:Motor control test\n");
   printf("7:SystemLoop Test\n");
   
   delayms(300);
   
   if(gpio_get(PORTE,6)==0){
     
     adc_init(ADC1,AD5b);
     adc_init(ADC1,AD7b);
        
     printf("We are now in gyro calibration mode, Please keep car stationery and wait till light dies");
     gpio_init(PORTE,24,GPO,0);
     gpio_init(PORTE,25,GPO,0);
     gpio_init(PORTE,26,GPO,0);
     gpio_init(PORTE,27,GPO,0);
     
     delayms(3000);
     
     turn_gyro_offset=ad_ave(ADC1,AD5b,ADC_12bit,1000);
     balance_gyro_offset=ad_ave(ADC1,AD7b,ADC_12bit,1000);
     
     store_u32_to_flashmem1(turn_gyro_offset);
     store_u32_to_flashmem2(balance_gyro_offset);
     
     gpio_turn(PORTE,24);
     gpio_turn(PORTE,25);
     gpio_turn(PORTE,26);
     gpio_turn(PORTE,27);
     
     while(1){}
     
   }
   
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
        adc_init(ADC1,AD4b);
        
        
        balance_centerpoint_set=ad_ave(ADC0,AD14,ADC_12bit,10);

        printf("\nEverything Initialized alright\n");
        
        while(1)
        { 
          //printf("\n\f====================================");
          //control_tilt=(ad_ave(ADC1,AD6b,ADC_12bit,8)-3300)+(balance_centerpoint_set/10);
          //printf("\nMain gyro%d",control_tilt);//theta
          //printf("\n%d",ad_once(ADC1,AD7b,ADC_12bit)-1940);//omega
          printf("\n%d",ad_ave(ADC1,AD6b,ADC_12bit,8)-940);
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
     
     case '3':
        uart_sendStr(UART3,"The mode now is 3:Flash Memory\n");
        Flash_init();
        printf("Flash Memory init ok\n");
        printf("writing the u32 : 3125 to memory 1\n");
        store_u32_to_flashmem1(3125);
        printf("writing the u32: 1019 to memory 2\n");
        store_u32_to_flashmem2(1019);
        
        printf("Now reading from memory\n");
        printf("Memory 1:%d\n",get_u32_from_flashmem1());
        
        printf("Memory 2:%d\n",get_u32_from_flashmem2());
        
        
       
        while(1)
        { 
            //stops loop to see results
        }
     break;
        
      case '4':
        //temporary case for debuggine encoder libraries, move to encoder.h later
        uart_sendStr(UART3,"The mode now is 4: encoder test");
             
        DisableInterrupts;
        exti_init(PORTA,6,rising_up);    //inits left encoder interrupt capture
        exti_init(PORTA,7,rising_up);    //inits right encoder interrupt capture
             
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
        adc_init(ADC1,AD5b);
        
        balance_centerpoint_set=ad_ave(ADC0,AD14,ADC_12bit,10);
        
        motor_init();
        
        gpio_set(PORTD,9,0); //dir
        gpio_set(PORTD,7,0); //dir
        
        FTM_PWM_Duty(FTM1, CH0, 3000); //initital speed
        FTM_PWM_Duty(FTM1, CH1, 3000); //initital speed               
        
        ccd_interrupts_init();
        pit_init_ms(PIT3,1);
        
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
  
 /****************** 2nd Gen Main Board ******************/
   gpio_init(PORTB, 8, GPO, 1);    //PTB8 , SI
   gpio_init(PORTB, 9, GPO, 1);    //PTB9 , Clock / CLK
   gpio_init(PORTB, 10, GPI, 1);   //PTB10, AO(D1)
   
   //gpio_init(PORTE,6,GPI,0); // SW2
   gpio_init(PORTE,8,GPI,0); // SW4
   gpio_init(PORTE,9,GPI,0); // SW5
   
   LED_init(); // To test ccd sampling function is operating
   
   light_sensor_init();
   
   gpio_set(PORTE,26,0); // defatul speed mode = 0
} 

void light_sensor_init(){
   gpio_init(PORTB, 21, GPO, 1);    //PTB21 , Right
   gpio_init(PORTB, 22, GPO, 1);    //PTB22 , Middle
   gpio_init(PORTB, 23, GPI, 1);    //PTB23 , Left
}

void motor_init(void){
  
  //motor_deadzone_left=100;
  //motor_deadzone_right=100;
     
  /*connection config:
     Hardware        DIR             PWM             Physical location
     ---------------+---------------+---------------+-----------------
     Motor right     PTD9           FTM1_CH1        top??
     Motor left      PTD7           FTM1_CH0        top?? */
     
  FTM_PWM_init(FTM1,CH0,10000,0);//motor takes 0-10000 pwm values for duty
  FTM_PWM_init(FTM1,CH1,10000,0);//motor takes 0-10000 pwm values for duty
 
  gpio_init(PORTD,9,GPO,0); // Right motor dir
  gpio_init(PORTD,7,GPO,0); // Left motor dir
  
  gpio_init(PORTC,4,GPI_UP,1);//encoder rt dir
  gpio_init(PORTC,5,GPI_UP,1);//encoder lt dir
  
  DisableInterrupts;
  
  exti_init(PORTA,6,rising_up);    //inits left encoder interrupt capture
  exti_init(PORTA,7,rising_up);    //inits right encoder interrupt capture
  
  EnableInterrupts;
}  