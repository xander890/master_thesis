#ifndef STEPRECEIVER_H
#define STEPRECEIVER_H

#include <QObject>
#include <detector.h>
#include <QDebug>

class StepReceiver : public QObject
{
    Q_OBJECT
public:
    explicit StepReceiver(QObject *parent = 0);
    
signals:
    void stepSignal(QVariant value);
    
public slots:
    void step(int value);
    void activityChanged(int value);
};

#endif // STEPRECEIVER_H
