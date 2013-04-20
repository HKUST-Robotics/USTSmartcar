/******************** (C) COPYRIGHT 2011 野火嵌入式開發工作室 ********************
 * 文件名       ：isr.h
 * 描述         ：重新宏定義中斷號，重映射中斷向量表裡的中斷函數地址，
 *                使其指向我們所定義的中斷服務函數。聲明中斷服務函數
 *                警告：只能在"vectors.c"包含，而且必須在"vectors.h"包含的後面！！！
 *          
 * 實驗平台     ：野火kinetis開發板
 * 庫版本       ：
 * 嵌入系統     ：
 *
 * 作者         ：野火嵌入式開發工作室 
 * 淘寶店       ：http://firestm32.taobao.com
 * 技術支持論壇 ：http://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008   
**********************************************************************************/	



#ifndef __ISR_H
#define __ISR_H 1

#include  "include.h"  

/*                          重新定義中斷向量表
 *  先取消默認的中斷向量元素宏定義       #undef  VECTOR_xxx
 *  在重新定義到自己編寫的中斷函數       #define VECTOR_xxx    xxx_IRQHandler
 *  例如：
 *       #undef  VECTOR_003
 *       #define VECTOR_003    HardFault_Handler    重新定義硬件上訪中斷服務函數
 */

#undef  VECTOR_084
#define VECTOR_084    PIT0_IRQHandler     //重新定義84號中斷為PIT0_IRQHandler中斷

extern void PIT0_IRQHandler();            //PIT0 定時中斷服務函數

#endif  //__ISR_H

/* End of "isr.h" */
