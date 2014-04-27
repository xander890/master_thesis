#ifndef SLIDERLABEL_H
#define SLIDERLABEL_H

#include <QWidget>
#include <QString>

namespace Ui {
class SliderLabel;
}


class SliderLabel : public QWidget
{
    Q_OBJECT

public:
    explicit SliderLabel(QWidget *parent = 0);
    ~SliderLabel();

    void init(const char *name, float initialValue, float minimum, float maximum, bool integerLabel);
    void setValue(float value);

signals:
    void valueChanged(float newValue);

private slots:
    void on_slider_valueChanged(int value);

private:
    Ui::SliderLabel *ui;
    float scale;
    float minimum;
    bool integerMode;
};

#endif // SLIDERLABEL_H
