import cv2
import time
from picamera2 import Picamera2

import servo
import comn
import find_color
import circle

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

    while True:
        frame = camera.capture_array()
        frame = frame[:,:1000]
        frame = cv2.resize(frame,None,fx=0.5,fy=0.5)

        red = find_color.find_red(frame)
        print(red.sum())

        circles = circle.find_circle(frame)
        for i in circles:
            if len(circles[0]) > 4:
                comn.motor(ser,0,0,debug=True)
            else:
                for i in circles[0,:]:
                    x = 50
                    y = 50
                    x += (i[1] - 320) / 30
                    y -= (i[1] - 320) / 30
                    comn.motor(ser,x,y,debug=True)
    
        key =cv2.waitKey(10)
        if key == 27:
            break
    
    
    camera.stop()
    ser.close()
    cv2.destroyAllWindows()