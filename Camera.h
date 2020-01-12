#include <murAPI.hpp>
#include <cmath>
#include <iostream>
#include <vector>
#include <string>
//using namespace cv;
class Camera
{
	public:
		Camera(short num);
                void setupBinary(short num);
		void setBinary(cv::Scalar lowThresholdArg, cv::Scalar highThresholdArg);	
                short searchSquare(bool blur = 1, int minPerimetr = 50, int rectQuality = 40);
                bool searchRectangle(bool blur = 1, bool mode = 0, double minPerimetr = 300, short centerQuality = 40);
		bool searchMaxContour(bool blur = 1, double minPerimetr = 50);
		bool searchCircle(bool blur = 1, int quality = 320);
                void erode(int radi);
                double currentArea;
                cv::Size blurSize;
		int targetAngle = -1;
		int targetSize = -1;
		short X_Screen = -1;
		short Y_Sreen = -1;
		bool debug = 1;
		double targetx, targety; 
                double xSize, ySize;
	private:
                int minSquare, maxSquare;
		void blur();
		void binary(cv::Mat frame);
		cv::Scalar lowThreshold;
		cv::Scalar highThreshold; 
		cv::RotatedRect fitEllipse;
		cv::Mat frame;
		cv::Mat image;
		cv::Mat elem;
		cv::Mat toShowDebug;
		std::string str;
		std::vector<std::vector<cv::Point>> contours;
   	 	std::vector<cv::Point> maxContour;
                std::vector<cv::Vec3f> contoursCircle;
   	 	float radius, maxRadius;
   	 	short camNum = -1;
   	 	//double maxPerimetr;
};	
Camera::Camera(short num)
{
	if(debug)
        {
		str = "cam" + std::to_string(num);
                cv::namedWindow(str+"_object");
        }
	camNum = num;
        if(camNum == 1)    
            frame = mur.getCameraOneFrame();
        else if(camNum == 2)
            frame = mur.getCameraTwoFrame();
        xSize = frame.size().width;
        ySize = frame.size().height;
}
void Camera::setupBinary(short num)
{
    cv::namedWindow("Bin");
    int hMin = 0;
    int hMax = 0;
    int sMin = 0;
    int sMax = 0;
    int vMin = 0;
    int vMax = 0;
    int chMax = 255;
    cv::Mat image;
    cv::createTrackbar("hMin", "Bin", &hMin, chMax);
    cv::createTrackbar("hMax", "Bin", &hMax, chMax);
    cv::createTrackbar("sMin", "Bin", &sMin, chMax);
    cv::createTrackbar("sMax", "Bin", &sMax, chMax);
    cv::createTrackbar("vMin", "Bin", &vMin, chMax);
    cv::createTrackbar("vMax", "Bin", &vMax, chMax);
    while (true) {
        if(num == 1)
            image = mur.getCameraOneFrame();
        else
            image = mur.getCameraTwoFrame();
        cv::imshow("Image", image);
        cv::cvtColor(image, image, CV_BGR2Lab);
        cv::Scalar lower(hMin, sMin, vMin);
        cv::Scalar upper(hMax, sMax, vMax);
        cv::inRange(image, lower, upper, image);
        cv::imshow("Bin", image);
        cv::waitKey(10);
    }
}
void Camera::erode(int radi)
{
    cv::Mat elem = cv::getStructuringElement(2,cv::Size(2*radi+1,2*radi+1),cv::Point(radi,radi));
    cv::erode(image,image,elem);

}
void Camera::blur()
{
	cv::GaussianBlur(image, image, blurSize, 0, 0);
}
void Camera::setBinary(cv::Scalar lowThresholdArg, cv::Scalar highThresholdArg)
{
    lowThreshold = lowThresholdArg;
    highThreshold = highThresholdArg;
}
void Camera::binary(cv::Mat frame)
{
    cv::cvtColor(frame, frame, CV_BGR2Lab);
    cv::inRange(frame, lowThreshold, highThreshold, image);
}
//Поиск прямоугольников mode - по умолчанию 0, усли 1 учитываются только четырехугольнике расположенные по примерному центру экрана
bool Camera::searchRectangle(bool blurArg, bool mode, double minPerimetr, short centerQuality)
{
    if(camNum == 1)    
    	frame = mur.getCameraOneFrame();
    else if(camNum == 2)
        frame = mur.getCameraTwoFrame();	
    binary(frame);
    if(blurArg)
    	blur();
    erode(3);
    cv::findContours(image.clone(), contours, CV_RETR_TREE,CV_CHAIN_APPROX_NONE, cvPoint(0,0));
    if(contours.size()!=0)
    {
        if(debug)
    	{
            toShowDebug = image;
            //cv::circle(toShowDebug, cvPoint((int)fitEllipse.center.x, (int)fitEllipse.center.y), (int)fitEllipse.size.width*2, cv::Scalar(255,0,0), 5, 8);
            cv::imshow(str + "_object", toShowDebug);
            cv::waitKey(33);
    	}
        cv::Moments mu;
        for(int i = 0;i<contours.size();i++)
        {
            double perimetr = cv::arcLength(contours.at(i),1);
            mu = moments( contours[i], true);
            //std::cout<<mu.m00<<std::endl;
            if(perimetr > minPerimetr && mu.m00 > 1000)//Игнорируем контура с периметром меньше 50
            {
                maxContour = contours.at(i);
               	minPerimetr = perimetr;
            }   
        }
        if(maxContour.size()>=5)
        {
            fitEllipse = cv::fitEllipse(maxContour); 
            if(!mode || (mode && abs((X_Screen/2) - (int)fitEllipse.center.x) < centerQuality && abs((Y_Sreen/2) - (int)fitEllipse.center.y) < centerQuality))
            {
                cv::approxPolyDP(maxContour,maxContour, 20, 1);
                if(maxContour.size() == 4)
                {
                    targetAngle = (int)fitEllipse.angle; 
                    maxContour.clear();
                    return 1; 
                }
            }
        }    
    }
    contours.clear();
    return 0;
} 
//Поиск контура с максимальным радиусом, должна быть запущена после функции binary 
bool Camera::searchMaxContour(bool blurArg, double minPerimetr)
{
    if(camNum == 1)    
    	frame = mur.getCameraOneFrame();
    else if(camNum == 2)
        frame = mur.getCameraTwoFrame();	
    binary(frame);
    if(blurArg)
    	blur();
    cv::findContours(image.clone(), contours, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
    if(contours.size() != 0)
    {
        for(int i = 0;i<(int)contours.size();i++)
        {
            double perimetr = cv::arcLength(contours.at(i),1);
            if(perimetr > minPerimetr)
            {
                maxContour = contours.at(i);
                minPerimetr = perimetr;
            }   
        }
        if(maxContour.size()!=0)
        {
            if(debug)
            {
                toShowDebug = image;
                cv::circle(toShowDebug, cvPoint(targetx,targety), radius, cv::Scalar(0,255,255), 5, 8);
                cv::imshow(str+"_object", toShowDebug);
                cv::waitKey(33);
            }
            cv::Point2f point;
            cv::minEnclosingCircle((cv::Mat)maxContour,point, radius);
            targetx = point.x;
            targety = point.y;
            currentArea = cv::contourArea(maxContour);
            return 1;
        }    
    }
    return 0;
}
//Поиск круга, должна быть запущена после binary
bool Camera::searchCircle(bool blurArg, int quality)
{
    if(camNum == 1)    
    	frame = mur.getCameraOneFrame();
    else if(camNum == 2)
        frame = mur.getCameraTwoFrame();	
    binary(frame);
    if(blurArg)
    	blur();
    cv::HoughCircles(image.clone(), contoursCircle, CV_HOUGH_GRADIENT, 2, image.clone().rows/4, 200, 100, 30, quality);//4 аргумент - качество поиска круга, чем больше тем больше диапозон, последние 2 аргумента максимальный и минимальный радиусы
    maxRadius = 30;
    if(contours.size() != 0)
    {
	    for(int i = 0;i < (int)contours.size();i++)
	    {
	            radius = contoursCircle[i][2];
	            if(radius>maxRadius)
	            {
	                maxRadius = radius;
	                targetx = (int)contoursCircle[i][0];
	                targety = (int)contoursCircle[i][1];
	            }
	     }
	    if(debug)
	    {
                toShowDebug = image;
	        cv::circle(toShowDebug ,cvPoint(targetx,targety),maxRadius,cv::Scalar(0,255,255), 5, 8);
	        imshow(str + "_object", toShowDebug);
	        cv::waitKey(33);
	    }
	    return 1;
	}
	else 
    	return 0;
}
short Camera::searchSquare(bool blurArg, int minPerimetr, int rectQuality)
{
    if(camNum == 1)    
    	frame = mur.getCameraOneFrame();
    else if(camNum == 2)
        frame = mur.getCameraTwoFrame();	
    binary(frame);
    if(blurArg)
    	blur();
    cv::findContours(image.clone() ,contours, CV_RETR_TREE,CV_CHAIN_APPROX_NONE, cvPoint(0,0));
    if(contours.size()!=0)
    {
        for(int i = 0;i<contours.size();i++)
        {
            double perimetr = cv::arcLength(contours.at(i),1);
            if(perimetr > minPerimetr)//Игнорируем контура с периметром меньше 50
            {
                maxContour = contours.at(i);
               	minPerimetr = perimetr;
            }   
        }
        if(maxContour.size()!=0)
        {
        	if(debug)
    		{
                        toShowDebug = image;
         		cv::circle(toShowDebug, cvPoint((int)fitEllipse.center.x, (int)fitEllipse.center.y), (int)fitEllipse.size.width, cv::Scalar(255,0,0), 5, 8);
        		cv::imshow(str + "_object", toShowDebug);
        		cv::waitKey(33);
    		}
                cv::approxPolyDP(maxContour,maxContour, 20, 1);
                if(maxContour.size() == 4)
                {
                    int summContour[4];
                    int diffContour[4];
                    for(int i = 0;i < 4;i++)
                    {
                        summContour[i] = maxContour[i].x + maxContour[i].y;
                        diffContour[i] = maxContour[i].x - maxContour[i].y; 
                    }
                    int summPos[4];
                    int diffPos[4];
                    summPos[0] = 0;
                    summPos[2] = 100000;
                    diffPos[0] = 0;
                    diffPos[2] = 100000;
                    int rect[4][2];
                    for(int i = 0;i<4;i++)
                    {
                        if(diffContour[i] > diffPos[0])
                        {
                            diffPos[1] = i;
                            diffPos[0] = diffContour[i];
                        }
                        if(summContour[i] > summPos[0])
                        {
                            summPos[1] = i;
                            summPos[0] = summContour[i];
                        }
                        if(diffContour[i] < diffPos[2])
                        {
                            diffPos[3] = i;
                            diffPos[2] = diffContour[i];
                        }
                        if(summContour[i] < summPos[2])
                        {
                            summPos[3] = i;
                            summPos[2] = summContour[i];
                        }
                    }
                    for(int i = 0;i<4;i++)
                    {
                        if(i%2 == 0)
                        {
                            rect[i][0] = maxContour[summPos[i+1]].x;
                            rect[i][1] = maxContour[summPos[i+1]].y;
                        }
                        else
                        {
                            rect[i][0] = maxContour[summPos[i]].x;
                            rect[i][1] = maxContour[summPos[i]].y;
                        }
                    } 
                    float maxWidth = std::max(sqrt(pow((double)(rect[2][0] - rect[3][0]), 2) + pow((double)(rect[2][1] - rect[3][1]), 2)), sqrt(pow((double)(rect[1][0] - rect[0][0]), 2) + pow((double)(rect[1][1] - rect[0][1]), 2)));
                    float maxHeight = std::max(sqrt(pow((double)(rect[1][0] - rect[2][0]), 2) + pow((double)(rect[1][1] - rect[2][1]), 2)), sqrt(pow((double)(rect[0][0] - rect[3][0]), 2) + pow((double)(rect[0][1] - rect[3][1]), 2)));
                    if(abs(maxWidth - maxHeight) < rectQuality)
                    {
                        fitEllipse = cv::fitEllipse(maxContour); 
                        targetAngle = (int)fitEllipse.angle;
                        if(maxHeight * maxWidth <= minSquare)
                            return 1;
                        else if(maxHeight * maxWidth >= maxSquare)
                            return 3;
                        else 
                            return 2;
                    }
                    maxContour.clear();
                    
                    
                }
            
        }    
    }
    contours.clear();
    return 0;
}