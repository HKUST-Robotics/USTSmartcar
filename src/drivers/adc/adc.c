/******************** (C) COPYRIGHT 2011 �����O�J���}�o�u�@�� ********************
 * ���W       �Gadc.c
 * �y�z         �Gadc�X�ʨ��
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
#include "adc.h"


tADC_Config Master_Adc_Config;          //�ӵ��c��]�t�F�ݭn��ADC/PGA�t�m

volatile struct ADC_MemMap *ADCx[2]={ADC0_BASE_PTR,ADC1_BASE_PTR}; //�w�q��ӫ��w�ƲիO�s ADCx ���a�}

/*************************************************************************
*                             �����O�J���}�o�u�@��
*
*  ��ƦW�١Gadc_init
*  �\�໡���GAD��l�ơA�ϯ����
*  �Ѽƻ����GADCn        �Ҷ����] ADC0�B ADC1�^
*  ��ƪ�^�G�L
*  �ק�ɶ��G2012-2-10
*  ��    �`�G�Ѧ�Ĭ�{�j�Ǫ��ҵ{
*************************************************************************/
void adc_init(ADCn adcn,ADC_Ch ch)
{
    ASSERT( ((adcn == ADC0) && (ch>=AD8 && ch<=AD18)) || ((adcn == ADC1)&& (ch>=AD4a && ch<=AD17)) ) ;   //�ϥ��_���˴�ADCn_CHn�O�_���`

    switch(adcn)
    {
    case ADC0:       /*   ADC0  */
        SIM_SCGC6 |= (SIM_SCGC6_ADC0_MASK );        //�}��ADC0����
        SIM_SOPT7 &= ~(SIM_SOPT7_ADC0ALTTRGEN_MASK  |SIM_SOPT7_ADC0PRETRGSEL_MASK);
        SIM_SOPT7 = SIM_SOPT7_ADC0TRGSEL(0);

        switch(ch)
        {
        case AD8:   //ADC0_SE8 -- PTB0
        case AD9:   //ADC0_SE9 -- PTB1
            SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
            PORT_PCR_REG(PORTB_BASE_PTR, ch-AD8+0) =  PORT_PCR_MUX(0);
            break;
        case AD10:  //ADC0_SE10 -- PTA7
        case AD11:  //ADC0_SE11 -- PTA8
            SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
            PORT_PCR_REG(PORTA_BASE_PTR, ch-AD10+7) =  PORT_PCR_MUX(0);
            break;
        case AD12:  //ADC0_SE12 -- PTB2
        case AD13:  //ADC0_SE13 -- PTB3
            SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
            PORT_PCR_REG(PORTB_BASE_PTR, ch-AD12+2) =  PORT_PCR_MUX(0);
            break;
        case AD14:  //ADC0_SE14 -- PTC0
        case AD15:  //ADC0_SE15 -- PTC1
            SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
            PORT_PCR_REG(PORTC_BASE_PTR, ch-AD14+0) =  PORT_PCR_MUX(0);
            break;
        case AD17:   //ADC0_SE17 -- PTE24
        case AD18:   //ADC0_SE17 -- PTE25
            SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
            PORT_PCR_REG(PORTE_BASE_PTR, ch-AD17+24) =  PORT_PCR_MUX(0);
            break;
        default:
            return;
        }
        return;

  case ADC1:       /*   ADC1    */
      SIM_SCGC3 |= (SIM_SCGC3_ADC1_MASK );
      SIM_SOPT7 &= ~(SIM_SOPT7_ADC1ALTTRGEN_MASK  |SIM_SOPT7_ADC1PRETRGSEL_MASK) ;
      SIM_SOPT7 = SIM_SOPT7_ADC1TRGSEL(0);

      switch(ch)
      {
      case AD4a:   //ADC1_SE4a -- PTE0
      case AD5a:   //ADC1_SE5a -- PTE1
      case AD6a:   //ADC1_SE6a -- PTE2
      case AD7a:   //ADC1_SE7a -- PTE3
          SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
          PORT_PCR_REG(PORTE_BASE_PTR, ch-AD4a+0) =  PORT_PCR_MUX(0);
          break;
      case AD8:  //ADC1_SE8 -- PTB0
      case AD9:  //ADC1_SE9 -- PTB1
          SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
          PORT_PCR_REG(PORTB_BASE_PTR, ch-AD8+0) =  PORT_PCR_MUX(0);
          break;
      case AD10:  //ADC1_SE10 -- PTB4
      case AD11:  //ADC1_SE11 -- PTB5
      case AD12:  //ADC1_SE12 -- PTB6
      case AD13:  //ADC1_SE13 -- PTB7
          SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
          PORT_PCR_REG(PORTB_BASE_PTR, ch-6) =  PORT_PCR_MUX(0);
          break;
      case AD14:  //ADC1_SE14 -- PTB10
      case AD15:  //ADC1_SE15 -- PTB11
          SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
          PORT_PCR_REG(PORTB_BASE_PTR, ch-AD10+4) =  PORT_PCR_MUX(0);
          break;
      case AD17:  //ADC1_SE17 -- PTA17
          SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
          PORT_PCR_REG(PORTA_BASE_PTR, ch) =  PORT_PCR_MUX(0);
          break;
      default:
          break;
      }
      break;
  default:
      break;
  }
}


/*************************************************************************
*                             �����O�J���}�o�u�@��
*
*  ��ƦW�١Gad_once
*  �\�໡���G�����@���@�������q��AD��
*  �Ѽƻ����GADCn        �Ҷ����] ADC0�B ADC1�^
*            ADC_Channel �q�D��
*            ADC_nbit    ��ס] ADC_8bit,ADC_12bit, ADC_10bit, ADC_16bit �^
*  ��ƪ�^�G�L�Ÿ����G��
*  �ק�ɶ��G2012-2-10
*  ��    �`�G�Ѧ�Ĭ�{�j�Ǫ��ҵ{�AB�q�D����n��Ĳ�o�I�I�I�I
*************************************************************************/
u16 ad_once(ADCn adcn,ADC_Ch ch,ADC_nbit bit)  //�����Y�������q��AD��
{
    u16 result = 0;
    ASSERT( ((adcn == ADC0) && (ch>=AD8 && ch<=AD18)) || ((adcn == ADC1)&& (ch>=AD4a && ch<=AD17)) ) ;   //�ϥ��_���˴�ADCn_CHn�O�_���`

    adc_start(adcn, ch, bit);	  //�Ұ�ADC�ഫ

    while (( ADC_SC1_REG(ADCx[adcn], 0 ) & ADC_SC1_COCO_MASK ) != ADC_SC1_COCO_MASK);
    result = ADC_R_REG(ADCx[adcn],0);
    ADC_SC1_REG(ADCx[adcn],0) &= ~ADC_SC1_COCO_MASK;
    return result;
}

/*************************************************************************
*                             �����O�J���}�o�u�@��
*
*  ��ƦW�١Gad_mid
*  �\�໡���G�����T���@�������q��AD�ȡA��^ ����
*  �Ѽƻ����GADCx        �Ҷ����] ADC0�B ADC1�^
*            ADC_Channel �q�D��
*            ADC_nbit    ��ס] ADC_8bit,ADC_12bit, ADC_10bit, ADC_16bit �^
*  ��ƪ�^�G�L�Ÿ����G��
*  �ק�ɶ��G2012-2-10
*  ��    �`�G�ק�Ĭ�{�j�Ǫ��ҵ{
*************************************************************************/
u16 ad_mid(ADCn adcn,ADC_Ch ch,ADC_nbit bit)
{
    u16 i,j,k,tmp;
    ASSERT( ((adcn == ADC0) && (ch>=AD8 && ch<=AD18)) || ((adcn == ADC1)&& (ch>=AD4a && ch<=AD17)) ) ;   //�ϥ��_���˴�ADCn_CHn�O�_���`

    //3��ADC�ഫ
    i = ad_once(adcn,ch,bit);
    j = ad_once(adcn,ch,bit);
    k = ad_once(adcn,ch,bit);

    //������
    tmp = i>j ? i:j;              //tmp����̳̤j��
    return k > tmp ?    tmp :  (   k>i  ?    k   :     i  );
    //                k>tmp>i             tmp>k>i   tmp>i>k
}

/*************************************************************************
*                             �����O�J���}�o�u�@��
*
*  ��ƦW�١Gad_ave
*  �\�໡���G�h�����ˡA��������
*  �Ѽƻ����GADCx        �Ҷ����] ADC0�B ADC1�^
*            ADC_Channel �q�D��
*            ADC_nbit    ��ס] ADC_8bit,ADC_12bit, ADC_10bit, ADC_16bit �^
*            N           �����o�i����(�d��:0~255)
*  ��ƪ�^�G16��L�Ÿ����G��
*  �ק�ɶ��G2012-2-10
*  ��    �`�G�ק�Ĭ�{�j�Ǫ��ҵ{
*************************************************************************/
u16 ad_ave(ADCn adcn,ADC_Ch ch,ADC_nbit bit,u8 N) //�����o�i
{
    u32 tmp = 0;
    u8  i;
    ASSERT( ((adcn == ADC0) && (ch>=AD8 && ch<=AD18)) || ((adcn == ADC1)&& (ch>=AD4a && ch<=AD17)) ) ;   //�ϥ��_���˴�ADCn_CHn�O�_���`

    for(i = 0; i < N; i++)
        tmp += ad_once(adcn,ch,bit);
    tmp = tmp / N;
    return (u16)tmp;
}

/*************************************************************************
*                             �����O�J���}�o�u�@��
*
*  ��ƦW�١Gadc_start
*  �\�໡���G�Ұ�adc�n����ˡAB�q�D����Ω�n��Ĳ�o�I�I�I�I
*  �Ѽƻ����GADCx        �Ҷ����] ADC0�B ADC1�^
*            ADC_Channel �q�D��
*            ADC_nbit    ��ס] ADC_8bit,ADC_12bit, ADC_10bit, ADC_16bit �^
*  ��ƪ�^�G�L
*  �ק�ɶ��G2012-2-10
*  ��    �`�G�ק�Ĭ�{�j�Ǫ��ҵ{
*************************************************************************/
void adc_start(ADCn adcn,ADC_Ch ch,ADC_nbit bit)
{

    Master_Adc_Config.STATUS1A = AIEN_ON | DIFF_SINGLE | ADC_SC1_ADCH( ch );

    //��l��ADC�q�{�t�m
    Master_Adc_Config.CONFIG1  = ADLPC_NORMAL
                                 | ADC_CFG1_ADIV(ADIV_4)
                                 | ADLSMP_LONG
                                 | ADC_CFG1_MODE(bit)
                                 | ADC_CFG1_ADICLK(ADICLK_BUS);
    Master_Adc_Config.CONFIG2  = MUXSEL_ADCB    //MUXSEL_ADCB
                                 | ADACKEN_DISABLED
                                 | ADHSC_HISPEED
                                 | ADC_CFG2_ADLSTS(ADLSTS_20) ;

    Master_Adc_Config.COMPARE1 = 0x1234u ;                 //���N��
    Master_Adc_Config.COMPARE2 = 0x5678u ;                 //���N��

    adc_config_alt(ADCx[adcn], &Master_Adc_Config);       // �t�m ADCn
}

/*************************************************************************
*                             �����O�J���}�o�u�@��
*
*  ��ƦW�١Gadc_stop
*  �\�໡���G����ADC�ഫ
*  �Ѽƻ����GADCx        �Ҷ����] ADC0�B ADC1�^
*            ADC_Channel �q�D��
*  ��ƪ�^�G�L
*  �ק�ɶ��G2012-2-10
*  ��    �`�G�ק�Ĭ�{�j�Ǫ��ҵ{
*************************************************************************/
void adc_stop(ADCn adcn)
{
    Master_Adc_Config.STATUS1A = AIEN_ON | DIFF_SINGLE | ADC_SC1_ADCH(Module_disabled);
    adc_config_alt(ADCx[adcn], &Master_Adc_Config);  // �t�mADC0
}


/*************************************************************************
*                             �����O�J���}�o�u�@��
*
*  ��ƦW�١Gadc_config_alt
*  �\�໡���G�Nadc�H�s�����c��t�m�iadc�H�s��
*  �Ѽƻ����Gadcmap      adc��}�H�s���a�}�]ADC0_BASE_PTR,ADC1_BASE_PTR�^
*            ADC_CfgPtr  �s�� �H�s���Ȫ����c��
*  ��ƪ�^�G�L
*  �ק�ɶ��G2012-2-10
*  ��    �`�G�ק�x��u�{���ҵ{
*************************************************************************/
void adc_config_alt(ADC_MemMapPtr adcmap, tADC_ConfigPtr ADC_CfgPtr)
{
    ADC_CFG1_REG(adcmap) = ADC_CfgPtr->CONFIG1;
    ADC_CFG2_REG(adcmap) = ADC_CfgPtr->CONFIG2;
    ADC_CV1_REG(adcmap)  = ADC_CfgPtr->COMPARE1;
    ADC_CV2_REG(adcmap)  = ADC_CfgPtr->COMPARE2;
    ADC_SC2_REG(adcmap)  = ADC_CfgPtr->STATUS2;
    ADC_SC3_REG(adcmap)  = ADC_CfgPtr->STATUS3;
    ADC_PGA_REG(adcmap)  = ADC_CfgPtr->PGA;
    ADC_SC1_REG(adcmap,A)= ADC_CfgPtr->STATUS1A;
    ADC_SC1_REG(adcmap,B)= ADC_CfgPtr->STATUS1B;
}



