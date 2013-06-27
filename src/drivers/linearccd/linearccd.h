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

void ccd_scan_dummy_sample_result(char array[]);
// scan the array value

void ccd_print(char array[]); 
// function prints out length128 array to bluetooth

/*********** CCD track decision ************/
void ccd_detect_current_left_right_edge_and_filter_middle_noise(char array[]);
// detect left and right edge for each sample and filter middle noise

void ccd_calculate_current_mid_error(char array[]);
// calculate current middle error

void ccd_output_edge_to_UART();
// temporary display edge output

/*********** CCD sample filtering ************/
void ccd_sample_filtering(char array[]);
// checking similarity with previous sample to sample

/*********** Supplementary function************/
void convert_char_to_readable_integer(int intput_int, char output_char[]); 
// convert character into readable integer

void ccd_save_previous_sampling(char input_array[], char output_stored_array[]);
// copy sample from input array to output array


/*********** dummy function***********
void ccd_save_benchmark_sample(char array[]);
// record benchmark

void ccd_decide_range_for_detection(char array[]);
// dynamic record detection range, will be deleted

void ccd_analyze_track_from_sample(char array[]);
// motor decision from sample, will be deleted
*/