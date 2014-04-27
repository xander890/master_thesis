#include "scatteringmaterialgui.h"
#include "ui_scatteringmaterialgui.h"
#include "vectorform.h"

using namespace std;
using namespace CGLA;

ScatteringMaterialGUI::ScatteringMaterialGUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScatteringMaterialGUI)
{
    ui->setupUi(this);
    connect(ui->iorLabel,SIGNAL(valueChanged(float)),this,SLOT(iorChanged(float)));
    connect(ui->absorptionLabel,SIGNAL(vectorChanged(CGLA::Vec3f&)),this,SLOT(absorptionChanged(CGLA::Vec3f&)));
    connect(ui->scatteringLabel,SIGNAL(vectorChanged(CGLA::Vec3f&)),this,SLOT(scatteringChanged(CGLA::Vec3f&)));
    connect(ui->meanCosineLabel,SIGNAL(vectorChanged(CGLA::Vec3f&)),this,SLOT(meanCosineChanged(CGLA::Vec3f&)));
    initialized = false;
}

ScatteringMaterialGUI::~ScatteringMaterialGUI()
{
    delete ui;
}

void ScatteringMaterialGUI::setObject(GLGraphics::ThreeDObject *object)
{
    ui->absorptionLabel->init("Absorption: ", Vec3f(0.0f));
    ui->scatteringLabel->init("Scattering: ",Vec3f(0.0f));
    ui->meanCosineLabel->init("g :", Vec3f(0.0f));
    ui->iorLabel->init("ior", 1.3, 0.5,3.0, false);

    this->object = object;
    Mesh::ScatteringMaterial * material = (Mesh::ScatteringMaterial *) object->mesh.getMaterial();
    ui->absorptionLabel->setValue(material->absorption);
    ui->meanCosineLabel->setValue(material->meancosine);
    ui->scatteringLabel->setValue(material->scattering);
    ui->iorLabel->setValue(material->indexOfRefraction);
    initialized = true;
}

void ScatteringMaterialGUI::iorChanged(float ior)
{
    if(!initialized)
        return;
    Mesh::ScatteringMaterial * material = (Mesh::ScatteringMaterial *) object->mesh.getMaterial();
    material->indexOfRefraction = ior;
    material->computeCoefficients();
}

void ScatteringMaterialGUI::absorptionChanged(CGLA::Vec3f &value)
{
    if(!initialized)
        return;

    Mesh::ScatteringMaterial * material = (Mesh::ScatteringMaterial *) object->mesh.getMaterial();
    material->absorption = value;
    material->computeCoefficients();
}

void ScatteringMaterialGUI::scatteringChanged(CGLA::Vec3f &value)
{
    if(!initialized)
        return;

    Mesh::ScatteringMaterial * material = (Mesh::ScatteringMaterial *) object->mesh.getMaterial();
    material->scattering = value;
    material->computeCoefficients();

}

void ScatteringMaterialGUI::meanCosineChanged(CGLA::Vec3f &value)
{
    if(!initialized)
        return;

    Mesh::ScatteringMaterial * material = (Mesh::ScatteringMaterial *) object->mesh.getMaterial();
    material->meancosine = value;
    material->computeCoefficients();

}
