
#поддержка глубины курса и скорости
import pymurapi as mur
from time import sleep
auv=mur.mur_init()
dephtG=20
azimutT=0

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
    
while(1):
    depht(1,0,0)
    sleep(0.1)
    
