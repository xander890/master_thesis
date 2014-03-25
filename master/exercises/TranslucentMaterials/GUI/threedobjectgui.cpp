#include "threedobjectgui.h"
#include "ui_threedobject.h"

ThreeDObjectGUI::ThreeDObjectGUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ThreeDObjectGUI)
{
    ui->setupUi(this);

    connect(ui->name,SIGNAL(onTextChanged(QString &)),this,SLOT(nameChanged(QString&)));
    connect(ui->position,SIGNAL(vectorChanged(CGLA::Vec3f &)),this, SLOT(positionChanged(CGLA::Vec3f&)));
    connect(ui->rotation,SIGNAL(vectorChanged(CGLA::Vec3f &)),this, SLOT(rotationChanged(CGLA::Vec3f&)));
    connect(ui->scale,SIGNAL(vectorChanged(CGLA::Vec3f &)),this, SLOT(scaleChanged(CGLA::Vec3f&)));
}

void ThreeDObjectGUI::setObject(GLGraphics::ThreeDObject *obj)
{
    this->obj = obj;
    QString name = QString(obj->name.c_str());
    this->ui->name->setPlainText(name);
    this->ui->position->init("Position: ", obj->getPosition());
    this->ui->rotation->init("Rotation: ", obj->getEulerAngles());
    this->ui->scale->init("Scale: ", obj->getScale());
    this->ui->groupBox->setChecked(obj->enabled);
}

ThreeDObjectGUI::~ThreeDObjectGUI()
{
    delete ui;
}

void ThreeDObjectGUI::nameChanged(QString &newName)
{
    if(obj)
        obj->name = newName.toStdString().c_str();
}

void ThreeDObjectGUI::positionChanged(CGLA::Vec3f &vector)
{
    if(obj)
        obj->translate(vector);
}

void ThreeDObjectGUI::rotationChanged(CGLA::Vec3f &vector)
{
    if(obj)
        obj->setEulerAngles(vector);
}

void ThreeDObjectGUI::scaleChanged(CGLA::Vec3f &vector)
{
    if(obj)
        obj->scale(vector);
}

void ThreeDObjectGUI::on_groupBox_toggled(bool arg1)
{
    if(obj)
        obj->enabled = arg1;
}
