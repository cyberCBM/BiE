#pragma once

#include "stdafx.h"

// Observer Pattern's intent is to define a one-to-many dependency 
// between objects so that when one object changes state, 
// all its dependents are notified and updated automatically.

class Subject;
class Observer
{
    public:
        Observer(){}
        ~Observer() {}
        virtual void Update(Subject* sub)=0;
};
class Subject
{
public:
    Subject() {};
    virtual ~Subject() {};
    virtual void Attach(Observer*o)
    {
        _observers.push_back(o);
    }
    virtual void Detach(Observer*o)
    {
        int count = _observers.size();
        int i;
        auto itr = std::find(_observers.begin(), _observers.end(), o);
        if (itr != _observers.end())
        {
            _observers.erase(itr);
        }

        /*for (i = 0; i < count; i++) {
            if (_observers[i] == o)
                break;
        }
        if (i < count)
            _observers.erase(_observers.begin() + i);*/

    }
    virtual void Notify() {
        //int count = _observers.size();
        for (auto obs : _observers)
        {
            obs->Update(this);
        }
        /*for (int i = 0; i < count; i++)
            (_observers[i])->Update(this);*/
    }
private:
    vector<Observer*> _observers;
};

class ClockTimer : public Subject
{
public:
    ClockTimer() { _strtime(tmpbuf); };
    int GetHour();
    int GetMinute();
    int GetSecond();
    void Tick();
private:
    char tmpbuf[128];
};

/* Set time zone from TZ environment variable. If TZ is not set,
* the operating system is queried to obtain the default value
* for the variable.
*/
void ClockTimer::Tick()
{
    _tzset();

    // Obtain operating system-style time. 
    _strtime(tmpbuf);
    Notify();
}

int ClockTimer::GetHour()
{
    char timebuf[128];
    strncpy(timebuf, tmpbuf, 2);
    timebuf[2] = NULL;

    return atoi(timebuf);
}

int ClockTimer::GetMinute()
{
    char timebuf[128];
    strncpy(timebuf, tmpbuf + 3, 2);
    timebuf[2] = NULL;

    return atoi(timebuf);
}

int ClockTimer::GetSecond()
{
    char timebuf[128];
    strncpy(timebuf, tmpbuf + 6, 2);
    timebuf[2] = NULL;

    return atoi(timebuf);
}


class DigitalClock : public Observer
{
public:
    DigitalClock(ClockTimer *s)
    {
        _subject = s;
        _subject->Attach(this);
    }

    ~DigitalClock() {
        _subject->Detach(this);
    }
    void Update(Subject *sub) {
        if (sub == _subject)
            Draw();
    }

    void Draw() {
        int hour = _subject->GetHour();
        int minute = _subject->GetMinute();
        int second = _subject->GetSecond();

        cout << "Digital time is " << hour << ":"
            << minute << ":"
            << second << endl;
    }
private:
    ClockTimer *_subject;
};

class AnalogClock : public Observer
{
public:
    AnalogClock(ClockTimer *s)
    {
        _subject = s;
        _subject->Attach(this);
    }
    ~AnalogClock()
    {
        _subject->Detach(this);
    }
    void Update(Subject *sub)
    {
        if (sub == _subject)
            Draw();
    }

    void Draw() 
    {
        int hour = _subject->GetHour();
        int minute = _subject->GetMinute();
        int second = _subject->GetSecond();

        cout << "Analog time is " << hour << ":"
            << minute << ":"
            << second << endl;
    }
private:
    ClockTimer *_subject;
};

void observerPattern_Example()
{
    ClockTimer timer;

    DigitalClock digitalClock(&timer);
    AnalogClock analogClock(&timer);

    timer.Tick();
}
