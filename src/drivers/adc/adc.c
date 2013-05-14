/******************** (C) COPYRIGHT 2011 野火嵌入式開發工作室 ********************
 * 文件名       ：adc.c
 * 描述         ：adc驅動函數
 *
 * 實驗平台     ：野火kinetis開發板
 * 庫版本       ：
 * 嵌入系統     ：
 *
 * 作者         ：
 * 淘寶店       ：http://firestm32.taobao.com
 * 技術支持論壇 ：http://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
**********************************************************************************/	

#include "common.h"
#include "adc.h"


tADC_Config Master_Adc_Config;          //該結構體包含了需要的ADC/PGA配置

volatile struct ADC_MemMap *ADCx[2]={ADC0_BASE_PTR,ADC1_BASE_PTR}; //定義兩個指針數組保存 ADCx 的地址

/*************************************************************************
*                             野火嵌入式開發工作室
*
*  函數名稱：adc_init
*  功能說明：AD初始化，使能時鐘
*  參數說明：ADCn        模塊號（ ADC0、 ADC1）
*  函數返回：無
*  修改時間：2012-2-10
*  備    注：參考蘇州大學的例程
*************************************************************************/
void adc_init(ADCn adcn,ADC_Ch ch)
{
    ASSERT( ((adcn == ADC0) && (ch>=AD8 && ch<=AD18)) || ((adcn == ADC1)&& (ch>=AD4a && ch<=AD17)) ) ;   //使用斷言檢測ADCn_CHn是否正常

    switch(adcn)
    {
    case ADC0:       /*   ADC0  */
        SIM_SCGC6 |= (SIM_SCGC6_ADC0_MASK );        //開啟ADC0時鐘
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
*                             野火嵌入式開發工作室
*
*  函數名稱：ad_once
*  功能說明：采集一次一路模擬量的AD值
*  參數說明：ADCn        模塊號（ ADC0、 ADC1）
*            ADC_Channel 通道號
*            ADC_nbit    精度（ ADC_8bit,ADC_12bit, ADC_10bit, ADC_16bit ）
*  函數返回：無符號結果值
*  修改時間：2012-2-10
*  備    注：參考蘇州大學的例程，B通道不能軟件觸發！！！！
*************************************************************************/
u16 ad_once(ADCn adcn,ADC_Ch ch,ADC_nbit bit)  //采集某路模擬量的AD值
{
    u16 result = 0;
    ASSERT( ((adcn == ADC0) && (ch>=AD8 && ch<=AD18)) || ((adcn == ADC1)&& (ch>=AD4a && ch<=AD17)) ) ;   //使用斷言檢測ADCn_CHn是否正常

    adc_start(adcn, ch, bit);	  //啟動ADC轉換

    while (( ADC_SC1_REG(ADCx[adcn], 0 ) & ADC_SC1_COCO_MASK ) != ADC_SC1_COCO_MASK);
    result = ADC_R_REG(ADCx[adcn],0);
    ADC_SC1_REG(ADCx[adcn],0) &= ~ADC_SC1_COCO_MASK;
    return result;
}

/*************************************************************************
*                             野火嵌入式開發工作室
*
*  函數名稱：ad_mid
*  功能說明：采集三次一路模擬量的AD值，返回 中值
*  參數說明：ADCx        模塊號（ ADC0、 ADC1）
*            ADC_Channel 通道號
*            ADC_nbit    精度（ ADC_8bit,ADC_12bit, ADC_10bit, ADC_16bit ）
*  函數返回：無符號結果值
*  修改時間：2012-2-10
*  備    注：修改蘇州大學的例程
*************************************************************************/
u16 ad_mid(ADCn adcn,ADC_Ch ch,ADC_nbit bit)
{
    u16 i,j,k,tmp;
    ASSERT( ((adcn == ADC0) && (ch>=AD8 && ch<=AD18)) || ((adcn == ADC1)&& (ch>=AD4a && ch<=AD17)) ) ;   //使用斷言檢測ADCn_CHn是否正常

    //3次ADC轉換
    i = ad_once(adcn,ch,bit);
    j = ad_once(adcn,ch,bit);
    k = ad_once(adcn,ch,bit);

    //取中值
    tmp = i>j ? i:j;              //tmp取兩者最大值
    return k > tmp ?    tmp :  (   k>i  ?    k   :     i  );
    //                k>tmp>i             tmp>k>i   tmp>i>k
}

/*************************************************************************
*                             野火嵌入式開發工作室
*
*  函數名稱：ad_ave
*  功能說明：多次采樣，取平均值
*  參數說明：ADCx        模塊號（ ADC0、 ADC1）
*            ADC_Channel 通道號
*            ADC_nbit    精度（ ADC_8bit,ADC_12bit, ADC_10bit, ADC_16bit ）
*            N           均值濾波次數(範圍:0~255)
*  函數返回：16位無符號結果值
*  修改時間：2012-2-10
*  備    注：修改蘇州大學的例程
*************************************************************************/
u16 ad_ave(ADCn adcn,ADC_Ch ch,ADC_nbit bit,u8 N) //均值濾波
{
    u32 tmp = 0;
    u8  i;
    ASSERT( ((adcn == ADC0) && (ch>=AD8 && ch<=AD18)) || ((adcn == ADC1)&& (ch>=AD4a && ch<=AD17)) ) ;   //使用斷言檢測ADCn_CHn是否正常

    for(i = 0; i < N; i++)
        tmp += ad_once(adcn,ch,bit);
    tmp = tmp / N;
    return (u16)tmp;
}

/*************************************************************************
*                             野火嵌入式開發工作室
*
*  函數名稱：adc_start
*  功能說明：啟動adc軟件采樣，B通道不能用於軟件觸發！！！！
*  參數說明：ADCx        模塊號（ ADC0、 ADC1）
*            ADC_Channel 通道號
*            ADC_nbit    精度（ ADC_8bit,ADC_12bit, ADC_10bit, ADC_16bit ）
*  函數返回：無
*  修改時間：2012-2-10
*  備    注：修改蘇州大學的例程
*************************************************************************/
void adc_start(ADCn adcn,ADC_Ch ch,ADC_nbit bit)
{

    Master_Adc_Config.STATUS1A = AIEN_ON | DIFF_SINGLE | ADC_SC1_ADCH( ch );

    //初始化ADC默認配置
    Master_Adc_Config.CONFIG1  = ADLPC_NORMAL
                                 | ADC_CFG1_ADIV(ADIV_4)
                                 | ADLSMP_LONG
                                 | ADC_CFG1_MODE(bit)
                                 | ADC_CFG1_ADICLK(ADICLK_BUS);
    Master_Adc_Config.CONFIG2  = MUXSEL_ADCB    //MUXSEL_ADCB
                                 | ADACKEN_DISABLED
                                 | ADHSC_HISPEED
                                 | ADC_CFG2_ADLSTS(ADLSTS_20) ;

    Master_Adc_Config.COMPARE1 = 0x1234u ;                 //任意值
    Master_Adc_Config.COMPARE2 = 0x5678u ;                 //任意值

    adc_config_alt(ADCx[adcn], &Master_Adc_Config);       // 配置 ADCn
}

/*************************************************************************
*                             野火嵌入式開發工作室
*
*  函數名稱：adc_stop
*  功能說明：停止ADC轉換
*  參數說明：ADCx        模塊號（ ADC0、 ADC1）
*            ADC_Channel 通道號
*  函數返回：無
*  修改時間：2012-2-10
*  備    注：修改蘇州大學的例程
*************************************************************************/
void adc_stop(ADCn adcn)
{
    Master_Adc_Config.STATUS1A = AIEN_ON | DIFF_SINGLE | ADC_SC1_ADCH(Module_disabled);
    adc_config_alt(ADCx[adcn], &Master_Adc_Config);  // 配置ADC0
}


/*************************************************************************
*                             野火嵌入式開發工作室
*
*  函數名稱：adc_config_alt
*  功能說明：將adc寄存器結構體配置進adc寄存器
*  參數說明：adcmap      adc基址寄存器地址（ADC0_BASE_PTR,ADC1_BASE_PTR）
*            ADC_CfgPtr  存放 寄存器值的結構體
*  函數返回：無
*  修改時間：2012-2-10
*  備    注：修改官方工程的例程
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



