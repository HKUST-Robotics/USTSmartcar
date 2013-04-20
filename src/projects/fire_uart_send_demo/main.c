/******************** (C) COPYRIGHT 2011 Ұ��Ƕ��ʽ���������� ********************
 * �ļ���       ��main.c
 * ����         ��UART���ڷ���ʵ��
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
*                               ���ڷ���ʵ�����
*
*  ʵ��˵����Ұ�𴮿ڷ���ʵ��
*            Ұ�𴮿�Ĭ��Ϊ�� UART1��TX��PTC4��RX��PTC3
*            k60_fire.h�ﶨ���� printf ������������ã�
*                #define FIRE_PORT           UART1
*                #define FIRE_BAUD           19200
*            ����Ĵ��ڷ���ʵ��Ҳʹ�� UART1 ������������ʾ��
*             
*  ʵ��������򿪴������� �����ò�����Ϊ 19200 ��
*            �����ߣ�����MAX3232��ƽת������TX��PTC4��RX��PTC3
*
*  ʵ��Ч�����ڴ�����������Կ������������Ϣ��
*
*                        Ұ��kinetis���İ���Գ���
*                �ں�Ƶ�ʣ�200MHz	����Ƶ�� ��66MHz
*                flexƵ�ʣ�66MHz 	flashƵ�ʣ�28MHz
*                
*                Software Reset
*                
*                K60-144pin      Silicon rev 1.2  
*                Flash parameter version 0.0.7.0
*                Flash version ID 3.2.7.0
*                512 kBytes of P-flash	P-flash only
*                128 kBytes of RAM
*                
*                ���ڷ���ʵ��
*                
*                                ����Ұ��kinetis������
*                
*                Fuart_sendNuart_str������������ 
*                -------------Ұ��kinetis������-------------
*
*
*  ʵ��Ŀ�ģ����Դ��ڷ��͵ĸ���������printf��uart_putchar��uart_sendN��uart_sendStr
*
*  �޸�ʱ�䣺2012-2-29     �Ѳ���
*
*  ��    ע��printf�����ĵײ��ǵ��� uart_putchar �����͡�
*************************************************************************/
void  main(void)     
{
    uart_init(UART1, 19200);                                          //��ʼ������
    
    printf("���ڷ���ʵ��\n\n");                                       //ʹ��printf������
    printf("\t\t����Ұ��kinetis������\n\n");
    
    while(1)
    {  
        uart_putchar (UART1,'F');                                     //����һ���ַ�'F'
        uart_sendN (UART1,"uart_sendN\n",11);                         //����11���ַ�
        uart_sendStr (UART1,"uart_str������������");                  //�����ַ���
        printf("\n-------------Ұ��kinetis������-------------\n\n");  //ʹ��printf������
        time_delay_ms(500);                                           //��ʱ
    }
}
