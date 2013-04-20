#ifndef __USB_REGULATOR__
#define __USB_REGULATOR__

#include "FslTypes.h"
#include "common.h"

/* Defines */
#define USB_REG_SET_ENABLE      FLAG_SET(SIM_SOPT1_USBREGEN_SHIFT,SIM_SOPT1)
#define USB_REG_CLEAR_ENABLE    FLAG_CLR(SIM_SOPT1_USBREGEN_SHIFT,SIM_SOPT1)
#define USB_REG_SET_STDBY       FLAG_SET(SIM_SOPT1_USBSTBY_SHIFT,SIM_SOPT1)
#define USB_REG_CLEAR_STDBY     FLAG_CLR(SIM_SOPT1_USBSTBY_SHIFT,SIM_SOPT1)



/* Prototypes*/
void USBReg_Testing(void);

#endif /* __USB_REGULATOR__*/