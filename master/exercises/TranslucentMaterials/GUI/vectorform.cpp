#include "vectorform.h"
#include "ui_vectorform.h"

using namespace CGLA;

VectorForm::VectorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VectorForm)
{
    ui->setupUi(this);
    connect(ui->x,SIGNAL(onTextChanged()),this,SLOT(vchange()));
    connect(ui->y,SIGNAL(onTextChanged()),this,SLOT(vchange()));
    connect(ui->z,SIGNAL(onTextChanged()),this,SLOT(vchange()));
    connect(ui->a,SIGNAL(onTextChanged()),this,SLOT(vchange()));

}

void VectorForm::init(const char *name, Vec2f &vec)
{
    ui->name->setText(name);
    ui->x->setPlainText(QString::number(vec[0], 'f', 3));
    ui->y->setPlainText(QString::number(vec[1], 'f', 3));
    ui->zLabel->setVisible(false);
    ui->z->setVisible(false);
    ui->aLabel->setVisible(false);
    ui->a->setVisible(false);
    type = VecType::V2;
}

void VectorForm::init(const char *name, Vec3f &vec)
{
    ui->name->setText(name);
    ui->x->setPlainText(QString::number(vec[0], 'f', 3));
    ui->y->setPlainText(QString::number(vec[1], 'f', 3));
    ui->z->setPlainText(QString::number(vec[2], 'f', 3));
    ui->aLabel->setVisible(false);
    ui->a->setVisible(false);
    type = VecType::V3;

}

void VectorForm::init(const char *name, Vec4f &vec)
{
    ui->name->setText(name);
    ui->x->setPlainText(QString::number(vec[0], 'f', 3));
    ui->y->setPlainText(QString::number(vec[1], 'f', 3));
    ui->z->setPlainText(QString::number(vec[2], 'f', 3));
    ui->a->setPlainText(QString::number(vec[3], 'f', 3));
    type = VecType::V4;

}

VectorForm::~VectorForm()
{
    delete ui;
}
void VectorForm::setValue(Vec2f &vec)
{
    ui->x->setPlainText(QString::number(vec[0], 'f', 3));
    ui->y->setPlainText(QString::number(vec[1], 'f', 3));

}
void VectorForm::setValue(Vec3f &vec)
{
    ui->x->setPlainText(QString::number(vec[0], 'f', 3));
    ui->y->setPlainText(QString::number(vec[1], 'f', 3));
    ui->z->setPlainText(QString::number(vec[2], 'f', 3));

}

void VectorForm::setValue(Vec4f &vec)
{
    ui->x->setPlainText(QString::number(vec[0], 'f', 3));
    ui->y->setPlainText(QString::number(vec[1], 'f', 3));
    ui->z->setPlainText(QString::number(vec[2], 'f', 3));
    ui->a->setPlainText(QString::number(vec[3], 'f', 3));
}

void VectorForm::vchange()
{
    float x = ui->x->toPlainText().toFloat();
    float y = ui->y->toPlainText().toFloat();
    float z = ui->z->toPlainText().toFloat();
    float a = ui->a->toPlainText().toFloat();


    switch(type)
    {
        case VectorForm::VecType::V2 :
        {
            Vec2f vec = Vec2f(x,y);
            setValue(vec);
            emit vectorChanged(vec);
        }
        break;
        case VectorForm::VecType::V3 :
        {
            Vec3f vec = Vec3f(x,y,z);
            setValue(vec);
            emit vectorChanged(vec);
        }
        break;
        case VectorForm::VecType::V4 :
        {
            Vec4f vec = Vec4f(x,y,z,a);
            setValue(vec);
            emit vectorChanged(vec);
        }
        break;
    }
}


