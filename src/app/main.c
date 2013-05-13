/* ************************************************************************
 *
 * HKUST Smart Car 2013
 * Pin Test
 * By Yumi Chan
 *
 * ------ README ------
 * Use PTC0 (pin 103) as a testing pin
 *
 * *************************************************************************/

#include "include.h"

volatile unsigned int systemclock=0;             // systemclock
volatile int SI_state_flag=0;                    // SI flag mode
volatile int sampling_state_flag=0;              // Sample flag mode

/* for external use
 * just simply call this function to do pin test */
void pin_test();

/* for internal use */
void pin_test_action(PINTEST_ACT action, GPIO_CFG state);
void pintesting(PORTx portX, u16 no, GPIO_CFG CASE);
void vcc_gnd(u16 vcc, u16 gnd);

typedef enum PINTEST_ACT {
	INIT = 0,
	TEST = 1
}PINTEST_ACT;

void pin_test() {
	u8 portAno, portBno, portCno, portDno, portEno;
	u16 pinNo = 1;

	uart_init(UART3, 115200);
	delayms(1000);
	gpio_init(PORTE, 11, GPO, 1);

	printf("           HKUST Smart Car 2013\n");
	printf("                 Pin Test\n");
	printf("------------- Pin Test Starts -------------\n");

	pin_test_action(INIT, GPI_UP);
	pin_test_action(TEST, GPI_UP);
	pin_test_action(INIT, GPI_DOWN);
	pin_test_action(TEST, GPI_DOWN);

	printf("------------ Pin Test Finished ------------\n");

	return;
}

void pin_test_action(PINTEST_ACT action, GPIO_CFG state) {
	u8 portAno, portBno, portCno, portDno, portEno;
	u16 pinNo = 1;

	for (portEno = 0; pinNo < 50; ++portEno, ++pinNo) {
		switch (portEno) {
		case 4: // Skip pin 5 & 6
			vcc_gnd(5, 6);
			pinNo = pinNo + 2;
			break;
		case 13:
			vcc_gnd(16, 17);
			printf("Pin 18: GND -- Skipped\n"); // Skip pin 18
			printf("Pin 19: USB0R_DP -- Skipped\n"); // Skip pin 19
			printf("Pin 20: USB0R_DM -- Skipped\n"); // Skip pin 20
			printf("Pin 21: Unused -- Skipped\n"); // Skip pin 21
			printf("Pin 22: Unused -- Skipped\n"); // Skip pin 22
			printf("Pin 23-30 -- Skipped\n"); // Skip pin 23-30
			printf("Pin 31: A3V3 -- Skipped\n"); // Skip pin 31
			printf("Pin 32-33 -- Skipped\n"); // Skip pin 32-33
			printf("Pin 34: GND -- Skipped\n"); // Skip pin 34
			printf("Pin 35-39 -- Skipped\n"); // Skip pin 35-39
			printf("Pin 40: Unused -- Skipped\n"); // Skip pin 40
			printf("Pin 41: Unused -- Skipped\n"); // Skip pin 41
			printf("Pin 42: VBAT -- Skipped\n"); // Skip pin 42
			vcc_gnd(43, 44);
			pinNo = pinNo + 29;
			portEno = 24;
		}

		switch (action) {
		case INIT:
			gpio_init(PORTE, portEno, state, 0);
			break;
		case TEST:
			printf("Testing Pin %u: ", pinNo);
			pintesting(PORTE, portEno, state);
			break;
		}
	}

	for (portAno = 0; pinNo < 81; ++portAno, ++pinNo) {
		switch (portAno) {
		case 6:
			vcc_gnd(56, 57);
			pinNo = pinNo + 2;
			break;
		case 18:
			vcc_gnd(70, 71);
			pinNo = pinNo + 2;
			break;
		case 20:
			printf("Pin 74: Reset -- Skipped\n"); //	Skip pin 74
			++pinNo;
			portAno = 24;
		}

		switch (action) {
		case INIT:
			gpio_init(PORTA, portAno, state, 0);
			break;
		case TEST:
			printf("Testing Pin %u: ", pinNo);
			pintesting(PORTA, portAno, state);
			break;
		}
	}

	for (portBno = 0; pinNo < 103; ++portBno, ++pinNo) {
		if (portBno == 12) {
			vcc_gnd(94, 93);
			pinNo = pinNo + 2;
			portBno = 16;
		}

		switch (action) {
		case INIT:
			gpio_init(PORTB, portBno, state, 0);
			break;
		case TEST:
			printf("Testing Pin %u: ", pinNo);
			pintesting(PORTB, portBno, state);
			break;
		}
	}

	if (action == TEST) {
		printf("Pin 103: This is the testing pin -- Skipped\n");
		portCno = 1;
		++pinNo;
	} else
		portCno = 0;

	for (; pinNo < 127; ++portCno, ++pinNo) {
		switch (portCno) {
		case 4:
			vcc_gnd(108, 107);
			pinNo = pinNo + 2;
			break;
		case 16:
			vcc_gnd(122, 121);
			pinNo = pinNo + 2;
		}

		switch (action) {
		case INIT:
			gpio_init(PORTC, portCno, state, 0);
			break;
		case TEST:
			printf("Testing Pin %u: ", pinNo);
			pintesting(PORTC, portCno, state);
			break;
		}
	}

	for (portDno = 0; pinNo < 145; ++portDno, ++pinNo) {
		if (portDno == 7) {
			vcc_gnd(135, 134);
			pinNo = pinNo + 2;
		}

		switch (action) {
		case INIT:
			gpio_init(PORTD, portDno, state, 0);
			break;
		case TEST:
			printf("Testing Pin %u: ", pinNo);
			pintesting(PORTD, portDno, state);
			break;
		}
	}

	if (pinNo == 145) {
		if (action == INIT) {
			if (state == GPI_UP) {
				printf("**All pins of Port A - E are now initialized -- Pull Up\n");
			} else if (state == GPI_DOWN) {
				printf("**All pins of Port A - E are now initialized -- Pull Down\n");
			}
		} else if (action == TEST) {
			printf("**All pins of Port A - E are tested\n");
			if (state == GPI_UP) {
				printf("\nReady to initialize for 2nd test..\n");
			}
		}
	} else {
		printf("Initialization failed");
	}
}

void pintesting(PORTx portX, u16 no, GPIO_CFG CASE) {
	u32 i;
	u16 value;
	u8 isWork = 0;

	switch (CASE) {
	case GPI_UP:
		gpio_init(portX, no, GPO, 0);
		break;
	case GPI_DOWN:
		gpio_init(portX, no, GPO, 1);
	}

	for (i = 0; 1; i++) {
		if (isWork == 1)
			break;

		if (i == 3000000) {
			value = ad_once(PORTC, 0, ADC_10bit);
			printf("\nvalue: %u", value);
			i = 0;

			switch (CASE) {
			case GPI_UP:
				if (value < 100 && value > 0) {
					isWork = 1;
				}
				break;
			case GPI_DOWN:
				if (value > 900) {
					isWork = 1;
				}
				break;
			}
		}
	}

	if (isWork == 1)
		printf("-- Successful\n");
	else
		printf("-- Failed\n");

	gpio_init(portX, no, state, 0);

	return;
}

void vcc_gnd(u16 vcc, u16 gnd) {
	printf("Pin %u: VCC_33V -- Skipped\n", vcc);
	printf("Pin %u: GND -- Skipped\n", gnd);
}
