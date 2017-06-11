/*
 * powertrain_logic.c
 *
 *  Created on: May 15, 2015
 *      Author: gabriel
 */

#include "powertrain_logic.h"
#include "sysclck.h"

#define DUMMY_TIMEOUT 300 //in ms
volatile uint32_t snapshot_motor_controller = 0;
volatile int powertrain_stop_flag = 0;

void powertrain_stop_motor() {
	powertrain_stop_flag = 1;
}

void powertrain_start_motor() {
	pm100dx_clear_fault();
	powertrain_stop_flag = 0;
	pm100dx_set_torque(0, REVERSEDIR, 0, 0);

}

void powertrain_send_dummy_can() {
	if (powertrain_stop_flag == 1) {
		if (millis() - snapshot_motor_controller > 2000) {
			powertrain_stop_flag = 0;
		}
	} else {
		pm100dx_set_torque(0, REVERSEDIR, 0, 1);
		snapshot_motor_controller = millis();
	}
}

//isrev 0 is foward, isrev 1 is reverse, torque is torque ....
void powertrain_send_torque(float torque, int isrev) {
	static int isrevlast = 0;

	if (powertrain_stop_flag == 1) {
		if (millis() - snapshot_motor_controller > 2000) {
			powertrain_stop_flag = 0;
		}
	} else {
		if (REVERSEDIR)
			isrev = !isrev;

		if (isrevlast != isrev)
			pm100dx_set_torque(0, isrevlast, 0, 1);

		if (pm100dx_inv_enable_lockout == 1 || torque == 0.0f)
			pm100dx_set_torque(0, isrev, 0, 1);
		else {
			pm100dx_set_torque(torque, isrev, 1, 1);
		}
		snapshot_motor_controller = millis();
	}

	isrevlast = isrev;

}
