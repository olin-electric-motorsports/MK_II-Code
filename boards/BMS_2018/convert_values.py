import serial
import csv
import math

def voltage_to_temp(voltage):
    beta = 3950.0
    R_0 = 100000.0
    R_top = 100000.0
    T_0 = 273 + 25
    #R = (self.vref2 * R_top / voltage) - R_top
    vref2 = 30000
    try:
        R = voltage * R_top/(vref2 - voltage)
    except:
        return 0.0
    if R < 0: #can't take the log of a negative number
        R = 0.0001

    #print "Vref2: " + str(self.vref2) + " V: "+str(voltage)+" R: "+str(R)
    temp = beta / (math.log(R) - math.log(R_0) + (beta / T_0)) - 273
    return temp

def voltage_to_voltage(voltage):
    return voltage / 10000

if __name__ == "__main__":
    ser = serial.Serial('/dev/ttyUSB0')

    while(True):
        vals = ser.readline()
        string = "".join([ chr(i) for i in vals]).strip()

        kind = string[0]
        cell = string[1]
        err = string[3]
        string = string[4:].split(',')

        if kind == 't':
            print("Temperatures: {} [{}] ".format(cell, err))

            for i in string[1:-1]:
                val = voltage_to_temp(int(i))
                print("{:.4}".format(-val), end=" ")

            print()

        elif kind == 'v':
            print("Voltages: {} [{}] ".format(cell, err))
            for i in string[1:]:
                print(voltage_to_voltage(float(i)), end=" ")

            print()

