import cv2
import numpy as np

def find_red(frame:cv2.typing.MatLike)->cv2.typing.MatLike:
    img_mask = cv2.inRange(frame, np.array([0,0,125,0]), np.array([100,100,255,255]))
    return img_mask