#include <murAPI.hpp>
#include <cmath>
#include <iostream>
class Robo
{
    public:
        Robo(float kpdepthArg = 5, float kidepthArg = 1, float kddepthArg = 0, float kpbalanceArg = 4, float kibalanceArg = 0, float kdbalanceArg = 0.03333);
        float getdepth();
        void setDepth(float depthToSet);
        void balance(float yaw);
        void powerSetDepth(float errorArg, float size);
        void go(float yaw);
        void powerGo(float errorArg, float size);
        void powerBalance(float errorArg, float size);
        void angleGo(float angle);
        void angleBalance(float angle);
        void stop();
        float kpdepth;
        float kddepth;
        float kidepth;
        float kpbalance;
        float kdbalance;
        float kibalance;
        void reset();
    private:
        float absoluteYaw = mur.getYaw();
        float errorDepth,lastDepthError = 0,errorDepthIntegral = 0,errorBalanceIntegral = 0, errorBalance, lastBalanceError;
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
float Robo::getdepth()
{
    filtre[2]=filtre[1]; 
    filtre[1]=filtre[0];
    filtre[0]=mur.getInputAOne(); 
    return (filtre[0]+filtre[1]+filtre[2])/3;
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
void Robo::powerSetDepth(float errorArg, float size)
{
    lastDepthError = errorDepth;
    errorDepth = (errorArg - size/2)*(100/size);
    errorDepthIntegral+=errorDepth;
    mur.setPortC(errorDepth*kpdepth);//+errorDepthIntegral*kidepth
    if(errorDepthIntegral>3000){
        errorDepthIntegral=3000;}
    if(errorDepthIntegral>-3000){
        errorDepthIntegral=-3000;}
}
void Robo::angleGo(float angle)
{
    lastBalanceError = errorBalance;
    errorBalance = angle;
    errorBalanceIntegral+=errorBalance;
    if(abs(errorBalance)<20)
    {
        errorBalance *= kpbalance;
        mur.setPortA(-90-errorBalance - (errorBalance - lastBalanceError)*kdbalance);
        mur.setPortB(-90+errorBalance + (errorBalance - lastBalanceError)*kdbalance);
    }
    else
        angleBalance(angle);
    if(errorBalanceIntegral>3000){
        errorBalanceIntegral=3000;}
    if(errorBalanceIntegral>-3000){
        errorBalanceIntegral=-3000;}
}
void Robo::angleBalance(float angle)
{
    lastBalanceError = errorBalance;
    errorBalance = angle;
    errorBalanceIntegral+=errorBalance;
    errorBalance *= kpbalance;
    mur.setPortA(-errorBalance );
    mur.setPortB(errorBalance);
    if(errorBalanceIntegral>3000){
        errorBalanceIntegral=3000;}
    if(errorBalanceIntegral>-3000){
        errorBalanceIntegral=-3000;}
}
void Robo::balance(float yaw)
{
    lastBalanceError = errorBalance;
    errorBalance = ((int)(yaw-mur.getYaw()+540)%360-180);
    errorBalanceIntegral+=errorBalance;
    errorBalance *= kpbalance;
    mur.setPortA(-errorBalance - (errorBalance - lastBalanceError)*kdbalance);
    mur.setPortB(errorBalance + (errorBalance - lastBalanceError)*kdbalance);
    if(errorBalanceIntegral>3000){
        errorBalanceIntegral=3000;}
    if(errorBalanceIntegral>-3000){
        errorBalanceIntegral=-3000;}
}
void Robo::go(float yaw)
{
    lastBalanceError = errorBalance;
    errorBalance = ((int)(yaw-mur.getYaw()+540)%360-180);
    errorBalanceIntegral+=errorBalance;
    if(abs(errorBalance)<5)
    {
        errorBalance *= kpbalance;
        mur.setPortA(-90-errorBalance - (errorBalance - lastBalanceError)*kdbalance);
        mur.setPortB(-90+errorBalance + (errorBalance - lastBalanceError)*kdbalance);
    }
    else
        balance(yaw);
    if(errorBalanceIntegral>3000){
        errorBalanceIntegral=3000;}
    if(errorBalanceIntegral>-3000){
        errorBalanceIntegral=-3000;}
}
void Robo::powerGo(float errorArg, float size)
{
    lastBalanceError = errorBalance;
    errorBalance = (errorArg - size/2)*(100/size);
    errorBalanceIntegral+=errorBalance;
    if(abs(errorBalance)<5)
    {
        errorBalance *= kpbalance;
        mur.setPortA(-90-errorBalance-(errorBalance-lastBalanceError)*kdbalance);
        mur.setPortB(-90+errorBalance+(errorBalance-lastBalanceError)*kdbalance);
    }
    else
        powerBalance(errorArg, size);
    if(errorBalanceIntegral>3000){
        errorBalanceIntegral=3000;}
    if(errorBalanceIntegral>-3000){
        errorBalanceIntegral=-3000;}
}
void Robo::powerBalance(float errorArg, float size)
{
    lastBalanceError = errorBalance;
    errorBalance = (errorArg - size/2)*(100/size);
    errorBalanceIntegral+=errorBalance;
    mur.setPortA(-errorBalance*kpbalance - (errorBalance - lastBalanceError)*kdbalance);//-errorBalanceIntegral*kibalance
    mur.setPortB(errorBalance*kpbalance + (errorBalance - lastBalanceError)*kdbalance);//+errorBalanceIntegral*kibalance
    if(errorBalanceIntegral>3000){
        errorBalanceIntegral=3000;}
    if(errorBalanceIntegral>-3000){
        errorBalanceIntegral=-3000;}
}
