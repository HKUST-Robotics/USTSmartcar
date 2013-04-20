

#ifndef __GPIO_H__
#define __GPIO_H__


#include "delay.h"



/*-------------fuzzy logic-------------*/
//slope   
#define a_LB 0x00
#define a_LM 0x10
#define a_LS 0x20
#define a_M 0x30
#define a_RS 0x40
#define a_RM 0x50
#define a_RB 0x60

//y-intercept
#define e_RB 0x01
#define e_R 0x11
#define e_M 0x21
#define e_L 0x31
#define e_LB 0x41

//servo output       
#define LB 0x80
#define LM 0x90
#define LS 0xA0
#define F  0xB0
#define RS 0xC0
#define RM 0xD0
#define RB 0xE0

//speed output       
#define LB 0x80
#define LM 0x90
#define LS 0xA0
#define F  0xB0
#define RS 0xC0
#define RM 0xD0
#define RB 0xE0

void defuzzify();


#endif