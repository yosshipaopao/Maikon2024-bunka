import pigpio

PINS = [14,15,23,24]

def setup()->pigpio.pi:
    pi = pigpio.pi()
    return pi

def set_angle(pi:pigpio.pi,pin:int,angle:int)->None:
    pulse_width = (angle / 180) * (2500 - 500) + 500
    pi.set_servo_pulsewidth(pin, pulse_width)

# 110 -> 0
def move_catcher(pi:pigpio.pi,angle:int)->None:
    set_angle(pi,PINS[0],angle)
    set_angle(pi,PINS[1],180-angle)

# 180 -> 80
def move_holder(pi:pigpio.pi,angle:int)->None:
    set_angle(pi,PINS[2],angle)
    set_angle(pi,PINS[3],180-angle)
