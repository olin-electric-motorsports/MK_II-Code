import serial

class CAN(object):
	def __init__(self, port):
		self.ser = serial.Serial(port, 115200)
		self.ser.readline()

	def run(self):
		while True:
			line = self.ser.readline()
			can_id = line[4:8]
			print line


if __name__ == '__main__':
	can = CAN('/dev/cu.usbmodem14121')
    can.run()