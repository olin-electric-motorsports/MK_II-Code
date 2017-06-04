/*
 * powertrain_logic.h
 *
 *  Created on: May 15, 2015
 *      Author: gabriel
 */

#ifndef POWERTRAIN_LOGIC_H_
#define POWERTRAIN_LOGIC_H_

#include "global_var.h"
#include "pm100dx.h"

void powertrain_send_dummy_can();
void powertrain_start_motor();
void powertrain_stop_motor();
void powertrain_verify_timeout();
void powertrain_send_torque(float torque,int isrev);
#endif /* POWERTRAIN_LOGIC_H_ */
