#pragma once

#include "stdafx.h"

// Provide a unified interface to a set of interfaces in a subsystem. Facade defines a higher-level interface that makes the subsystem easier to use.

class SubSystemOne
{
    public:
        void MethodOne()
        {
            cout<< "method one" << endl;
        }
};

class SubSystemTwo
{
public:
    void MethodTwo()
    {
        cout << "method Two" << endl;
    }
};

class SubSystemThree
{
public:
    void MethodThree()
    {
        cout << "method Three" << endl;
    }
};

class Facade
{
    public:
        Facade()
        {
            auto pOne = new SubSystemOne;
            auto pTwo = new SubSystemTwo;
            auto pThree = new SubSystemThree;
        }

        void MethodA()
        {
            cout<<"Facade:MethodA" <<endl;
            pOne->MethodOne();
            pThree->MethodThree();
        }

        void MethodB()
        {
            std::cout << "Facade::MethodB" << std::endl;
            pTwo->MethodTwo();
            pThree->MethodThree();
        }

private:
    SubSystemOne *pOne;
    SubSystemTwo *pTwo;
    SubSystemThree *pThree;
};

void facadePattern_Example()
{
    Facade *pFacade = new Facade();

    pFacade->MethodA();
    pFacade->MethodB();
}