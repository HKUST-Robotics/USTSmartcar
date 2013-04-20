/******************** (C) COPYRIGHT 2011 野火嵌入式开发工作室 ********************
 * 文件名       ：common.h
 * 描述         ：通用配置头文件，适合全部的工程
 *
 * 实验平台     ：野火kinetis开发板
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：
 * 淘宝店       ：http://firestm32.taobao.com
 * 技术支持论坛 ：http://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
**********************************************************************************/	

#ifndef _COMMON_H_
#define _COMMON_H_

//为使用仿真模式而添加的，仿真模式应该屏蔽串口发送、和PLL锁相环设置
//#define   Simulator
#ifdef    Simulator
  #define NO_PLL_INIT   //禁用锁相环
  #define NPRINTF       //禁用printf
#endif

/********************************************************************/

/*
 * Debug prints ON (#define) or OFF (#undef)
 */
//#define DEBUG_PRINT



/*****************************设置数据类型*****************************/
typedef 	unsigned 	char		u8;	  //无符号型
typedef 	unsigned 	short int	u16;
typedef 	unsigned 	long  int	u32;

typedef 			char		s8;	  //有符号型
typedef 			short int	s16;
typedef 			long  int	s32;



/*
 * Include the generic CPU header file
 */
#include "arm_cm4.h"

/*
 * Include the platform specific header file
 */
#if (defined(TWR_K40X256))
  #include "k40_tower.h"
#elif (defined(TWR_K60N512))
// #include "k60_tower.h"
  #include "k60_fire.h"
#elif (defined(TWR_K53N512))
 #include "k53_tower.h"
#else
  #error "No valid platform defined"
#endif

/*
 * Include the cpu specific header file
 */
#if (defined(CPU_MK40N512VMD100))
//  #include "MK40N512VMD100.h"
  #include "MK40DZ10.h"
#elif (defined(CPU_MK60N512VMD100))
// #include "MK60N512VMD100.h"
  #include "MK60DZ10.h"
#elif (defined(CPU_MK53N512CMD100))
//  #include "MK53N512CMD100.h"
  #include "MK53DZ10.h"
#else
  #error "No valid CPU defined"
#endif


/*
 * Include any toolchain specfic header files
 */
#if (defined(CW))
  #include "cw.h"
#elif (defined(IAR))
  #include "iar.h"
#else
#warning "No toolchain specific header included"
#endif

/*
 * Include common utilities
 */
#include "assert.h"
#include "io.h"
#include "startup.h"
#include "stdlib.h"


#if (defined(IAR))
	#include "intrinsics.h"
#endif


#include  "sysinit.h"           //系统配置
#include "mcg.h"
#include "fire_drivers_cfg.h"   //管脚复用配置


/********************************************************************/

#endif /* _COMMON_H_ */
