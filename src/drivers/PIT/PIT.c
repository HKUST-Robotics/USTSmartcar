/******************** (C) COPYRIGHT 2011 �����O�J���}�o�u�@�� ********************
 * ���W       �GPIT.c
 * �y�z         �GPIT�P�����_�p�ɾ��A�w�ɤ��_
 * �ƪ`         �G
 *
 * ���祭�x     �G����kinetis�}�o�O
 * �w����       �G
 * �O�J�t��     �G
 *
 * �@��         �G
 * �^�_��       �Ghttp://firestm32.taobao.com
 * �޳N����׾� �Ghttp://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
**********************************************************************************/	


#include "common.h"
#include  "PIT.h"     //�P�����_�p�ɾ�

/*************************************************************************
*                             �����O�J���}�o�u�@��
*
*  ��ƦW�١Gpit_init
*  �\�໡���GPITn�w�ɤ��_
*  �Ѽƻ����GPITn        �Ҷ����]PIT0~PIT3�^
             cnt         ���ɮɶ�(��쬰bus�����P��)
*  ��ƪ�^�G�L
*  �ק�ɶ��G2012-1-24
*  ��    �`�G
*************************************************************************/
void pit_init(PITn pitn,u32 cnt)
{
    //PIT �Ϊ��O Bus Clock �`�u�W�v
    //���X�p�� = �`�u�W�v * �ɶ�

    /* �}�Ү���*/
    SIM_SCGC6       |=SIM_SCGC6_PIT_MASK;                             //�ϯ�PIT����

    /* PIT�Ҷ����� PIT Module Control Register (PIT_MCR) */
    PIT_MCR         &=~(PIT_MCR_MDIS_MASK | PIT_MCR_FRZ_MASK );       //�ϯ�PIT�w�ɾ����� �A�ոռҦ��U�~��B��

    /* �w�ɾ��[���ȳ]�m Timer Load Value Register (PIT_LDVALn) */
    PIT_LDVAL(pitn)  =cnt;                                           //�]�m���X���_�ɶ�

    //�w�ɮɶ���F��ATIF �m 1 �C�g1���ɭԴN�|�M0
    PIT_Flag_Clear(pitn);                                             //�M���_�ЧӦ�

    /* �w�ɾ�����H�s�� Timer Control Register (PIT_TCTRL0) */
    PIT_TCTRL(pitn) |=( PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK );    //�ϯ� PITn�w�ɾ�,�ö}PITn���_

    enable_irq(pitn+68);			                                  //�}�����޸}��IRQ���_
}