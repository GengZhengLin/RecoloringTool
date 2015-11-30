#include "buffingtool.h"
#include <vector>
#include <math.h>
using namespace cv;
using namespace std;
BuffingTool::BuffingTool()
{
    lamda = 0.4;
    sigma = 20;
}

void BuffingTool::ProcessImage(Mat &src)
{
    CreateSkinMask(src);
    Mat tmp = src.clone();
    BuffingImage(tmp);
    BlendImage(src, tmp, src);
}

void BuffingTool::ProcessLine(Mat &line)
{
    Mat line1 = line.clone(), line2 = line.clone();
    // calculate p1
    Vec3b lastv = line.at<Vec3b>(0,0);
    for(int i = 1; i < line.cols; i++)
    {
        Vec3b &curv = line1.at<Vec3b>(0, i);
        double q = Distance(curv, line1.at<Vec3b>(0, i-1));
        double r1 = 1 - q * lamda;
        double r2 = q * lamda;
        // curv = r1 * curv + r2 * lastv;
        curv[0] = saturate_cast<uchar>(r1 * curv[0] + r2 * lastv[0]);
        curv[1] = saturate_cast<uchar>(r1 * curv[1] + r2 * lastv[1]);
        curv[2] = saturate_cast<uchar>(r1 * curv[2] + r2 * lastv[2]);
        lastv = curv;
    }

    // calculate p2
    lastv = line.at<Vec3b>(0,line.cols-1);
    for(int i = line.cols-2; i >= 0; i--)
    {
        Vec3b &curv = line2.at<Vec3b>(0, i);
        double q = Distance(curv, line2.at<Vec3b>(0, i+1));
        double r1 = 1 - q * lamda;
        double r2 = q * lamda;
        curv[0] = saturate_cast<uchar>(r1 * curv[0] + r2 * lastv[0]);
        curv[1] = saturate_cast<uchar>(r1 * curv[1] + r2 * lastv[1]);
        curv[2] = saturate_cast<uchar>(r1 * curv[2] + r2 * lastv[2]);
        lastv = curv;
    }

    // average
    for(int i = 1; i < line.cols; i++)
    {
        Vec3b &curv = line.at<Vec3b>(0, i);
        Vec3b p1 = line1.at<Vec3b>(0,i);
        Vec3b p2 = line2.at<Vec3b>(0,i);
        double r1 = 1 / (1 + lamda), r2 = - (1 - lamda) / (1 + lamda);
        double t = r1 * p1[0] + r2 * curv[0] + r1 * p2[0];
        uchar u1 = (uchar)t;
        uchar u2 = saturate_cast<uchar>(t);
        curv[0] = saturate_cast<uchar>(r1 * p1[0] + r2 * curv[0] + r1 * p2[0]);
        curv[1] = saturate_cast<uchar>(r1 * p1[1] + r2 * curv[1] + r1 * p2[1]);
        curv[2] = saturate_cast<uchar>(r1 * p1[2] + r2 * curv[2] + r1 * p2[2]);
    }
}

double BuffingTool::Distance(Vec3b v1, Vec3b v2)
{
    double r2 = (v1[0]-v2[0])*(v1[0]-v2[0])+(v1[1]-v2[1])*(v1[1]-v2[1])+(v1[2]-v2[2])*(v1[2]-v2[2]);
    r2 /= (256 * 256);
    return exp(-r2/(2*sigma*sigma));
}

void BuffingTool::CreateSkinMask(Mat frame)
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
    skin_mask = Mat::ones(frame.rows, frame.cols, CV_8U) * 255;
}

void BuffingTool::BuffingImage(Mat &src)
{
    Mat horizontal = src.clone();
    Mat vertical; transpose(src, vertical);

    for (int i = 0; i < horizontal.rows; i++){
        ProcessLine(horizontal.row(i));
    }

    for (int i = 0; i < vertical.rows; i++){
        ProcessLine(vertical.row(i));
    }
    transpose(vertical, vertical);
    src = (horizontal + vertical) / 2;
}

void BuffingTool::BlendImage(Mat &src, Mat &processed, Mat &dst)
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

