#ifndef GAUSSIANFILTERDIALOG_H
#define GAUSSIANFILTERDIALOG_H

#include <QDialog>

namespace Ui {
class GaussianFilterDialog;
}

class GaussianFilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GaussianFilterDialog(QWidget *parent = 0);
    ~GaussianFilterDialog();
    void ResetDialog();

signals:
    void slider_value_changed(double sigma);
    void process_complete();

private:
    Ui::GaussianFilterDialog *ui;
    int slider_max;

private slots:
    void on_slider_value_changed(int value);
    void on_buttonBox_accepted();
};

#endif // GAUSSIANFILTERDIALOG_H
