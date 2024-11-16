import cv2
import time
from picamera2 import Picamera2
import numpy as np

import servo
import comn
import find_color
import circle

def is_linemode(ser):
    l=ser.readline()
    if l=="c:0":
        return True
    return False
def is_catchmode(ser):
    l=ser.readline()
    if l=="c:1":
        return True
    return False

def main():
    camera = Picamera2()
    camera.configure(camera.create_preview_configuration(main={
        "format": 'XRGB8888',
        "size": (1640, 1232)
    }))
    camera.start()
    ser = comn.setup()
    time.sleep(2)
    gpio = servo.setup()

    mode = False
    while True:
        if mode:
            print("mode",0)
            servo.move_catcher(gpio,0)
            servo.move_holder(gpio,180)
            while mode:
                i=0
                xs=np.array([])
                while mode:
                    if len(xs) < 4:
                        comn.turn(ser,30)
                    else:
                        mx = np.mean(xs) / 410
                        comn.turn(ser,mx*20)
                    time.sleep(1)
                    comn.motor(ser,0,0)
                    for _ in range(10):
                        frame = camera.capture_array()
                        frame = frame[:,:1000]
                        frame = cv2.resize(frame,None,fx=0.5,fy=0.5)
                        circles = circle.find_circle(frame,debug_image=True)
                        if len(circles) < 3:
                            for i in circles:
                                x = (i[1] - 410)
                                xs.append(x)
                        if is_linemode(ser):
                            mode=False
                            break
                    xs = np.array([])
                if not mode:break
                comn.motor(ser,30,30)
                time.sleep(10)
                comn.motor(ser,-30,-30)
                time.sleep(2)
                comn.motor(ser,0,0)
                servo.move_catcher(gpio,110)
                time.sleep(2)
                servo.move_catcher(gpio,0)
                time.sleep(2)

                while mode:
                    if len(xs) < 4:
                        comn.turn(ser,30)
                    else:
                        mx = np.mean(xs) / 410
                        comn.turn(ser,mx*20)
                    time.sleep(1)
                    comn.motor(ser,0,0)
                    frame = camera.capture_array()
                    frame = frame[:,:1000]
                    frame = cv2.resize(frame,None,fx=0.5,fy=0.5)
                    red = find_color.find_red(frame,debug_image=False)
                    if red.sum() > 500:    
                        break
                    else:
                        comn.turn(ser,30)
                    if is_linemode(ser):
                        mode=False
                        break
                    xs = np.array([])
                if not mode:break
                comn.motor(ser,30,30)
                time.sleep(10)
                comn.motor(ser,-30,-30)
                time.sleep(2)
                comn.motor(ser,0,0)
                comn.turn(ser,180)
                comn.motor(ser,-30,-30)
                time.sleep(2)
                servo.move_holder(gpio,80)
                time.sleep(2)
                servo.move_holder(gpio,180)
        else:
            print("mode",1)
            servo.move_catcher(gpio,180)
            if is_catchmode(ser):
                mode=True
                continue

        
    camera.stop()
    ser.close()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()