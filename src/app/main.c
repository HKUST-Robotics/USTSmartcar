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
volatile u32 g_u32encoder_lf=0;
volatile u32 g_u32encoder_rt=0;

volatile u32 g_u32encoder_lflast=0;
volatile u32 g_u32encoder_rtlast=0;

volatile int motor_deadzone_left,motor_deadzone_right;

u16 motor_test=0;

u8 todis[];//for sprintf usage

/*************************************************************************
Function header
*************************************************************************/
void main(void){
  gpio_init(PORTE,24,GPO,1);
  gpio_init(PORTE,25,GPO,1);
  gpio_init(PORTE,26,GPO,1);
  gpio_init(PORTE,27,GPO,1);
  
  while(1){
    for(int i=24;i<28;i++){
      gpio_turn(PORTE,i);
      delayms(300);
    }
  }
}
  