#include "stepreceiver.h"

StepReceiver::StepReceiver(QObject *parent) :
    QObject(parent)
{
    Detector* stepDetector = new Detector();
    stepDetector->setRunning(true);

    QObject::connect(stepDetector, SIGNAL(step(int)), this, SLOT(step(int)));
    QObject::connect(stepDetector, SIGNAL(activityChanged(int)), this, SLOT(activityChanged(int)));
}

void StepReceiver::step(int value)
{
    qDebug() << Q_FUNC_INFO << value;
    emit stepSignal(value);
}

void StepReceiver::activityChanged(int value)
{
    qDebug() << Q_FUNC_INFO << value;
}
