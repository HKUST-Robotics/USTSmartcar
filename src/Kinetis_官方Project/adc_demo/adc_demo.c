/*
 * File:		adc_demo.c
 * Purpose:		Demo the Analog to Digtal Converter triggered by the Programable Delay Block  ͨ�� �ɱ����ʱģ�� �� ģ��ת��
 *                      Call the function Hw_Trig_Test which demonstrates and tests this function.  ���� Hw_Trig_Test ������չʾ�Ͳ����⺯��
 *                      Runs on the Tower Kinetis Card, using the potentiometer as input.     ��Tower Kinetis��������ʹ�� ��λ���� Ϊ����
 *                      The operator will be prompted on the serial port at 115200 baud, 8n1. 
 *                      The LED's will also indicate the timing of the conversions.   LED ����ʾ ת��ʱ��
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
// setup an output pin, "PIN",  to indirectly observe adc status changes  ��������ܽ��� ��ӵع۲�ADC״̬�ı仯
// 
//******************************************************************************

void Init_Gpio(void)

{

 //the "PIN" will be visible as PORTA29 on TOWER board
 // setup PORTA29  for output "PIN" as called in test programs as on TOWER board
  PORTA_PCR29 = PORT_PCR_MUX(1) ;        // selec GPIO function     PA29Ϊ��ͨIO�ڣ�Ĭ�ϵ͵�ƽ���  (�����ϵ�PB2��ADC0_SE12)
  GPIOA_PCOR = 0x01 << 29 ;              // initial out low 
  GPIOA_PDDR = 0x01 << 29 ;              // output enable 

}
