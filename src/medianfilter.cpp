#include "medianfilter.h"
#include <vector>
#include <algorithm>
#include <opencv2/core/core.hpp>
using namespace std;
using namespace cv;
MedianFilter::MedianFilter()
{
    window_size = 5;
}

void MedianFilter::ProcessImage(cv::Mat src)
{
    cv::Mat mat(src.rows+window_size-1, src.cols+window_size-1, src.type(), Scalar(0));
    int offset_pos = (window_size - 1) / 2;
    src.copyTo(mat(Rect(offset_pos, offset_pos, src.cols, src.rows)));
    int n = mat.channels();
    if (n == 1){
        uchar* p;
        for( int i = 0; i < src.rows; ++i){
            p = src.ptr<uchar>(i);
            for( int j = 0; j < src.cols; ++j ) {
                  p[j] = FindMedian_uchar(mat(Rect(j, i, window_size, window_size)));
            }
        }
    }else if (n == 3){
        Vec3b* p;
        for( int i = 0; i < src.rows; ++i){
            p = src.ptr<Vec3b>(i);
            for( int j = 0; j < src.cols; ++j ) {
                  p[j] = FindMedian_Vec3b(mat(Rect(j, i, window_size, window_size)));
            }
        }
    }
}

Vec3b MedianFilter::FindMedian_Vec3b(Mat &I)
{
    vector<uchar> vec0, vec1, vec2;
    Mat_<Vec3b> _I = I;

    for( int i = 0; i < I.rows; ++i)
       for( int j = 0; j < I.cols; ++j )
          {
              vec0.push_back(_I(i,j)[0]);
              vec1.push_back(_I(i,j)[1]);
              vec2.push_back(_I(i,j)[2]);
       }
    sort(vec0.begin(), vec0.end());
    sort(vec1.begin(), vec1.end());
    sort(vec2.begin(), vec2.end());
    int pos = ((int)vec0.size())/ 2;
    Vec3b r;
    r[0] = vec0[pos]; r[1] = vec1[pos]; r[2] = vec2[pos];
    return r;
}

uchar MedianFilter::FindMedian_uchar(cv::Mat &I)
{
    // generate vector
    vector<uchar> vec;
    Mat_<uchar> _I = I;

    for( int i = 0; i < I.rows; ++i)
       for( int j = 0; j < I.cols; ++j )
          {
              vec.push_back(_I(i,j));
       }
    sort(vec.begin(), vec.end());
    return vec[((int)vec.size())/ 2];
}

