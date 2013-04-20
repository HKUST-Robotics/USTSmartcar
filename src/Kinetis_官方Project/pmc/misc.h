
#include "common.h"

#ifdef CPU_MK60N512VMD100

  #define ENABLE_GPIO_CLOCKS (SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK))
  #define LED0_EN (PORTA_PCR11 = PORT_PCR_MUX(1))
  #define LED1_EN (PORTA_PCR29 = PORT_PCR_MUX(1))
  #define LED2_EN (PORTA_PCR28 = PORT_PCR_MUX(1))
  #define LED3_EN (PORTA_PCR10 = PORT_PCR_MUX(1))

  #define LED0_TOGGLE (GPIOA_PTOR = (1<<11))
  #define LED1_TOGGLE (GPIOA_PTOR = (1<<29))
  #define LED2_TOGGLE (GPIOA_PTOR = (1<<28))
  #define LED3_TOGGLE (GPIOA_PTOR = (1<<10))

#else

  #define ENABLE_GPIO_CLOCKS (SIM_SCGC5 |= (SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK))
  #define LED0_EN (PORTB_PCR11 = PORT_PCR_MUX(1))
  #define LED1_EN (PORTC_PCR7 = PORT_PCR_MUX(1))
  #define LED2_EN (PORTC_PCR8 = PORT_PCR_MUX(1))
  #define LED3_EN (PORTC_PCR9 = PORT_PCR_MUX(1))

  #define LED0_TOGGLE (GPIOB_PTOR = (1<<11))
  #define LED1_TOGGLE (GPIOC_PTOR = (1<<7))
  #define LED2_TOGGLE (GPIOC_PTOR = (1<<8))
  #define LED3_TOGGLE (GPIOC_PTOR = (1<<9))

#endif