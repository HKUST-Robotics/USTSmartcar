/****************************************
   isr.h
   HKUST Smartcar 2013 - Zyan,John,Louis,Yumi
**********************************************************************************/	

#ifndef __ISR_H
#define __ISR_H 1

#include  "include.h"

#undef  VECTOR_084
#define VECTOR_084    PIT0_IRQHandler     //PIT0_IRQHandler

#undef  VECTOR_085
#define VECTOR_085    PIT1_IRQHandler     //john's testing encoder interrupt handler

#undef  VECTOR_087
#define VECTOR_087    pit3_system_loop     //System multitask loop, run on 1ms

#undef  VECTOR_103
#define VECTOR_103    encoder_counter    //for getting encoder count


extern void PIT0_IRQHandler();            
extern void PIT1_IRQHandler();
extern void pit3_system_loop();
extern void encoder_counter();

#endif  //__ISR_H

/* End of "isr.h" */