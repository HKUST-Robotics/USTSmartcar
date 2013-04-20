/******************** (C) COPYRIGHT 2011 Ұ��Ƕ��ʽ���������� ********************
 * �ļ���       ��main.c
 * ����         ��lptmr����ʵ��
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


extern volatile u32 LPT_INT_count;
extern volatile u8  pit_flag;
#if 0
/*************************************************************************
*                             Ұ��Ƕ��ʽ����������
*                   LPT��������ж�ʵ�飨����FTM����PWM���岨��
*
*  ʵ��˵�������� FTM ����PWM���壬LPT�������
*
*  ʵ��������̽� PTC5 �� PTA8
*
*  ʵ��Ч���������������ֵ��Ч������
*                Ұ��kinetis���İ���Գ���
*        �ں�Ƶ�ʣ�200MHz	����Ƶ�� ��100MHz
*        flexƵ�ʣ�100MHz 	flashƵ�ʣ�28MHz
*
*        Core Lockup Event Reset
*
*        Ұ��Kinetis������������ʽ��flash����
*
*        K60-144pin      Silicon rev 1.2
*        Flash parameter version 0.0.7.0
*        Flash version ID 3.2.7.0
*        512 kBytes of P-flash	P-flash only
*        128 kBytes of RAM
*
*        LPT����һ���ж���:100
*        LPT����һ���ж���:100
*        ����
*
*  ʵ��Ŀ�ģ���������������������
*
*  �޸�ʱ�䣺2012-3-16     �Ѳ���
*
*  ��    ע�������޸�FTMƵ��������Ƶ��Խ�죬LPT�����ж�Խ��
*************************************************************************/
#define INT_COUNT  100          //LPT �����жϵļ�������
void  main(void)
{
    u16 count;

    DisableInterrupts;                                      //��ֹ���ж�

    FTM_PWM_init(FTM1,CH0,100,50);                          //FTMģ�����PWM���� FTM1_CH0 ���� PTA8 ��Ƶ��Ϊ 100

    lptmr_counter_init(LPT0_ALT2,INT_COUNT,2,LPT_Rising);   //��ʼ���������������LPT0_ALT2����PTC5���룬ÿ��INT_COUNT�����жϣ���ʱ2��ʱ���˲��������ش���

    EnableInterrupts;                                       //�����ж�

    while(1)
    {
        if( LPT_INT_count > 0 )
        {
            count           =   LPTMR0_CNR;                         //�����������������ֵ
            lptmr_counter_clean();                                  //����������������ֵ��������գ��������ܱ�֤����ֵ׼ȷ��
            printf("LPT����һ���ж���:%d\n",LPT_INT_count*INT_COUNT +  count);  //��ӡ����ֵ
            LPT_INT_count   =   0;                                  //���LPT�жϴ���
        }
    }
}

#else
/*************************************************************************
*                             Ұ��Ƕ��ʽ����������
*                   PIT��ʱ��ȡLPT�������ʵ�飨����FTM����PWM���岨��
*
*  ʵ��˵�������� FTM ����PWM���壬LPT���������PIT��ʱ�ж϶�ȡ����
*
*  ʵ��������̽� PTC5 �� PTA8
*
*  ʵ��Ч���������������ֵ��Ч������
*                Ұ��kinetis���İ���Գ���
*        �ں�Ƶ�ʣ�200MHz	����Ƶ�� ��100MHz
*        flexƵ�ʣ�100MHz 	flashƵ�ʣ�28MHz
*
*        Core Lockup Event Reset
*
*        Ұ��Kinetis������������ʽ��flash����
*
*        K60-144pin      Silicon rev 1.2
*        Flash parameter version 0.0.7.0
*        Flash version ID 3.2.7.0
*        512 kBytes of P-flash	P-flash only
*        128 kBytes of RAM
*
*        1����LPT��ȡ����:81��                 ��һ�����ȶ�
*        1����LPT��ȡ����:199��
*        1����LPT��ȡ����:199��
*        1����LPT��ȡ����:199��
*        ����
*
*  ʵ��Ŀ�ģ�������ζ�ʱͳ���������
*
*  �޸�ʱ�䣺2012-3-16     �Ѳ���
*
*  ��    ע�������޸�FTMƵ�ʣ�1��ʱ������������Ƶ�ʷǳ����������
*************************************************************************/
#define INT_COUNT  100
void main()
{
    u16 count;

    DisableInterrupts;                                                              //��ֹ���ж�

    FTM_PWM_init(FTM1,CH0,200,50);                                                  //FTMģ�����PWM���� FTM1_CH0 ���� PTA8 ��Ƶ��Ϊ 200

    lptmr_counter_init(LPT0_ALT2,INT_COUNT,2,LPT_Rising);                           //��ʼ���������������LPT0_ALT2����PTC5���룬ÿ��INT_COUNT�����жϣ���ʱ2��ʱ���˲��������ش���

    pit_init_ms(PIT0,1000);                                                         //��ʱ 1�� �ж�

    EnableInterrupts;                                                               //�����ж�

    while(1)
    {
        if( pit_flag > 0 )                                                          /*   1 ���ж���  */
        {
            count=LPTMR0_CNR;                                                       //�����������������ֵ
            lptmr_counter_clean();                                                  //����������������ֵ��������գ��������ܱ�֤����ֵ׼ȷ��
            printf("1����LPT��ȡ����:%d��\n",LPT_INT_count*INT_COUNT +  count);     //��ȡ���1����������
            LPT_INT_count=0;                                                        //���LPT�жϴ���
            pit_flag=0;                                                             //���pit�жϱ�־λ
        }
    }
}

#endif

