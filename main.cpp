#include "Robo.h"
#include "Camera.h"
#include <murAPI.hpp>
const bool binarySetup = 0;
cv::Scalar redMin = cv::Scalar(0,171,80);
cv::Scalar redMax = cv::Scalar(74,255,207);
cv::Scalar redMinCircle = cv::Scalar(70,163,77);
cv::Scalar redMaxCircle = cv::Scalar(255,255,250);
cv::Scalar orangeMin = cv::Scalar(30,147,139);
cv::Scalar orangeMax = cv::Scalar(161,172,174);
cv::Scalar whiteMin = cv::Scalar(78,0,0);
cv::Scalar whiteMax = cv::Scalar(255,138,255);
cv::Scalar yelowMin = cv::Scalar(163,0,147);
cv::Scalar yelowMax = cv::Scalar(255,255,255);
float getAngle(float angle)
{
    if(angle>90)
        return angle - 180;
    else 
        return angle;
}
int main()
{
    int targetYaw;
    Timer time;
    short flag = -1;
    int currentMission = 0;
    int currentDepth;
    Robo robo(5,1,1,0.5,0,0.033);
    Camera cam2(2);
    Camera cam1(1);
    //cam2.debug = 0;
    //cam1.debug = 0;
    cam1.blurSize = cv::Size(3,3);
    cam2.blurSize = cv::Size(3,3);
    if(binarySetup)
        cam1.setupBinary(2);
    else
        while(1)
        {
            if(currentMission == -1)
            {
             
                    cam2.setBinary(yelowMin,yelowMax);
                cam2.searchMaxContour();
            }
            else if(currentMission == 0)
            {
                cam2.setBinary(redMinCircle,redMaxCircle);
                cam2.searchMaxContour();
                robo.powerSetDepth(cam2.targety, cam2.ySize);
                if(abs(cam2.targety - cam2.ySize/2) < 10)
                {
                    currentMission++;
                    currentDepth = mur.getInputAOne();
                    robo.reset();
                    std::cout<<currentDepth<<std::endl;
                }
            }
            else if(currentMission == 1)
            {
                cam2.searchMaxContour();
                if(abs(robo.getdepth() - robo.getdepth())<5)
                    robo.powerBalance(cam2.targetx, cam2.xSize);
                else
                    robo.setDepth(currentDepth);
                if(abs(cam2.targetx - cam2.xSize/2) < 2)
                {
                    currentMission++;
                    targetYaw = mur.getYaw();
                    std::cout<<targetYaw<<std::endl;
                    robo.reset();
                }
            }
            else if(currentMission == 2)
            {
                //if(abs(robo.getdepth() - robo.getdepth()) < 5)
                robo.go(targetYaw);
                robo.setDepth(currentDepth);
                cam1.setBinary(orangeMin,orangeMax);
                if(cam1.searchRectangle())
                {
                    currentMission++;
                    robo.reset();
                    while(abs(getAngle(cam1.targetAngle))>10)
                    {
                        cam1.searchRectangle();
                        robo.angleBalance(getAngle(cam1.targetAngle)); 
                    }
                }
                
            }
            else if(currentMission == 3)
            {
                if(cam1.searchRectangle())
                {
                    robo.angleGo(getAngle(cam1.targetAngle));
                    robo.setDepth(currentDepth);
                }
                else
                {
                    currentMission++;
                    targetYaw = mur.getYaw();
                    robo.reset();
                }
                
            }
            else if(currentMission == 4)
            {
                //if(abs(robo.getdepth() - robo.getdepth()) < 5)
                robo.go(targetYaw);
                robo.setDepth(currentDepth);
                cam1.setBinary(redMin,redMax);
                if(cam1.searchCircle())
                {
                    flag = 0;
                    robo.reset();
                    std::cout<<"JP"<<std::endl;
                }
                else
                {
                    cam1.setBinary(whiteMin,whiteMax);
                    if(cam1.searchRectangle())
                    {
                        flag = 1;
                        robo.reset();
                        std::cout<<"RUS"<<std::endl;
                    }
                }
                cam1.setBinary(orangeMin,orangeMax);
                if(cam1.searchRectangle())
                {
                    currentMission++;
                    robo.reset();
                    while(abs(getAngle(cam1.targetAngle))>10)
                    {
                        cam1.searchRectangle();
                        robo.angleBalance(getAngle(cam1.targetAngle)); 
                    }
                }
                
            }
            else if(currentMission == 5)
            {
                cam1.setBinary(orangeMin,orangeMax);
                if(cam1.searchRectangle())
                {
                    robo.angleGo(getAngle(cam1.targetAngle));
                    robo.setDepth(currentDepth);
                    time.start();
                }
                else if(time.elapsed() > 500)
                {
                    currentMission++;
                    targetYaw = mur.getYaw();
                    robo.reset();
                }
               
                
            }
           /* else if(currentMission == 6)
            {
                cam2.setBinary(yelowMin,yelowMax);
                cam2.searchMaxContour();
                robo.powerSetDepth(cam2.targety, cam2.ySize);
                if(abs(cam2.targety - cam2.ySize/2) < 10)
                {
                    currentMission++;
                    currentDepth = mur.getInputAOne();
                    robo.reset();
                    std::cout<<currentDepth<<std::endl;
                }
            }
            else if(currentMission == 7)
            {
                cam2.searchMaxContour();
                robo.powerBalance(cam2.targetx, cam2.xSize);
                robo.setDepth(currentDepth);
                if(abs(cam2.targetx - cam2.xSize/2) < 2)
                {
                    currentMission++;
                    targetYaw = mur.getYaw();
                    std::cout<<targetYaw<<std::endl;
                    robo.reset();
                }
            }*/
            else if(currentMission == 6)
            {
                //if(abs(robo.getdepth() - robo.getdepth()) < 5)
                robo.go(targetYaw);
                robo.setDepth(currentDepth);
                cam1.setBinary(orangeMin,orangeMax);
                if(cam1.searchRectangle())
                {
                    currentMission++;
                    robo.reset();
                    while(abs(getAngle(cam1.targetAngle))>10)
                    {
                        cam1.searchRectangle();
                        robo.angleBalance(getAngle(cam1.targetAngle)); 
                    }
                    targetYaw = mur.getYaw(); 
                }
                
            }
            else if(currentMission == 7)
            {
                //if(abs(robo.getdepth() - robo.getdepth()) < 5)
                if(flag == 0)
                    cam2.setBinary(redMinCircle,redMaxCircle);
                else if(flag == 1) 
                    cam2.setBinary(yelowMin,yelowMax);
                if(cam2.searchMaxContour() && cam2.currentArea < (cam2.xSize * cam2.xSize)/6)
                {
                    robo.powerSetDepth(cam2.targety, cam2.ySize);
                    robo.powerGo(cam2.targetx, cam2.xSize);
                }
                else
                {
                    currentMission++;
                    currentDepth = mur.getInputAOne()-50;
                }
                
            }
            else if(currentMission == 8)
            {
                cam1.setBinary(orangeMin,orangeMax);
                robo.go(targetYaw);
                robo.setDepth(currentDepth);
                cam1.setBinary(orangeMin,orangeMax);
                if(cam1.searchRectangle())
                {
                    time.start();
                }
                else if(time.elapsed() > 500)
                {
                    currentMission++;
                    robo.reset();
                    
                }
            }
            else if(currentMission == 9)
            {
                cam1.setBinary(orangeMin,orangeMax);
                robo.go(targetYaw);
                robo.setDepth(currentDepth);
                cam1.setBinary(orangeMin,orangeMax);
                if(cam1.searchRectangle())
                {
                    currentMission++;
                    robo.reset();
                    while(abs(getAngle(cam1.targetAngle))>10)
                    {
                        cam1.searchRectangle();
                        robo.angleBalance(getAngle(cam1.targetAngle)); 
                    }
                    targetYaw = mur.getYaw();
                }
            }
            else if(currentMission == 10)
            {
                cam1.setBinary(orangeMin,orangeMax);
                if(cam1.searchRectangle())
                {
                    robo.angleGo(getAngle(cam1.targetAngle));
                    robo.setDepth(currentDepth);
                    time.start();
                }
                else if(time.elapsed() > 500)
                {
                    currentMission++;
                    targetYaw = mur.getYaw();
                    robo.reset();
                }
            }
            else if(currentMission == 11)
            {
                robo.stop();
            }
           std::cout<<"Misssion: "<<currentMission<<std::endl;
        }
    return 0;
}