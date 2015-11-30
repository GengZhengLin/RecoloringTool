#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QDebug>
using namespace cv;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    graphics_scene = new QGraphicsScene(this);
    ui->image_view->setScene(graphics_scene);
    graphics_scene->addItem(&image_pixmap);

    InitDialogs();
    InitImageWinEvents();
    cur_state = S_NOIMAGE;
}

MainWindow::~MainWindow()
{
    delete ui;
}

QImage MainWindow::MatToQImage(const Mat &mat)
{
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type()==CV_8UC1)
    {
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++)
            colorTable.push_back(qRgb(i,i,i));
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    else if(mat.type()==CV_8UC3)
    {
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    else
    {
//        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

void MainWindow::ShowImage(Mat img)
{
    int cx = (ui->centralWidget->width() - img.cols) / 2;
    int cy = (ui->centralWidget->height() - img.rows) / 2;
    ui->image_view->setGeometry(cx, cy, img.cols, img.rows);
    image_pixmap.setPixmap(QPixmap::fromImage(MatToQImage(img)));

    ui->image_view->show();
}

void MainWindow::InitDialogs()
{
    gaussian_filter_dialog = new GaussianFilterDialog(this);
    connect(gaussian_filter_dialog, &GaussianFilterDialog::slider_value_changed, this, &MainWindow::on_guassian_slider_changed);
    connect(gaussian_filter_dialog, &GaussianFilterDialog::process_complete, this, &MainWindow::ProcessComplete);

    saturation_dialog = new SaturationDialog(this);
    connect(saturation_dialog, &SaturationDialog::slider_value_changed, this, &MainWindow::on_saturation_slider_changed);
    connect(saturation_dialog, &SaturationDialog::process_complete, this, &MainWindow::ProcessComplete);

    recolor_dialog = new RecolorDialog(this);
    for (int i = 0; i < 5; i++){
        connect(recolor_dialog->color_widgets[i], &RecolorWidget::color_changed, this, &MainWindow::on_color_changed);
    }
}

void MainWindow::ResetDialogs()
{
    gaussian_filter_dialog->ResetDialog();
    saturation_dialog->ResetDialog();
}

void MainWindow::InitImageWinEvents()
{
    connect(ui->image_view, &CenterImageView::mouseMove, this, [=](QMouseEvent *event){
        if (cur_state == S_LIQUIFY_TOOL){
            Point start(ui->image_view->last_click.x(), ui->image_view->last_click.y()), end(event->x(), event->y());
            liquify_tool.ProcessImage(cur_mat, start, end);
            ShowImage(cur_mat);
        }
    });
}

void MainWindow::ProcessBegin(MainWindow::WinState state)
{
    if (src_mat.empty()){
        ui->statusBar->showMessage("Please Load Image First!");
        return;
    }
    if (cur_state == S_LIQUIFY_TOOL){
        ui->image_view->setCursor(Qt::ArrowCursor);
    }
    switch(state){
    case S_MEDIAN_FILTER:
        ui->statusBar->showMessage("Median Filtering...");
        break;
    case S_GAUSSIAN_FILTER:
        ui->statusBar->showMessage("Gaussian Filtering...");
        break;
    case S_SATURATION_TOOL:
        ui->statusBar->showMessage("Changing saturation...");
        break;
    case S_SHARPEN_TOOL:
        ui->statusBar->showMessage("Sharpening...");
        break;
    case S_LIQUIFY_TOOL:
        ui->statusBar->showMessage("Liquifying...");
        ui->image_view->setCursor(Qt::OpenHandCursor);
        break;
    case S_BUFFING_TOOL:
        ui->statusBar->showMessage("Buffing...");
        break;
    case S_WHITEN_TOOL:
        ui->statusBar->showMessage("Whitening...");
        break;
    default:
        break;
    }
    cur_state = state;
    cur_mat.copyTo(last_mat);
}


void MainWindow::on_actionLoad_image_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Load Image"), "", "Image (*.bmp | *.png | *.jpg)");
    src_mat = cv::imread(filename.toStdString());
    cur_mat = src_mat.clone();
    ShowImage(cur_mat);
    ResetDialogs();
}

void MainWindow::on_actionSave_image_triggered()
{
    if (src_mat.empty()){
        ui->statusBar->showMessage("Please Load Image First!");
        return;
    }
    ui->statusBar->showMessage("Save Image...");
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), "", "Image (*.bmp | *.png | *.jpg)");
    cv::imwrite(filename.toStdString(), cur_mat);
    ui->statusBar->showMessage("Save Image Completed!");
}

void MainWindow::on_actionMedianFilter_triggered()
{
    ProcessBegin(S_MEDIAN_FILTER);
    median_filter.ProcessImage(cur_mat);
    ProcessComplete();
}

void MainWindow::on_actionGaussianFilter_triggered()
{
    ProcessBegin(S_GAUSSIAN_FILTER);
    gaussian_filter_dialog->show();
}

void MainWindow::on_guassian_slider_changed(double sigma)
{
    gaussian_filter.ProcessImage(last_mat, cur_mat, sigma);
    ShowImage(cur_mat);
}

void MainWindow::on_saturation_slider_changed(double scale)
{
    saturation_tool.ProcessImage(last_mat, cur_mat, scale);
    ShowImage(cur_mat);
}

void MainWindow::on_color_changed(QColor color)
{
    cvtColor(recolor_tool.GetCurMat(), cur_mat, CV_RGB2BGR);
    ShowImage(cur_mat);
}

void MainWindow::ProcessComplete()
{
    ShowImage(cur_mat);
    switch(cur_state){
    case S_MEDIAN_FILTER:
        ui->statusBar->showMessage("Median Complete!");
        break;
    case S_GAUSSIAN_FILTER:
        ui->statusBar->showMessage("Gaussian Complete!");
        break;
    case S_SATURATION_TOOL:
        ui->statusBar->showMessage("Saturation Complete!");
        break;
    case S_SHARPEN_TOOL:
        ui->statusBar->showMessage("Sharpening Complete!");
        break;
    case S_LIQUIFY_TOOL:
        ui->statusBar->showMessage("Liquifying Complete!");
        break;
    case S_BUFFING_TOOL:
        ui->statusBar->showMessage("Buffing Complete!");
        break;
    case S_WHITEN_TOOL:
        ui->statusBar->showMessage("Whitening Complete!");
        break;
    default:
        break;
    }
}

void MainWindow::on_actionSaturation_triggered()
{
    ProcessBegin(S_SATURATION_TOOL);
    saturation_dialog->show();
}

void MainWindow::on_actionSharpen_triggered()
{
    ProcessBegin(S_SHARPEN_TOOL);
    sharpen_tool.ProcessImage(last_mat, cur_mat);
    ProcessComplete();
}

void MainWindow::on_actionLiquidify_triggered()
{
    ProcessBegin(S_LIQUIFY_TOOL);
    ProcessComplete();
}

void MainWindow::on_actionBuffing_triggered()
{
    ProcessBegin(S_BUFFING_TOOL);
    buffing_tool.ProcessImage(cur_mat);
    ProcessComplete();
}

void MainWindow::on_actionWhitening_triggered()
{
    ProcessBegin(S_WHITEN_TOOL);
    whiten_tool.ProcessImage(cur_mat);
    ProcessComplete();
}

void MainWindow::on_actionChangeColor_triggered()
{
    
}

void MainWindow::on_actionRecolor_triggered()
{
    recolor_tool.PreprocessImage(cur_mat);

    recolor_dialog->SetRecolorTool(&recolor_tool);
    recolor_dialog->SetColors();
    recolor_dialog->show();
}
