/******************** (C) COPYRIGHT 2011 ≥•§ı¥O§J¶°∂}µo§uß@´« ********************
 * §Â•Û¶W       °Gisr.h
 * ¥y≠z         °G≠´∑sßª©w∏q§§¬_∏π°A≠´¨MÆg§§¬_¶V∂q™Ì∏Ã™∫§§¬_®Áº∆¶aß}°A
 *                ®œ®‰´¸¶Vß⁄≠Ã©“©w∏q™∫§§¬_™A∞»®Áº∆°C¡n©˙§§¬_™A∞»®Áº∆
 *                ƒµßi°G•uØ‡¶b"vectors.c"•]ßt°A¶”•B•≤∂∑¶b"vectors.h"•]ßt™∫´·≠±°I°I°I
 *          
 * πÍ≈Á•≠•x     °G≥•§ıkinetis∂}µo™O
 * Æw™©•ª       °G
 * ¥O§J®t≤Œ     °G
 *
 * ß@™Ã         °G≥•§ı¥O§J¶°∂}µo§uß@´« 
 * ≤^ƒ_©±       °Ghttp://firestm32.taobao.com
 * ßﬁ≥N§‰´˘Ω◊æ¬ °Ghttp://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008   
**********************************************************************************/	



#ifndef __ISR_H
#define __ISR_H 1

#include  "include.h"  

/*                          ≠´∑s©w∏q§§¬_¶V∂q™Ì
 *  •˝®˙Æ¯¿qª{™∫§§¬_¶V∂q§∏Ø¿ßª©w∏q       #undef  VECTOR_xxx
 *  ¶b≠´∑s©w∏q®Ï¶€§vΩsºg™∫§§¬_®Áº∆       #define VECTOR_xxx    xxx_IRQHandler
 *  ®“¶p°G
 *       #undef  VECTOR_003
 *       #define VECTOR_003    HardFault_Handler    ≠´∑s©w∏qµw•Û§W≥X§§¬_™A∞»®Áº∆
 */

#undef  VECTOR_084
#define VECTOR_084    PIT0_IRQHandler     //≠´∑s©w∏q84∏π§§¬_¨∞PIT0_IRQHandler§§¬_

#undef  VECTOR_085
#define VECTOR_085    PIT1_IRQHandler     //john's testing encoder interrupt handler

#undef  VECTOR_079
#define VECTOR_079    FTM1_IRQHandler     //FTM1 input capture for getting encoder count


extern void PIT0_IRQHandler();            //PIT0 ©wÆ…§§¬_™A∞»®Áº∆
extern void PIT1_IRQHandler();
extern void FTM1_IRQHandler();

#endif  //__ISR_H

/* End of "isr.h" */
