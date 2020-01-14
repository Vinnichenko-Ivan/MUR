#include <murAPI.hpp>
#include "Robo.h"
#include "Camera.h"
#include <iostream>
cv::Scalar redMin = cv::Scalar(0,186,0);
cv::Scalar redMax = cv::Scalar(255,255,255);
cv::Scalar orangeMin = cv::Scalar(5,163,156);
cv::Scalar orangeMax = cv::Scalar(75,197,255);
cv::Scalar whiteMin = cv::Scalar(78,0,0);
cv::Scalar whiteMax = cv::Scalar(255,138,255);
cv::Scalar yelowMin = cv::Scalar(163,0,147);
cv::Scalar yelowMax = cv::Scalar(255,255,255);
cv::Scalar blackMin = cv::Scalar(0,0,147);
cv::Scalar blackMax = cv::Scalar(34,142,172);
const bool binaryMode = 0;
int main()
{
    int currentMission = 0;
    bool search = 0;
    Robo robo;
    Camera camBottom(1,1);
    Camera camFront(2);
    Timer time;
    int currentYaw = mur.getYaw();
    time.start();
    float currentDepth = mur.getInputAOne();
    if(binaryMode)
    {
        camBottom.setupBinary(1);
    }
    else
    {
        while(1)
        {
            if(currentMission == -1)
            {                
               
            }
            else if(currentMission == 0)
            {
                camBottom.setBinary(orangeMin, orangeMax);
                robo.go(currentYaw, 30);
                robo.setDepth(currentDepth);
                if(camBottom.searchNFigure(4))
                {
                    while(abs(camBottom.targetAngle) > 5)
                    {
                        robo.go(robo.getAngle(camBottom.targetAngle), 0);
                        camBottom.searchNFigure(4);
                    }
                    currentYaw = robo.getAngle(camBottom.targetAngle);
                    camFront.setBinary(redMin, redMax);
                    currentMission++;
                }
            }
            else if(currentMission == 1)
            {
                robo.go(currentYaw, 30);
                robo.setDepth(currentDepth);
                if(camFront.searchMaxContour())
                {
                    currentMission++;
                }
            }
            else if(currentMission == 2)
            {
                if(camFront.searchMaxContour())
                {
                     if(camFront.currentArea < (camFront.xSize * camFront.ySize)/8)
                     {
                         robo.powerGo(camFront.xDelta() * 0.5, camFront.xSize, 30);
                         robo.powerSetDepth(camFront.yDelta() * 0.5, camFront.ySize);
                     }
                     else
                     {
                         
                         mur.setPortA(0);
                         mur.setPortB(0);
                         currentMission++;
                         currentDepth -= 40;
                         while(abs(robo.getdepth()-currentDepth) > 5)
                            robo.setDepth(currentDepth);
                     }
                     time.start();
                }
                else if(time.elapsed() > 500)
                {
                    currentMission++;
                    mur.setPortA(0);
                    mur.setPortB(0);
                    currentMission++;
                    currentDepth -= 40;
                    while(abs(robo.getdepth()-currentDepth) > 5)
                        robo.setDepth(currentDepth);
                }
                    
                
            }
            else if(currentMission == 3)
            {
                camBottom.setBinary(orangeMin, orangeMax);
                robo.go(currentYaw, 30);
                robo.setDepth(currentDepth);
                if(camBottom.searchNFigure(4))
                {
                    if(search == 0)
                    {
                        time.start();
                        search = 1;
                    }
                    if(search==1 && time.elapsed() >500)
                    {
                                                            std::cout<<"heee"<<std::endl;

                    while(abs(camBottom.targetAngle) > 5)
                    {
                        robo.go(robo.getAngle(camBottom.targetAngle), 0);
                        camBottom.searchNFigure(4);
                        
                    }
                    currentYaw = robo.getAngle(camBottom.targetAngle);
                    camFront.setBinary(redMin, redMax);
                    currentMission++;
                    }
                }
                else
                {
                    search = 0;
                    time.start();
                }
            }
            else if(currentMission == 4)
            {
                camBottom.setBinary(blackMin, blackMax);
                robo.go(currentYaw, 30);
                robo.setDepth(currentDepth);
                if(camBottom.searchNFigure(3))
                {
                    mur.setPortA(0);
                    mur.setPortB(0);
                    while(robo.getdepth() >5)
                        robo.setDepth(0);
                    break;
                }
            }
            
            
            
 
        }
    }
}