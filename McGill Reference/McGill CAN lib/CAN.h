/*
 * CAN.h
 *
 *  Created on: Dec 7, 2014
 *      Author: gabriel
 */

#ifndef CAN_H_
#define CAN_H_

#include <inttypes.h>
#include <avr\io.h>
#include <stdio.h>
#include <avr/interrupt.h>
uint8_t can_tx(uint32_t id,char data[],uint8_t length,uint8_t vb,uint8_t RTR);
void can_init(int);
uint8_t can_rx(char data[],uint32_t * id);
uint8_t can_isstarted();
uint8_t can_available();
void can_add_rx_filter(uint32_t id, uint32_t msk, uint8_t mob);

#endif /* CAN_H_ */
