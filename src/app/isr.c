#include "common.h"
#include "include.h"
#include "linearccd.h"

extern volatile unsigned int systemclock;   // systemclock counter
extern volatile int SI_state_flag;         // SI flag mode
extern volatile int sampling_state_flag;   // Smapling state flag mode
extern volatile u32 g_u32encoder_lf=0;
extern volatile u32 g_u32encoder_rt=0;

void PIT0_IRQHandler(void)
{
    
      gpio_turn(PORTB, 18);       // systemclock Rising and Failing edge
      
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
      
      systemclock++;
      PIT_Flag_Clear(PIT0);       //≤M§§¬_º–ß”¶Ï
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
    //this triggers on encoder pulse's rising edge
    /*connection config:
     
     Hardware        Port name       Program name    Physical location
     ---------------+---------------+---------------+-----------------
     encoder_left    PTA8            FTM1,CH0        JP6
     encoder_right   PTA9            FTM1,CH1        JP7
     */
    
    u8 s=FTM1_STATUS;
    u8 CHn;
    
    FTM1_STATUS=0x00;               //clear the register
    
    CHn=0;
    if( s & (1<<CHn) )
    {
        FTM_IRQ_DIS(FTM1,CHn);      //prevents input capture interrupt
 
        
        g_u32encoder_lf++;                             //计数+1
        
        //try not to use disableinterrupt; here, do it in main.c
        FTM_IRQ_EN(FTM1,CHn);  //matches with FTM_IRQ_DIS(FTM1,CHn); 

    }
    
    /* 这里添加 n=1 的模版，根据模版来添加 */
    CHn=1;
    if( s & (1<<CHn) )
    {
        FTM_IRQ_DIS(FTM1,CHn);      //prevents input capture interrupt
        
        
        g_u32encoder_rt++;                             //计数+1
        
        FTM_IRQ_EN(FTM1,CHn);  //开启输入捕捉中断
    }
    
}

