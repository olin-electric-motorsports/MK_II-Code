/*
 * CAN.cpp
 *
 *  Created on: Dec 7, 2014
 *      Author: gabriel
 */

#include "CAN.h"
#include "SystemClock.h"

#define SLAVE_ADDRESS 0 // can be from 0-255, will only put the data in buffer if its for this slave
#define CAN_BUFFER_SIZE 20

struct Buffer {
	char data[CAN_BUFFER_SIZE][8];
	uint32_t id[CAN_BUFFER_SIZE];
	uint8_t readpos;
	uint8_t writepos;
	uint8_t available;
};

struct Buffer rxbuf;

//***** CAN ialization *****************************************************
void can_init(int baud) {

	CANGCON = (1 << SWRES);   // Software reset

	//MOB0 FOR TX DATA OR REMOTE
	CANPAGE = (0 << MOBNB0);     	// Selects Message Object 0
	CANCDMOB = 0x00;
	CANSTMOB = 0x00;     		// Clear mob status register;

	//MOB1 FOR TX DATA OR REMOTE
	CANPAGE = (1 << MOBNB0);     	// Selects Message Object 0
	CANCDMOB = 0x00;
	CANSTMOB = 0x00;     		// Clear mob status register;

	//DISABLE THE REST, will be RX
	for (int8_t mob = 2; mob < 6; mob++) {
		CANPAGE = (mob << MOBNB0);     	// Selects Message Object 2-5
		CANCDMOB = 0x00;       		// Disable mob
		CANSTMOB = 0x00;     		// Clear mob status register;
	}

	if (baud == 1000) {
		CANBT1 = 0x02;      // Set baud rate to 1000kb (assuming 16Mhz IOclk)
		CANBT2 = 0x04;
		CANBT3 = 0x13;
	} else if (baud == 500) {
		CANBT1 = 0x02;      	// Set baud rate to 500kb (assuming 16Mhz IOclk)
		CANBT2 = 0x0c;
		CANBT3 = 0x37;
	} else if (baud == 250) {
		CANBT1 = 0x06;      	// Set baud rate to 250kb (assuming 16Mhz IOclk)
		CANBT2 = 0x0c;
		CANBT3 = 0x37;
	}

	CANGIE = (1 << ENIT) | (1 << ENRX);   // Enable interrupts on receive
	CANIE2 = (1 << IEMOB2) | (1 << IEMOB3) | (1 << IEMOB4) | (1 << IEMOB5);

	CANGCON |= (1 << ENASTB); // Enable mode. CAN channel enters in enable mode once 11 recessive bits have been read
	rxbuf.readpos = 0;
	rxbuf.writepos = 0;
	rxbuf.available = 0;
}

//THIS IS ONLY GOOD FOR 11 bit ID
void can_add_rx_filter(uint32_t id, uint32_t msk, uint8_t mob) {
	CANPAGE = (mob << MOBNB0);     	// Selects Message Object 0-5
	CANCDMOB = (2 << CONMOB0) | (8 << DLC0); //expect RX with 8 bitS
	CANSTMOB = (0 << IDE);     		// Clear mob status register;
	CANIDT4 = 0; //EXPECT only 11 bit
	CANIDT3 = 0; //EXPECT RTR AND RB0TAG = 0
	CANIDT2 = (id << 5); // Expect this id
	CANIDT1 = (id >> 3); // Expect this id

	CANIDM4 = (1 << 0); //Accept any RTR but only 11 bit
	CANIDM3 = 0; //
	CANIDM2 = (msk << 5); //ACCEPT ONLY ADDRESS WITH MSK =1
	CANIDM1 = (msk >> 3); //ACCEPT ONLY ADDRESS WITH MSK =1
}

uint8_t can_tx(uint32_t id, char data[], uint8_t length, uint8_t vb,
		uint8_t RTR) {

	if (can_isstarted()) {

		uint8_t toggle = 0;
		while (toggle == 0) {
			switch (CANEN2 & 3) {
			case 0:
				CANPAGE = (0 << MOBNB0);
				toggle = 1;
				break;
			case 1:
				CANPAGE = (1 << MOBNB0);
				toggle = 1;
				break;
			case 2:
				CANPAGE = (0 << MOBNB0);
				toggle = 1;
				break;
			case 3:
				return 0;
				break;
			}

		}

		CANSTMOB = 0x00;    	// Clear mob status register

		//SET THE CAN ID
		if (vb) {
			CANIDT4 = id << 3 | RTR << 2;
			CANIDT3 = id >> 5;
			CANIDT2 = id >> 13;
			CANIDT1 = id >> 21;
		} else {
			CANIDT4 = RTR << 2;
			CANIDT3 = 0x00;
			CANIDT2 = id << 5;
			CANIDT1 = id >> 3;
		}

		//SET THE DATA IN THE REGISTER
		for (int i = 0; i < 8; i++) {
			if (i < length)
				CANMSG = data[i];
			else
				CANMSG = 0x00;
		}

		CANCDMOB = (1 << CONMOB0) | (vb << IDE) | (length << DLC0);
		uint32_t timeout = sysc_Uptimems();
		while (!(CANSTMOB & (1 << TXOK)))
			if (sysc_Uptimems() - timeout > 100) {
				CANCDMOB = 0x00;
				return 0;
			};	// wait for TXOK flag set

		CANCDMOB = 0x00;	// Disable Transmission
		CANSTMOB = 0x00;	// Clear TXOK flag
		return 1;

	}
	return 0;
}

uint8_t can_rx(char data[], uint32_t * id) {
	if (can_available()) {
		rxbuf.available--;
		//put the data in and verify if there is data actually, put 0 when no data transmitted
		for (int i = 0; i < 8; i++)
			data[i] = rxbuf.data[rxbuf.readpos][i];
		*id = rxbuf.id[rxbuf.readpos];
		rxbuf.readpos++;
		if (rxbuf.readpos >= CAN_BUFFER_SIZE)
			rxbuf.readpos = 0;
		return 1;
	}
	return 0;
}

uint8_t can_isstarted() {
	return ((CANGSTA & (1 << ENFG)) >> ENFG);
}

uint8_t can_available() {
	return rxbuf.available;
}

ISR( CAN_INT_vect) {
	char canpage = CANPAGE;
	while ((CANPAGE = CANHPMOB) < 0xF0) {
		if (((CANPAGE >> MOBNB0) >= 2 && (CANPAGE >> MOBNB0) <= 5)) { //if it is the rx mob
			//PUT THE data in buffer
			for (int i = 0; i < 8; i++) {
				rxbuf.data[rxbuf.writepos][i] = CANMSG;
			}
			//GET THE ID IN BUFFER
			if (CANCDMOB & (1 << IDE)) {
				//if it is 29 bit
				rxbuf.id[rxbuf.writepos] = ((uint32_t) CANIDT1 << 21)
						| ((uint32_t) CANIDT2 << 13) | ((uint32_t) CANIDT3 << 5)
						| ((uint32_t) CANIDT4 >> 3);
			} else {
				//if it is 11 bit
				rxbuf.id[rxbuf.writepos] = (CANIDT1 << 3) | (CANIDT2 >> 5);
			}
			//INCREMEENT BUFFER
			rxbuf.writepos++;
			rxbuf.available++;
			if (rxbuf.writepos >= CAN_BUFFER_SIZE)
				rxbuf.writepos = 0;
			CANSTMOB = 0; //CLEAR flag
			CANCDMOB = (2 << CONMOB0) | (8 << DLC0); //expect RX with 8 bitS
		} else
			//if it is not the rx mob
			CANSTMOB = 0; //CLEAR flag
		break;
	}
	CANPAGE = canpage;
	CANGIT = 0b11111111;
}
