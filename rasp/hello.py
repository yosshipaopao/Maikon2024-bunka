import cv2
import numpy as np

def main():
    cap = cv2.VideoCapture(0)
    #繰り返しのためのwhile文
    while True:
        #カメラからの画像取得
        ret, frame = cap.read()
        if not ret:
            print("カメラからの画像取得に失敗")
            continue
        img = cv2.medianBlur(frame, 5)
        cimg = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        cimg = cv2.equalizeHist(cimg)
        circles = cv2.HoughCircles(cimg, cv2.HOUGH_GRADIENT, dp=1, minDist=100, param1=100, param2=70, minRadius=30, maxRadius=200)
        if circles is not None:
            circles = np.uint16(np.around(circles))
            for i in circles[0,:]:
                # draw the outer circle
                cv2.circle(img,(i[0],i[1]),i[2],(0,255,0),2)
                # draw the center of the circle
                cv2.circle(img,(i[0],i[1]),2,(0,0,255),3)
                print("({},{}) r={}".format(i[0],i[1],i[2]))
        #カメラの画像の出力
        cv2.imshow('camera' , img)

        #繰り返し分から抜けるためのif文
        key =cv2.waitKey(10)
        if key == 27:
            break
    #メモリを解放して終了するためのコマンド
    cap.release()
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
