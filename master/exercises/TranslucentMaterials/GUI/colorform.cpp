#include "colorform.h"
#include "ui_colorform.h"
#include <Utils/miscellaneous.h>

using namespace std;
ColorForm::ColorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorForm)
{
    ui->setupUi(this);
    connect(ui->r,SIGNAL(onTextChanged()),this,SLOT(vchange()));
    connect(ui->g,SIGNAL(onTextChanged()),this,SLOT(vchange()));
    connect(ui->b,SIGNAL(onTextChanged()),this,SLOT(vchange()));
    connect(ui->a,SIGNAL(onTextChanged()),this,SLOT(vchange()));

}

void ColorForm::init(const char *name, CGLA::Vec4f &vec)
{
    ui->name->setText(name);
    setColor(vec);
}

void ColorForm::setColor(CGLA::Vec4f &vec)
{
    ui->r->setPlainText(QString::number(vec[0], 'f', 3));
    ui->g->setPlainText(QString::number(vec[1], 'f', 3));
    ui->b->setPlainText(QString::number(vec[2], 'f', 3));
    ui->a->setPlainText(QString::number(vec[3], 'f', 3));
}

ColorForm::~ColorForm()
{
    delete ui;
}


void ColorForm::vchange()
{
    float r = clamp01(ui->r->toPlainText().toFloat());
    float g = clamp01(ui->g->toPlainText().toFloat());
    float b = clamp01(ui->b->toPlainText().toFloat());
    float a = clamp01(ui->a->toPlainText().toFloat());
    CGLA::Vec4f v = CGLA::Vec4f(r,g,b,a);
    setColor(v);
    emit vectorChanged(v);
}
