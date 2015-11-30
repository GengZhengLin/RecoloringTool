#ifndef SATURATIONDIALOG_H
#define SATURATIONDIALOG_H

#include <QDialog>

namespace Ui {
class SaturationDialog;
}

class SaturationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaturationDialog(QWidget *parent = 0);
    ~SaturationDialog();
    double GetScale(){return saturation_scale;}
    void SetScale(double scale);
    void ResetDialog();

signals:
    void slider_value_changed(double scale);
    void process_complete();

private:
    Ui::SaturationDialog *ui;
    double saturation_scale;

private slots:
    void on_slider_value_changed(int value);
    void on_buttonBox_accepted();
};

#endif // SATURATIONDIALOG_H
