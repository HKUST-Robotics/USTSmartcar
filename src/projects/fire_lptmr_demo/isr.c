/******************** (C) COPYRIGHT 2011 Ұ��Ƕ��ʽ���������� ********************
 * �ļ���       ��isr.c
 * ����         ���жϴ�������
 *
 * ʵ��ƽ̨     ��Ұ��kinetis������
 * ��汾       ��
 * Ƕ��ϵͳ     ��
 *
 * ����         ��Ұ��Ƕ��ʽ����������
 * �Ա���       ��http://firestm32.taobao.com
 * ����֧����̳ ��http://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
**********************************************************************************/	

#include "common.h"
#include "include.h"


volatile u32 LPT_INT_count=0;
volatile u8  pit_flag   = 0;



void  LPT_Handler(void)
{
    LPTMR0_CSR|=LPTMR_CSR_TCF_MASK;     //���LPTMR�Ƚϱ�־
    LPT_INT_count++;                    //�ж������1
}

void PIT0_IRQHandler()
{
    pit_flag=1;                 //��ǽ���PIT�ж�
    PIT_Flag_Clear(PIT0);       //���жϱ�־λ
}
