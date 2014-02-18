#include "sensors.h"
#include <QDebug>

Sensors::Sensors(QObject *parent) :
    QObject(parent)
{
#if defined(MEEGO_EDITION_HARMATTAN)
    lightSensor = new QtMobility::QLightSensor(this);
    connect(lightSensor,SIGNAL(readingChanged()),this,SLOT(lightUpdated()));

    accSensor = new QtMobility::QAccelerometer(this);
    connect(accSensor,SIGNAL(readingChanged()),this,SLOT(accUpdated()));

    compass = new QtMobility::QCompass(this);
    connect(compass,SIGNAL(readingChanged()),this,SLOT(compassUpdated()));

    lightSensor->start();
    accSensor->start();
    compass->start();

#endif

}



void Sensors::lightUpdated()
{
    emit update();
}

void Sensors::accUpdated()
{
    emit update();
}

void Sensors::compassUpdated()
{
    emit update();
}

double Sensors::accX()
{
#if defined(MEEGO_EDITION_HARMATTAN)
    return accSensor->reading()->x();
#endif
    return 0.0;
}

double Sensors::light()
{
#if defined(MEEGO_EDITION_HARMATTAN)
    return lightSensor->reading()->lux();
#endif
    return 0.0;

}

double Sensors::accY()
{
#if defined(MEEGO_EDITION_HARMATTAN)
    return accSensor->reading()->y();
#endif
    return 0.0;
}

double Sensors::accZ()
{
#if defined(MEEGO_EDITION_HARMATTAN)
    return accSensor->reading()->z();
#endif
    return 0.0;
}

double Sensors::azimuth()
{
#if defined(MEEGO_EDITION_HARMATTAN)
    return compass->reading()->azimuth();
#endif
    return 0.0;
}

double Sensors::compassCalibrationLevel()
{
#if defined(MEEGO_EDITION_HARMATTAN)
    return compass->reading()->calibrationLevel();
#endif
    return 0.0;
}

