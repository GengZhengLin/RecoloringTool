#ifndef WHITENTOOLS_H
#define WHITENTOOLS_H

#include <opencv2/opencv.hpp>
class WhitenTools
{
public:
    WhitenTools();
    void ProcessImage(cv::Mat &src);

private:
    double beta;
    cv::Mat skin_mask;
    cv::CascadeClassifier face_cascade;
    uchar Adjust(uchar v);
    void CreateSkinMask( cv::Mat frame );
    void WhitenImage(cv::Mat &src);
    void BlendImage(cv::Mat &src, cv::Mat &processed, cv::Mat &dst);
};

#endif // WHITENTOOLS_H
