#ifndef SATURATION_H
#define SATURATION_H
#include <opencv2/opencv.hpp>

class SaturationTool
{
public:
    SaturationTool();
    void ProcessImage(cv::Mat src, cv::Mat dst, double scale);
};

#endif // SATURATION_H
