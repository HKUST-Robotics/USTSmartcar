/*******************************************

accl.c

A library for the mma8451 accelerometer

Authored by John Ching
for HKUST SmartCar team 2013

*******************************************/

#include "accl.h"
#include "include.h"
#include <math.h>

#define GRAVITY_ADJUST_TIME_CONSTANT 

float g_fcarAngle;  //global variables for the balencing control algorithm
float g_fGyroscopeAngleIntegral;  //

float accl_x16, accl_y16, accl_z16;
volatile float accl_tilt16;
u8 accl_buffer[2];
char accl_todis[];

    
void accl_init(void){
    I2C_init(I2C0);
    I2C_WriteAddr(I2C0, 0x1c, 0x0f, 0x10);  //cutoff frequency setting
    I2C_WriteAddr(I2C0, 0x1c, 0x0e, 0x00);  //set range to 2G
    I2C_WriteAddr(I2C0, 0x1c, 0x2a, 0x01);  //activate mma8451
    
    
    adc_init  (ADC1,10);//Init for gyro ADC
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
    accl_tilt16=accl_tilt();
//    accl_buffer[0]   =   I2C_ReadAddr(I2C0, 0x1c, 0x05);
//    delayms(0);
//    accl_buffer[1]   =   I2C_ReadAddr(I2C0, 0x1c, 0x06);
//    delayms(0);
//    accl_z16=accl_convert(accl_buffer);
    
    //z axis values are not read to save time
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
  
  //This prints out the XYZ Reading from the accelerometer
  
//  sprintf(accl_todis,"\nx = %f",accl_x16);
//  uart_sendStr (UART3, (u8*)accl_todis);
//  sprintf(accl_todis,"\ny = %f",accl_y16);
//  uart_sendStr (UART3, (u8*)accl_todis);
//  sprintf(accl_todis,"\nz = %f",accl_z16);
//  uart_sendStr (UART3, (u8*)accl_todis); 
  
  printf("\n\nAccelerometer and Gyro data\n====================");
  sprintf(accl_todis,"\naccl_tilt()   = %f",accl_tilt());
  uart_sendStr (UART3, (u8*)accl_todis); 
  sprintf(accl_todis,"\ngyro_dtheta() = %f",gyro_dtheta());
      uart_sendStr(UART3, (u8*)accl_todis);
  
}

float gyro_dtheta(void){
  float gyro_scaledval;
  gyro_scaledval=((ad_once(ADC1,10,ADC_16bit)-28040)/13.305);
  return ((-1)*gyro_scaledval);
}

/*

void angl_calculate(void){
  float fDeltaValue;
  
  gyro_dtheta();
  accl_update();
  accl_tilt();
  
  g_fCarAngle = g_fGyroscopeAngleIntegral;
  fDeltaValue = (g_fGravityAngle - g_fCarAngle) / GRAVITY_ADJUST_TIME_CONSTANT;
}
*/