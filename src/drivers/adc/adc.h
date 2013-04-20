/******************** (C) COPYRIGHT 2011 �����O�J���}�o�u�@�� ********************
 * ���W       �Gadc.h
 * �y�z         �Gadc�X�ʨ���Y���
 *
 * ���祭�x     �G����kinetis�}�o�O
 * �w����       �G
 * �O�J�t��     �G
 *
 * �@��         �G�����O�J���}�o�u�@��
 * �^�_��       �Ghttp://firestm32.taobao.com
 * �޳N����׾� �Ghttp://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
**********************************************************************************/	




#ifndef __ADC_H__
#define __ADC_H__ 1

#include "adc_cfg.h"


typedef enum ADCn  //ADC�ݤf
{
  ADC0,
  ADC1
}ADCn;

//ADC�q�D
//�����A�Ϊ��ɭԡA�i�H������ DP0  �N�� DAD0 �A��L���]�����A�]�����ӧ��w�q�G#define DP0       DAD0
typedef enum ADC_Ch
{
            //SC1n[DIFF]= 0
            //  ------ADC0------�����}�o�O���L�h----       ------ADC1------�����}�o�O���L�h----
  DAD0=0,   //	    ADC0_DP0				                    ADC1_DP0
  DAD1=1,   //	    ADC0_DP1				                    ADC1_DP1
  DAD2=2,   //	    PGA0_DP					                    PGA1_DP
  DAD3=3,   //	    ADC0_DP3				                    ADC1_DP3

  //ADCx_CFG2[MUXSEL] ��M�w ADCx_SEn �q�D�� a �� b.
  AD4a=4,   //	    �O�d					                    ADC1_SE4a   -- PTE0
  AD5a=5,   //	    �O�d					                    ADC1_SE5a   -- PTE1
  AD6a=6,   //	    �O�d					                    ADC1_SE6a   -- PTE2
  AD7a=7,   //	    �O�d					                    ADC1_SE7a   -- PTE3

  //�]�O 4�B5�B6�B7
  AD4b=AD4a, //	    ADC0_SE4b	-- PTC2			                ADC1_SE4b   -- PTC8
  AD5b=AD5a, //	    ADC0_SE5b	-- PTD1			                ADC1_SE5b   -- PTC9
  AD6b=AD6a, //	    ADC0_SE6b	-- PTD5			                ADC1_SE6b   -- PTC10
  AD7b=AD7a, //	    ADC0_SE7b	-- PTD6			                ADC1_SE7b   -- PTC11

  AD8=8,    //	    ADC0_SE8  	-- PTB0			                ADC1_SE8    -- PTB0
  AD9=9,    //	    ADC0_SE9  	-- PTB1			                ADC1_SE9    -- PTB1
  AD10=10,  //	    ADC0_SE10 	-- PTA7			                ADC1_SE10   -- PTB4
  AD11=11,  //	    ADC0_SE11 	-- PTA8			                ADC1_SE11   -- PTB5
  AD12=12,  //	    ADC0_SE12 	-- PTB2			                ADC1_SE12   -- PTB6
  AD13=13,  //	    ADC0_SE13 	-- PTB3			                ADC1_SE13   -- PTB7
  AD14=14,  //	    ADC0_SE14 	-- PTC0			                ADC1_SE14   -- PTB10
  AD15=15,  //	    ADC0_SE15 	-- PTC1			                ADC1_SE15   -- PTB11
  AD16=16,  //	    ADC0_SE16				                    ADC1_SE16
  AD17=17,  //	    ADC0_SE17 	-- PTE24		                ADC1_SE17   -- PTA17
  AD18=18,  //	    ADC0_SE18 	-- PTE25		                VREF Output
  AD19=19,  //	    ADC0_DM0				                    ADC1_DM0
  AD20=20,  //	    ADC0_DM1				                    ADC1_DM1
  AD21=21,  //				        	                        �O�d
  AD22=22,  //				
  AD23=23,  //	    DAC0_OUT(12-bit) -- DAC0_OUT		        DAC1_OUT(12-bit)
  AD24=24,  //	    �O�d					�O�d
  AD25=25,  //	    �O�d					�O�d
  AD26=26,  //	    Temperature Sensor (S.E)		            Temperature Sensor (S.E)
  AD27=27,  //	    Bandgap (S.E)				                Bandgap (S.E)
  AD28=28,  //	    �O�d					                    �O�d
  AD29=29,  //	    VREFH (S.E)				                    VREFH (S.E)
  AD30=30,  //	    VREFL					                    VREFL
  AD31=31   //	    �T��ADC0				                    �T��ADC1
}ADC_Ch;

//��צ��
typedef enum ADC_nbit
{
  ADC_8bit   =0x00,
  ADC_10bit  =0x02,
  ADC_12bit  =0x01,
  ADC_16bit  =0x03
}ADC_nbit;


//�~����Ʊ��f�n��
extern void     adc_init  (ADCn,ADC_Ch);               //AD��l��
extern u16      ad_once   (ADCn,ADC_Ch,ADC_nbit);      //�����@���@�������q��AD��

extern u16      ad_mid    (ADCn,ADC_Ch,ADC_nbit);      //�����o�i�᪺���G
extern u16      ad_ave    (ADCn,ADC_Ch,ADC_nbit,u8 N); //�����o�i�᪺���G

extern void     adc_start (ADCn,ADC_Ch,ADC_nbit);      //�}�ladc�ഫ
extern void     adc_stop  (ADCn);                      //����ADC�ഫ


//��������n��
static void adc_config_alt(ADC_MemMapPtr adcmap, tADC_ConfigPtr ADC_CfgPtr);//�Nadc�H�s�����c��t�m�iadc�H�s��


#endif /* __ADC16_H__ */
