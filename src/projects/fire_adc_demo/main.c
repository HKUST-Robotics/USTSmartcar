/******************** (C) COPYRIGHT 2011 �����O�J���}�o�u�@�� ********************
 * ���W       �Gmain.c
 * �y�z         �GADC�Ҽ��ഫ����
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
*                              ADC�Ҽ��ഫ�������
*
*  ���绡���G����ADC�Ҽ��ഫ����A�Φ�f�o�e�ഫ�ᵲ�G�C
*            ������f�q�{���G UART1�ATX��PTC4�ARX��PTC3
*            k60_fire.h�̩w�q�F printf ��ƪ���X�]�m�G
*                #define FIRE_PORT           UART1
*                #define FIRE_BAUD           19200
*
*  ����ާ@�G�o�̥� ADC1_SE4a �A�ҥH ADC1_SE4a �٭n���@��0~3.3V���i�չq��
*            ADC1_SE4a �N�O PTE0 , �b adc.h ���̦����
*            ���}��f�U��A�]�m�i�S�v�� 19200 �C
*            ��f�u�]�g�LMAX3232�q���ഫ�^�GTX��PTC4�ARX��PTC3
*
*  ����ĪG�G�b��f�U��̡A�i�H�ݨ��X�p�U�H���G
*
*                    ����kinetis�֤ߪO���յ{��
*       �����W�v�G200MHz	�`�u�W�v �G66MHz
*       flex�W�v�G66MHz 	flash�W�v�G28MHz
*
*       Software Reset
*
*       K60-144pin      Silicon rev 1.2
*       Flash parameter version 0.0.7.0
*       Flash version ID 3.2.7.0
*       512 kBytes of P-flash	P-flash only
*       128 kBytes of RAM
*
*       AD�ഫ�@�������G��:36983
*       AD�ഫ�T�������ȵ��G��:37143
*       AD�ഫ�Q���������ȵ��G��:36912
*
*
*
*  ����ت��G���� ADC �ഫ���U�ӵ��G
*
*  �ק�ɶ��G2012-2-29     �w����
*
*  ��    �`�Gadc.h �� �U��ADC�q�D�ҹ����޸}�����A��K�d��
*            �p�G�a�ź޸}�A�����X���ȷ|�ܤƤ���j
*************************************************************************/
void  main()
{
    u16 ADresult;                           //�O�sADC�ഫ���G

    uart_init(UART1,19200);                 //��l�Ʀ�f�A�Ψӵo�e�ഫ�ƾ�

    adc_init(ADC1,SE4a);                    //��l��ADC1_SE4a ,�qadc.h�̪����N�i�H�ݨ�ADC1_SE4a������PTE0

    while(1)
    {
        /****** Ū���@�� ******/
        ADresult    =   ad_once(ADC1,SE4a,ADC_16bit);           //Ū�� ADC1_SE4a �A16����
        printf("AD�ഫ�@�������G��:%d\n\n",ADresult);

        time_delay_ms(500);                //����500ms

        /****** Ū���T���A������ ******/
        ADresult    =   ad_mid(ADC1,SE4a,ADC_16bit);            //Ū�� ADC1_SE4a �A16����
        printf("AD�ഫ�T�������ȵ��G��:%d\n\n",ADresult);

        time_delay_ms(500);                //����500ms

        /****** Ū���Q�����A�������� ******/
        ADresult    =   ad_ave(ADC1,SE4a,ADC_16bit,10);         //Ū�� ADC1_SE4a �A16����
        printf("AD�ഫ�Q���������ȵ��G��:%d\n\n",ADresult);

        time_delay_ms(500);                //����500ms
    }
}
