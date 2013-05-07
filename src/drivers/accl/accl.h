/*******************************************

accl.h

A library for the mma8451 accelerometer

Authored by John Ching
for HKUST SmartCar team 2013

*******************************************/

#include "include.h"


float accl_x16, accl_y16, accl_z16, accl_tilt16;

void      accl_init(void);
float accl_getx(void);
float accl_gety(void);
float accl_getz(void);
double    accl_tilt(void);//usually use this for getting the tilt

void      accl_update(void);
float accl_convert(u8 axis[]);
void      accl_print(void);
float gyro_dtheta(void);

float angl_calculate(void);


/*************************************************************
usage:

accl_init();

while(1){
  accl_print();
}

**************************************************************/