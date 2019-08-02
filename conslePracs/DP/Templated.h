#pragma once

#include "stdafx.h"

// Define the skeleton of an algorithm in an operation, deferring some steps to subclasses. 
// Template Method lets subclasses redefine certain steps of an algorithm without changing the algorithm's structure.

class AbstractClass
{
public:
    void templateMethod() {
        oper1();
        oper2();
        concreteOper();
        hook();
    }
    
    void concreteOper() {
        cout << "Mandatory Operations for all ConcreteClasses" << endl;
    }
private:
    virtual void hook() {}
    virtual void oper1() = 0;
    virtual void oper2() = 0;
};

class ConcreteClassA : public AbstractClass
{
private:
    void oper1() {
        cout << "primitiveOp1 A" << endl;
    }
    void oper2() {
        cout << "primitiveOp2 A" << endl;
    }
};

class ConcreteClassB : public AbstractClass
{
private:
    void oper1() {
        cout << "primitiveOp1 B" << endl;
    }
    void oper2() {
        cout << "primitiveOp2 B" << endl;
    }
    void hook() {
        cout << "hook() B" << endl;
    }
};

void templatePattern_Example()
{
    ConcreteClassA ca;
    ConcreteClassB cb;
    ca.templateMethod();
    cb.templateMethod();
}