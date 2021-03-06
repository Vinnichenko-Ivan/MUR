dephtG=20
azimutT=0
dephtN=1.5
import cv2
import pymurapi as mur
auv = mur.mur_init()
import numpy as np
import math
from time import sleep
import time
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
        
def ellipseX():
    try:
        low = np.array([0,0,0])
        high = np.array([0,0,0])
        counter_max = np.array([[]], dtype = np.int32)
        max_area = 0
        orig = auv.get_image_bottom()
        img = np.copy(orig)
        img = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        high[0] = 40
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
            rect = cv2.minAreaRect(counter_max) 
            center = rect[0]
            return center[1]
        else:
            return(1000)  
    except:
        return(1000)
        
def ellipseY():
    try:
        low = np.array([0,0,0])
        high = np.array([0,0,0])
        counter_max = np.array([[]], dtype = np.int32)
        max_area = 0
        orig = auv.get_image_bottom()
        img = np.copy(orig)
        img = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        high[0] = 40
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
            rect = cv2.minAreaRect(counter_max) 
            center = rect[0]
            return center[0]
        else:
            return(1000)  
    except:
        return(1000)
        
        
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
def off():
    auv.set_motor_power(4,0) 
    auv.set_motor_power(3,0) 
    auv.set_motor_power(2,0) 
    auv.set_motor_power(1,0) 
    auv.set_motor_power(0,0)   
sleep(1)

yawZ=auv.get_yaw()
now = time.time()
while True:
    depht(dephtN,yawZ,0)
    if (time.time() - now) > 3:
        break
    sleep(0.05)

    
while(angleLine()==360):
    depht(1.5,yawZ,50)
    sleep(0.05)
yawZ-=angleLine() 

now = time.time()
while True:
    depht(dephtN,yawZ,0)
    
    if (time.time() - now) > 5:
        break
    sleep(0.05)
off() 

  
while(abs(160-ellipseY())>10):
    depht(dephtN,yawZ,0)
    auv.set_motor_power(4,(160-ellipseY())*0.75)  
    sleep(0.05)
off()

now = time.time()
while abs((auv.get_yaw()+540-yawZ)%360-180)<5:
    depht(dephtN,yawZ,0)
    auv.set_motor_power(4,160-ellipseY())
    if (time.time() - now) > 3:
        break
        sleep(0.05)
off()
        
now = time.time()

while True:
    depht(dephtN,yawZ,60)
    if (time.time() - now) > 5:
        break
    sleep(0.05)
off()    
    

while(angleLine()==360):
    depht(1.5,yawZ,50)
    sleep(0.05)
off()    
    
yawZ-=angleLine() 

now = time.time()
while True:
    depht(dephtN,yawZ,0)
    
    if (time.time() - now) > 5:
        break
    sleep(0.05)
off() 

  
while(abs(160-ellipseY())>10):
    depht(dephtN,yawZ,0)
    auv.set_motor_power(4,(160-ellipseY())*1.3)  
    sleep(0.05)
off()    

now = time.time()
while True:
    depht(dephtN,yawZ,70)
    if (time.time() - now) > 4:
        break
        sleep(0.05)
        
off()   
while(ellipseX()==1000):
    depht(dephtN,yawZ,50)
    
off() 
while(1):
    if(ellipseX()!=1000):
        depht(dephtN,yawZ,(125-ellipseX())*0.75)
        auv.set_motor_power(4,(160-ellipseY())*0.75)
        
    if(abs(125-ellipseX())>5 or abs(160-ellipseY())>5):
        now = time.time()
    if(abs(125-ellipseX())<5 and abs(160-ellipseY())<5 and time.time() - now>2):
        sleep(0.05)
        off()
        auv.drop()  
        sleep(1)#если сбрасываю один раз, то лагает и не сбрасывает
        auv.drop()  
        off() 
        break
    sleep(0.05)
    
while(1):
    depht(0,yawZ,0)  
    sleep(0.05)
