/*
 * pm100dx_1.h
 *
 *  Created on: May 14, 2015
 *      Author: gabriel
 */

#ifndef PM100DX_1_H_
#define PM100DX_1_H_

#include "can.h"
#include "Config.h"

#define PM100DX_CAN_BASE_ID 0x0A0
#define PM100DX_SENT_PARAM_ID   0x01C - 0x0A0 + PM100DX_CAN_BASE_ID
#define PM100DX_RESP_PARAM_ID   0x02C - 0x0A0 + PM100DX_CAN_BASE_ID

void pm100dx_init();

void pm100dx_can_tx(uint32_t id, char data[]);
void pm100dx_can_read(uint32_t id, char data[]);
float pm100dx_getMaxControllerT();
void pm100dx_set_torque(float torque, int dir, int enable, int discharge);
uint8_t pm100dx_set_voltage_limits(float dc_overvoltage, float dc_undervoltage);
uint8_t pm100dx_set_temperature_limits(float inverter_over_temp,
		float motor_over_temp);
uint8_t pm100dx_set_current_limits(float iq_limit, float id_limit);
uint8_t pm100dx_set_torque_config(float motor_limit, float regen_limit,
		float brake_limit, float zero_torque_temp, float full_torque_temp);
uint8_t pm100dx_set_torque_pid(float kp_torque, float ki_torque,
		float kd_torque, float kip_torque, float torque_rate_limit);

void pm100dx_command(int address, int p_data[], int rw);
void pm100dx_clear_fault();

float pm100dx_read_temperature(char data[]);
float pm100dx_read_LV(char data[]);
float pm100dx_read_Torque(char data[]);
float pm100dx_read_HV(char data[]);
float pm100dx_read_Current(char data[]);
float pm100dx_read_AngVel(char data[]);
float pm100dx_read_Power(char data[]);

#endif /* PM100DX_1_H_ */
