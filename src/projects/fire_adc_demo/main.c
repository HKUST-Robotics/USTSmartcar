/******************** (C) COPYRIGHT 2011 野火嵌入式開發工作室 ********************
 * 文件名       ：main.c
 * 描述         ：ADC模數轉換實驗
 *
 * 實驗平台     ：野火kinetis開發板
 * 庫版本       ：
 * 嵌入系統     ：
 *
 * 作者         ：野火嵌入式開發工作室
 * 淘寶店       ：http://firestm32.taobao.com
 * 技術支持論壇 ：http://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
**********************************************************************************/	


#include "common.h"
#include "include.h"


/*************************************************************************
*                             野火嵌入式開發工作室
*                              ADC模數轉換實驗測試
*
*  實驗說明：野火ADC模數轉換實驗，用串口發送轉換後結果。
*            野火串口默認為： UART1，TX接PTC4，RX接PTC3
*            k60_fire.h裡定義了 printf 函數的輸出設置：
*                #define FIRE_PORT           UART1
*                #define FIRE_BAUD           19200
*
*  實驗操作：這裡用 ADC1_SE4a ，所以 ADC1_SE4a 還要接一個0~3.3V的可調電路
*            ADC1_SE4a 就是 PTE0 , 在 adc.h 文件裡有表格
*            打開串口助手，設置波特率為 19200 。
*            串口線（經過MAX3232電平轉換）：TX接PTC4，RX接PTC3
*
*  實驗效果：在串口助手裡，可以看到輸出如下信息：
*
*                    野火kinetis核心板測試程序
*       內核頻率：200MHz	總線頻率 ：66MHz
*       flex頻率：66MHz 	flash頻率：28MHz
*
*       Software Reset
*
*       K60-144pin      Silicon rev 1.2
*       Flash parameter version 0.0.7.0
*       Flash version ID 3.2.7.0
*       512 kBytes of P-flash	P-flash only
*       128 kBytes of RAM
*
*       AD轉換一次的結果為:36983
*       AD轉換三次的中值結果為:37143
*       AD轉換十次的平均值結果為:36912
*
*
*
*  實驗目的：測試 ADC 轉換的各個結果
*
*  修改時間：2012-2-29     已測試
*
*  備    注：adc.h 有 各個ADC通道所對應管腳的表格，方便查看
*            如果懸空管腳，那測出的值會變化比較大
*************************************************************************/
void  main()
{
    u16 ADresult;                           //保存ADC轉換結果

    uart_init(UART1,19200);                 //初始化串口，用來發送轉換數據

    adc_init(ADC1,SE4a);                    //初始化ADC1_SE4a ,從adc.h裡的表格就可以看到ADC1_SE4a對應為PTE0

    while(1)
    {
        /****** 讀取一次 ******/
        ADresult    =   ad_once(ADC1,SE4a,ADC_16bit);           //讀取 ADC1_SE4a ，16位精度
        printf("AD轉換一次的結果為:%d\n\n",ADresult);

        time_delay_ms(500);                //延時500ms

        /****** 讀取三次，取中值 ******/
        ADresult    =   ad_mid(ADC1,SE4a,ADC_16bit);            //讀取 ADC1_SE4a ，16位精度
        printf("AD轉換三次的中值結果為:%d\n\n",ADresult);

        time_delay_ms(500);                //延時500ms

        /****** 讀取十次次，取平均值 ******/
        ADresult    =   ad_ave(ADC1,SE4a,ADC_16bit,10);         //讀取 ADC1_SE4a ，16位精度
        printf("AD轉換十次的平均值結果為:%d\n\n",ADresult);

        time_delay_ms(500);                //延時500ms
    }
}
