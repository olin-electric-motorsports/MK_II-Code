/*
 * pm100dx_1.c
 *
 *  Created on: May 14, 2015
 *      Author: gabriel
 */

#include "pm100dx.h"

volatile float pm100dx_temp_modA;
volatile float pm100dx_temp_modB;
volatile float pm100dx_temp_modC;
volatile float pm100dx_temp_gate;
volatile float pm100dx_temp_motor;
volatile float pm100dx_mot_speed;
volatile float pm100dx_current_phaseA;
volatile float pm100dx_current_phaseB;
volatile float pm100dx_current_phaseC;
volatile float pm100dx_current_phaseDC;
volatile float pm100dx_voltage_DCbus;
volatile float pm100dx_voltage_Output;
volatile float pm100dx_voltage_phaseAB;
volatile float pm100dx_voltage_phaseBC;
volatile float pm100dx_voltage_1_5V;
volatile float pm100dx_voltage_2_5V;
volatile float pm100dx_voltage_5V;
volatile float pm100dx_voltage_12V;
volatile int pm100dx_inv_state;
volatile int pm100dx_inv_run_state = 0;
volatile int pm100dx_inv_command_mode = 0;
volatile int pm100dx_inv_enable_state = 0;
volatile int pm100dx_inv_enable_lockout = 0;
volatile int pm100dx_inv_dir_command = 0;
volatile uint32_t pm100dx_fault[2] = { 0, 0 };

volatile float pm100dx_commanded_torque;
volatile float pm100dx_feedback_torque;
volatile int pm100dx_date; //stored as ddmmyyyy ( first bit is day, second is month, third and fourth is year)

void pm100dx_can_read(uint32_t id, char data[]) {
	int temp = (int) (id - PM100DX_CAN_BASE_ID);
	int year;
	int month;
	switch (temp) { //see page 13
	case 0: //Temp 1, 10hz
		pm100dx_temp_modA = pm100dx_read_temperature(&data[0]); //Module A temp
		pm100dx_temp_modB = pm100dx_read_temperature(&data[2]); //Module B temp
		pm100dx_temp_modC = pm100dx_read_temperature(&data[4]); //Module C temp
		pm100dx_temp_gate = pm100dx_read_temperature(&data[6]); //Gate Driver Temp
		break;
	case 1: // temp 2, 10hz ,disabled
		break;
	case 2: //temp 3, 10hz
		pm100dx_temp_motor = pm100dx_read_temperature(&data[4]); //Motor Temp
		break;
	case 3: //Analog Input Voltage, 100hz, disabled
		break;
	case 4: //digital input status ,100hz, disabled
		break;
	case 5: //motor position info, 100hz
		pm100dx_mot_speed = pm100dx_read_AngVel(&data[2]);
		break;
	case 6: //current info, 100hz
		pm100dx_current_phaseA = pm100dx_read_Current(&data[0]); //PHASE A
		pm100dx_current_phaseB = pm100dx_read_Current(&data[2]);
		pm100dx_current_phaseC = pm100dx_read_Current(&data[4]);
		pm100dx_current_phaseDC = pm100dx_read_Current(&data[6]);
		break;
	case 7: //voltage info, 100hz
		pm100dx_voltage_DCbus = pm100dx_read_HV(&data[0]);
		pm100dx_voltage_Output = pm100dx_read_HV(&data[2]);
		pm100dx_voltage_phaseAB = pm100dx_read_HV(&data[4]);
		pm100dx_voltage_phaseBC = pm100dx_read_HV(&data[6]);
		break;
	case 8: //flux info, 100hz , disabled
		break;
	case 9: // internal voltage, 10hz
		pm100dx_voltage_1_5V = pm100dx_read_LV(&data[0]);
		pm100dx_voltage_2_5V = pm100dx_read_LV(&data[2]);
		pm100dx_voltage_5V = pm100dx_read_LV(&data[4]);
		pm100dx_voltage_12V = pm100dx_read_LV(&data[6]);
		break;
	case 10: //internal states, 10 hz
		pm100dx_inv_state = (int) data[2]; //Inverter state
		pm100dx_inv_run_state = (int) data[4] & (1 << 0); //Inverter Run Mode
		pm100dx_inv_command_mode = data[5]; //Inverter Command Mode
		pm100dx_inv_enable_state = data[6] & (1 << 0); //Inverter Enabled
		pm100dx_inv_enable_lockout = (int) ((data[6] & (1 << 7)) >> 7); //Inverter Enable Lockout
		pm100dx_inv_dir_command = data[7]; //Direction Command
		break;
	case 11: //fault codes, 10 hz
		pm100dx_fault[0] = (uint32_t) data[0] | (uint32_t) (data[1] << 8)
				| (uint32_t) (data[2] << 16) | (uint32_t) (data[3] << 24);
		pm100dx_fault[1] = (uint32_t) (data[4]) | (uint32_t) (data[5] << 8)
				| (uint32_t) (data[6] << 16) | (uint32_t) (data[7] << 24);
		break;
	case 12: // torque & timer info, 100hz
		pm100dx_commanded_torque = pm100dx_read_Torque(&data[0]); // commanded torque
		pm100dx_feedback_torque = pm100dx_read_Torque(&data[2]); // torque feedback
		break;
	case 13: //modulation index, 100hz ,  not used
		break;
	case 14: // firmware info, 10hz
		month = (int) data[4] | (int) (data[5] << 8);
		year = ((int) (data[6]) | (int) (data[7] << 8));
		pm100dx_date = month + year;
		break;
	case 15: // diagnostic data, 100hz , not used
		break;
	case 22: //parameter answer
		break;
	}

}

//return V in degC
float pm100dx_read_temperature(char data[]) {
	return (float) ((int16_t) ((uint16_t) data[0] | (uint16_t) data[1] << 8))
			/ 10.0f;
}

//return V in volts
float pm100dx_read_LV(char data[]) {
	return (float) ((int16_t) ((uint16_t) data[0] | (uint16_t) data[1] << 8))
			/ 100.0f;
}

//return torque in Nm
float pm100dx_read_Torque(char data[]) {
	return (float) ((int16_t) ((uint16_t) data[0] | (uint16_t) data[1] << 8))
			/ 10.0f;
}

//return V in volts
float pm100dx_read_HV(char data[]) {
	return (float) ((int16_t) ((uint16_t) data[0] | (uint16_t) data[1] << 8))
			/ 10.0f;
}

//in A
float pm100dx_read_Current(char data[]) {
	return (float) ((int16_t) ((uint16_t) data[0] | (uint16_t) data[1] << 8))
			/ 10.0f;
}

//in RPM
float pm100dx_read_AngVel(char data[]) {
	return (float) ((int16_t) ((uint16_t) data[0] | (uint16_t) data[1] << 8));
}

//in kW
float pm100dx_read_Power(char data[]) {
	return (float) ((int16_t) ((uint16_t) data[0] | (uint16_t) data[1] << 8))
			/ 10.0f;
}

void pm100dx_init() {
	int data[2];
	//first disable some broadcast message we wont use
	//disable temp2 , analog input, digital input, flux
	data[0] = 0b11100101;
	data[1] = 0b11111110;
	pm100dx_command(148, data, 1);
}

float pm100dx_getMaxControllerT() {

	float temp = 0;
	if (pm100dx_temp_modA > temp)
		temp = pm100dx_temp_modA;
	if (pm100dx_temp_modB > temp)
		temp = pm100dx_temp_modB;
	if (pm100dx_temp_modC > temp)
		temp = pm100dx_temp_modC;
	if (pm100dx_temp_gate > temp)
		temp = pm100dx_temp_gate;
	return temp;
}

void pm100dx_command(int address, int p_data[], int rw) {
	char data[8];
	data[0] = (char) address; //paramater address
	data[1] = (char) address >> 8; //paramater address
	data[2] = (char) rw; //read or write
	data[3] = 0; //reserved
	data[4] = (char) p_data[0]; //data
	data[5] = (char) p_data[1]; //data
	data[6] = 0; //reserved
	data[7] = 0; //reserved
	pm100dx_can_tx(0X0C1 - 0x0A0 + PM100DX_CAN_BASE_ID, data);
}

void pm100dx_can_tx(uint32_t id, char data[]) {
	can_transmit(PWT_CAN, id, data, 8, 0, 0);
}

void pm100dx_clear_fault() {
	int data[2];
	data[0] = 0;
	data[1] = 0;
	pm100dx_command(20, data, 1);
}

void pm100dx_set_torque(float torque, int dir, int enable, int discharge) {
	char data[8];
	int16_t itorque = (int16_t) (torque * 10.0f);
	data[0] = (char) itorque;
	data[1] = (char) (itorque >> 8);
	data[2] = 0;
	data[3] = 0;
	data[4] = (char) dir;
	data[5] = (char) (enable | (discharge << 1));
	data[6] = 0;
	data[7] = 0;
	pm100dx_can_tx(0X0C0 - 0x0A0 + PM100DX_CAN_BASE_ID, data);
}
