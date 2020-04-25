/*
 * @Author: verylazycat 
 * @Date: 2020-02-06 12:29:42 
 * @Last Modified by: verylazycat
 * @Last Modified time: 2020-02-08 13:19:28
 */
#include <main.h>
#include <cmath>
// 欧式距离计算
double PointsCompute(int x1,int y1,int x2,int y2)
{
    double results = 0;
    results = sqrt(pow(x1-x2,2) + pow(y1 - y2,2));
    return results;
}
// Right Eye:42~47
double RightEyeCompute(cv::Mat &img,const dlib::full_object_detection& d)
{
    double result = 0;
    std::vector<cv::Point> points;
    for(int i = 42;i <=47;++i)
    {
        points.push_back(cv::Point(d.part(i).x(), d.part(i).y()));
    }
    result = (PointsCompute(points[1].x,points[1].y,points[5].x,points[5].y) + PointsCompute(points[2].x,points[2].y,points[4].x,points[4].y))/(2*PointsCompute(points[0].x,points[0].y,points[3].x,points[3].y)); 
    cout<<"右眼高宽比例:";
    cout<<result<<endl;
    return result;
}
//Left Eye:36 ~ 41
double LeftEyeCompute(cv::Mat &img,const dlib::full_object_detection& d)
{
    double result = 0;
    std::vector<cv::Point> points;
    for(int i = 36;i <=41;++i)
    {
        points.push_back(cv::Point(d.part(i).x(), d.part(i).y()));
    }
    result = (PointsCompute(points[1].x,points[1].y,points[5].x,points[5].y) + PointsCompute(points[2].x,points[2].y,points[4].x,points[4].y))/(2*PointsCompute(points[0].x,points[0].y,points[3].x,points[3].y)); 
    cout<<"左眼高宽比例:";
    cout<<result<<endl;
    return result;
}
//inner lip :60 ~ 67
double InnerLipCompute(cv::Mat &img,const dlib::full_object_detection& d)
{
    double result = 0;
    std::vector<cv::Point> points;
    for(int i = 60;i <=67;++i)
    {
        points.push_back(cv::Point(d.part(i).x(), d.part(i).y()));
    }
    result = (PointsCompute(points[1].x,points[1].y,points[7].x,points[7].y) + PointsCompute(points[3].x,points[3].y,points[5].x,points[5].y))/(2*PointsCompute(points[0].x,points[0].y,points[4].x,points[4].y)); 
    cout<<"内嘴唇高宽比例:";
    cout<<result<<endl;
    return result;
}