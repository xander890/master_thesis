#ifndef CORE3_2_CONTEXT_H
#define CORE3_2_CONTEXT_H
#include <QGLContext>

struct Core4_3_context : public QGLContext {

    QGLFormat getStandard()
    {
        QGLFormat * format = new QGLFormat();
        format->setVersion(4,3);
        format->setProfile(QGLFormat::CoreProfile);

        return *format;
    }

    Core4_3_context(const QGLFormat& format, QPaintDevice* device) : QGLContext(format,device) {}
    Core4_3_context() : QGLContext(getStandard())
    {
    }

};

#endif // CORE3_2_CONTEXT_H
