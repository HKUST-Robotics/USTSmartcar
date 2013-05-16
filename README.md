USTSmartcar
===========

This is a library developed by the HKUST Robotics team for the SmartCar Competition. 
Compiles in the IAR IDE for the Freescale k60 MCU on a custom made development board.

Gyroscope, Accelerometer, encoders and Linear CCD libraries are also written for input.

Useful Links:
-------------
Good tool for merging conflicts:
http://adventuresincoding.com/2010/04/how-to-setup-git-to-use-diffmerge

Conventions
-----------
This library attempts to follow the following conventions:

1.If your function is part of a Hardware Ability:
- have it live inside of a library eventually ( i.e `accl.c`, `accl.h`)

2.Prefix all functions with library name:
- i.e. `accl_tilt();`

3.for global variables, affix g_ to the front of the variable, and label the type with f,u8,ch etc.
- i.e. `g_u8ar_ccd_pixel` would mean it is a global variable with type u8 array called pixel, related to the ccd library,
- this allows code to be self documenting and easier to read

4.With the exception of special deployment scenarios, (i.e. pintest), refrain from defining functions inside of main.c, 
- increase readability. 
- try to place them in their own .h & .c files