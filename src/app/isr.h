/******************** (C) COPYRIGHT 2011 �����O�J���}�o�u�@�� ********************
 * ���W       �Gisr.h
 * �y�z         �G���s���w�q���_���A���M�g���_�V�q��̪����_��Ʀa�}�A
 *                �Ϩ���V�ڭ̩ҩw�q�����_�A�Ȩ�ơC�n�����_�A�Ȩ��
 *                ĵ�i�G�u��b"vectors.c"�]�t�A�ӥB�����b"vectors.h"�]�t���᭱�I�I�I
 *          
 * ���祭�x     �G����kinetis�}�o�O
 * �w����       �G
 * �O�J�t��     �G
 *
 * �@��         �G�����O�J���}�o�u�@�� 
 * �^�_��       �Ghttp://firestm32.taobao.com
 * �޳N����׾� �Ghttp://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008   
**********************************************************************************/	



#ifndef __ISR_H
#define __ISR_H 1

#include  "include.h"  

/*                          ���s�w�q���_�V�q��
 *  �������q�{�����_�V�q�������w�q       #undef  VECTOR_xxx
 *  �b���s�w�q��ۤv�s�g�����_���       #define VECTOR_xxx    xxx_IRQHandler
 *  �Ҧp�G
 *       #undef  VECTOR_003
 *       #define VECTOR_003    HardFault_Handler    ���s�w�q�w��W�X���_�A�Ȩ��
 */

#undef  VECTOR_084
#define VECTOR_084    PIT0_IRQHandler     //���s�w�q84�����_��PIT0_IRQHandler���_

extern void PIT0_IRQHandler();            //PIT0 �w�ɤ��_�A�Ȩ��

#endif  //__ISR_H

/* End of "isr.h" */
