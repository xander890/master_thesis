#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionSave_Image_triggered();

    void on_actionQuit_triggered();

    void on_intensity_valueChanged(int value);

    void timeMeasurement(int millis);

    void radiusChanged(float value);
    void samplesChanged(float value);
    void epsilonGBufferChanged(float value);
    void epsilonCombinationChanged(float value);
    void shadowBiasChanged(float value);
    void LODChanged(float value);

    void on_plusXcheck_toggled(bool checked);

    void on_minusXcheck_toggled(bool checked);

    void on_plusYcheck_toggled(bool checked);

    void on_minusYcheck_toggled(bool checked);

    void on_plusZcheck_toggled(bool checked);

    void on_minusZcheck_toggled(bool checked);

    void on_cubemapVisible_toggled(bool checked);

    void on_jensenbutton_clicked();

    void on_directionalbutton_clicked();

private:
    Ui::MainWindow *ui;
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
};

#endif // MAINWINDOW_H
