import cv2
#import cv2.typing as cvt
import numpy as np
import numpy.typing as npt

def find_red(frame:npt.NDArray,debug_image:bool=False)->npt.NDArray:
    img_mask = cv2.inRange(frame, np.array([0,0,125,0]), np.array([100,100,255,255]))
    if debug_image:
        cimg = frame.copy()
        cimg[img_mask != 0] = cimg[len(cimg)//2][len(cimg[0])//2]
        cv2.imshow("red",cimg)
    return img_mask