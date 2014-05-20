#ifndef PERFORMANCETIMER_H
#define PERFORMANCETIMER_H

#include <QElapsedTimer>
#include <map>
#include <vector>
#include <GLGraphics/GLHeader.h>

class PerformanceTimer
{
private:
    QElapsedTimer timer;
    uint refreshInterval;
    uint currentFrame;
    std::map<std::string, std::vector<qint64>* > timeTracker;

    bool interenabled;
    qint64 lastTime;

    void registerEvent(std::string & event, bool usePrevious, bool intermediate);
    float getAverage(std::vector<qint64> *t);

public:
    PerformanceTimer(uint refreshInterval) :
        timer(QElapsedTimer()),
        refreshInterval(refreshInterval),
        currentFrame(0),
        interenabled(true),
        lastTime(0)
    {
        timeTracker = *(new std::map<std::string, std::vector<qint64>* >());
    }

    void start();
    void end();

    void registerEvent(std::string & event);
    void registerEvent(char *event);
    float getEventAverage(std::string & event);
    void printResults(); //averages the last results
    void refresh();
    void setIntermediateEnabled(bool enabled) {this->interenabled = enabled;}


};

#endif // PERFORMANCETIMER_H
