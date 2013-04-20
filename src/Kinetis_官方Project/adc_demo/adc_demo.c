/*
 * File:		adc_demo.c
 * Purpose:		Demo the Analog to Digtal Converter triggered by the Programable Delay Block  通过 可编程延时模块 来 模数转换
 *                      Call the function Hw_Trig_Test which demonstrates and tests this function.  调用 Hw_Trig_Test 函数来展示和测试这函数
 *                      Runs on the Tower Kinetis Card, using the potentiometer as input.     在Tower Kinetis开发板上使用 电位器作 为输入
 *                      The operator will be prompted on the serial port at 115200 baud, 8n1. 
 *                      The LED's will also indicate the timing of the conversions.   LED 会提示 转换时间
 *
 */


#include "common.h"    // Common Kinetis defines
#include "adc16.h"     // ADC driver defines
#include "adc_demo.h"   // For this function

/*    global variable    */
tADC_Config Master_Adc_Config;  // This is the structure that contains the desired ADC/PGA configuration.


extern uint8_t  Hw_Trig_Test(void);

/********************************************************************/
void main (void)
{
 Init_Gpio(); 
 Hw_Trig_Test(); //  demo the adc/pdb
}  



//******************************************************************************
// setup an output pin, "PIN",  to indirectly observe adc status changes  设置输出管脚来 间接地观察ADC状态的变化
// 
//******************************************************************************

void Init_Gpio(void)

{

 //the "PIN" will be visible as PORTA29 on TOWER board
 // setup PORTA29  for output "PIN" as called in test programs as on TOWER board
  PORTA_PCR29 = PORT_PCR_MUX(1) ;        // selec GPIO function     PA29为普通IO口，默认低电平输出  (光耦，耦合到PB2：ADC0_SE12)
  GPIOA_PCOR = 0x01 << 29 ;              // initial out low 
  GPIOA_PDDR = 0x01 << 29 ;              // output enable 

}
