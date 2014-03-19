#ifndef COLORFORM_H
#define COLORFORM_H

#include <QWidget>
#include "vectorform.h"
#include <CGLA/Vec2f.h>
#include <CGLA/Vec3f.h>
#include <CGLA/Vec4f.h>


namespace Ui {
class ColorForm;
}

class ColorForm : public QWidget
{
    Q_OBJECT

public:
    explicit ColorForm(QWidget *parent = 0);
    ~ColorForm();
    void init(const char * name, CGLA::Vec4f & vec);

public slots:
    void setColor(CGLA::Vec4f & vec);

signals:
    void vectorChanged(CGLA::Vec4f & vector);

private slots:
    void vchange();

private:
    Ui::ColorForm *ui;
};

#endif // COLORFORM_H
