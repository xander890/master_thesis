#ifndef SENSORS_H
#define SENSORS_H

#include <QObject>
#include <qplatformdefs.h>
#if defined(MEEGO_EDITION_HARMATTAN)
#include <QLightSensor>
#include <QAccelerometer>
#include <QCompass>
#endif

/**
  Light Sensor: http://doc.qt.nokia.com/qtmobility/qlightreading.html
  Accelerometer: http://doc.qt.nokia.com/qtmobility/qaccelerometerreading.html
  Compas: http://doc.qt.nokia.com/qtmobility/qcompassreading.html
  */


class Sensors : public QObject
{
    Q_OBJECT
public:
    explicit Sensors(QObject *parent = 0);
    double light();
    double accX();
    double accY();
    double accZ();
    double azimuth();
    double compassCalibrationLevel();

private:
#if defined(MEEGO_EDITION_HARMATTAN)
 QtMobility::QLightSensor* lightSensor;
 QtMobility::QAccelerometer* accSensor;
 QtMobility::QCompass* compass;

#endif

signals:
    void update();

public slots:
    void lightUpdated();
    void accUpdated();
    void compassUpdated();

};

#endif // SENSORS_H
