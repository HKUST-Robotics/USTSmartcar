/*mma8451 accelerometer library
for hkust smartcar team 2013

accl.c

authored by John Ching
*/

#include "accl.h"
#include "include.h"
#include <math.h>

u8 accl_buffer[2];
char accl_todis[];

float accl_x16, accl_y16, accl_z16;
    
void accl_init(void){
    I2C_init(I2C0);
    I2C_WriteAddr(I2C0, 0x1c, 0x0f, 0x10);  //cutoff frequency setting
    I2C_WriteAddr(I2C0, 0x1c, 0x0e, 0x00);  //set range to 2G
    I2C_WriteAddr(I2C0, 0x1c, 0x2a, 0x01);  //activate mma8451
}


float accl_getx(void){
  return accl_x16;
}

float accl_gety(void){
  return accl_y16;
}

float accl_getz(void){
  return accl_z16;
}


void accl_update(void){
  

    accl_buffer[0]   =   I2C_ReadAddr(I2C0, 0x1c, 0x01);
    delayms(0);//i dont know why this works, but dont remove
    accl_buffer[1]   =   I2C_ReadAddr(I2C0, 0x1c, 0x02);
    delayms(0);
    accl_x16=accl_convert(accl_buffer);
    accl_buffer[0]   =   I2C_ReadAddr(I2C0, 0x1c, 0x03);
    delayms(0);
    accl_buffer[1]   =   I2C_ReadAddr(I2C0, 0x1c, 0x04);
    delayms(0);
    accl_y16=accl_convert(accl_buffer);
    accl_buffer[0]   =   I2C_ReadAddr(I2C0, 0x1c, 0x05);
    delayms(0);
    accl_buffer[1]   =   I2C_ReadAddr(I2C0, 0x1c, 0x06);
    delayms(0);
    accl_z16=accl_convert(accl_buffer);
}

float accl_convert(u8 axis[]){
  u16 V1;
  float vv;
  int sign;
  if(axis[0] > 0x7F)
  {
    sign=-1;
    V1   = (int)axis[0];	
    axis[1]= axis[1]>>2;
    V1   = V1<<6 | axis[1];
    V1   = (~V1 + 1)&0X3FFF;
    //V1= (~(V.mword>>2) + 1);
  }
  else
  {
    sign=1;
    V1   = (int)axis[0];	
    axis[1]= axis[1]>>2;
    V1   = V1<<6 | axis[1];
    //V1=(V.mword>>2)&0X3FFF;
  }
  vv=sign*(((float)V1)*0.0219726562);
  return vv;
}


double    accl_tilt(void){
  return (57.295779513*atan(accl_y16/accl_x16));
}

void      accl_print(void){
  accl_update();
  sprintf(accl_todis,"\nx = %f",accl_x16);
  uart_sendStr (UART3, (u8*)accl_todis);
  sprintf(accl_todis,"\ny = %f",accl_y16);
  uart_sendStr (UART3, (u8*)accl_todis);
  sprintf(accl_todis,"\nz = %f",accl_z16);
  uart_sendStr (UART3, (u8*)accl_todis); 
  

  sprintf(accl_todis,"\ntilt = %f",accl_tilt());
  uart_sendStr (UART3, (u8*)accl_todis); 
  
}