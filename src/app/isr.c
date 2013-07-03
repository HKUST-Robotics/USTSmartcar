#include "common.h"
#include "include.h"

#define DISTANCE 2//one line every two line

extern int area_L;
extern int area_R;

unsigned char data[ROW][COLUMN];

int cur_line = 0;
int cur_line_start = -80;
unsigned char p_counter = 0;

void PORTB_IRQHandler(void){  
//HS PTB8, field interrupt
  if(PORTB_ISFR & (1<<8)){
		PORTB_ISFR  |= (1<<8);//exit
		//DisableInterrupts;
    LED_turn(LED0);
//p_counter++;
 //   area_L = 0;
 //   area_R = 0;
		cur_line = cur_line_start;
		//EnableInterrupts;
	}

//VS PTB9, line interrupt

  if(PORTB_ISFR & (1<<9)){
    PORTB_ISFR  |= (1<<9);//exit
    cur_line++;
    if((cur_line >= 0) && (cur_line < ROW*DISTANCE) && (cur_line % DISTANCE == 0)){
      for (unsigned int delay=0; delay<620; delay++)
        ;  //get dummy 250  
      for (u8 cur_column = 0 ; cur_column < COLUMN; cur_column++)
        data[(ROW-1) - cur_line/DISTANCE][cur_column] = PTB10_IN;
    }
  }
}
