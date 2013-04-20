/******************** (C) COPYRIGHT 2011 ³¥¤õ´O¤J¦¡¶}µo¤u§@«Ç ********************
 * ¤å¥ó¦W       ¡Gmain.c
 * ´y­z         ¡GPIT©w®É¤¤Â_¹êÅç
 *          
 * ¹êÅç¥­¥x     ¡G³¥¤õkinetis¶}µoªO
 * ®wª©¥»       ¡G
 * ´O¤J¨t²Î     ¡G
 *
 * §@ªÌ         ¡G³¥¤õ´O¤J¦¡¶}µo¤u§@«Ç 
 * ²^Ä_©±       ¡Ghttp://firestm32.taobao.com
 * §Þ³N¤ä«ù½×¾Â ¡Ghttp://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008   
**********************************************************************************/	


#include "common.h"
#include "include.h"
#include <math.h>
#include "i2c.h"

/*************************************************************************
*                             ³¥¤õ´O¤J¦¡¶}µo¤u§@«Ç
*                               PIT©w®É¤¤Â_´ú¸Õ
*
*  ¹êÅç»¡©ú¡G³¥¤õ PIT ©w®É¤¤Â_¹êÅç¡A¦b¤¤Â_¨ç¼Æ¤F¥Î LED0 Åã¥Ü¶i¤J¤F¤¤Â_¨ç¼Æ¡C
*            
*  ¹êÅç¾Þ§@¡GµL
*
*  ¹êÅç®ÄªG¡GLED0¶¡¹j1s°{Ã{
*
*  ¹êÅç¥Øªº¡G´ú¸Õ PIT ¬O§_©w®É²£¥Í¤¤Â_
*
*  ­×§ï®É¶¡¡G2012-2-29     ¤w´ú¸Õ
*
*  ³Æ    ª`¡G³¥¤õKinetis¶}µoªOªº LED0~3 ¡A¤À§O±µPTD15~PTD12 ¡A§C¹q¥­ÂI«G
*            
*************************************************************************/

void ALL_PIN_Init();
volatile u16 clock=0;                            // for locking SI
volatile int SI_state_flag=0;                    // SI flag mode
volatile int smapling_state_flag=0;              // sample flag mode

void louis_init(void);
float acclx,accly,acclz,accl_totalforce;
u16 u16x,u16y,u16z;
double accl_tilt;
double number;

u8 accl_xx[2];
u8 accl_yy[2];
u8 accl_zz[2];

u8 todis[];

float accl_gra_cov(u8 axis[]);
void accl_update(void);
double accl_tiltangle(void);
void accl_init(void);

void accl_init(void){
    I2C_init(I2C0);
    I2C_WriteAddr(I2C0, 0x1c, 0x0f, 0x10);
    I2C_WriteAddr(I2C0, 0x1c, 0x0e, 0x00);
    I2C_WriteAddr(I2C0, 0x1c, 0x2a, 0x01); 
}

float accl_gra_cov(u8 axis[])//?™ªª=…O«?»
{
  u16 V1;
  float vv;
  int sign;
  if(axis[0] > 0x7F)
  {
    sign=-1;
    V1   = (int)axis[0];	
    axis[1]= axis[1]>>2;
    V1   = V1<<6 | axis[1];
    V1   = (~V1 + 1)&0X3FFF;
    //V1= (~(V.mword>>2) + 1);
  }
  else
  {
    sign=1;
    V1   = (int)axis[0];	
    axis[1]= axis[1]>>2;
    V1   = V1<<6 | axis[1];
    //V1=(V.mword>>2)&0X3FFF;
  }
  vv=sign*(((float)V1)*0.0219726562);
  return vv;
}

u16 accl_u16_conv(u8 array[]){
  u16 magic=array[0];
  magic |= array[1]<<8;
  return magic;
}

void accl_update(void){
        accl_xx[0]   =   I2C_ReadAddr(I2C0, 0x1c, 0x01);  
        delayms(0);//i dont know why this works, but dont remove
        accl_xx[1]   =   I2C_ReadAddr(I2C0, 0x1c, 0x02);
        delayms(0);
        accl_yy[0]   =   I2C_ReadAddr(I2C0, 0x1c, 0x03); 
        delayms(0);
        accl_yy[1]   =   I2C_ReadAddr(I2C0, 0x1c, 0x04);
        delayms(0);
        accl_zz[0]   =   I2C_ReadAddr(I2C0, 0x1c, 0x05); 
        delayms(0);
        accl_zz[1]   =   I2C_ReadAddr(I2C0, 0x1c, 0x06);
        
        
        acclx=accl_gra_cov(accl_xx);
        accly=accl_gra_cov(accl_yy);
        acclz=accl_gra_cov(accl_zz);
        
        accl_totalforce=sqrt((acclx*acclx)+(accly*accly)+(acclz*acclz));
        accl_tilt=accl_tiltangle();
        u16x=accl_u16_conv(accl_xx);
        printf("\n\nTHIS IS THE u16!===>%d",u16x);
}

double accl_tiltangle(void){
  return 57.295779513*atan(accl_gra_cov(accl_yy)/accl_gra_cov(accl_xx));
}

void accl_printdata(void){
        sprintf(todis,"\r\n\r\nAccelerometer data\r\nacclx: %f\r\naccly: %f\r\nacclz: %f\r\nTotal force:%f\r\n=============>accl_tiltangle:%f ",acclx,accly,acclz,accl_totalforce,accl_tilt);                
        uart_sendStr (UART3, (u8*)todis); 
}

void main()
{
    //louis_init();
    uart_init(UART3, 115200);
    
    accl_init();
    
    while(1)
    { 
       
        accl_update();
        
        //updates float values acclx,accly, and acclz
        //and calculates accl_tilt of the car
        accl_printdata();
        //prints all accelerometer data to bluetooth
        
        delayms(500);
    }
}

void louis_init(void){
      DisableInterrupts;                                //¸T¤îÁ`¤¤Â_
    ALL_PIN_Init();
    //pit_init_ms(PIT0,5);                            // Clock, 10ms period, 50% duty cycle
    //pit_init_ms(PIT0,10);                           // Clock, 20ms period, 50% duty cycle
    pit_init_ms(PIT0,0.01);                           // Clock, 20us period, 50% duty cycle
    
    // Maximum clock is 8us cycle by using PIT
    
    EnableInterrupts;			              //¶}Á`¤¤Â_
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