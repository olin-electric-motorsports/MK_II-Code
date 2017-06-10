import math

#Reference: https://en.wikipedia.org/wiki/Thermistor#B_or_.CE.B2_parameter_equation
def voltage_to_temp(voltage):
    beta = 3950000.0
    R_0 = 100000.0
    R_top = 100000.0
    T_0 = 298 + 25
    R = (5.0 * R_top / voltage) - R_top
    temp = beta / (math.log(R) - math.log(R_0) + (beta / T_0)) - 298
    return temp

if __name__ == '__main__':
    # can = CAN('/dev/cu.usbmodem14111')
    # can.run()
    voltages = [1.5345, 1.5456, 1.5454, 1.5406, 1.5414, 1.5434, 1.5547, 1.5527, 1.5454, 1.5445, 1.5554]
    temps = [voltage_to_temp(voltage) for voltage in voltages]
    print(temps)
