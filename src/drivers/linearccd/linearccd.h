/*******************************************

linearccd.h

Authored by Louis Mo
for HKUST SmartCar team 2013

Edited & compiled by John Ching

*******************************************/

/*********** CCD basic library ************/
void ccd_sampling(char array[], int state);
// ccd main loop

void ccd_clock_turn(); 
// ccd clock

void ccd_trigger_SI();
// trigger SI to high

void ccd_detect_track(char array[]); 
// reads from AV pin: 0=white 1=black
    
void ccd_SI_failing_edge_condition();
// check SI condition
   
void ccd_finish_one_sampling(char array[]);
// called when sampling is complete
    
void ccd_output_sample_to_UART(char array[]); 
// print the sample result to UART

void ccd_shift_sample_to_manageable_position(char array[]);
// shift sample position

void ccd_scan_all_white_or_all_black_sample(char array[]);
// scan while or black sample

void ccd_print(char array[]); 
// function prints out sample array to bluetooth

void ccd_compressed_print(char array[]);
// prints out compressed sample to bluetooth

/*********** CCD Direction PID decision ************/
void ccd_recongize_left_right_edge_and_return_dir_error(char array[]);
// algorithm 2: analyze ccd sample and return dir error to system loop

void calculate_two_edge_middle_distance(char array[]);
// return edge distance

/*********** CCD sample filtering ************/
void ccd_sample_filtering(char array[]);
// checking similarity with previous sample to sample