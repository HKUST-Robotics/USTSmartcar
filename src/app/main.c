/******************** (C) COPYRIGHT 2011 野火嵌入式開發工作室 ********************
 * 文件名       ：main.c
 * 描述         ：PIT定時中斷實驗
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
*                               PIT定時中斷測試
*
*  實驗說明：野火 PIT 定時中斷實驗，在中斷函數了用 LED0 顯示進入了中斷函數。
*            
*  實驗操作：無
*
*  實驗效果：LED0間隔1s閃爍
*
*  實驗目的：測試 PIT 是否定時產生中斷
*
*  修改時間：2012-2-29     已測試
*
*  備    注：野火Kinetis開發板的 LED0~3 ，分別接PTD15~PTD12 ，低電平點亮
*            
*************************************************************************/

void ALL_PIN_Init();
volatile unsigned int systemclock=0;                            // for locking SI
volatile int SI_state_flag=0;                    // SI flag mode
volatile int smapling_state_flag=0;              // sample flag mode

void interrupts_init(void);



void main()
{
    interrupts_init();
    //uart_init(UART3, 115200);
    //accl_init();
    
    printf("Everything inited fine");
    
    while(1)
    { 
       
        //accl_print();
        delayms(500);
    }
}

void interrupts_init(void){
      DisableInterrupts;                                //禁止總中斷
    ALL_PIN_Init();
    //pit_init_ms(PIT0,5);                            // Clock, 10ms period, 50% duty cycle
    //pit_init_ms(PIT0,10);                           // Clock, 20ms period, 50% duty cycle
    pit_init_ms(PIT0,0.01);                           // Clock, 20us period, 50% duty cycle
    
    // Maximum clock is 8us cycle by using PIT
    
    EnableInterrupts;			              //開總中斷
}

void ALL_PIN_Init(){
  
    gpio_init(PORTB, 3, GPI, 1); //PTB3 = SW2
    gpio_init(PORTB, 4, GPI, 1); //PTB3 = SW3
    
    gpio_init(PORTD, 0, GPO, 1); //PTD0, D2 LED
    gpio_init(PORTD, 1, GPO, 1); //PTD1, D3 LED
    gpio_init(PORTD, 2, GPO, 1); //PTD2, D4 LED
    gpio_init(PORTD, 3, GPO, 1); //PTD3, D5 LED
        
    gpio_init(PORTB, 18, GPO, 1);  //PTB18 , Clock / CLK
    gpio_init(PORTC, 19, GPO, 1);  //PTC19 , SI
    gpio_init(PORTA, 11, GPI, 1);  //PTA11 , AO
    
    //gpio_init(PORTC, 17, GPO, 1); //PTC17 UART BlueTooth TX
    //gpio_init(PORTC, 16, GPI, 1); //PTC17 UART BlueTooth RX
  
    uart_init(UART3, 115200); // BlueTooth UART init
}