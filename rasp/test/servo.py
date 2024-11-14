import pigpio
import time

SERVO_PIN = 14

PINS = [14,15,23,24]

pi = pigpio.pi()
def set_angle(pin,angle):
    # 角度を500から2500のパルス幅にマッピングする
    pulse_width = (angle / 180) * (2500 - 500) + 500
    # パルス幅を設定してサーボを回転させる
    pi.set_servo_pulsewidth(pin, pulse_width)

# 使用例
while True:
    set_angle(PINS[0],110)
    set_angle(PINS[1],70)
    time.sleep(1)    
    set_angle(PINS[0],0)
    set_angle(PINS[1],180)
    time.sleep(5)
