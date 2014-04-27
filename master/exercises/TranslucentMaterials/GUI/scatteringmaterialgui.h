#ifndef SCATTERINGMATERIALGUI_H
#define SCATTERINGMATERIALGUI_H

#include <QWidget>
#include "Mesh/scatteringmaterial.h"
#include "GLGraphics/ThreeDObject.h"
#include "GUI/sliderlabel.h"

namespace Ui {
class ScatteringMaterialGUI;
}

class ScatteringMaterialGUI : public QWidget
{
    Q_OBJECT

public:
    explicit ScatteringMaterialGUI(QWidget *parent = 0);
    ~ScatteringMaterialGUI();
    void setObject(GLGraphics::ThreeDObject *object);

private slots:
    void iorChanged(float ior);
    void absorptionChanged(CGLA::Vec3f & value);
    void scatteringChanged(CGLA::Vec3f & value);
    void meanCosineChanged(CGLA::Vec3f & value);


private:
    bool initialized;
    Ui::ScatteringMaterialGUI * ui;
    GLGraphics::ThreeDObject * object;
};

#endif // SCATTERINGMATERIALGUI_H
