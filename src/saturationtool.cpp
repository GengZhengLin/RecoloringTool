#include "saturationtool.h"
using namespace cv;
SaturationTool::SaturationTool()
{

}

void SaturationTool::ProcessImage(cv::Mat src, cv::Mat dst, double scale)
{
    Mat hsv;
    cvtColor(src, hsv, CV_BGR2HSV);
    MatIterator_<Vec3b> it, end;
    for( it = hsv.begin<Vec3b>(), end = hsv.end<Vec3b>(); it != end; ++it)
    {
        (*it)[1] = saturate_cast<uchar>((*it)[1]*scale);
    }
    cvtColor(hsv, dst, CV_HSV2BGR);
}

