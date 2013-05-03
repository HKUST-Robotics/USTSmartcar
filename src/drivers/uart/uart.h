/******************** (C) COPYRIGHT 2011 Ұ��Ƕ��ʽ���������� ********************
  uart.h
  
  Copyright to  http://firestm32.taobao.com
  Support       http://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008

  for demo projects, check under the "Projects" folder in app
**********************************************************************************/	

#ifndef __UART_H__
#define __UART_H__

typedef enum  UARTn
{//                     --TXD--      --RXD--        for other channels, 
  UART0,    //           PTD6         PTD7          check UART init, or 
  UART1,    //           PTC4         PTC3          fire_drivers_cfg.h
  UART2,    //           PTD3         PTD2
  UART3,    //           PTC17        PTC16
  UART4,    //           PTE24        PTE25
  UART5     //           PTE8         PTE9
}UARTn;

extern volatile struct UART_MemMap *UARTx[6];
/********************************************************************/



void uart_init (UARTn,u32 baud);                      //initialises UART ports

char uart_getchar   (UARTn);                          //���޵ȴ�����1���ֽ�
//pauses main and waits forever until a character is received, returns charater
char uart_pendchar  (UARTn,char * ch);                //����ʱ��ȴ�����һ���ַ�
//pauses main and waits short time until a character is received, returns charater
char uart_pendstr   (UARTn,char * str);               //����ʱ��ȴ������ַ���

int uart_query      (UARTn);                          //��ѯ�Ƿ���յ�һ���ֽ�
//see if uart received any characters
void uart_putchar   (UARTn, char ch);                    //����1���ֽ�
//sends one char via UART
void uart_sendN     (UARTn ,uint8* buff,uint16 len);  //����n���ֽ�
//sends n char via UART
void uart_sendStr   (UARTn ,const u8* str);           //�����ַ���
//sends string via UART

/*************************************************************************
  note: You can also use printf(); to print to uart
  but %f does not work properly, if you need to print float, try this:
    
      u8 todis[];
      sprintf(todis,"\ngyro_dtheta() = %f",gyro_dtheta());//formats a string like printf
      uart_sendStr(UART3, (u8*)accl_todis);

***********************************************************************/

void uart_irq_EN    (UARTn);                          //�����ڽ����ж�
void uart_irq_DIS   (UARTn);                          //�ش��ڽ����ж�



#define UART_IRQ_EN(UARTn)   UART_C2_REG(UARTx[UARTn])|=UART_C2_RIE_MASK; enable_irq((UARTn<<1)+45)     //�궨�忪���ڽ����ж�
#define UART_IRQ_DIS(UARTn)  UART_C2_REG(UARTx[UARTn])&=~UART_C2_RIE_MASK; disable_irq((UARTn<<1)+45)   //�궨��ؽ������ŵ�IRQ�ж�




/********************************************************************/

#endif /* __UART_H__ */
