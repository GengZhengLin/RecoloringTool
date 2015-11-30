#include "recolortool.h"
#include <cmath>
#include "inverse_matrix.h"
using namespace cv;
using namespace std;

static double grid_weight[256/GRID_STEP+1][256/GRID_STEP+1][256/GRID_STEP+1][K];
static cv::Vec3d grid_rgb[256/GRID_STEP+1][256/GRID_STEP+1][256/GRID_STEP+1];

RecolorTool::RecolorTool()
{

}

void RecolorTool::PreprocessImage(Mat src)
{
    cvtColor(src, src_mat_rgb, CV_BGR2RGB);
    cvtColor(src_mat_rgb, src_mat_lab, CV_RGB2Lab);
    cur_mat_rgb = src_mat_rgb.clone();
    cur_mat_lab = src_mat_lab.clone();
    fout.open("output.txt", ios::out);
    CalBins();
    InitKmeans();
    Kmeans(0, 20);
    // SetSourceColors();
    CalSigma_r();
    CalLamda();
    // CalGridWeights();
}

void RecolorTool::SetColor(int index, Vec3b rgb)
{
    cur_colors_lab[index] = RGBToLab(rgb);
    // for i < index l[i] > l[i+1]
    for (int i = index-1; i > 0; i--){
        if (cur_colors_lab[i][0] < cur_colors_lab[i+1][0]){
            cur_colors_lab[i][0] = cur_colors_lab[i+1][0];
        }
    }
    for (int i = index + 1; i < K; i++){
        if (cur_colors_lab[i][0] > cur_colors_lab[i-1][0] ){
            cur_colors_lab[i][0] = cur_colors_lab[i-1][0];
        }
    }

    for (int i = 0; i < K; i++){
        cur_colors_rgb[i] = LabToRGB(cur_colors_lab[i]);
    }
    for (int i = 0; i < K; i++){
        if (cur_colors_lab[i] != src_colors_lab[i]){
            Vec3d dir = cur_colors_lab[i] - src_colors_lab[i];
            Vec3d out = src_colors_lab[i] + 300 * dir;
            colors_b[i] = GetBoundary(src_colors_lab[i], out);
        }
    }
    CalGridRGB();
    ProcessImage();
}

Vec3b RecolorTool::GetColor(int index)
{
    return cur_colors_rgb[index];
}

Vec3b RecolorTool::LabToRGB(Vec3d v)
{
    Mat lab(1,1,CV_8UC3); lab.at<Vec3b>(0,0) = v;
    Mat bgr(1,1,CV_8UC3);
    cvtColor(lab, bgr, CV_Lab2RGB);
    return bgr.at<Vec3b>(0,0);
}

Vec3d RecolorTool::RGBToLab(Vec3b v)
{
    Mat bgr(1,1,CV_8UC3); bgr.at<Vec3b>(0,0) = v;
    Mat lab(1,1,CV_8UC3);
    cvtColor(bgr, lab, CV_RGB2Lab);
    return lab.at<Vec3b>(0,0);
}

void RecolorTool::CalBins()
{
    // iterating calculate num in bins
    // get lab sum in a array
    Vec3d sum_lab_in_bins[BINS][BINS][BINS];

    // init stats
    for (int i0 = 0; i0 < BINS; i0++){
        for (int i1 = 0; i1 < BINS; i1++){
            for (int i2 = 0; i2 < BINS; i2++){
                sum_lab_in_bins[i0][i1][i2] = Vec3d(0,0,0);
                pix_num_in_bin[i0][i1][i2] = 0;
            }
        }
    }


    int step = (256 + BINS - 1) / BINS; // step = 16
    for (int y = 0; y < src_mat_rgb.rows; y++){
        for (int x = 0; x < src_mat_rgb.cols; x++){
            Vec3b v_bgr = src_mat_rgb.at<Vec3b>(y, x);
            Vec3b v_lab = src_mat_lab.at<Vec3b>(y, x);
            int p0 = v_bgr[0] / step;
            int p1 = v_bgr[1] / step;
            int p2 = v_bgr[2] / step;
            sum_lab_in_bins[p0][p1][p2] += v_lab;
            pix_num_in_bin[p0][p1][p2] ++;
        }
    }

    // calculate mean
    for (int i0 = 0; i0 < BINS; i0++){
        for (int i1 = 0; i1 < BINS; i1++){
            for (int i2 = 0; i2 < BINS; i2++){
                if (pix_num_in_bin[i0][i1][i2] == 0) continue;
                mean_lab_in_bin[i0][i1][i2] = (sum_lab_in_bins[i0][i1][i2] / pix_num_in_bin[i0][i1][i2]);
                // fout << "mean_lab_in_bin " << mean_lab_in_bin[i0][i1][i2] << endl;
            }
        }
    }
}

void RecolorTool::InitKmeans()
{
    double tmp_num_in_bin[BINS][BINS][BINS];

    for (int i0 = 0; i0 < BINS; i0++){
        for (int i1 = 0; i1 < BINS; i1++){
            for (int i2 = 0; i2 < BINS; i2++){
                double d = pix_num_in_bin[i0][i1][i2];
                tmp_num_in_bin[i0][i1][i2] = pix_num_in_bin[i0][i1][i2];
            }
        }
    }

    kmeans_seeds[0] = Vec3d(0,0,0);
    for (int i = 1; i <= K; i++){
        // find the largest one in tmp_num_in_bin
        int mi0 = 0, mi1 = 0, mi2 = 0;
        int mnum = tmp_num_in_bin[0][0][0];
        for (int i0 = 0; i0 < BINS; i0++){
            for (int i1 = 0; i1 < BINS; i1++){
                for (int i2 = 0; i2 < BINS; i2++){
                    int num = tmp_num_in_bin[i0][i1][i2];
                    if (num > mnum){
                        mi0 = i0; mi1 = i1; mi2 = i2;
                        mnum = num;
                    }
                }
            }
        }

        kmeans_seeds[i] = mean_lab_in_bin[mi0][mi1][mi2];
        fout << "kmeans_seeds " << i << kmeans_seeds[i] << endl;
        sigma_a = 80;
        if (i == K) continue;
        // attenuate tmp_num_in_bin
        for (int i0 = 0; i0 < BINS; i0++){
            for (int i1 = 0; i1 < BINS; i1++){
                for (int i2 = 0; i2 < BINS; i2++){
                    double dij2 = LabDistance2(kmeans_seeds[i], mean_lab_in_bin[i0][i1][i2]);
                    double factor = 1 - exp(-(dij2) / (sigma_a * sigma_a));
                    tmp_num_in_bin[i0][i1][i2] *= factor;
                }
            }
        }

    }
}

void RecolorTool::Kmeans(double epsilon, int max_iter)
{
    Vec3d kmeans_centers[K+1];
    Vec3d kmeans_acc[K+1];
    double kmeans_w[K+1];
    int labels[BINS][BINS][BINS];
    // set centers = seeds
    for (int i = 0; i <= K; i++){
        kmeans_centers[i] = kmeans_seeds[i];
    }

    bool label_changed = false;
    for (int iter = 0; iter < max_iter; iter++){
        // clear variables
        label_changed = false;
        for (int i = 0; i <= K; i++){
            kmeans_acc[i] = Vec3d(0,0,0);
            kmeans_w[i] = 0;
        }

        // first iteration: cal Labels, kmeans_acc, kmeans_w
        for (int i0 = 0; i0 < BINS; i0++){
            for (int i1 = 0; i1 < BINS; i1++){
                for (int i2 = 0; i2 < BINS; i2++){
                    if (pix_num_in_bin[i0][i1][i2] == 0) continue;
                    Vec3d v = mean_lab_in_bin[i0][i1][i2];
                    int index = FindNearestCenter(kmeans_centers, K+1, v);
                    if (labels[i0][i1][i2] != index){
                        labels[i0][i1][i2] = index;
                        label_changed = true;
                    }
                    kmeans_acc[index] += v * pix_num_in_bin[i0][i1][i2];
                    kmeans_w[index] += pix_num_in_bin[i0][i1][i2];
                }
            }
        }

        // cal kmeans_centers
        fout << "kmeans_centers" << endl;
        for (int i = 1; i <= K; i++){
            if (kmeans_w[i] == 0){
                continue;
            }
            kmeans_centers[i] = kmeans_acc[i] / kmeans_w[i];
            fout << kmeans_centers[i] << endl;
        }

        if (!label_changed) break;
    }
    for (int i = 0; i < K; i++){
        src_colors_lab[i] = kmeans_centers[i+1];
    }
    SortColors();
    SetColorsBySrcLab();
}

double RecolorTool::LabDistance(Vec3d v0, Vec3d v1)
{
    double d2 = LabDistance2(v0, v1);
    return sqrt(d2);
}

double RecolorTool::LabDistance2(Vec3d v0, Vec3d v1)
{
    return ((v0[0]-v1[0])*(v0[0]-v1[0])/255*100/255*100+(v0[1]-v1[1])*(v0[1]-v1[1])+(v0[2]-v1[2])*(v0[2]-v1[2]));
//    double k1 = 0.045, k2 = 0.015;
//    double del_l = (v0[0] - v1[0]) * 100 / 255;
//    v0[1] -= 128; v0[2] -= 128;
//    v1[1] -= 128; v1[2] -= 128;
//    double c1 = sqrt(v0[1]*v0[1] + v0[2]*v0[2]);
//    double c2 = sqrt(v1[1]*v1[1] + v1[2]*v1[2]);
//    double c_ab = c1 - c2;
//    double h_ab = (v0[1]-v1[1])*(v0[1]-v1[1]) + (v0[2]-v1[2])*(v0[2]-v1[2]) - c_ab * c_ab;
//    return del_l * del_l + c_ab * c_ab / (1 + k1 * c1) / (1 + k1 * c1) + h_ab / (1 + k2 * c1) / (1 + k2 * c1);

}

int RecolorTool::FindNearestCenter(Vec3d *centers, int n, Vec3d point)
{
    double min_dis = LabDistance2(centers[0], point);
    int index = 0;
    for (int i = 1; i < n; i++){
        double dis = LabDistance2(centers[i], point);
        if (dis < min_dis){
            index = i;
            min_dis = dis;
        }
    }
    return index;
}

void RecolorTool::SortColors()
{
    // sort cur_colors_lab
    // bubble sort start from zero to i
    for (int i = K-1; i > 0; i--){
        // compare j and j+1, if j < j+1 -> swap
        for (int j = 0; j < i; j++){
            if (src_colors_lab[j][0] < src_colors_lab[j+1][0]){
                Vec3d temp = src_colors_lab[j];
                src_colors_lab[j] = src_colors_lab[j+1];
                src_colors_lab[j+1] = temp;
            }
        }
    }
}

void RecolorTool::SetColorsBySrcLab()
{
    for (int i = 0; i < K; i++){
        src_colors_rgb[i] = LabToRGB(src_colors_lab[i]);
        cur_colors_lab[i] = src_colors_lab[i];
        cur_colors_rgb[i] = src_colors_rgb[i];
    }
}

double RecolorTool::Transferl(double l)
{
    // find the interval: find the first i s.t. l > l[i]
    int i = 0;
    for (; i < K; i++){
        if (l > cur_colors_lab[i][0]){
            break;
        }
    }

    // interpolate between i-1 and i
    double l1 = 0, l2 = 0;
    double c1 = 0, c2 = 0;
    if (i == 0){
        l1 = 255 - l; l2 = l - cur_colors_lab[i][0];
        c1 = 255; c2 = cur_colors_lab[i][0];
    } else if (i == K){
        return l;
    } else {
        l1 = cur_colors_lab[i-1][0] - l; l2 = l - cur_colors_lab[i][0];
        c1 = cur_colors_lab[i-1][0]; c2 = cur_colors_lab[i][0];
    }
    double rl = (l2 * c1 + l1 * c2) / (l1 + l2);
    return rl;
}

Vec2d RecolorTool::Transferab(int index, Vec3d lab)
{
    if (src_colors_lab[index] == cur_colors_lab[index]){
        return Vec2d(lab[1], lab[2]);
    }
    Vec3d xout = lab + cur_colors_lab[index] - src_colors_lab[index]; Vec3d xb;
    if (OutOfBoundary(xout)){
        xb = GetBoundary(cur_colors_lab[index], xout);
    } else {
        Vec3d tdir = xout - lab;
        xb = GetBoundary(lab, lab+300*tdir);
    }
    Vec2d cur_color_ab = Vec2d(cur_colors_lab[index][1], cur_colors_lab[index][2]);
    Vec2d src_color_ab = Vec2d(src_colors_lab[index][1], src_colors_lab[index][2]);
    Vec2d xb2d = Vec2d(xb[1], xb[2]);
    Vec2d x2d = Vec2d(lab[1], lab[2]);
    double c2 = GetDistance(cur_color_ab, src_color_ab);
    double x2 = GetDistance(xb2d, x2d);
    double d2 = x2 / c2;
    if (d2 > 1) d2 = 1;
    double d = sqrt(c2 * d2 / x2);
    Vec2d dir = xb2d - x2d;
    return x2d + dir * d;
}

bool RecolorTool::OutOfBoundary(Vec3d v)
{
    if (v[0] < 0 || v[0] > 255 || v[1] < 0 || v[1] > 255 || v[2] < 0 || v[2] > 255) return true;
    v[0] = v[0] / 255 * 100; v[1] = v[1] - 128; v[2] = v[2] - 128;
    Mat lab(1,1,CV_32FC3); lab.at<Vec3f>(0,0) = v;
    Mat rgb(1,1, CV_32FC3);
    cvtColor(lab, rgb, CV_Lab2RGB);
    Vec3d vd = rgb.at<Vec3f>(0,0);
    if (vd[0] == 0 || vd[0] == 1 || vd[1] == 0 || vd[1] == 1 || vd[2] == 0 || vd[2] == 1)
        return true;
    return false;
}

Vec3d RecolorTool::GetBoundary(Vec3d in, Vec3d out)
{
    Vec3d mid;
//    if (!OutOfBoundary(out)){
//        int x = 0;
//    }
    for (int i = 0; i < 15; i++){
        mid = (in + out) / 2;
        if (OutOfBoundary(mid)){
            out  = mid;
        } else {
            in = mid;
        }
    }
    return mid;
}

double RecolorTool::GetDistance(Vec2d v1, Vec2d v2)
{
    return sqrt(GetDistance2(v1, v2));
}

double RecolorTool::GetDistance2(Vec2d v0, Vec2d v1)
{
    return ((v0[0]-v1[0])*(v0[0]-v1[0])+(v0[1]-v1[1])*(v0[1]-v1[1]));
}

double RecolorTool::GetDistance(Vec3d v0, Vec3d v1)
{
    return sqrt(GetDistance2(v0, v1));
}

double RecolorTool::GetDistance2(Vec3d v0, Vec3d v1)
{
    return ((v0[0]-v1[0])*(v0[0]-v1[0])+(v0[1]-v1[1])*(v0[1]-v1[1])+(v0[2]-v1[2])*(v0[2]-v1[2]));
}

double RecolorTool::myLabDis2(Vec3d v0, Vec3d v1)
{
    double k1 = 0.045, k2 = 0.015;
    double del_l = (v0[0] - v1[0]) * 100 / 255;
    v0[1] -= 128; v0[2] -= 128;
    v1[1] -= 128; v1[2] -= 128;
    double c1 = sqrt(v0[1]*v0[1] + v0[2]*v0[2]);
    double c2 = sqrt(v1[1]*v1[1] + v1[2]*v1[2]);
    double c_ab = c1 - c2;
    double h_ab = (v0[1]-v1[1])*(v0[1]-v1[1]) + (v0[2]-v1[2])*(v0[2]-v1[2]) - c_ab * c_ab;
    return del_l * del_l + c_ab * c_ab / (1 + k1 * c1) / (1 + k1 * c1) + h_ab / (1 + k2 * c1) / (1 + k2 * c1);
}

bool RecolorTool::CalSigma_r()
{
    double sum = 0;
    for (int i = 0; i < K; i++){
        for (int j = i+1; j < K; j++){
            sum += sqrt(myLabDis2(src_colors_lab[i], src_colors_lab[j]));
        }
    }
    if (sum == 0) return false;
    sum /= (K*(K-1)/2);
    sigma_r2 = sum * sum / K;
    return true;
}

void RecolorTool::CalLamda()
{
    fout << "lamda:" << endl;
    double phi[K][K];
    for (int i = 0; i < K; i++){
        for (int j = 0; j < K; j++){
            double d = Phi(src_colors_lab[i], src_colors_lab[j]);
            phi[i][j] = d;
        }
    }
    double lamdaT[K][K];
    inverseMatrix(phi, lamdaT, K);
    for (int i = 0; i < K; i++){
        for (int j = 0; j < K; j++){
            lamda[i][j] = lamdaT[j][i];
            fout << lamda[i][j] << " ";
        }
        fout << endl;
    }
}

double RecolorTool::Phi(Vec3d c0, Vec3d c1)
{
    double d = -myLabDis2(c0,c1) / sigma_r2;
    return exp(d);
}

void RecolorTool::CalW(Vec3d color, double w[])
{
    double sum_w = 0;
    for (int i = 0; i < K; i++){
        double d = 0;
        for (int j = 0; j < K; j++){
            d += lamda[i][j] * Phi(color, src_colors_lab[j]);
        }
        if (d < 0) d = 0;
        w[i] = d;
        sum_w += d;
    }

    // normalize
    // fout << "weights: ";
    for (int i = 0; i < K; i++){
        w[i] /= sum_w;
        // fout << w[i] << " ";
    }
    // fout << endl;
}

Vec3d RecolorTool::TransferColor(Vec3d lab)
{
    double l = Transferl(lab[0]);
    double w[K]; CalW(lab, w);
    Vec2d ab(0,0);
    for (int i = 0; i < K; i++){
        if (w[i] == 0) continue;
        Vec2d v = Transferab(i, lab);
        ab += v * w[i];
    }
    return Vec3d(l, ab[0], ab[1]);
}

Vec3d RecolorTool::TransferColor_CachedWeights(Vec3d rgb)
{
    int r = rgb[0] / GRID_STEP, g = rgb[1] / GRID_STEP, b = rgb[2] / GRID_STEP;
    if (rgb[0] == 255) r++;
    if (rgb[1] == 255) g++;
    if (rgb[2] == 255) b++;
    double *w = grid_weight[r][g][b];
    Vec3d lab = RGBToLab(rgb);

    double l = Transferl(lab[0]);
    Vec2d ab(0,0);
    for (int i = 0; i < K; i++){
        if (w[i] == 0) continue;
        Vec2d v = Transferab(i, lab);
        ab += v * w[i];
    }

    return Vec3d(l, ab[0], ab[1]);
}

Vec3d RecolorTool::TransferColor_CachedColor(Vec3b rgb)
{
    int pr = rgb[0] / GRID_STEP, pg = rgb[1] / GRID_STEP, pb = rgb[2] / GRID_STEP;
    int qr = rgb[0] % GRID_STEP, qg = rgb[1] % GRID_STEP, qb = rgb[2] % GRID_STEP;
    if (qr == 0 && qg == 0 && qb == 0) return grid_rgb[pr][pg][pb];
    Vec3d grids[8];
    grids[0] = Vec3d(0,0,0);
    grids[1] = Vec3d(0,0,GRID_STEP);
    grids[2] = Vec3d(0,GRID_STEP,0);
    grids[3] = Vec3d(0,GRID_STEP,GRID_STEP);
    grids[4] = Vec3d(GRID_STEP,0,0);
    grids[5] = Vec3d(GRID_STEP,0,GRID_STEP);
    grids[6] = Vec3d(GRID_STEP,GRID_STEP,0);
    grids[7] = Vec3d(GRID_STEP,GRID_STEP,GRID_STEP);

    double length[8];
    double acc_w = 1;
    for (int i = 0; i < 8; i++){
        length[i] = GetDistance(grids[i], Vec3d(qr, qg, qb));
        acc_w *= length[i];
    }
    double weight[8];
    double sum_w = 0;
    Vec3d ret(0,0,0);
    for (int i = 0; i < 8; i++){
        weight[i] = acc_w / length[i];
        sum_w += weight[i];
        int gr = pr + i % 4;
        int gg = pg + i % 2;
        int gb = pb + i % 1;
        ret += grid_rgb[gr][gg][gb] * weight[i];
    }
    ret /= sum_w;
    return ret;
}

void RecolorTool::ProcessImage()
{
    for (int y = 0; y < src_mat_lab.rows; y++){
        for (int x = 0; x < src_mat_lab.cols; x++){
            cur_mat_lab.at<Vec3b>(y, x) = TransferColor(src_mat_lab.at<Vec3b>(y, x));
        }
    }
    cvtColor(cur_mat_lab, cur_mat_rgb, CV_Lab2RGB);
}

void RecolorTool::CalGridWeights()
{
    int grid_count = 256/GRID_STEP + 1;

    for (int r = 0; r < grid_count; r++){
        for (int g = 0; g < grid_count; g++){
            for (int b = 0; b < grid_count; b++){
                uchar vr = r * GRID_STEP;
                uchar vg = g * GRID_STEP;
                uchar vb = b * GRID_STEP;
                if (r == grid_count - 1) vr = 255;
                if (g == grid_count - 1) vg = 255;
                if (b == grid_count - 1) vb = 255;
                Vec3d lab = RGBToLab(Vec3b(vr, vg, vb));
                CalW(lab, grid_weight[r][g][b]);
            }
        }
    }
}

void RecolorTool::CalGridRGB()
{
    int grid_count = 256/GRID_STEP + 1;
    for (int r = 0; r < grid_count; r++){
        for (int g = 0; g < grid_count; g++){
            for (int b = 0; b < grid_count; b++){
                uchar vr = r * GRID_STEP;
                uchar vg = g * GRID_STEP;
                uchar vb = b * GRID_STEP;
                if (r == grid_count - 1) vr = 255;
                if (g == grid_count - 1) vg = 255;
                if (b == grid_count - 1) vb = 255;
                grid_rgb[r][g][b] = TransferColor_CachedWeights(Vec3b(vr, vg, vb));
            }
        }
    }
}

void RecolorTool::SetSourceColors()
{
    src_colors_rgb[0] = Vec3b(228, 222, 201);
    src_colors_rgb[1] = Vec3b(230, 196, 76);
    src_colors_rgb[2] = Vec3b(163, 120, 33);
    src_colors_rgb[3] = Vec3b(113, 88, 66);
    src_colors_rgb[4] = Vec3b(38, 34, 27);
    for (int i = 0; i < K; i++){
        src_colors_lab[i] = RGBToLab(src_colors_rgb[i]);
        cur_colors_lab[i] = src_colors_lab[i];
        cur_colors_rgb[i] = src_colors_rgb[i];
    }
}


