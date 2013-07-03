#include "common.h"
#include "include.h"


extern unsigned char data[ROW][COLUMN];
/*-----------edge detection----------*/


/*-----------area cal----------------*/
#define MAX_LENGTH 25
#define MIDDLE COLUMN/2
int area_L = 0;
int area_R = 0;


/*-----------servo-------------------*/
#define MAX_TURN 110  //right
#define MID_TURN 70
#define MIN_TURN 30   //left
float servo_kp = 4;
float servo_kd = 3;
float servo_error = 0;
float servo_old_error = 0;
float servo_output = 0;


float area_cal(int area_L, int area_R){
	const float p_percent = 0.8;
	const float area_percent = 0.2;
	int i;
	int left_pt;
	int right_pt;
	int firstline_mid = COLUMN/2;
	int delta_p;
//	int delta_p_square;
	int delta_area;
//	int old_central = COLUMN/2;
	for(i = 0; i < MAX_LENGTH && data[i][firstline_mid] == 0; i++){
		int j;
		for(j = firstline_mid; j > 0 && data[i][j] == 0; j--)
			;
		left_pt = j;
		for(j = firstline_mid; j < COLUMN-1 && data[i][j] == 0; j++)
			;
		right_pt = j;
//		old_central = (left_pt + right_pt)/2;
		if(i == 0){
			firstline_mid = (left_pt + right_pt)/2;
			delta_p = firstline_mid - COLUMN/2;
		}
		//if start, cal the delta_p
		area_L += (firstline_mid - left_pt);
		area_R += (right_pt - firstline_mid);
	}
	// if(delta_p > 0)
	// 	delta_p_square = delta_p * delta_p;
	// else
	// 	delta_p_square = -delta_p * delta_p;
	delta_area = area_R - area_L;
	if(i == 0)
		return 0;
	else
		return (delta_p * p_percent + delta_area/i * area_percent); 
}

void servo_pid(float input){
	servo_error = input;
	servo_output = servo_kp * servo_error + servo_kd * (servo_error - servo_old_error);
	servo_old_error = servo_error;
	return;
}

void initialization(void){
 //--------motor PWM----------
    gpio_init(PORTA,10,GPO, 1);
    FTM_PWM_init (FTM1, CH1, 10000, 220);
 
 //   FTM_PWM_Duty(FTM1, CH1, 400);

 //--------servo PWM----------
    FTM_PWM_init (FTM0, CH0, 50, MID_TURN);
	//camera input
    exti_init(PORTB,8, falling_down); //HS field, falling_down
    exti_init(PORTB,9, rising_down);  //VS line, rising_down
    gpio_init(PORTB,10,GPI, 1);       // GPIN for camera signal
    //set_irq_priority(88, 1);line_duty);
	
    //set_irq_priority(89, 1);`
    
    //speed input        
    //pit_init_ms(PIT0, 3);
    //set_irq_priority(68,0);

    LED_init();
    uart_init(UART3, 115200);     // UART initialise

    gpio_init(PORTE,6,GPI,HIGH);//sw2
    gpio_init(PORTE,7,GPI,HIGH);//sw3
}

void main(void)
{
	DisableInterrupts
	initialization();
	EnableInterrupts
	while(1){
		float input = area_cal(area_L, area_R);
		if(input != 0){
		
		servo_pid(input);
		if(servo_output > 40)
			servo_output = 40;
		if(servo_output < -40)
			servo_output = -40;
		FTM_PWM_Duty(FTM0, CH0, MID_TURN + servo_output);
		
		}
	}
}