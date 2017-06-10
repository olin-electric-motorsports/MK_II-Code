import math

#Reference: https://en.wikipedia.org/wiki/Thermistor#B_or_.CE.B2_parameter_equation
def voltage_to_temp(voltage):
    beta = 3950.0
    R_0 = 100000.0
    R_top = 100000.0
    T_0 = 273 + 25
    R = voltage * R_top / (3.0 - voltage)
    temp = beta / (math.log(R) - math.log(R_0) + (beta / T_0)) - 273
    return temp

def res_to_temp(res):
    beta = 3950.0
    R_0 = 100000.0
    R_top = 100000.0
    T_0 = 273 + 25
    temp = beta / (math.log(res) - math.log(R_0) + (beta / T_0)) - 273
    return temp

if __name__ == '__main__':
    # can = CAN('/dev/cu.usbmodem14111')
    # can.run()
    voltages = [1.5345, 1.5456, 1.5454, 1.5406, 1.5414, 1.5434, 1.5547, 1.5527, 1.5454, 1.5445, 1.5554]
    temps = [voltage_to_temp(voltage) for voltage in voltages]
    print(temps)

    resistances = [1787900, 554701.6, 327019.5, 199200.7, 100000, 80527, 53163, 35884.2, 24717.1]
    temps = [res_to_temp(res) for res in resistances]
    print(temps)
