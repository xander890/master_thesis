#include "performancetimer.h"
#include <iostream>
#include <string>
#include <QString>
using namespace std;



void PerformanceTimer::start()
{
    glFinish();
    lastTime = 0;
    timer.start();
}

void PerformanceTimer::end()
{
    glFinish();

    registerEvent(string("Total time"), false, true);
    currentFrame = (currentFrame + 1) % refreshInterval;

    if(currentFrame == 0)
    {
        printResults();
    }

    timer.invalidate();
}

void PerformanceTimer::registerEvent(string &event, bool usePrevious, bool intermediate)
{
    if(!intermediate)
        return;
    glFinish();
    map<string,vector<qint64>* >::iterator it = timeTracker.find(event);
    vector<qint64>* found;
    if(it != timeTracker.end())
    {
       //element found
        found = it->second;
    }
    else
    {
        timeTracker[event] = (new vector<qint64>(refreshInterval));
        found = timeTracker[event];
        std::fill(found->begin(),found->end(), -1);
    }

    qint64 time = timer.nsecsElapsed();
    if(usePrevious)
    {
        found->at(currentFrame) = time - lastTime;
        lastTime = time;
    }
    else
    {
        found->at(currentFrame) = time;
    }
}

void PerformanceTimer::registerEvent(string &event)
{
    registerEvent(event,true, interenabled);
}

void PerformanceTimer::registerEvent(char *event)
{
    registerEvent(string(event));
}

float PerformanceTimer::getAverage(std::vector<qint64> *found)
{
    vector<qint64>::iterator vec_iter;
    qint64 sum = 0;
    uint samples = 0;
    for(vec_iter = found->begin(); vec_iter != found->end(); vec_iter++)
    {
        qint64 num = *vec_iter;
        if(num != -1)
        {
            sum += num;
            samples++;
        }
    }
    return float(sum) / samples;
}

float PerformanceTimer::getEventAverage(string &event)
{
    map<string,vector<qint64>* >::iterator it = timeTracker.find(event);
    if(it != timeTracker.end())
    {
        vector<qint64>* found = it->second;
        return getAverage(found);
    }
    else
    {
        std::cout << "Error: event " << event.c_str() << " not found in timer.";
    }
    return 0.0f;
}

void PerformanceTimer::printResults()
{
    map<string,vector<qint64>* >::iterator map_iter;

    std::cout << "Timer measurements: " << std::endl;
    for(map_iter=timeTracker.begin(); map_iter!=timeTracker.end(); ++map_iter)
    {
        float avg = getAverage(map_iter->second) * 0.000001f; //nsecs --> millis
        QString a = QString::number(avg,'f', 3);
        std::cout << "[" << map_iter->first.c_str() << "]: " << a.toStdString().c_str() << std::endl;
    }
}

void PerformanceTimer::refresh()
{
    map<string,vector<qint64>* >::iterator map_iter;

    std::cout << "Timer measurements: " << std::endl;
    for(map_iter=timeTracker.begin(); map_iter!=timeTracker.end(); ++map_iter)
    {
        vector<qint64>* found = map_iter->second;
        std::fill(found->begin(),found->end(), -1);
    }
    currentFrame = 0;
    lastTime = 0;
}



















