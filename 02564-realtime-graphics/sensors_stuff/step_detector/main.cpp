#include <QApplication>
#include "qmlapplicationviewer.h"
#include <QtDeclarative>

#include <stepreceiver.h>

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    StepReceiver* stepReceiver = new StepReceiver();

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/step_detector/main.qml"));
    viewer.showExpanded();

    QObject *rootObject = dynamic_cast<QObject*>(viewer.rootObject());

    QObject::connect(stepReceiver, SIGNAL(stepSignal(QVariant)), rootObject, SLOT(step(QVariant)));

    return app->exec();
}
