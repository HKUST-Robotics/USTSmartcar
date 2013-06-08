/*******************************************

linearccd.h

Authored by Louis Mo
for HKUST SmartCar team 2013

Edited & compiled by John Ching

*******************************************/

void ccd_print(char array[]); // function prints out length128 array to bluetooth
void ccd_output_sample(); // print the sample result
void ccd_clock_turn(); // CCD clock

void convert_char_to_readable_integer(int intput_int, char output_char[]); // convert character into readable integer
void ccd_sampling(char mode);

//Internal use:

void ccd_hard_code_benchmark();
// hard code two benchmark arrays for sample filtering use

void ccd_save_previous_sampling();
// records previous smapling

void ccd_detect_track(); 
//reads from AV pin: 0=white 1=black

void ccd_trigger_SI();
//mode: 2=Prints debug messages to bluetooth
//      3=No notice messages
//      5=CCD Sampling with filter algorithm

//void ccd_SI_failing_edge_condition(char mode);

void ccd_monitor_SI_faling_edge();
//Starts falling edge events if conditions are met.

//void ccd_finish_one_sampling(char mode);

void ccd_finish_one_sample();
//internal function called when sampling is complete

void ccd_adjust_SI_length();
// tune SI duration

void ccd_array_position_handler();
// to synchronize array position with ccd