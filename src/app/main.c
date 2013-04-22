/******************** (C) COPYRIGHT 2011 �����O�J���}�o�u�@�� ********************
 * ���W       �Gmain.c
 * �y�z         �GPIT�w�ɤ��_����
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
*                               PIT�w�ɤ��_����
*
*  ���绡���G���� PIT �w�ɤ��_����A�b���_��ƤF�� LED0 ��ܶi�J�F���_��ơC
*            
*  ����ާ@�G�L
*
*  ����ĪG�GLED0���j1s�{�{
*
*  ����ت��G���� PIT �O�_�w�ɲ��ͤ��_
*
*  �ק�ɶ��G2012-2-29     �w����
*
*  ��    �`�G����Kinetis�}�o�O�� LED0~3 �A���O��PTD15~PTD12 �A�C�q���I�G
*            
*************************************************************************/

void ALL_PIN_Init();
volatile unsigned int systemclock=0;             // for locking SI
volatile int SI_state_flag=0;                    // SI flag mode
volatile int smapling_state_flag=0;              // sample flag mode
char str[1];

void interrupts_init(void);
void trigger_si(void);

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
        
        
      
      
     trigger_si();
        
    }
}

void interrupts_init(void){
      DisableInterrupts;                                //�T���`���_
    ALL_PIN_Init();
    //pit_init_ms(PIT0,5);                            // Clock, 10ms period, 50% duty cycle
    //pit_init_ms(PIT0,10);                           // Clock, 20ms period, 50% duty cycle
    pit_init_ms(PIT0,0.01);                           // Clock, 20us period, 50% duty cycle
    
    // Maximum clock is 8us cycle by using PIT
    
    //pit_init(PIT0,10);  // �p�� Suggest: Try this to get faster clock
    
    EnableInterrupts;			              //�}�`���_
}

void trigger_si(void){
  
        //if(SI_state_flag == 0 ){        // Replace this with, if(uart_pendstr(UART3,str) == 1) , can Auto sampling repeatedly
  
       if(uart_pendstr(UART3,str) == 1){ // Using Bluetooth to trigger , when any key in PC keyboard is pressed
           uart_sendStr(UART3,"*.*.*.* SI Trigger, rising edge generated *.*.*.*");
           uart_sendStr(UART3,"\014");  // New page form feed same as uart_sendStr(UART3,"\n\f");
           SI_state_flag = 1;           // SI Flag on
           smapling_state_flag = 1;     // sampling Flag on
           systemclock = 0;                   // systemclock count from begin,lock the SI status not duplicate
           gpio_set(PORTD, 0, 0);       // LED D2 ON, 0 means on, notify the switch is on
           gpio_set(PORTD, 1, 0);       // LED D3 ON, 0 means on, notify the SI is in locking mode
           gpio_set(PORTC, 19, 1);      // SI rising edge
       }
        else{                          // if SW2 not press
           gpio_set(PORTD, 0, 1);         // LED D2 OFF, 1 means off, means keyboard is not pressed
       }
}

void ALL_PIN_Init(){
  
    //gpio_init(PORTB, 3, GPI, 1); //PTB3 = SW2
    //gpio_init(PORTB, 4, GPI, 1); //PTB3 = SW3
    
    gpio_init(PORTD, 0, GPO, 1); //PTD0, D2 LED
    gpio_init(PORTD, 1, GPO, 1); //PTD1, D3 LED
    gpio_init(PORTD, 2, GPO, 1); //PTD2, D4 LED
    gpio_init(PORTD, 3, GPO, 1); //PTD3, D5 LED
        
    gpio_init(PORTB, 18, GPO, 1);  //PTB18 , Clock / CLK
    gpio_init(PORTC, 19, GPO, 1);  //PTC19 , SI
    gpio_init(PORTA, 11, GPI, 1);  //PTA11 , AO
  
    uart_init(UART3, 115200); // BlueTooth UART init
}