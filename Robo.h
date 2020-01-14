#include <murAPI.hpp>
#include <cmath>
#include <iostream>
class Robo
{
    public:
        Robo(float kpdepthArg = 2, float kidepthArg = 1, float kddepthArg = 0, float kpbalanceArg = 1, float kibalanceArg = 0, float kdbalanceArg = 0.03333);
        float getdepth();
        void setDepth(float depthToSet);
        void powerSetDepth(int, int);
        void go(int, int);
        void powerGo(int, int, int);
        void stop();
        int getAngle(int delta);
        float kpdepth;
        float kddepth;
        float kidepth;
        float kpbalance;
        float kdbalance;
        float kibalance;
        void reset();
        float errorDepth,lastDepthError = 0,errorDepthIntegral = 0,errorBalanceIntegral = 0, errorBalance, lastBalanceError;
    private:
        float absoluteYaw = mur.getYaw();
        float up;
        float filtre[3];
};  
Robo::Robo(float kpdepthArg, float kidepthArg, float kddepthArg, float kpbalanceArg, float kibalanceArg, float kdbalanceArg)
{
    kpdepth = kpdepthArg;
    kidepth = kidepthArg;
    kddepth = kddepthArg;
    kpbalance = kpbalanceArg;
    kibalance = kibalanceArg;
    kdbalance = kdbalanceArg;
}
void Robo::stop()
{
    mur.setPortA(0);
    mur.setPortB(0);
    mur.setPortC(0);
}
void Robo::reset()
{
    errorBalanceIntegral = 0;
    lastBalanceError = 0;
    errorDepthIntegral = 0;
    lastDepthError = 0;
  
}

void Robo::setDepth(float depthToSet){
    float depth = getdepth();
    lastDepthError = errorDepth;
    errorDepth = depthToSet-depth;
    errorDepthIntegral+=errorDepth;
    mur.setPortC(errorDepth*kpdepth);
    if(errorDepthIntegral>3000){
        errorDepthIntegral=3000;}
    if(errorDepthIntegral>-3000){
        errorDepthIntegral=-3000;}
}
void Robo::powerSetDepth(int errorArg, int size)
{
    lastDepthError = errorDepth;
    errorDepth = errorArg;
    errorDepthIntegral+=errorDepth;
    mur.setPortC(errorDepth*kpdepth);//+errorDepthIntegral*kidepth
    if(errorDepthIntegral>3000){
        errorDepthIntegral=3000;}
    if(errorDepthIntegral>-3000){
        errorDepthIntegral=-3000;}
}

void Robo::go(int yaw, int speed)
{
    lastBalanceError = errorBalance;
    errorBalance = ((int)(yaw-mur.getYaw()+540)%360-180);
    errorBalanceIntegral+=errorBalance;
    errorBalance *= kpbalance;
    mur.setPortA(-speed-errorBalance);
    mur.setPortB(-speed+errorBalance);
    if(errorBalanceIntegral>3000){
        errorBalanceIntegral=3000;}
    if(errorBalanceIntegral>-3000){
        errorBalanceIntegral=-3000;}
}
void Robo::powerGo(int errorArg, int size, int speed)
{
    lastBalanceError = errorBalance;
    errorBalance = errorArg;
    errorBalanceIntegral+=errorBalance;
    errorBalance *= kpbalance;
    mur.setPortA(-speed-errorBalance);
    mur.setPortB(-speed+errorBalance);
    if(errorBalanceIntegral>3000){
        errorBalanceIntegral=3000;}
    if(errorBalanceIntegral>-3000){
        errorBalanceIntegral=-3000;}
}
float Robo::getdepth()
{
    filtre[2]=filtre[1]; 
    filtre[1]=filtre[0];
    filtre[0]=mur.getInputAOne(); 
    return (filtre[0]+filtre[1]+filtre[2])/3;
}
int Robo::getAngle(int delta)
{
    if(mur.getYaw() + delta > 360)
        return mur.getYaw() + delta - 360;
    else if(mur.getYaw() + delta < 0)
        return mur.getYaw() + delta + 360;  
    else 
        return mur.getYaw() + delta;
}
