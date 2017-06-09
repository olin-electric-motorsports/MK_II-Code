import serial
import time
import math

class Cell(object):
	def __init__(self, segment, cell, voltage):
		self.segment = segment
		self.cell = cell
		self.voltage = voltage
		self.temp = 0

	def display(self):
		print "Seg: " + str(self.segment) + " Cell: " + str(self.cell) + " V: " + str(self.voltage) + " T: " + str(self.temp)

	#Reference: https://en.wikipedia.org/wiki/Thermistor#B_or_.CE.B2_parameter_equation
	def voltage_to_temp(self, voltage):
		if voltage == 0:
			return 999
		voltage = voltage / 10000.0
		beta = 3950000.0
		R_0 = 100000.0
		R_top = 100000.0
		R = (5000.0 * R_top / voltage) - R_top
		r_inf = R_0 * math.exp((-1*beta)/298.0)
		print "R: "+str(R)+" r_inf: "+str(r_inf)+ " exp: "+str((-1*beta)/298.0)
		temp = beta/(math.log(R/r_inf))
		return temp


class CAN(object):

	def __init__(self, port):
		self.ser = serial.Serial(port, 115200)
		self.ser.readline()
		foo = Cell(None,None,None)
		self.battery = [[foo for j in range(12)] for i in range(6)]

	def run(self):
		count = 0
		while True:
			count += 1
			if count >= 1:
				count = 0
				self.display()
			line = self.ser.readline()
			can_id = line[4:8]
			if (can_id == '0x13'):
				message = line.split("MSG:", 1)[1]
				bytes = message.split(",")[:8]
				segment = int(bytes[0],0)
				cell_num = int(bytes[1],0)
				for i in range(3):
					volts = (int(bytes[(i+1)*2],0)<<8)|int(bytes[(i+1)*2+1],0)
					cell = Cell(segment,cell_num,volts/10000.0)
					#cell.display()
					if self.battery[segment][cell_num].cell is None:
						self.battery[segment][cell_num] = cell
					else:
						self.battery[segment][cell_num].cell = cell_num
						self.battery[segment][cell_num].segment = segment
						self.battery[segment][cell_num].voltage = volts/10000.0
					cell_num += 1
			if (can_id == '0x14'):
				message = line.split("MSG:", 1)[1]
				bytes = message.split(",")[:8]
				segment = int(bytes[0],0)
				cell_num = int(bytes[1],0)
				for i in range(3):
					temp_volt = (int(bytes[(i+1)*2],0)<<8)|int(bytes[(i+1)*2+1],0)
					temp_volt = temp_volt/10000.0
					#print "temp_volt: " + str(temp_volt)
					if not self.battery[segment][cell_num].cell is None:
						self.battery[segment][cell_num].temp = temp_volt
					cell_num += 1


	def display(self):
		for segment in self.battery:
			#bar = [SOME EXPRESSION for item in some_iterable]
			v_list =  [str(cell.voltage)+"V "+str(cell.temp)+"CV |" for cell in segment]
			string = " ".join(str(x) for x in v_list)
			print str(segment[0].segment) + ": " + string
		print "-----------------------------"





if __name__ == '__main__':
    can = CAN('/dev/cu.usbmodem14111')
    can.run()
