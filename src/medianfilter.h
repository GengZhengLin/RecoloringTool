#ifndef MEDIANFILTER_H
#define MEDIANFILTER_H

#include <opencv2/opencv.hpp>

class MedianFilter
{
public:
    MedianFilter();
    void ProcessImage(cv::Mat src);
private:
    int window_size;
    uchar FindMedian_uchar(cv::Mat &map);
    cv::Vec3b FindMedian_Vec3b(cv::Mat &map);
};

#endif // MEDIANFILTER_H
