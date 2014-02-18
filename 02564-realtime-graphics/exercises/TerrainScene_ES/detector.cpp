#include <QtGlobal>
#include <QDebug>
#include <iostream>

#include "detector.h"

#if defined(Q_OS_SYMBIAN)
const qreal MIN_READING_DIFF = 30; ///< Minimum acceleration difference.
#else
const qreal MIN_READING_DIFF = 10; ///< Minimum acceleration difference.
#endif

const qint64 MIN_WALKING_STEP_TIME_DIFF = 250; ///< Minimum time difference between steps while walking (ms).

#if defined(Q_OS_SYMBIAN)
const int DATA_RATE_WALKING = 0; ///< Accelerometer data rate for walking (Hz).
#else
const int DATA_RATE_WALKING = 10; ///< Accelerometer data rate for walking (Hz).
#endif

const qint64 IDLE_TIME = 300; ///< Set activity to Idle after this time (ms).
const int REPORTER_INTERVAL = 150; ///< How often steps are reported.

Detector::Detector(QObject *parent)
    : QObject(parent), accelerometer_(0), running_(false), minReadingDiff_(MIN_READING_DIFF), acceleration_(0,0,0){
    reset();
    reporter_ = new QTimer(this);
    reporter_->setSingleShot(false);
    reporter_->setInterval(REPORTER_INTERVAL);
    connect(reporter_, SIGNAL(timeout()), this, SLOT(report()));
}

void Detector::report() {
    if (stepCount_) {
        emit step(stepCount_);
        stepCount_ = 0;
    }
}

void Detector::init() {
    if (accelerometer_) {
        return;
    }
    accelerometer_ = new QAccelerometer(this);
    accelerometer_->connectToBackend();
    accelerometer_->setProperty("alwaysOn", true);
    // accelerometer_->setProperty("maximumReadingCount", 10);
    accelerometer_->setProperty("processAllReadings", true);

    qDebug() << "Detector::Detector: Available rates:";
    foreach (qrange range, accelerometer_->availableDataRates()) {
        qDebug() << "" << range.first << "-" << range.second;
    }
    qDebug() << "Detector::Detector: Sensor ID:" << accelerometer_->identifier();
    connect(accelerometer_, SIGNAL(readingChanged()), this, SLOT(detect()));
}

void Detector::setRunning(bool v) {
    if (v != running_) {
        init();
        if (v) {
            reset();
            accelerometer_->setDataRate(DATA_RATE_WALKING);
            accelerometer_->start();
            reporter_->start();
        } else {
            reporter_->stop();
            accelerometer_->stop();
            setActivity(Idle);
        }
        running_ = v;
        emit runningChanged(v);
    }
}

void Detector::detect() {
    if (accelerometer_->isConnectedToBackend()){
        QAccelerometerReading *r = accelerometer_->reading();
        qreal x = r->x();
        qreal y = r->y();
        qreal z = r->z();
        qreal reading = x * x + y * y + z * z;
        qreal readingDiff = reading - lastReading_;

        setAcceleration(QVec3f(x,y,z));

        // Get the time of the reading, work around system clock going backwards
        qint64 now = (qint64)r->timestamp() / 1000;
        if (now < lastStepTime_) {
            qDebug() << "Detector::detect: Clock going backwards: Reading's time is" << now;
            lastStepTime_ = now;
        }

        // Guess current activity
        adapt(reading, now);

        // Filter out small changes
        if (qAbs(readingDiff) < minReadingDiff_) {
            return;
        }

        // Detect peak
        bool nowIncreasing = readingDiff > 0;
        if (increasing_ && !nowIncreasing) {
            // If didn't peak too early, register a step, and adapt to current activity
            qint64 timeDiff = now - lastStepTime_;
            if (timeDiff > minStepTimeDiff_) {
                ++stepCount_;
                lastStepTime_ = now;
                if (activity_ ==Idle) {
                    setActivity(Walking);
                }
            }
        }

        increasing_ = nowIncreasing;
        lastReading_ = reading;
    }
}

void Detector::adapt(qreal reading, qint64 timeStamp) {
    totalReading_ += reading;
    int stepInterval = 4;
    if (++readingCount_ % stepInterval) {
        return;
    }

    qreal averageReading = totalReading_ / stepInterval;
    totalReading_ = 0;

    bool isStepping = (timeStamp - lastStepTime_) < IDLE_TIME;

    setActivity(isStepping? Walking: Idle);
}

void Detector::reset() {
    increasing_ = false;
    lastReading_ = 100000.;
    lastStepTime_ = 0;
    minStepTimeDiff_ = MIN_WALKING_STEP_TIME_DIFF;
    readingCount_ = 0;
    totalReading_ = 0;
    stepCount_ = 0;
    setActivity(Idle);
}

void Detector::setAcceleration(QVec3f f){
    acceleration_ = f;
    emit accelerationChanged(f);
}

void Detector::setActivity(Activity v) {
    if (activity_ != v) {
        activity_ = v;
        emit activityChanged((int)v);
    }
}

void Detector::setMinReadingDiff(int diff) {
    if (diff <= 0) {
        diff = MIN_READING_DIFF;
    }
    if (diff != minReadingDiff_) {
        minReadingDiff_ = diff;
        emit minReadingDiffChanged(diff);
    }
}
