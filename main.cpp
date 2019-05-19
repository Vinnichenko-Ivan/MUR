/////////////////////////////////////////Поиск центра круга, угол на четырехугольник, бинаризация и настройка ползунками + функции поворота на угол и установка глубины/////////////////////////
#include <murAPI.hpp>
#include <cmath>
#include <iostream>
#include<vector>
using namespace std;
//using namespace cv;
float absoluteYaw = mur.getYaw();
float errorOld=0,error=0,errorIntegral=0,errorBalance,errorBalanceOld,ErrorBalanceIntegral,uOld;
//Регуляторные коофиценты
float kpdepth = 5;
float kddepth = 1;
float kidepth =  0;
float kpbalance = 4;
float kdbalance = 0.0333;
float kibalance = 0;
float u;
float filtre[3];
cv::Mat frame;
cv::Mat hsv_and;
int Hmin = 0;
int Hmax = 256;
int Smin = 0;
int Smax = 256;
int Vmin = 0;
int Vmax = 256;
int HSVmax = 256;
//Функции обработки ползунков
void myTrackbarHmin(int pos){ Hmin = pos;}
void myTrackbarHmax(int pos){ Hmax = pos;}
void myTrackbarSmin(int pos){ Smin = pos;}
void myTrackbarSmax(int pos){ Smax = pos;}
void myTrackbarVmin(int pos){ Vmin = pos;}
void myTrackbarVmax(int pos){Vmax = pos;}
Timer t;
bool simul = 0;
//Сюда пишем координаты центров круга
int targetx,targety;
int counter = 0;
float max_r = 0;
int findCircle = 0;
//Режим отладки DEBUG = 1 - включить, DEBUG = 0 - выключить 
int DEBUG = 1;
char ch; 
int areaBall=0;
float height,width;
//Получение глубины со усредненым скользящим фильтром
float getdepth()
{
    filtre[2]=filtre[1]; 
    filtre[1]=filtre[0];
    filtre[0]=mur.getInputAOne(); 
	return (filtre[0]+filtre[1]+filtre[2])/3;
}
//Установка глубины
void setdepth(float depthToSet){
    float depth = getdepth();
    error=depthToSet-depth;
    errorIntegral+=error;
    mur.setPortC((depthToSet-depth)*kpdepth+(error-errorOld)*kddepth+errorIntegral*kidepth);
    errorOld=error;
    if(errorIntegral>3000){
        errorIntegral=3000;}
    if(errorIntegral>-3000){
        errorIntegral=-3000;}
       
}
//Сыда угол на четырехугольник
float angleToTarget = 0;
//Размытие или сжатие изображения, 1 аргумент 0 - размытие, 1 - сжатие, 2 аргумент - радиус полей размытия, 3 - само изображение
cv::Mat erodeDilate(bool type,int radi,cv::Mat img)
{
    
    cv::Mat elem = cv::getStructuringElement(2,cv::Size(2*radi+1,2*radi+1),cv::Point(radi,radi));
    if(type ==  0)
          cv::dilate(img,img,elem);
    else
        cv::erode(img,img,elem);
    return img;
}
//Поиск прямоугольников mode - по умолчанию 0, усли 1 учитываются только четырехугольнике расположенные по примерному центру экрана
bool searchRectangle(bool mode=0)
{
    int findRect = 0;
    cv::RotatedRect fitEll;
    cv::Mat forRect = hsv_and;
    cv::GaussianBlur(forRect,forRect,cv::Size(5,5),0,0);
    forRect = erodeDilate(1,1,forRect);
    double perim = 0,per;
    vector<vector<cv::Point>> contours;
    vector<cv::Point> maxContour;
    cv::findContours(forRect,contours,CV_RETR_TREE,CV_CHAIN_APPROX_NONE,cvPoint(0,0));
    if(contours.size()!=0)
    {
       
        for(int i = 0;i<contours.size();i++)
        {
            per = cv::arcLength(contours.at(i),1);
            if(per>perim&&per>50)//Игнорируем контура с периметром меньше 50
            {
                maxContour = contours.at(i);
                perim = per;
            }   
        }
        if(maxContour.size()!=0)
        {
            if(mode)
            {
                cv::RotatedRect fitEll = cv::fitEllipse(maxContour); 
                if(abs(160-(int)fitEll.center.x)<40 && abs(120-(int)fitEll.center.y)<40)//+-40 по центру экрана
                {
                    cv::approxPolyDP(maxContour,maxContour,20,1);
                    if(maxContour.size() == 4)
                    {
                        findRect = 1;
                        angleToTarget = (int)fitEll.angle;   
                        if(DEBUG==1) 
                            cout<<"ANGLE:     "<<angleToTarget<<endl; 
                    }
                }
            }
            else
            {
                fitEll = cv::fitEllipse(maxContour); 
                cv::approxPolyDP(maxContour,maxContour,20,1);
                if(maxContour.size() == 4)
                {
                    findRect = 1;
                    angleToTarget = (int)fitEll.angle;   
                    if(DEBUG==1) 
                        cout<<"ANGLE:     "<<angleToTarget<<endl; 
                }
            }   
        }    
    
    }
    if(DEBUG == 1)
    {
         cv::circle(frame,cvPoint((int)fitEll.center.x,(int)fitEll.center.y),(int)fitEll.size.width,cv::Scalar(255,0,0), 5, 8);
         imshow("main",frame);
    }
    return findRect;
}
//Поиск контура с максимальным радиусом, должна быть запущена после функции binary 
bool searchObruch()
{
    int findCircle = 0;
    float radius = 0;
    cv::Mat forCircle = hsv_and;
    cv::GaussianBlur(forCircle,forCircle,cv::Size(5,5),0,0);
    forCircle = erodeDilate(0,1,forCircle);
    double perim = 0,per;
    vector<vector<cv::Point>> contours;
    vector<cv::Point> maxContour;
    cv::findContours(forCircle,contours,CV_RETR_TREE,CV_CHAIN_APPROX_NONE,cvPoint(0,0));
    if(contours.size()!=0)
    {
        findCircle = 1;
        for(int i = 0;i<contours.size();i++)
        {
            per = cv::arcLength(contours.at(i),1);
            if(per>perim&&per>50)
            {
                maxContour = contours.at(i);
                perim = per;
            }   
        }
        if(maxContour.size()!=0)
        {
            cv::Point2f point;
            cv::minEnclosingCircle((cv::Mat)maxContour,point,radius);
            targetx = point.x;
            targety = point.y;
        }    
    }
    if(DEBUG == 1)
    {
         cv::circle(frame,cvPoint(targetx,targety),radius,cv::Scalar(0,255,255), 5, 8);
         imshow("main",frame);
    }
    return findCircle;
}
//Поиск круга, должна быть запущена после binary
bool searchCircle()
{
    bool findCircle = 0;
    cv::Mat forCircle = hsv_and;
    cv::blur(forCircle,forCircle,cv::Size(5,5),cv::Point(0,0));
    forCircle = erodeDilate(0,1,forCircle);
    if(DEBUG == 1)
        cv::imshow("binary",forCircle);
    vector<cv::Vec3f> contours;
    cv::HoughCircles(forCircle,contours,CV_HOUGH_GRADIENT,2,forCircle.rows/4,200,100,30,320);//4 аргумент - качество поиска круга, чем больше тем больше диапозон, последние 2 аргумента максимальный и минимальный радиусы
    int max_r = 0,radius;
    for(int i = 0;i<contours.size();i++)
    {
            radius = contours[i][2];
            if(radius>max_r)
            {
                max_r = radius;
                targetx = (int)contours[i][0];
                targety = (int)contours[i][1];
            }
     }
     
     if(DEBUG == 1)
     {
         cv::circle(frame,cvPoint(targetx,targety),max_r,cv::Scalar(0,255,255), 5, 8);
         imshow("main",frame);
     }
     return findCircle;
}


//Функция бинаризации записывает бинаризованые параметры в hsv_and, 1 аргумент - номер камеры, остальные для настройки диапозона
void binary(bool camNum,int hsv1,int hsv2,int hsv3,int hsv4,int hsv5,int hsv6)
{
    if(camNum==0)    
    frame = mur.getCameraOneFrame();
    else
        frame = mur.getCameraTwoFrame();
    //height = frame.height;
    //width = frame.width;
    cv::Scalar low(hsv1,hsv3,hsv5);
    cv::Scalar max(hsv2,hsv4,hsv6);
    cv::cvtColor(frame,frame,CV_BGR2HSV);
    cv::inRange(frame,low,max,hsv_and);
    if(DEBUG==1)
        cv::waitKey(33);
 
}
//Балансировка на угол
void balance(float yaw ){
    uOld=u;
    u =((int)(yaw-mur.getYaw()+540)%360-180)*kpbalance;
mur.setPortA(-u-(u-uOld)*kdbalance);
mur.setPortB(u+(u-uOld)*kdbalance);
}
float yaw=0;
float setyaw = 0; 
bool flag=1;
bool onGo = 0;
int yawCenter = 0;
//orange poloska 0,20,121,255,0,255
//Yellow sqr 13,255,193,255,0,255
int main()
{
    int FLAG = 0;
    std::cout <<"start";
    int deta=100;
    int yaww;       
    simul = 0;
    int dep = 0;
    int targ1;
    bool rec;
    //Если равен 0 запускается основной алгоритм, если 1 режим настройки ползунками 
    bool bin_mode = 0;
    int mis = 0;
    bool toggle = 0,obruch = 0,toBall = 0;
    //Инициализируем камеру
    mur.initCamera(1);
    if(!bin_mode)
    {
    while(1)
    {
    	//Простой пример программы снимаем показание с фронтальной камеры, бинаризуемся на красный, если есть четырехугольник крутим мотором
		binary(1,0,3,88,255,0,255);
        if(searchRectangle()==1)
        	mur.setPortB(-50);
        else
            mur.setPortB(0);
    	cout<<"ANGLE:    "<<searchCircle()<<endl;
    }
  
}


    else
    {
       
        IplImage* onlyBinary = 0;
        IplImage* hsv2= 0;
        IplImage* hsv_and2;
        IplImage* h_plane2 = 0;
        IplImage* s_plane2 = 0;
        IplImage* v_plane2 = 0;
        IplImage* frame1;
        //Подгружаем файл для настройки с помощью ползунков 
        frame1 =  cvLoadImage("D:/khams/image777.png");
        hsv2 = cvCreateImage( cvGetSize(frame1), IPL_DEPTH_8U, 3 );
        h_plane2 = cvCreateImage( cvGetSize(frame1), IPL_DEPTH_8U, 1 );
        s_plane2 = cvCreateImage( cvGetSize(frame1), IPL_DEPTH_8U, 1 );
        v_plane2 = cvCreateImage( cvGetSize(frame1), IPL_DEPTH_8U, 1 );
        hsv_and2 = cvCreateImage( cvGetSize(frame1), IPL_DEPTH_8U, 1 );
        cvNamedWindow("original",CV_WINDOW_AUTOSIZE);
        cvNamedWindow("hsv_and",CV_WINDOW_AUTOSIZE);
        cvCreateTrackbar("Hmin", "hsv_and", &Hmin, HSVmax, myTrackbarHmin);
        cvCreateTrackbar("Hmax", "hsv_and", &Hmax, HSVmax, myTrackbarHmax);
        cvCreateTrackbar("Smin", "hsv_and", &Smin, HSVmax, myTrackbarSmin);
        cvCreateTrackbar("Smax", "hsv_and", &Smax, HSVmax, myTrackbarSmax);
        cvCreateTrackbar("Vmin", "hsv_and", &Vmin, HSVmax, myTrackbarVmin);
        cvCreateTrackbar("Vmax", "hsv_and", &Vmax, HSVmax, myTrackbarVmax);
    	while(true){
               cvCvtColor( frame1, hsv2, CV_BGR2HSV ); 
               cvSplit( hsv2, h_plane2, s_plane2, v_plane2, 0 );
               cvInRangeS(h_plane2, cvScalar(Hmin), cvScalar(Hmax), h_plane2);
               cvInRangeS(s_plane2, cvScalar(Smin), cvScalar(Smax), s_plane2);
               cvInRangeS(v_plane2, cvScalar(Vmin), cvScalar(Vmax), v_plane2);
               cvAnd(h_plane2, s_plane2, hsv_and2);
               cvAnd(hsv_and2, v_plane2, hsv_and2);
               cvShowImage( "hsv_and", hsv_and2 );
               cvShowImage( "original", frame1 );
               char c = cvWaitKey(33);
                if(c == 115)
                        cvSaveImage("HSV.jpg",hsv_and2);
            	//Выйти на escape
                if (c == 27) { 
                        break;
                }
        }
    
     
        cvReleaseImage(&hsv2);
        cvReleaseImage(&h_plane2);
        cvReleaseImage(&s_plane2);
        cvReleaseImage(&v_plane2);
        cvReleaseImage(&hsv_and2);
        cvDestroyAllWindows();
    }
    return 0;
}
