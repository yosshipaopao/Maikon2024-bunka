import serial

ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
ser.flush()

while True:
    s = input()
    ser.write(s.encode())
