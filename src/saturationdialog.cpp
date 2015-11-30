#include "saturationdialog.h"
#include "ui_saturationdialog.h"

SaturationDialog::SaturationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaturationDialog)
{
    ui->setupUi(this);
    saturation_scale = 100;
    ui->saturation_slider->setMaximum(200);
    ui->saturation_slider->setMinimum(0);
    SetScale(100);
    connect(ui->saturation_slider, &QSlider::valueChanged, this, &SaturationDialog::on_slider_value_changed);
}

SaturationDialog::~SaturationDialog()
{
    delete ui;
}

void SaturationDialog::SetScale(double scale)
{
    saturation_scale = scale;
    ui->saturation_slider->setValue(saturation_scale);
    ui->saturation_label->setText(QString::number(saturation_scale)+"%");
}

void SaturationDialog::ResetDialog()
{
    SetScale(100);
}

void SaturationDialog::on_slider_value_changed(int value)
{
    SetScale(value);
    double scale = (double)value / 100.0;
    emit slider_value_changed(scale);
}

void SaturationDialog::on_buttonBox_accepted()
{
    emit process_complete();
}
