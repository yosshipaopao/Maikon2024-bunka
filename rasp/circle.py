import cv2
import numpy as np
import numpy.typing as npt

def find_circle(frame:npt.NDArray,debug_console:bool=False,debug_image:bool=False)->npt.NDArray[np.uint16]:
    cimg = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    cimg = cv2.blur(cimg,(3,3))
    circles = cv2.HoughCircles(cimg, cv2.HOUGH_GRADIENT, dp=1, minDist=50, param1=100, param2=40, minRadius=10, maxRadius=150)
    if circles is None:
        circles = [[]]
    circles = np.uint16(np.around(circles))
    for i in circles[0,:]:
        if debug_image:
            cv2.circle(cimg,(i[0],i[1]),i[2],(0,255,0),2)
            cv2.circle(cimg,(i[0],i[1]),2,(0,0,255),3)
        if debug_console:
            print("({},{}) r={}".format(i[0],i[1],i[2]))
    if debug_image:
        cv2.imshow('camera' , cimg)
    return circles[0,:]