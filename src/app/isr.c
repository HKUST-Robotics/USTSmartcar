#include "common.h"
#include "include.h"
#include "linearccd.h"

extern volatile unsigned int systemclock;   // systemclock counter
extern volatile int SI_state_flag;         // SI flag mode
extern volatile int sampling_state_flag;   // Smapling state flag mode

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
      PIT_Flag_Clear(PIT0);       //清中斷標志位
}




