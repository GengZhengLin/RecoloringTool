#ifndef GAUSSIANFILTER_H
#define GAUSSIANFILTER_H

#include <opencv2/opencv.hpp>
class GaussianFilter
{
public:
    GaussianFilter();
    void ProcessImage(cv::Mat src, cv::Mat dst, double kernal_sigma);

private:
    cv::Mat kernel;
    int size;
    double sigma;
    void InitKernel();
    void Filter2D(cv::Mat src, cv::Mat dst, int channels,  cv::Mat kernel);
    void PrintMat(cv::Mat mat);
    void PrintKernel(cv::Mat mat);
    uchar WeightedAverage_uchar(cv::Mat src, cv::Mat kernel);
    cv::Vec3b WeightedAverage_vec3b(cv::Mat src, cv::Mat kernel);
};

#endif // GAUSSIANFILTER_H
