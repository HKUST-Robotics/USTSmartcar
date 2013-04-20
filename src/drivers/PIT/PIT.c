/******************** (C) COPYRIGHT 2011 野火嵌入式開發工作室 ********************
 * 文件名       ：PIT.c
 * 描述         ：PIT周期中斷計時器，定時中斷
 * 備注         ：
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
#include  "PIT.h"     //周期中斷計時器

/*************************************************************************
*                             野火嵌入式開發工作室
*
*  函數名稱：pit_init
*  功能說明：PITn定時中斷
*  參數說明：PITn        模塊號（PIT0~PIT3）
             cnt         延時時間(單位為bus時鐘周期)
*  函數返回：無
*  修改時間：2012-1-24
*  備    注：
*************************************************************************/
void pit_init(PITn pitn,u32 cnt)
{
    //PIT 用的是 Bus Clock 總線頻率
    //溢出計數 = 總線頻率 * 時間

    /* 開啟時鐘*/
    SIM_SCGC6       |=SIM_SCGC6_PIT_MASK;                             //使能PIT時鐘

    /* PIT模塊控制 PIT Module Control Register (PIT_MCR) */
    PIT_MCR         &=~(PIT_MCR_MDIS_MASK | PIT_MCR_FRZ_MASK );       //使能PIT定時器時鐘 ，調試模式下繼續運行

    /* 定時器加載值設置 Timer Load Value Register (PIT_LDVALn) */
    PIT_LDVAL(pitn)  =cnt;                                           //設置溢出中斷時間

    //定時時間到了後，TIF 置 1 。寫1的時候就會清0
    PIT_Flag_Clear(pitn);                                             //清中斷標志位

    /* 定時器控制寄存器 Timer Control Register (PIT_TCTRL0) */
    PIT_TCTRL(pitn) |=( PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK );    //使能 PITn定時器,並開PITn中斷

    enable_irq(pitn+68);			                                  //開接收引腳的IRQ中斷
}