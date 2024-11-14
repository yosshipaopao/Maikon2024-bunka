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
        frame = frame[:,:1000]
        cimg = cv2.resize(frame,None,fx=0.5,fy=0.5)
        img_mask = cv2.inRange(cimg, np.array([0,0,125,0]), np.array([100,100,255,255]))
        cimg[img_mask != 0] = cimg[len(cimg)//2][len(cimg[0])//2]
        #cimg = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        #cimg = cv2.blur(cimg,(3,3))
        #cimg = cv2.equalizeHist(cimg)


        cv2.imshow('camera' , cimg)

        key =cv2.waitKey(10)
        if key == 27:
            break

    camera.stop()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
