import cv2
import numpy as np
from picamera2 import Picamera2

def main():
    # Picameraを起動
    camera = Picamera2()
    camera.configure(camera.create_preview_configuration(main={
        "format": 'XRGB8888',
        "size": (1640, 1232)
    }))
    camera.start()
    
    while True:
        #カメラからの画像取得
        frame = camera.capture_array()
        frame = frame[:,:800]
        frame = cv2.resize(frame,None,fx=0.5,fy=0.5)
        #img_mask = cv2.inRange(frame, np.array([0,0,125,0]), np.array([100,100,255,255]))
        #frame[img_mask != 0] = frame[len(frame)//2][len(frame[0])//2]
        cimg = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        cimg = cv2.blur(cimg,(3,3))
        #cimg = cv2.equalizeHist(cimg)
        circles = cv2.HoughCircles(cimg, cv2.HOUGH_GRADIENT, dp=1, minDist=50, param1=100, param2=40, minRadius=10, maxRadius=150)
        if circles is not None:
            circles = np.uint16(np.around(circles))
            for i in circles[0,:]:
                # draw the outer circle
                cv2.circle(cimg,(i[0],i[1]),i[2],(0,255,0),2)
                # draw the center of the ciarcle
                cv2.circle(cimg,(i[0],i[1]),2,(0,0,255),3)
                print("({},{}) r={}".format(i[0],i[1],i[2]))

        cv2.imshow('camera' , cimg)

        key =cv2.waitKey(10)
        if key == 27:
            break

    camera.stop()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
