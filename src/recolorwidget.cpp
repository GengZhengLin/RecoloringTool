#include "recolorwidget.h"
#include <QStyleOption>
#include <QPainter>
#include <QColorDialog>
#include <opencv2/opencv.hpp>
RecolorWidget::RecolorWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumWidth(40);
    setMinimumHeight(30);
}

void RecolorWidget::SetColor(QColor color)
{
    QString border = "border:2px solid rgb(80, 80, 80);\nborder-radius: 10px;\n";
    QString color_s = "background-color:"+GetRGB(color);
    QString s = border+color_s;
    setStyleSheet(s);
}

QString RecolorWidget::GetRGB(QColor color)
{
    QString s = "rgb(";
    s = s + QString::number(color.red()) + ",";
    s = s + QString::number(color.green()) + ",";
    s = s + QString::number(color.blue()) + ");";
    return s;
}

void RecolorWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void RecolorWidget::mousePressEvent(QMouseEvent *event)
{
    QColorDialog dlg;
    QColor color = dlg.getColor();
    SetColor(color);
    recolor_tool->SetColor(index, cv::Vec3b(color.red(), color.green(), color.blue()));
    emit color_changed(color);
}
