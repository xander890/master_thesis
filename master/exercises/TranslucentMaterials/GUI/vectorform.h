#ifndef VECTORFORM_H
#define VECTORFORM_H

#include <QWidget>
#include <CGLA/Vec2f.h>
#include <CGLA/Vec3f.h>
#include <CGLA/Vec4f.h>

namespace Ui {
class VectorForm;
}

class VectorForm : public QWidget
{
    Q_OBJECT

public:
    enum VecType {V2,V3,V4};
    VecType type;
    explicit VectorForm(QWidget *parent = 0);
    void init(const char * name, CGLA::Vec2f & vec);
    void init(const char * name, CGLA::Vec3f & vec);
    void init(const char * name, CGLA::Vec4f & vec);
    ~VectorForm();

signals:
    void vectorChanged(CGLA::Vec2f & vector);
    void vectorChanged(CGLA::Vec3f & vector);
    void vectorChanged(CGLA::Vec4f & vector);

private slots:
    void on_x_textChanged();

    void on_y_textChanged();

    void on_z_textChanged();

    void on_a_textChanged();

private:
    Ui::VectorForm *ui;
    void vchange();
};

#endif // VECTORFORM_H
