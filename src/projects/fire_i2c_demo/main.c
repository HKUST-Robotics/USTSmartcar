/******************** (C) COPYRIGHT 2011 Ұ��Ƕ��ʽ���������� ********************
 * �ļ���       ��main.c
 * ����         ��I2Cͨ��ʵ��
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
*                               I2Cͨ��ʵ��
*
*  ʵ��˵����Ұ�� I2Cͨ��ʵ�飬�� AT24C02 д�����ݣ��ٶ�������ͨ����������ʾ��
*            Ұ�𴮿�Ĭ��Ϊ�� UART1��TX��PTC4��RX��PTC3
*            k60_fire.h�ﶨ���� printf ������������ã�
*                #define FIRE_PORT           UART1
*                #define FIRE_BAUD           19200
*            
*  ʵ��������򿪴������� �����ò�����Ϊ 19200 ��
*            �����ߣ�����MAX3232��ƽת������TX��PTC4��RX��PTC3 ��
*            ���г�����ڴ��������￴����
*   
*  ʵ��Ч����������������ʾ���ݣ�
*  
*      		Ұ��kinetis���İ���Գ���
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
*       AT24C02 I2C ʵ��
*  
*        		����Ұ��kinetis������
*  
*       ���յ�������Ϊ��0
*  
*       ���յ�������Ϊ��1   
*
*       ......
*
*
*  ʵ��Ŀ�ģ����� I2C ͨ�Ź���
*
*  �޸�ʱ�䣺2012-2-29     �Ѳ���
*
*  ��    ע��
*            
*************************************************************************/

#if 1           //����ѡ�� 0 �� 1 ���л�����������ͬ��main��������������һ���Ĺ���
                // ��һ�� ��I2C��ĺ���
                // �ڶ��� ��AT24C02��ĺ���
                // ���ֶ�����ͬ��

void main()
{
#define ADDR    0x00  
    u8 i=0;
    u8 Data;
    uart_init(UART1, 19600);                        //��ʼ������
    
    I2C_init(I2C0);                                 //��ʼ��I2C0
    
    printf("AT24C02 I2C ʵ��\n\n");
    printf("\t\t����Ұ��kinetis������\n\n");
    
    while(1)
    {
        for(i=0;i<255;i++)
        {
            I2C_WriteAddr(I2C0,AT24C02_I2C_ADDRESS,ADDR,i);             //I2C��AT24C02_I2C_ADDRESSоƬд������ i ����ַΪADDR�ļĴ���
            Data    =   I2C_ReadAddr(I2C0,AT24C02_I2C_ADDRESS,ADDR);    //I2C��AT24C02_I2C_ADDRESSоƬ��ȡ�Ĵ�����ַΪADDR������
            
            printf("���յ�������Ϊ��%d\n\n",Data);                      //���͵�������ʾ����

            time_delay_ms(1000);                                        //��ʱ1s
        }
    }
#undef  ADDR
}

#else           //������һ���ģ�ǰ���Ǻ��ߺ궨���չ��

void main()
{
#define ADDR    0x00  
    u8 i=0;
    u8 Data;
    uart_init(UART1, 19600);                    //��ʼ������
    
    AT24C02_init();                             //��ʼ��AT24C02������I2C����
    
    printf("AT24C02 I2C ʵ��\n\n");
    printf("\t\t����Ұ��kinetis������\n\n");
    while(1)
    {
        for(i=0;i<255;i++)
        {
            AT24C02_WriteByte(ADDR,i);          //���ַADDRд������i
            
            Data=AT24C02_ReadByte(ADDR);        //��ȡ��ַADDR������
            
            printf("���յ�������Ϊ��%d\n\n",Data);//���͵�������ʾ����
            
            time_delay_ms(1000);                //��ʱ1s
        }
    }
#undef  ADDR
}

#endif
