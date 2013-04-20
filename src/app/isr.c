/******************** (C) COPYRIGHT 2011 �����O�J���}�o�u�@�� ********************
 * ���W       �Gisr.c
 * �y�z         �G���_�B�z�ҵ{
 *          
 * ���祭�x     �G����kinetis�}�o�O
 * �w����       �G
 * �O�J�t��     �G
 *
 * �@��         �G�����O�J���}�o�u�@�� 
 * �^�_��       �Ghttp://firestm32.taobao.com
 * �޳N����׾� �Ghttp://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
**********************************************************************************/	



#include "common.h"
#include "include.h"


/*************************************************************************
*                             �����O�J���}�o�u�@��
*  
*  ��ƦW�١GPIT0_IRQHandler
*  �\�໡���GPIT0 �w�ɤ��_�A�Ȩ��
*  �Ѽƻ����G�L   
*  ��ƪ�^�G�L
*  �ק�ɶ��G2012-2-18    �w����
*  ��    �`�G
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
      PIT_Flag_Clear(PIT0);       //�M���_�ЧӦ�
}




