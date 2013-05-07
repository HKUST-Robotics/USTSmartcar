#include "common.h"
#include "include.h"
#include "linearccd.h"

extern volatile u32 g_u32_systemclock;   // systemclock counter

extern volatile u32 g_u32encoder_lf;
extern volatile u32 g_u32encoder_rt;

extern volatile u32 g_u32encoder_lflast;
extern volatile u32 g_u32encoder_rtlast;

int motor_command_left,motor_command_right;
int motor_command_balance;

u8 system_mode=0;

void PIT0_IRQHandler(void)
{
    
      gpio_turn(PORTB, 22);       // systemclock Rising and Failing edge
      
      /* Using SW3 to generate 0 or 1
      if(gpio_get(PORTB, 4) == 0) {  // if SW3 is press, the pixle respect to that systemclock is 1
      Pixel[systemclock] = '1';
      }else {                        // if SW3 not press, the pixle respect to that systemclock is 0
      Pixel[systemclock] = '0';
      }
      */

      //uart_putchar(UART3,Pixel[systemclock]); // Print the Pixel[] array, 1 or 0 each print
      //uart_putchar(UART3,(u8)systemclock); // Print the systemclock value
      
      // For testing SI Flag
      //uart_sendStr(UART3, "SI_state_flag now is");
      //uart_putchar(UART3,SI_state_flag);
      
      g_u32_systemclock++;
      PIT_Flag_Clear(PIT0);       //?�M§§¬_º?�ß”¶�?}
}

void PIT1_IRQHandler(void)
{
    DisableInterrupts;
    printf("\n\fg_u32encoder_lf:%d",g_u32encoder_lf);
    printf("\n\fg_u32encoder_rt:%d",g_u32encoder_rt);
    
    g_u32encoder_lf=0;
    g_u32encoder_rt=0;
    
    PIT_Flag_Clear(PIT1);
    EnableInterrupts;
}

void FTM1_IRQHandler()
{
    //this triggers on encoder pulse's rising edge, for use on newer mainboard with ftm encoder captures
    /*
    On new board
    Hardware        Port name       Program name    Physical location
    ---------------+---------------+---------------+-----------------
    encoder_left    PTA8            FTM1,CH0        encoder0
    encoder_right   PTA9            FTM1,CH1        encoder1
     */
    
    u8 s=FTM1_STATUS;
    u8 CHn;
    
    FTM1_STATUS=0x00;               //clear the register
    
    CHn=0;
    if( s & (1<<CHn) )
    {
        FTM_IRQ_DIS(FTM1,CHn);      //prevents input capture interrupt
        
        g_u32encoder_lf++; 
        
        //try not to use disableinterrupt; here, do it in main.c
        FTM_IRQ_EN(FTM1,CHn);  //matches with FTM_IRQ_DIS(FTM1,CHn); 

    }
    
    
    CHn=1;
    if( s & (1<<CHn) )
    {
        FTM_IRQ_DIS(FTM1,CHn);      //prevents input capture interrupt
        
        
        g_u32encoder_rt++;
        
        FTM_IRQ_EN(FTM1,CHn);
    
    }
}

void encoder_counter(void){
  /*connection config:
     
     Hardware        Port name       Program name    Physical location
     ---------------+---------------+---------------+-----------------
     encoder_left    PTC18            exti ptc        servo1
     encoder_right   PTC19            exti ptc        servo2
   

     */
 
    u8  n=0;
    n=18;
    if(PORTC_ISFR & (1<<n)) 
    {
        PORTC_ISFR  |= (1<<n);
        
        g_u32encoder_lf++;
        
    } 
    
    n=19;
    if(PORTC_ISFR & (1<<n))
    {
        PORTC_ISFR  |= (1<<n);
        g_u32encoder_rt++;
    }
  
}

void pit3_system_loop(void){
  //main system control loop, runs every 1ms, each case runs every 5 ms
    /*  int motor_command_left,motor_command_right;
     int motor_command_balance; are available for passing commands*/
  
  switch (system_mode){
    case '0':
      //get gyro values
      
      system_mode=1;//go to next state on next cycle
    break;
    case 1:
      //get ccd values
      //i.e. sample(2);
      
      system_mode=2;
    break;
    case 2:
      //calculate balance command with input angle
      //in the end edit motor_command_balance to desired value
      
      system_mode=3;
    break;
    case 3:
      //calculate turning command from ccd
      //in the end set motor_command_left and motor_command_right to desired values;
      
      system_mode=4;
    break;
    case 4:
      //excute motor pwm with PID
      
      //error_left=u32encoder_lf - set;
      //ditto right
      
      
      //d value = 
      
      
      
      //saves current encoder count to last count
      g_u32encoder_lflast=g_u32encoder_lf;
      g_u32encoder_rtlast=g_u32encoder_rt;
      
      system_mode=0;
    break;
      
      
  }
}