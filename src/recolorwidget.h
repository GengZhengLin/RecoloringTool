#ifndef RECOLORWIDGET_H
#define RECOLORWIDGET_H
#include <QWidget>
#include "recolortool.h"
class RecolorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RecolorWidget(QWidget *parent = 0);
    void SetColor(QColor color);
    void SetIndex(int i){index = i;}
    int GetIndex(){return index;}
    void SetRecolorTool(RecolorTool *rt){recolor_tool = rt;}
signals:
    void color_changed(QColor color);

private:
    int index;
    RecolorTool *recolor_tool;
    QString GetRGB(QColor color);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent * event);
};

#endif // RECOLORWIDGET_H
