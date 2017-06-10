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

def voltage_to_temp(voltage):
    beta = 3950000.0
    R_0 = 100000.0
    R_top = 100000.0
    T_0 = 298 + 25
    R = (5.0 * R_top / voltage) - R_top
    temp = beta / (math.log(R) - math.log(R_0) + (beta / T_0)) - 298
    return temp


class CAN(object):

	def __init__(self, port):
		self.ser = serial.Serial(port, 115200)
		self.ser.readline()
		foo = Cell(None,None,None)
		self.battery = [[foo for j in range(12)] for i in range(6)]

	def run(self):
		while True:
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
					temp = (int(bytes[(i+1)*2],0)<<8)|int(bytes[(i+1)*2+1],0)
					if not self.battery[segment][cell_num].cell is None:
						self.battery[segment][cell_num].temp = temp
					cell_num += 1


	def display(self):
		for segment in self.battery:
			#bar = [SOME EXPRESSION for item in some_iterable]
			v_list =  [str(cell.voltage)+"V "+str(cell.temp)+"C |" for cell in segment]
			string = " ".join(str(x) for x in v_list)
			print str(segment[0].segment) + ": " + string
		print "-----------------------------"





if __name__ == '__main__':
    # can = CAN('/dev/cu.usbmodem14111')
    # can.run()

	voltages = [1.5345, 1.5456, 1.5454, 1.5406, 1.5414, 1.5434, 1.5547, 1.5527, 1.5454, 1.5445, 1.5554]

	temps = [voltage_to_temp(voltage) for voltage in voltages]
