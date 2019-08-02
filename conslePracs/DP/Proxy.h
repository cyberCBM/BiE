#pragma once

#include"stdafx.h"

// Provide a surrogate or placeholder for another object to control access to it.

class PSubject
{
    public:
    virtual void request()=0;
    virtual ~PSubject(){}
};

class RealSubject : public PSubject
{
    public:
        void request()
        {
            cout << "RealSubject -> Request" << endl;
        }
};

class Proxy : public PSubject
{
private:
    PSubject* realSubject;
public:
    Proxy() : realSubject(new RealSubject())
    {}
    ~Proxy() {
        delete realSubject;
    }
    // Forward calls to the RealSubject:
    void request() {
        realSubject->request();
    }
};

void ProxyPattern_Example()
{
    Proxy p;
    p.request();
}
