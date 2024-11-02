import cv2
import numpy as np
import serial
import time
from picamera2 import Picamera2

def motor(ser,x,y):
    ser.write("m:{}:{}\n".format(x,y).encode())

def main():
    # Picameraを起動
    camera = Picamera2()
    camera.configure(camera.create_preview_configuration(main={
        "format": 'XRGB8888',
        "size": (1640, 1232)
    }))
    camera.start()
    ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
    ser.flush()
    time.sleep(2)
    
    while True:
        #カメラからの画像取得
        frame = camera.capture_array()
        frame = frame[:,:1200]
        frame = cv2.resize(frame,None,fx=0.5,fy=0.5)
        cimg = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        cimg = cv2.equalizeHist(cimg)
        circles = cv2.HoughCircles(cimg, cv2.HOUGH_GRADIENT, dp=.5, minDist=50, param1=100, param2=60, minRadius=10, maxRadius=150)
        if circles is not None:
            circles = np.uint16(np.around(circles))
            for i in circles[0,:]:
                # draw the outer circle
                cv2.circle(cimg,(i[0],i[1]),i[2],(0,255,0),2)
                # draw the center of the circle
                cv2.circle(cimg,(i[0],i[1]),2,(0,0,255),3)
                print("({},{}) r={}".format(i[0],i[1],i[2]))
                x = 50
                y = 50
                x += (i[1] - 320) / 10
                y -= (i[1] - 320) / 10
                motor(ser,x,y)
                print("m:{}:{}".format(x,y))
        else:
            motor(ser,0,0)
            print("m:0:0")

        cv2.imshow('camera' , cimg)

        key =cv2.waitKey(10)
        if key == 27:
            break

    camera.stop()
    ser.close()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
