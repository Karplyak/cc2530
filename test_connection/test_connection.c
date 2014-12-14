#include "hal_cc2530.h"
#include <stdio.h>

/* PACKET_LENGTH must be < 128-4=124*/
#define PACKET_LENGTH 100
#define TIMEOUT 1 << 20

enum Error {
	OK,
	ERR_TIMEOUT,
	ERR_DATA_CORRUPT
};

void send_packet(void)
{
	unsigned char i;
	printf("waiting TX ready\n\r");
	while (FSMSTAT1 & 0x22)
		__asm__("NOP");
	printf("send start\n\r");
	hal_cmd2rf(CSP_ISFLUSHTX);
	RFD = PACKET_LENGTH;
	for(i = 0; i < PACKET_LENGTH; i++)
		RFD = i; 
	hal_cmd2rf(CSP_ISTXON);
	/* wait TX finished */
	while(!(RFIRQF1 & 0x02)) {
		printf("busy transmitting\n\r");
	}
	RFIRQF1 = ~0x02;
	printf("send finished\n\r");
}

void recv_packet_irs(void) __interrupt(RF_VECTOR)
{
	volatile char data;
	printf("YRA RFIRQF0 = 0x%X RFIRQF1=0x%X\n\r", RFIRQF0, RFIRQF1);
	data = RXD;
}

enum Error recv_packet_timeout(void)
{
	unsigned int i;
	printf("recv start\n\r");
//	hal_cmd2rf(CSP_ISFLUSHRX);
	i = 1 << 15;
	while(1) {
		// printf("FSMSTAT0(FSM) = 0x%X FSMSTAT1 = 0x%X\n\r", FSMSTAT0, FSMSTAT1);
		// printf("RSSI = %X RSSISTAT = %X\n\r", RSSI, RSSISTAT);
		if (RFIRQF0 & 0x60) {
			printf("YRA!!!\n\r");
			RFIRQF0 = ~0x40;
			break;
		}
		if (0 == i--) {
			printf("timeout\n\r");
			return ERR_TIMEOUT;
		}
	}
	printf("recv finished\n\r");
	return OK;
}

void show_status(void)
{
	printf("FSMSTAT0(FSM) = 0x%X FSMSTAT1 = 0x%X\n\r", FSMSTAT0, FSMSTAT1);
	printf("RSSI = %X RSSISTAT = %X\n\r", RSSI, RSSISTAT);
}

void main(void) {
//	enum Error err;

	hal_init();
	RFIRQM0 = 0x60;
	IEN2 = 0x01;
	EA = 1;
	hal_cmd2rf(CSP_ISFLUSHRX);
	hal_cmd2rf(CSP_ISRXON);
	FRMFILT0 = 0x0C;
	while(1) {
//		show_status();
//		hal_led_blue(1);
//		send_packet();
//		hal_led_blue(0);
//		err = recv_packet_timeout();
//		hal_led_red(OK != err);
	}
}