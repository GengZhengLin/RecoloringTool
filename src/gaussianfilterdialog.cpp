#include "gaussianfilterdialog.h"
#include "ui_gaussianfilterdialog.h"
#include <QDebug>

GaussianFilterDialog::GaussianFilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GaussianFilterDialog)
{
    ui->setupUi(this);
    slider_max = 100;
    ui->size_slider->setMaximum(slider_max);
    connect(ui->size_slider, &QSlider::valueChanged, this, &GaussianFilterDialog::on_slider_value_changed);
}

GaussianFilterDialog::~GaussianFilterDialog()
{
    delete ui;
}

void GaussianFilterDialog::ResetDialog()
{
    ui->size_slider->setValue(0);
}

void GaussianFilterDialog::on_slider_value_changed(int value)
{
    double sigma = (double)value / 10.0;
    ui->size_label->setText(QString::number(sigma));
    emit slider_value_changed(sigma);
}

void GaussianFilterDialog::on_buttonBox_accepted()
{
    emit process_complete();
}
