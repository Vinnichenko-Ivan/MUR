

dephtG=20
azimutT=0
import cv2
import pymurapi as mur
auv = mur.mur_init()
import numpy as np
import math
from time import sleep

def angleLine():
    try:
        low = np.array([0,0,0])
        high = np.array([0,0,0])
        counter_max = np.array([[]], dtype = np.int32)
        max_area = 0
        orig = auv.get_image_bottom()
        img = np.copy(orig)
        img = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        high[0] = 20
        high[1] = 255
        high[2] = 255
        low[0] = 0
        low[1] = 99
        low[2] = 100
        mask = cv2.inRange(img, low, high)
        angle=0
        k = cv2.waitKey(1)
        contours, hierarchy = cv2.findContours(mask, 1, 2)
        if(len(contours)>0):
            max_area=0
            for cont in contours:
                if cv2.contourArea(cont)>max_area:
                    counter_max=cont
                    max_area = cv2.contourArea(cont)
            if(counter_max.shape[0]>0 and cv2.contourArea(counter_max)>500):
                rect = cv2.minAreaRect(counter_max)  
                box = cv2.boxPoints(rect) 
                box = np.int0(box)  
                center = (int(rect[0][0]), int(rect[0][1]))
                area = int(rect[1][0] * rect[1][1])  
                edge1 = np.int0((box[1][0] - box[0][0], box[1][1] - box[0][1]))
                edge2 = np.int0((box[2][0] - box[1][0], box[2][1] - box[1][1]))
                usedEdge = edge1
                if cv2.norm(edge2) > cv2.norm(edge1):
                    usedEdge = edge2
                reference = (1, 0)  
                angle = 180.0 / math.pi * math.acos((reference[0] * usedEdge[0] + reference[1] * usedEdge[1]) / (cv2.norm(reference) * cv2.norm(usedEdge)))
                return(angle-90)
            else:
                return(360)  
        else:
            return(360)  
    except:
        return(360)
        
def depht(depht,azimut,speed):
    kd=70
    kda=-1
    #rightMotor.setPower(((realAzimut+540-global.azimutT)%360-180) *global.kd);
	 #leftMotor.setPower(((realAzimut+540-global.azimutT)%360-180)*global.kd);
    error=auv.get_depth()-depht
    auv.set_motor_power(2,error*kd)
    auv.set_motor_power(3,error*kd)
    azimutError=(auv.get_yaw()+540-azimut)%360-180
    auv.set_motor_power(0,speed+azimutError*kda)
    auv.set_motor_power(1,speed-azimutError*kda)
sleep(1)
yawZ=auv.get_yaw()
while(angleLine()==360):
    depht(1.5,yawZ,50)
    print(angleLine())
yawZd=angleLine()
print(angleLine())
while(1):
    depht(1.5,yawZ,100)
    print(angleLine())
    
    
