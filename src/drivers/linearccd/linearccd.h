/*******************************************

linearccd.h

Authored by Louis Mo
for HKUST SmartCar team 2013

Edited & compiled by John Ching

*******************************************/


void ccd_print(char array[]); //function prints out length128 array to bluetooth
void ccd_sampling(char mode);

//Internal use:

void hard_code_benchmark();
// hard code two benchmark arrays for sample filtering use

void ccd_save_previous_sampling();
// records previous smapling

void ccd_detect_track(); 
//reads from AV pin: 0=white 1=black

void ccd_trigger_SI(char mode);
//mode: 2=Prints debug messages to bluetooth
//      3=No notice messages


void ccd_SI_failing_edge_condition();
//Starts falling edge events if conditions are met.


void ccd_finish_one_sampling(char mode);
//internal function called when sampling is complete