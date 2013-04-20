/*******************************************

linearccd.c

Authored by Louis Mo
for HKUST SmartCar team 2013

Edited by John Ching

*******************************************/

#include  "include.h"
#include  "linearccd.h"

void  linccd_print(char linccd_array[]){
  int i;
  
  uart_sendStr(UART3,"\n\014");     // New page form feed
  uart_sendStr(UART3,"111111lolokdjfoijaoisudhfioausdfhoaiushf");
  for( i = 0 ; i < 128 ; i++){
    uart_putchar(UART3,linccd_array[i]); // Print One pixel One loop
  }
           
  uart_sendStr(UART3,"\n\014");     // New page form feed
}