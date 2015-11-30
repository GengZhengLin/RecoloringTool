#ifndef RECOLORDIALOG_H
#define RECOLORDIALOG_H

#include <QDialog>
#include "recolortool.h"
#include "recolorwidget.h"
namespace Ui {
class RecolorDialog;
}

class RecolorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecolorDialog(QWidget *parent = 0);
    ~RecolorDialog();
    void SetRecolorTool(RecolorTool *rt);
    void SetColors();
    QColor LabToQColor(cv::Vec3d lab);
    RecolorWidget *color_widgets[5];

private:
    Ui::RecolorDialog *ui;
    RecolorTool *recolor_tool;
};

#endif // RECOLORDIALOG_H
