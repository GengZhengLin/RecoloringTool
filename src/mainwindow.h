#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <opencv2/opencv.hpp>
#include "medianfilter.h"
#include "gaussianfilter.h"
#include "gaussianfilterdialog.h"
#include "saturationdialog.h"
#include "recolordialog.h"
#include "saturationtool.h"
#include "sharpentool.h"
#include "liquifytool.h"
#include "buffingtool.h"
#include "whitentools.h"
#include "recolortool.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionLoad_image_triggered();
    void on_actionSave_image_triggered();
    void on_actionMedianFilter_triggered();
    void on_actionGaussianFilter_triggered();
    void on_actionSaturation_triggered();
    void on_actionSharpen_triggered();
    void on_actionLiquidify_triggered();
    void on_actionBuffing_triggered();
    void on_actionWhitening_triggered();
    void on_actionRecolor_triggered();
    void on_actionChangeColor_triggered();

private slots:
    void on_guassian_slider_changed(double sigma);
    void on_saturation_slider_changed(double scale);
    void on_color_changed(QColor color);
    void ProcessComplete();

private:
    enum WinState{
        S_NOIMAGE,
        S_MEDIAN_FILTER,
        S_GAUSSIAN_FILTER,
        S_SATURATION_TOOL,
        S_SHARPEN_TOOL,
        S_LIQUIFY_TOOL,
        S_BUFFING_TOOL,
        S_WHITEN_TOOL,
        S_OTHER
    };
private:
    QImage MatToQImage(const cv::Mat &mat);
    void ShowImage(cv::Mat img);
    void InitDialogs();
    void ResetDialogs();
    void InitImageWinEvents();
    void ProcessBegin(WinState state);

private:
    Ui::MainWindow *ui;
    QGraphicsScene *graphics_scene;
    QGraphicsPixmapItem image_pixmap;
    cv::Mat src_mat;
    cv::Mat cur_mat;
    cv::Mat last_mat;
    WinState cur_state;

    // dialog
    GaussianFilterDialog *gaussian_filter_dialog;
    SaturationDialog *saturation_dialog;
    RecolorDialog *recolor_dialog;

    // tools
    MedianFilter median_filter;
    GaussianFilter gaussian_filter;
    SaturationTool saturation_tool;
    SharpenTool sharpen_tool;
    LiquifyTool liquify_tool;
    BuffingTool buffing_tool;
    WhitenTools whiten_tool;
    RecolorTool recolor_tool;

};

#endif // MAINWINDOW_H
