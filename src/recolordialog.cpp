#include "recolordialog.h"
#include "ui_recolordialog.h"


RecolorDialog::RecolorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RecolorDialog)
{
    ui->setupUi(this);
    color_widgets[0] = ui->color_1;
    color_widgets[1] = ui->color_2;
    color_widgets[2] = ui->color_3;
    color_widgets[3] = ui->color_4;
    color_widgets[4] = ui->color_5;
    for (int i = 0; i < 5; i++){
        color_widgets[i]->SetIndex(i);
    }
}

RecolorDialog::~RecolorDialog()
{
    delete ui;
}

void RecolorDialog::SetRecolorTool(RecolorTool *rt)
{
    recolor_tool = rt;
    for (int i = 0; i < 5; i++){
        color_widgets[i]->SetRecolorTool(rt);
    }
}

void RecolorDialog::SetColors()
{
    for (int i = 0; i < 5; i++){
        cv::Vec3b v = recolor_tool->GetColor(i);
        QColor color(v[0], v[1], v[2]);
        color_widgets[i]->SetColor(color);
    }
}

QColor RecolorDialog::LabToQColor(cv::Vec3d lab)
{
    // lab to rgb
    cv::Mat lab_mat(1, 1, CV_8UC3); lab_mat.at<cv::Vec3b>(0,0) = lab;
    cv::Mat rgb_mat(1, 1, CV_8UC3);
    cv::cvtColor(lab_mat, rgb_mat, CV_Lab2RGB);
    cv::Vec3b rgb_color = rgb_mat.at<cv::Vec3b>(0,0);
    QColor r(rgb_color[0], rgb_color[1], rgb_color[2]);
    return r;
}
