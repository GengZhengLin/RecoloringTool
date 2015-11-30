#ifndef LIQUIFYTOOL_H
#define LIQUIFYTOOL_H
#include <opencv2/opencv.hpp>

class LiquifyTool
{
public:
    LiquifyTool();
    void ProcessImage(cv::Mat src, cv::Point start, cv::Point end);
    void SetRadius(int r){radius = r;}
private:
    int radius;
    double GetDistanceSquare(cv::Point p1, cv::Point p2);
    cv::Point GetNewStart(cv::Point start, cv::Point end);
};

#endif // LIQUIFYTOOL_H
