#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include  "common.h"

/* 
 * Include �û��Զ����ͷ�ļ�
 */

#include  "gpio.h"       //IO�ڲ���
#include  "LED.H"       //��ˮ��
#include  "uart.h"      //����
#include  "adc.h"       //ADCģ��
#include  "FTM.h"       //FTMģ�飨FTM0��������� / ͨ�� /PWM     FTM1��2���������� / ͨ�� /PWM ��
#include  "PIT.h"       //�����жϼ�ʱ��
#include  "i2c.h"
#include  "AT24C02.h"
#include  "lptmr.h"     //�͹��Ķ�ʱ��(��ʱ)
#include  "exti.h"       //EXTI�ⲿGPIO�ж�

/***************** ucos ר�� *****************/
#if USOC_EN > 0u
#include  "ucos_ii.h"  		        //uC/OS-IIϵͳ����ͷ�ļ�
#include  "BSP.h"			//�뿪������صĺ���
#include  "app.h"			//�û�������


#endif  //if  USOC_EN > 0


/***************** ��ʱ�� **********************
 *   1x8ch ������� / ͨ�� /PWM ��ʱ�� (FTM)
 *   1x2ch ���������� / ͨ�� /PWM ��ʱ�� (FTM)
 *   �ز����ƶ�ʱ�� (CMT)
 *   �ɱ���ӳ�ģ�� (PDB)
 *   1x4ch �ɱ���ж϶�ʱ�� (PIT)
 *   �͹��Ķ�ʱ�� (LPT)
**********************************************/

/*******************Ƶ������**********************
 *  �� Kinetis оƬ�ڲ����� 3 �ֲ�ͬʱ�ӣ�
 *  �ں�ʱ��    ����ʱ��    Flash ʱ��
 *        
 * ���ǿ������ϣ��� k60_tower.h ������
 * #define CORE_CLK_MHZ      PLL100      //�ں�Ƶ��Ϊ100M
 * �� pll_init �У�100M��ѡ�MCG=PLL, core = MCG, bus = MCG/2, FlexBus = MCG/2, Flash clock= MCG/4
 * �� �ں�Ƶ�� Ϊ 100MHz ������ʱ�� Ϊ 50MHz �� Flash ʱ��Ϊ 25MHz 
 * printf ����˿�Ҳ�� �� k60_tower.h ������
**********************************************/




#endif  //__INCLUDE_H__
