#include "whitentools.h"
#include <math.h>
#include <vector>
using namespace cv;

void WhitenTools::CreateSkinMask( Mat frame )
{
    Mat hsv;
    cvtColor( frame, hsv, CV_BGR2HSV );
    Mat bw;
    inRange(hsv, Scalar(0, 0, 0), Scalar(40, 150, 255), bw);

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours( bw, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    int indexOfBiggestContour = 0;
    int sizeOfBiggestContour = 0;
    for (int i = 0; i < contours.size(); i++){
        if(contours[i].size() > sizeOfBiggestContour){
            sizeOfBiggestContour = contours[i].size();
            indexOfBiggestContour = i;
        }
    }

    skin_mask = Mat::zeros(frame.rows, frame.cols, CV_8U);
    drawContours( skin_mask, contours, indexOfBiggestContour, Scalar(255), -1, 8, hierarchy, 0, Point() );
    blur(skin_mask, skin_mask, Size(5,5));
}

void WhitenTools::WhitenImage(Mat &src)
{
    uchar* p = src.data;
    for( unsigned int i =0, n = src.cols * src.rows * 3; i < n; ++i){
        p[i] = Adjust(p[i]);
    }
}

void WhitenTools::BlendImage(Mat &src, Mat &processed, Mat &dst)
{
    Mat tmp; tmp.create(src.rows, src.cols, src.type());
    int i,j;
    for( i = 0; i < src.rows; ++i)
    {
        for ( j = 0; j < src.cols; ++j)
        {
            Vec3b u1 = src.at<Vec3b>(i,j);
            Vec3b u2 = processed.at<Vec3b>(i,j);
            double r = (double)skin_mask.at<uchar>(i,j) / 255.0;
            Vec3b t = u1 * (1-r) + u2 * r;
            tmp.at<Vec3b>(i,j) = u1 * (1-r) + u2 * r;
        }
    }
    dst = tmp;
}

WhitenTools::WhitenTools()
{
    beta = 5;
}

void WhitenTools::ProcessImage(Mat &src)
{
    CreateSkinMask(src);
    Mat tmp = src.clone();
    WhitenImage(tmp);
    BlendImage(src, tmp, src);
}

uchar WhitenTools::Adjust(uchar v)
{
    double r = (double)(v) / 256 * (beta - 1) + 1;
    r = log(r);
    r = r / log(beta);
    r *= 256;
    return saturate_cast<uchar>(r);
}

