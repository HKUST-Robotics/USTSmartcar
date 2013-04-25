/* ************************************************************************
 * HKUST Smartcar 2013 Sensor Group
 * Pin Test
 *
 * ------ README ------
 * Use PTE11 (pin 14) as a testing pin
 * *************************************************************************/

#include "include.h"

volatile unsigned int systemclock=0;             // systemclock
volatile int SI_state_flag=0;                    // SI flag mode
volatile int sampling_state_flag=0;              // Sample flag mode

void pin_test();
void pintest(PORTx, u16);
void pintest_adc(ADCn, ADC_Ch, u16);
u16 vcc_gnd(u16, u16, u16);

void main(void) {
	u8 portAno, portBno, portCno, portDno, portEno;
	u16 pinNo = 1;

	uart_init(UART3, 115200);
	delayms(1000);
	gpio_init(PORTE, 11, GPO, 1);

	printf("           HKUST Smart Car(2013)\n");
	printf("         Pin Test for Light Sensor\n");
	printf("------------- Pin Test Starts -------------\n");

	for (portEno = 0; pinNo < 50; portEno++, pinNo++) {
		switch(portEno) {
		case 4: // Skip pin 5 & 6
			pinNo = vcc_gnd(5, 6, pinNo);
			break;
		case 11:
			printf("Testing Pin 14: This is the testing pin -- Skipped\n");
			++portEno;
			++pinNo;
			break;
		case 13:
			pinNo = vcc_gnd(16, 17, pinNo);
			printf("Testing Pin 18: GND -- Skipped\n"); //		Skip pin 18
			printf("Testing Pin 19: USB0R_DP -- Skipped\n"); //	Skip pin 19
			printf("Testing Pin 20: USB0R_DM -- Skipped\n"); //	Skip pin 20
			printf("Testing Pin 21: Unused -- Skipped\n"); //	Skip pin 21
			printf("Testing Pin 22: Unused -- Skipped\n"); //	Skip pin 22
			pinNo += 5;

			gpio_init(PORTE, 11, GPO, 0);
			pintest_adc(ADC0, DAD1, pinNo++); // pin 23
			pintest_adc(ADC0, AD20, pinNo++); // pin 24
			pintest_adc(ADC1, DAD1, pinNo++); // pin 25
			pintest_adc(ADC1, AD20, pinNo++); // pin 26
			pintest_adc(ADC0, DAD0, pinNo++); // pin 27
			pintest_adc(ADC0, AD19, pinNo++); // pin 28
			pintest_adc(ADC0, DAD3, pinNo++); // pin 29
			pintest_adc(ADC1, AD19, pinNo++); // pin 30
			printf("Testing Pin 31: A3V3 -- Skipped\n"); //		Skip pin 31
			++pinNo;
			pintest_adc(ADC0, AD29, pinNo++); // pin 32
			pintest_adc(ADC0, AD30, pinNo++); // pin 33
			printf("Testing Pin 34: GND -- Skipped\n"); //		Skip pin 34
			++pinNo;
			pintest_adc(ADC1, AD16, pinNo++); // pin 35
			pintest_adc(ADC0, AD16, pinNo++); // pin 36
			pintest_adc(ADC1, AD18, pinNo++); // pin 37
			pintest_adc(ADC0, AD23, pinNo++); // pin 38
			pintest_adc(ADC1, AD23, pinNo++); // pin 39
			gpio_init(PORTE, 11, GPO, 1);

			printf("Testing Pin 40: Unused -- Skipped\n"); //	Skip pin 40
			printf("Testing Pin 41: Unused -- Skipped\n"); //	Skip pin 41
			printf("Testing Pin 42: VBAT -- Skipped\n"); //		Skip pin 42
			pinNo += 3;
			pinNo = vcc_gnd(43, 44, pinNo);
			portEno = 24;
		}

		gpio_init(PORTE, portEno, GPI, 0);
		printf("Testing Pin %u: ", pinNo);
		pintest(PORTE, portEno);
	}

	for (portAno = 0; pinNo < 81; portAno++, pinNo++) {
		switch(portAno) {
		case 6:
			pinNo = vcc_gnd(56, 57, pinNo);
			break;
		case 18:
			pinNo = vcc_gnd(70, 71, pinNo);
			break;
		case 20:
			printf("Testing Pin 74: Reset -- Skipped\n"); //	Skip pin 74
			++pinNo;
			portAno = 24;
		}

		gpio_init(PORTA, portAno, GPI, 0);
		printf("Testing Pin %u: ", pinNo);
		pintest(PORTA, portAno);
	}

	for (portBno = 0; pinNo < 103; portBno++, pinNo++) {
		if (portBno == 12) {
			pinNo = vcc_gnd(94, 93, pinNo);
			portBno = 16;
		}

		gpio_init(PORTB, portBno, GPI, 0);
		printf("Testing Pin %u: ", pinNo);
		pintest(PORTB, portBno);
	}

	for (portCno = 0; pinNo < 127; portCno++, pinNo++) {
		switch(portCno) {
		case 4:
			pinNo = vcc_gnd(108, 107, pinNo);
			break;
		case 16:
			pinNo = vcc_gnd(122, 121, pinNo);
		}

		gpio_init(PORTC, portCno, GPI, 0);
		printf("Testing Pin %u: ", pinNo);
		pintest(PORTC, portCno);
	}

	for (portDno = 0; pinNo < 145; portDno++, pinNo++) {
		if (portDno == 7)
			pinNo = vcc_gnd(135, 134, pinNo);

		gpio_init(PORTD, portDno, GPI, 0);
		printf("Testing Pin %u: ", pinNo);
		pintest(PORTD, portDno);
	}

	if (pinNo == 145)
		printf("------------ Pin Test Finished ------------\n");
	else
		printf("Pin Test doesn't finish!!\n");

	return ;
}

void pintest(PORTx portX, u16 no) {
	u32 i;
	u8 isWork = 0;

	for (i = 0; 1; i++) {
		if (isWork == 1)
			break;

		if (i == 1000000) {
			printf(". ");
			i = 0;
		}

		isWork = gpio_get(portX, no);
	}

	if (isWork == 1)
		printf("-- Successful\n");
	else
		printf("-- Failed\n");

	return ;
}

void pintest_adc(ADCn n, ADC_Ch ch, u16 no) {
	u16 data;
	u32 i;
	u8 isWork = 0;

	printf("Testing Pin %u: ", no);
	for (i = 0; 1; i++) {
		data = ad_once(n, ch, ADC_8bit);
		if (data < 10) {
			isWork = 1;
			break;
		}

		if (i == 1000000) {
			printf(". ");
			i = 0;
		}
	}

	if (isWork == 1)
		printf("-- Successful\n");
	else
		printf("-- Failed\n");

	return ;
}

u16 vcc_gnd(u16 vcc, u16 gnd, u16 no) {
	printf("Testing Pin %u: VCC_33V -- Skipped\n", vcc);
	printf("Testing Pin %u: GND -- Skipped\n", gnd);
	return no + 2;
}
