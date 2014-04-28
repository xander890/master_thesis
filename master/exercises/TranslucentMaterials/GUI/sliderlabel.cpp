#include "sliderlabel.h"
#include "ui_sliderlabel.h"

SliderLabel::SliderLabel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SliderLabel),
    scale(1.0f),
    minimum(0.0f),
    integerMode(false)

{
    ui->setupUi(this);
}

SliderLabel::~SliderLabel()
{
    delete ui;
}


void SliderLabel::init(const char *name, float initialValue, float minimum, float maximum, bool integerMode)
{
    ui->nameLabel->setText(name);
    this->minimum = minimum;
    this->integerMode = integerMode;

    ui->slider->setMinimum(0);
    ui->slider->setMaximum(100);
    scale = maximum - minimum;

    setValue(initialValue);
}

void SliderLabel::setValue(float value)
{
    int val = (int)((value - minimum) / scale * 100);
    if(integerMode)
        ui->valueLabel->setText(QString::number(value,'f',4));
    else
        ui->valueLabel->setText(QString::number(value));

    ui->slider->setValue(val);
}

void SliderLabel::on_slider_valueChanged(int value)
{
    float val = (float)value;
    val = minimum + (val / 100 * scale);

    if(integerMode)
    {
        ui->valueLabel->setText(QString::number((int)val));
        emit valueChanged(float(int(val)));
    }
    else
    {
        ui->valueLabel->setText(QString::number(val,'f',4));
        emit valueChanged(val);
    }

}
