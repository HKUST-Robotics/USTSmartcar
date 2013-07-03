/******************** (C) COPYRIGHT 2011 Ұ��Ƕ��ʽ���������� ********************
 * �ļ���       ��main.c
 * ����         ��ADCģ��ת��ʵ��
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


/*************************************************************************
*                             Ұ��Ƕ��ʽ����������
*                              ADCģ��ת��ʵ�����
*
*  ʵ��˵����Ұ��ADCģ��ת��ʵ�飬�ô��ڷ���ת��������
*            Ұ�𴮿�Ĭ��Ϊ�� UART1��TX��PTC4��RX��PTC3
*            k60_fire.h�ﶨ���� printf ������������ã�
*                #define FIRE_PORT           UART1
*                #define FIRE_BAUD           19200
*
*  ʵ������������� ADC1_SE4a ������ ADC1_SE4a ��Ҫ��һ��0~3.3V�Ŀɵ���·
*            ADC1_SE4a ���� PTE0 , �� adc.h �ļ����б��
*            �򿪴������֣����ò�����Ϊ 19200 ��
*            �����ߣ�����MAX3232��ƽת������TX��PTC4��RX��PTC3
*
*  ʵ��Ч�����ڴ�����������Կ������������Ϣ��
*
*                    Ұ��kinetis���İ���Գ���
*       �ں�Ƶ�ʣ�200MHz	����Ƶ�� ��66MHz
*       flexƵ�ʣ�66MHz 	flashƵ�ʣ�28MHz
*
*       Software Reset
*
*       K60-144pin      Silicon rev 1.2
*       Flash parameter version 0.0.7.0
*       Flash version ID 3.2.7.0
*       512 kBytes of P-flash	P-flash only
*       128 kBytes of RAM
*
*       ADת��һ�εĽ��Ϊ:36983
*       ADת�����ε���ֵ���Ϊ:37143
*       ADת��ʮ�ε�ƽ��ֵ���Ϊ:36912
*
*
*
*  ʵ��Ŀ�ģ����� ADC ת���ĸ������
*
*  �޸�ʱ�䣺2012-2-29     �Ѳ���
*
*  ��    ע��adc.h �� ����ADCͨ������Ӧ�ܽŵı�񣬷���鿴
*            ������չܽţ��ǲ����ֵ��仯�Ƚϴ�
*************************************************************************/
void  main()
{
    u16 ADresult;                           //����ADCת�����

    uart_init(UART1,19200);                 //��ʼ�����ڣ���������ת������

    adc_init(ADC1,SE4a);                    //��ʼ��ADC1_SE4a ,��adc.h��ı��Ϳ��Կ���ADC1_SE4a��ӦΪPTE0

    while(1)
    {
        /****** ��ȡһ�� ******/
        ADresult    =   ad_once(ADC1,SE4a,ADC_16bit);           //��ȡ ADC1_SE4a ��16λ����
        printf("ADת��һ�εĽ��Ϊ:%d\n\n",ADresult);

        time_delay_ms(500);                //��ʱ500ms

        /****** ��ȡ���Σ�ȡ��ֵ ******/
        ADresult    =   ad_mid(ADC1,SE4a,ADC_16bit);            //��ȡ ADC1_SE4a ��16λ����
        printf("ADת�����ε���ֵ���Ϊ:%d\n\n",ADresult);

        time_delay_ms(500);                //��ʱ500ms

        /****** ��ȡʮ�δΣ�ȡƽ��ֵ ******/
        ADresult    =   ad_ave(ADC1,SE4a,ADC_16bit,10);         //��ȡ ADC1_SE4a ��16λ����
        printf("ADת��ʮ�ε�ƽ��ֵ���Ϊ:%d\n\n",ADresult);

        time_delay_ms(500);                //��ʱ500ms
    }
}
