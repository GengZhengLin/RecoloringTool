#include "gaussianfilter.h"
#include "math.h"
#include <QDebug>
# define M_PI           3.14159265358979323846
using namespace cv;
GaussianFilter::GaussianFilter()
{

}

void GaussianFilter::ProcessImage(cv::Mat src, cv::Mat dst, double kernel_sigma)
{
    sigma = kernel_sigma;
    size = (int)(3 * kernel_sigma);
    if (size < 3) return;
    InitKernel();
//    PrintKernel(kernel);
    Filter2D(src, dst, src.channels(), kernel);
}

void GaussianFilter::InitKernel()
{
    kernel = Mat(size, size, CV_64F);
    double sum = 0.0; // For accumulating the kernel values
    double offset = (size - 1) / 2;
    for (int x = 0; x < size; ++x)
        for (int y = 0; y < size; ++y) {
            kernel.at<double>(x,y) = exp( -0.5 * (pow((x - offset)/sigma, 2.0) + pow((x - offset)/sigma,2.0)) )
                             / (2 * M_PI * sigma * sigma);

            // Accumulate the kernel values
            sum += kernel.at<double>(x,y);
        }

    // Normalize the kernel
    for (int x = 0; x < size; ++x)
        for (int y = 0; y < size; ++y)
            kernel.at<double>(x,y) /= sum;
}

void GaussianFilter::Filter2D(Mat src, Mat dst, int channels, Mat kernel)
{

    cv::Mat mat(src.rows+size-1, src.cols+size-1, src.type(), Scalar(0));
    int offset_pos = (size - 1) / 2;
    src.copyTo(mat(Rect(offset_pos, offset_pos, src.cols, src.rows)));
    if (channels == 1){
        uchar* p;
        for( int i = 0; i < src.rows; ++i){
            p = dst.ptr<uchar>(i);
            for( int j = 0; j < src.cols; ++j ) {
                  p[j] = WeightedAverage_uchar(mat(Rect(j, i, size, size)), kernel);
            }
        }
    }else if (channels == 3){
        Vec3b* p;
        for( int i = 0; i < src.rows; ++i){
            p = dst.ptr<Vec3b>(i);
            for( int j = 0; j < src.cols; ++j ) {

                  p[j] = WeightedAverage_vec3b(mat(Rect(j, i, size, size)), kernel);
            }
        }
    }
}

void GaussianFilter::PrintMat(Mat mat)
{
    Mat_<Vec3b> _mat = mat;
    for( int i = 0; i < mat.rows; ++i){
        qDebug() << "row" << i;
       for( int j = 0; j < mat.cols; ++j ){
            qDebug() << _mat(i, j)[0];
       }
    }
}

void GaussianFilter::PrintKernel(Mat mat)
{
    Mat_<double> _mat = mat;
    for( int i = 0; i < mat.rows; ++i){
        qDebug() << "row" << i;
       for( int j = 0; j < mat.cols; ++j ){
            qDebug() << _mat(i, j);
       }
    }
}
uchar GaussianFilter::WeightedAverage_uchar(Mat src, Mat kernel)
{

    double kernel_sum = 0;
    double sum = 0;
    uchar* p_src;
    double* p_kernel;
    for( int i = 0; i < src.rows; ++i){
        p_src = src.ptr<uchar>(i);
        p_kernel = kernel.ptr<double>(i);
        for( int j = 0; j < src.cols; ++j ) {
              sum += p_src[j] * p_kernel[j];
              kernel_sum += p_kernel[j];
        }
    }
    return (uchar)(sum / kernel_sum);
}

Vec3b GaussianFilter::WeightedAverage_vec3b(Mat src, Mat kernel)
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
    r[0] = (uchar)(b_sum / kernel_sum);
    r[1] = (uchar)(g_sum / kernel_sum);
    r[2] = (uchar)(r_sum / kernel_sum);
    // qDebug() << b_sum / kernel_sum;
    return r;
}

