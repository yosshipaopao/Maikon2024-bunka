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

    mode = False
    if mode:
        while mode:
            i=0
            xs=np.array([])
            while True:
                if len(xs) < 4:
                    comn.turn(ser,30)
                time.sleep(1)
                for _ in range(10):
                    frame = camera.capture_array()
                    frame = frame[:,:1000]
                    frame = cv2.resize(frame,None,fx=0.5,fy=0.5)
                    circles = circle.find_circle(frame,debug_image=True)
                    if len(circles) < 3:
                        for i in circles:
                            x = 50
                            x += (i[1] - 410) / 30
                            xs.append(x)
                #comn.motor(ser,x,y,debug=True)
            

    
        key = cv2.waitKey(10)
        if key == 27:
            break
    
    
    camera.stop()
    ser.close()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
