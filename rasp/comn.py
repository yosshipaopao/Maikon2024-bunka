import serial

def setup()->serial.Serial:
    ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
    ser.flush()
    return ser

def motor(ser:serial.Serial,x:int,y:int,debug:bool=False)->None:
    ser.write("m:{}:{}\n".format(x,y).encode())
    if debug:
        print("m:{}:{}".format(x,y))

def turn(ser,to):
    ser.write("t:{}\n".format(to))

def set_mode(ser,mode):
    ser.write("c:{}\n".format(mode))