#ifndef __ISR_H
#define __ISR_H 1

#include  "include.h"  

#undef VECTOR_104//for portB interrupt
#define VECTOR_104 PORTB_IRQHandler
/*
#undef  VECTOR_084
#define VECTOR_084    PIT0_IRQHandler
*/
extern void PORTB_IRQHandler();//PORTE interrupt, for HS and VS
//extern void PIT0_IRQHandler();

#endif  //__ISR_H