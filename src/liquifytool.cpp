#include "liquifytool.h"
using namespace cv;
LiquifyTool::LiquifyTool()
{
    radius = 25;
}

void LiquifyTool::ProcessImage(cv::Mat src, cv::Point start, cv::Point end)
{
    if (GetDistanceSquare(start, end) > radius * radius){
        start = GetNewStart(start, end);
    }
    int margin = 2;
    int left = start.x - radius - margin; if (left < 0) left = 0;
    int top = start.y - radius - margin; if (top < 0) top = 0;
    int width = radius * 2 + 1 + 2 * margin; if (left + width > src.cols) width = src.cols - left;
    int height = radius * 2 + 1 + 2 * margin; if (top + height > src.rows) height = src.rows - top;
    Mat map_x, map_y;
    map_x.create(height, width, CV_32FC1);
    map_y.create(height, width, CV_32FC1);
    Point n_start(start.x - left, start.y - top), n_end(end.x - left, end.y - top);
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            double v = radius * radius - GetDistanceSquare(n_start, Point(i, j));
            if (v <= 0) {
                map_x.at<float>(i, j) = j;
                map_y.at<float>(i, j) = i;
            } else {
                double s = v / (v + GetDistanceSquare(n_end, n_start));
                s = s * s;
                map_x.at<float>(i, j) = j - (n_end.x - n_start.x) * s;
                map_y.at<float>(i, j) = i - (n_end.y - n_start.y) * s;
            }
        }
    }
    Mat moi = src(Rect(left, top, width, height));
    remap( moi, moi, map_x, map_y, CV_INTER_LINEAR, BORDER_CONSTANT);
}

double LiquifyTool::GetDistanceSquare(Point p1, Point p2)
{
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

Point LiquifyTool::GetNewStart(Point start, Point end)
{
    double d = sqrt(GetDistanceSquare(start, end));
    double scale = radius / d;
    double dx = (start.x - end.x) * scale;
    double dy = (start.y - end.y) * scale;
    return Point(end.x + dx, end.y + dy);
}

