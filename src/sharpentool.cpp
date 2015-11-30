#include "sharpentool.h"
using namespace cv;
SharpenTool::SharpenTool()
{

}

void SharpenTool::ProcessImage(cv::Mat src, cv::Mat dst)
{
    Filter2D(src, dst, InitKernel());
}

cv::Mat SharpenTool::InitKernel()
{
    cv::Mat kernel = (Mat_<double>(3,3) << -1, -1, -1, -1, 9, -1, -1, -1, -1);
    kernel_size = 3;
    return kernel;
}

void SharpenTool::Filter2D(cv::Mat src, cv::Mat dst, cv::Mat kernel)
{
    cv::Mat mat(src.rows+kernel_size-1, src.cols+kernel_size-1, src.type(), Scalar(0));
    int offset_pos = (kernel_size - 1) / 2;
    src.copyTo(mat(Rect(offset_pos, offset_pos, src.cols, src.rows)));

    Vec3b* p;
    for( int i = 0; i < src.rows; ++i){
        p = dst.ptr<Vec3b>(i);
        for( int j = 0; j < src.cols; ++j ) {
              p[j] = WeightedSum_vec3b(mat(Rect(j, i, kernel_size, kernel_size)), kernel);
        }
    }
}

Vec3b SharpenTool::WeightedSum_vec3b(Mat src, Mat kernel)
{
    // qDebug() << "WeightedAverage_vec3b" << src.rows << src.cols;
    Mat_<Vec3b> _src = src;
    Mat_<double> _kernel = kernel;
    double kernel_sum = 0;
    double b_sum = 0, g_sum = 0, r_sum = 0;
    for( int i = 0; i < kernel.rows; ++i)
       for( int j = 0; j < kernel.cols; ++j ){
            b_sum += _src(i, j)[0] * _kernel(i, j);
            g_sum += _src(i, j)[1] * _kernel(i, j);
            r_sum += _src(i, j)[2] * _kernel(i, j);
            kernel_sum += _kernel(i, j);
       }
    Vec3b r;
    r[0] = saturate_cast<uchar>(b_sum / kernel_sum);
    r[1] = saturate_cast<uchar>(g_sum / kernel_sum);
    r[2] = saturate_cast<uchar>(r_sum / kernel_sum);
    // qDebug() << b_sum / kernel_sum;
    return r;
}

