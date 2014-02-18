#ifndef DETECTOR_H
#define DETECTOR_H

#include <QAccelerometer>
#include <QTimer>
#include <QThread>
#include <CGLA/Vec3f.h>

QTM_USE_NAMESPACE

// Vec3f as QObject
class QVec3f : public QObject, public CGLA::Vec3f
 {
 public:
    QVec3f(float x=0, float y=0, float z=0):CGLA::Vec3f(x,y,z){}
    QVec3f(const QVec3f & o):QObject(),CGLA::Vec3f(o[0],o[1],o[2]){}
    QVec3f & operator=(const QVec3f & o){
        (*this)[0] = o[0];
        (*this)[1] = o[1];
        (*this)[2] = o[2];
        return *this;
    }
    Q_OBJECT
 };

/// Step Detector.
class Detector: public QObject {
    Q_OBJECT
    Q_ENUMS(Activity)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(int activity READ activity NOTIFY activityChanged)
    Q_PROPERTY(int minReadingDiff READ minReadingDiff WRITE setMinReadingDiff NOTIFY minReadingDiffChanged)
    Q_PROPERTY(QVec3f acceleration READ acceleration WRITE setAcceleration NOTIFY accelerationChanged)

public:
    enum Activity {
        Idle,
        Unknown,
        Walking
    };
    explicit Detector(QObject *parent = 0);
    void init(); ///< Initialize the accelerometer.

    bool running() {return running_;}
    int activity() {return (int)activity_;}
    int minReadingDiff() {return minReadingDiff_;}
    QVec3f acceleration(){ return acceleration_; }
signals:
    void runningChanged(bool value);
    void step(int value);
    void activityChanged(int value);
    void minReadingDiffChanged(int value);
    void accelerationChanged(QVec3f v);
public slots:
    void setAcceleration(QVec3f v);
    void setActivity(Activity value);
    void setRunning(bool value);
    void setMinReadingDiff(int value);
    void detect(); ///< Detect step.
    void reset(); ///< Reset all parameters to default.
    void report(); ///< Report step count.

    /// Guess current activity walking and adapt parameters to it.
    void adapt(qreal reading, qint64 timeStamp);

public:
    QAccelerometer *accelerometer_;
    bool increasing_; ///< True if accelerometer readings are increasing.
    qreal lastReading_; ///< Last accelerometer reading.
    qint64 lastStepTime_; ///< Time of the last step (ms since Epoch).
    qint64 minStepTimeDiff_; ///< Minimum time between steps (ms).
    unsigned readingCount_; ///< Reading count.
    qreal totalReading_; ///< Sum of the last N accelerometer readings.
    Activity activity_; ///< Current activity.
    bool running_; ///< True if the DetectorWorker is running.
    int minReadingDiff_; ///< Minimum reading delta to consider a step.
    int stepCount_; ///< Total unreported step count.
    QTimer *reporter_; ///< Step reporter timer.
    QVec3f acceleration_;
};

#endif // DETECTOR_H
