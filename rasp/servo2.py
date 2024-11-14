import pigpio
import time

PINS = [14,15,23,24]

pi = pigpio.pi()
def set_angle(pin,angle):
    # 角度を500から2500のパルス幅にマッピングする
    pulse_width = (angle / 180) * (2500 - 500) + 500
    # パルス幅を設定してサーボを回転させる
    pi.set_servo_pulsewidth(pin, pulse_width)

# 使用例
while True:
    set_angle(PINS[2],180)
    set_angle(PINS[3],0)
    time.sleep(3)    
    set_angle(PINS[2],80)
    set_angle(PINS[3],100)
    time.sleep(5)
