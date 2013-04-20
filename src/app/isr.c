/******************** (C) COPYRIGHT 2011 野火嵌入式開發工作室 ********************
 * 文件名       ：isr.c
 * 描述         ：中斷處理例程
 *          
 * 實驗平台     ：野火kinetis開發板
 * 庫版本       ：
 * 嵌入系統     ：
 *
 * 作者         ：野火嵌入式開發工作室 
 * 淘寶店       ：http://firestm32.taobao.com
 * 技術支持論壇 ：http://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
**********************************************************************************/	



#include "common.h"
#include "include.h"


/*************************************************************************
*                             野火嵌入式開發工作室
*  
*  函數名稱：PIT0_IRQHandler
*  功能說明：PIT0 定時中斷服務函數
*  參數說明：無   
*  函數返回：無
*  修改時間：2012-2-18    已測試
*  備    注：
*************************************************************************/

extern volatile u16 clock;                 // clock counter
extern volatile int SI_state_flag;         // SI flag mode
extern volatile int smapling_state_flag;   // Smapling state flag mode
char Pixel[128];
char str[1];
int i;

void PIT0_IRQHandler(void)
{
    
      gpio_turn(PORTD, 3);        // LED On or Off
      gpio_turn(PORTB, 18);       // Clock Rising and Failing edge
      
      /* Using SW3 to generate 0 or 1
      if(gpio_get(PORTB, 4) == 0) {  // if SW3 is press, the pixle respect to that clock is 1
      Pixel[clock] = '1';
      }else {                        // if SW3 not press, the pixle respect to that clock is 0
      Pixel[clock] = '0';
      }
      */

      
      //uart_putchar(UART3,Pixel[clock]); // Print the Pixel[] array, 1 or 0 each print
      //uart_putchar(UART3,(u8)clock); // Print the clock value
      
      // For testing SI Flag
      //uart_sendStr(UART3, "SI_state_flag now is");
      //uart_putchar(UART3,SI_state_flag);
      
      
      //&& SI_state_flag == 0 ){ 
      
      if(uart_pendstr(UART3,str) == 1){ // Using Bluetooth to trigger , when any key in PC keyboard is pressed
           uart_sendStr(UART3,"*.*.*.* SI Trigger, rising edge generated *.*.*.*");
           uart_sendStr(UART3,"\014");  // New page form feed same as uart_sendStr(UART3,"\n\f");
           SI_state_flag = 1;           // SI Flag on
           smapling_state_flag = 1;     // sampling Flag on
           clock = 0;                   // Clock count from begin,lock the SI status not duplicate
           gpio_set(PORTD, 0, 0);       // LED D2 ON, 0 means on, notify the switch is on
           gpio_set(PORTD, 1, 0);       // LED D3 ON, 0 means on, notify the SI is in locking mode
           gpio_set(PORTC, 19, 1);      // SI rising edge
       }
        else{                          // if SW2 not press
           gpio_set(PORTD, 0, 1);         // LED D2 OFF, 1 means off, means keyboard is not pressed
       }
      
      if(gpio_get(PORTA, 11) == 0) {  // if CCD receive black, the pixle respect to that clock is 1
        Pixel[clock] = '1';
      }else {                        // if CCD receive white, the pixle respect to that clock is 0
        Pixel[clock] = '0';
      } 
       
      if(clock == 20  &&  SI_state_flag == 1){     // condition for SI failing edge to end 
        gpio_set(PORTC, 19, 0); // SI faling edge
        uart_sendStr(UART3,"*.*.*.* SI failing edge happened *.*.*.*");
        uart_sendStr(UART3,"\014");  // New page form feed
      }
      
      
      if(clock == 128 && smapling_state_flag == 1){ // condition for locking SI to end
        
        clock = 0;
        SI_state_flag = 0;          // SI Flag off
        smapling_state_flag = 0;    // Sampling flag off
        gpio_set(PORTD, 1, 1);      // LED D3 ON, 1 means off, notify the SI is unlock
        uart_sendStr(UART3,"*.*.*.* SI locking mode end *.*.*.*");
        uart_sendStr(UART3,"\014"); // New page form feed
     
        // Print the sampling array
        uart_sendStr(UART3,"Just Sampled Array is: ");
        uart_sendStr(UART3,"\014");     // New page form feed
          
        for( i = 0 ; i < 128 ; i++){
           uart_putchar(UART3,Pixel[i]); // Print One pixel One loop
        }
           
          uart_sendStr(UART3,"\014");     // New page form feed
      }
     
      clock++;
      PIT_Flag_Clear(PIT0);       //清中斷標志位
}




