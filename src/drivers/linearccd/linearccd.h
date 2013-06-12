/*******************************************

linearccd.h

Authored by Louis Mo
for HKUST SmartCar team 2013

Edited & compiled by John Ching

*******************************************/

void ccd_sampling();
// ccd main loop

void ccd_clock_turn(); 
// ccd clock

void ccd_trigger_SI();
// trigger SI to high

void ccd_detect_track(); 
// reads from AV pin: 0=white 1=black
    
void ccd_SI_failing_edge_condition();
// check SI condition
   
void ccd_finish_one_sampling();
// called when sampling is complete
    
void ccd_output_sample_to_UART(); 
// print the sample result to UART

void ccd_print(char array[]); 
// function prints out length128 array to bluetooth

/************** 
Only useful when basic track detection finish

void ccd_hard_code_benchmark();
// hard code two benchmark arrays for sample filtering use

void ccd_save_previous_sampling();
// records previous smapling

void convert_char_to_readable_integer(int intput_int, char output_char[]); 
// convert character into readable integer

**************/