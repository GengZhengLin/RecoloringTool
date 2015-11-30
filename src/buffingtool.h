#ifndef BUFFINGTOOL_H
#define BUFFINGTOOL_H
#include <opencv2/opencv.hpp>

class BuffingTool
{
public:
    BuffingTool();
    void ProcessImage(cv::Mat &src);
private:
    double lamda;
    double sigma;
    cv::Mat skin_mask;
    void ProcessLine(cv::Mat &line);
    double Distance(cv::Vec3b v1, cv::Vec3b v2);
    void CreateSkinMask( cv::Mat frame );
    void BuffingImage(cv::Mat &src);
    void BlendImage(cv::Mat &src, cv::Mat &processed, cv::Mat &dst);
};

#endif // BUFFINGTOOL_H
