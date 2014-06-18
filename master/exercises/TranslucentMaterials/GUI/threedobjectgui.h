#ifndef THREEDOBJECT_H
#define THREEDOBJECT_H

#include <QWidget>
#include <GLGraphics/ThreeDObject.h>

namespace Ui {
class ThreeDObjectGUI;
}

class ThreeDObjectGUI : public QWidget
{
    Q_OBJECT

public:
    explicit ThreeDObjectGUI(QWidget *parent = 0);
    void setObject(GLGraphics::ThreeDObject * obj);
    ~ThreeDObjectGUI();

private slots:
    void nameChanged(QString & newName);
    void positionChanged(CGLA::Vec3f & vector);
    void rotationChanged(CGLA::Vec3f & vector);
    void scaleChanged(CGLA::Vec3f & vector);

    void on_groupBox_toggled(bool arg1);

    void on_modelcombobox_activated(const QString &arg1);

signals:
    void modelChanged(QString & newModel);

private:
    Ui::ThreeDObjectGUI *ui;
    GLGraphics::ThreeDObject *obj;
};

#endif // THREEDOBJECT_H
