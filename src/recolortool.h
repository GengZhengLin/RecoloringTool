#ifndef RECOLORTOOL_H
#define RECOLORTOOL_H
#include <vector>
#include <opencv2/opencv.hpp>
#include <fstream>
#define GRID_STEP 4

class RecolorTool
{
public:
    RecolorTool();
    void PreprocessImage(cv::Mat src);
    void SetColor(int index, cv::Vec3b rgb);
    cv::Vec3b GetColor(int index);
    cv::Mat GetCurMat(){return cur_mat_rgb;}
public:
#define K 5


private:
    cv::Mat src_mat_rgb;
    cv::Mat src_mat_lab;
    cv::Mat cur_mat_rgb;
    cv::Mat cur_mat_lab;
    cv::Vec3d src_colors_lab[K];
    cv::Vec3d cur_colors_lab[K];
    cv::Vec3d colors_b[K];
    cv::Vec3b src_colors_rgb[K];
    cv::Vec3b cur_colors_rgb[K];
    std::fstream fout;

    // tools
    cv::Vec3b LabToRGB(cv::Vec3d v);
    cv::Vec3d RGBToLab(cv::Vec3b v);


    // things for kmeans
#define BINS 16
    int pix_num_in_bin[BINS][BINS][BINS];
    cv::Vec3d mean_lab_in_bin[BINS][BINS][BINS];
    void CalBins();
    void InitKmeans();
    void Kmeans(double epsilon, int max_iter);
    cv::Vec3d kmeans_seeds[K+1];
    double LabDistance(cv::Vec3d v0, cv::Vec3d v1);
    double LabDistance2(cv::Vec3d v0, cv::Vec3d v1);
    // void PrintColor(cv::Vec3d v);
    double sigma_a;
    double kmeans_epsilon;
    int FindNearestCenter(cv::Vec3d *centers, int n, cv::Vec3d point);
    void SortColors();
    void SetColorsBySrcLab();


    // things for color transfer
    double Transferl(double l);
    cv::Vec2d Transferab(int index, cv::Vec3d lab);
    bool OutOfBoundary(cv::Vec3d v);
    cv::Vec3d GetBoundary(cv::Vec3d in, cv::Vec3d out);
    double GetDistance(cv::Vec2d v1, cv::Vec2d v2);
    double GetDistance2(cv::Vec2d v0, cv::Vec2d v1);
    double GetDistance(cv::Vec3d v0, cv::Vec3d v1);
    double GetDistance2(cv::Vec3d v0, cv::Vec3d v1);
    double myLabDis2(cv::Vec3d v0, cv::Vec3d v1);
    double lamda[K][K];
    double sigma_r2;
    bool CalSigma_r();
    void CalLamda();
    double Phi(cv::Vec3d c0, cv::Vec3d c1);
    void CalW(cv::Vec3d color, double w[K]);
    cv::Vec3d TransferColor(cv::Vec3d lab);
    cv::Vec3d TransferColor_CachedWeights(cv::Vec3d rgb);
    cv::Vec3d TransferColor_CachedColor(cv::Vec3b rgb);
    void ProcessImage();
    // double grid_weight[256/GRID_STEP+1][256/GRID_STEP+1][256/GRID_STEP+1][K];
    // double grid_weight[1][1][1][K];
    // cv::Vec3d grid_rgb[256/GRID_STEP+1][256/GRID_STEP+1][256/GRID_STEP+1];
    // cv::Vec3d grid_rgb[1][1][1];
    void CalGridWeights();
    void CalGridRGB();
    void SetSourceColors();

};

#endif // RECOLORTOOL_H
