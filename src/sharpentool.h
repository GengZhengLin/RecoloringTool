#ifndef SHARPENTOOL_H
#define SHARPENTOOL_H

#include <opencv2/opencv.hpp>
class SharpenTool
{
public:
    SharpenTool();
    void ProcessImage(cv::Mat src, cv::Mat dst);

private:
    cv::Mat InitKernel();
    void Filter2D(cv::Mat src, cv::Mat dst, cv::Mat kernel);
    cv::Vec3b WeightedSum_vec3b(cv::Mat src, cv::Mat kernel);
    int kernel_size;
};

#endif // SHARPENTOOL_H
