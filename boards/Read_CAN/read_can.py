import serial
import time

class Message(object):
    def __init__(self, can_id, bytes, line):
        self.can_id = can_id
        self.bytes = bytes
        self.time = time.gmtime()
        self.line = line


class CAN(object):
    def __init__(self, port):
        self.ser = serial.Serial(port, 115200)
        self.ser.readline()
        self.message_dict = {}

    def run(self):
        while True:
            self.display()
            line = self.ser.readline()
            can_id = line[4:8]
            try: 
                message = line.split("MSG:", 1)[1]
                bytes = message.split(",")[:8]
                #if can_id == "0x16" or can_id == "0x17":
                #    print line
                if can_id in self.message_dict.keys():
                    #update message in dictionary
                    self.message_dict[can_id].time = time.gmtime()
                    self.message_dict[can_id].bytes = bytes
                    self.message_dict[can_id].line = line
                else:
                    #add message to dictionary
                    self.message_dict[can_id] = Message(can_id,bytes,line)
            except:
                print "Received unexpected message"

    def display(self):
        for key in self.message_dict:
            #v_list =  [("%0.3f" % cell.voltage)+"V "+("%0.3f" % cell.temp)+"C |" for cell in segment]
            #string = " ".join(str(x) for x in v_list)
            #print str(segment[0].segment) + ": "+("%0.3f" % segment[0].vref2)+"Vref2 |" + string

            print self.message_dict[key].line

        print "-----------------------------"




if __name__ == '__main__':

    can = CAN('/dev/cu.usbmodem14111')
    can.run()