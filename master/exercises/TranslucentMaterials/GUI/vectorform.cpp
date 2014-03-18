#include "vectorform.h"
#include "ui_vectorform.h"

VectorForm::VectorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VectorForm)
{
    ui->setupUi(this);

    ui->x->setTabChangesFocus(true);
    ui->x->setWordWrapMode(QTextOption::NoWrap);
    ui->x->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->x->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->x->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    ui->y->setTabChangesFocus(true);
    ui->y->setWordWrapMode(QTextOption::NoWrap);
    ui->y->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->y->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->y->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    ui->z->setTabChangesFocus(true);
    ui->z->setWordWrapMode(QTextOption::NoWrap);
    ui->z->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->z->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->z->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    ui->a->setTabChangesFocus(true);
    ui->a->setWordWrapMode(QTextOption::NoWrap);
    ui->a->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->a->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->a->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

}

void VectorForm::init(const char *name, CGLA::Vec2f &vec)
{
    ui->name->setText(name);
    ui->x->setPlainText(QString::number(vec[0], 'g', 5));
    ui->y->setPlainText(QString::number(vec[1], 'g', 5));
    ui->zLabel->setVisible(false);
    ui->z->setVisible(false);
    ui->aLabel->setVisible(false);
    ui->a->setVisible(false);
    type = VecType::V2;
}

void VectorForm::init(const char *name, CGLA::Vec3f &vec)
{
    ui->name->setText(name);
    ui->x->setPlainText(QString::number(vec[0], 'g', 5));
    ui->y->setPlainText(QString::number(vec[1], 'g', 5));
    ui->z->setPlainText(QString::number(vec[2], 'g', 5));
    ui->aLabel->setVisible(false);
    ui->a->setVisible(false);
    type = VecType::V3;

}

void VectorForm::init(const char *name, CGLA::Vec4f &vec)
{
    ui->name->setText(name);
    ui->x->setPlainText(QString::number(vec[0], 'g', 5));
    ui->y->setPlainText(QString::number(vec[1], 'g', 5));
    ui->z->setPlainText(QString::number(vec[2], 'g', 5));
    ui->a->setPlainText(QString::number(vec[3], 'g', 5));
    type = VecType::V2;

}

VectorForm::~VectorForm()
{
    delete ui;
}

void VectorForm::vchange()
{
    float x = ui->x->toPlainText().toFloat();
    float y = ui->y->toPlainText().toFloat();
    switch(type)
    {
        case VecType::V2 :
            emit vectorChanged(CGLA::Vec2f(x,y));
        break;
        case VecType::V3 :
        {
            float z = ui->z->toPlainText().toFloat();
            emit vectorChanged(CGLA::Vec3f(x,y,z));
        }
        break;
        case VecType::V4 :
        {
            float z = ui->z->toPlainText().toFloat();
            float a = ui->z->toPlainText().toFloat();
            emit vectorChanged(CGLA::Vec4f(x,y,z,a));
        }
        break;
    }
}


void VectorForm::on_x_textChanged()
{
    vchange();
}

void VectorForm::on_y_textChanged()
{
    vchange();
}

void VectorForm::on_z_textChanged()
{
    vchange();
}

void VectorForm::on_a_textChanged()
{
    vchange();
}
