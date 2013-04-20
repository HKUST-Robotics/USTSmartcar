/*mma8451 accelerometer library
for hkust smartcar team 2013

accl.h

authored by John Ching
*/

#include "include.h"

void      accl_init(void);
float accl_getx(void);
float accl_gety(void);
float accl_getz(void);
double    accl_tilt(void);//usually use this for getting the tilt

void      accl_update(void);
float accl_convert(u8 axis[]);
void      accl_print(void);

