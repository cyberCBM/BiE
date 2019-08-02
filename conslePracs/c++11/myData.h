#pragma once

#include "stdafx.h"

class MyData
{
private:
    std::string objname;
public:
    MyData(const char * string) :objname(string)
    {
        //throw (runtime_error("Exeption from Constructor"));
        std::cout << "I am in Constructor of " << objname << endl;
    }
    MyData(const MyData &rhs)
    {
        std::cout << "I am in copy Constructor of " << objname << rhs.objname << endl;
    }

    MyData(const MyData &&rhs) : objname(rhs.objname)
    {
        std::cout << "I am in copy move Constructor of " << rhs.objname << endl;
    }

    MyData operator = (MyData &&rhs)
    {
        std::cout << "I am in  move assignment of " << objname << endl;
        objname = rhs.objname;
    }

    MyData operator = (const MyData &rhs)
    {
        std::cout << "I am in  assignment of" << objname << rhs.objname << endl;
    }

    ~MyData()
    {
        cout << "I am in destructor of " << this << " " << objname << endl;
    }
};

void CalculateTrajectory()
{
    //auto mydatapointer = new MyData("GPS Data");
    unique_ptr<MyData> uniqueptr(new MyData("GPS Data"));
    //Some logic goes here
    //Exception is thrown.
    //throw ("I Did not handle the leak properly");
}

void ExamplesOfCreation()
{
    //Example 1: Demonstrates two ways of creating Unique_ptr
    //unique_ptr<MyData> myDataobj = unique_ptr<MyData> (new MyData("With new"));
    unique_ptr<MyData> myDataobj1 = make_unique<MyData>(MyData("With make unique"));

    //Example 2: Demonstrates two wasy of creating shared_ptr
    //shared_ptr<MyData> myDataobj = shared_ptr<MyData> (new MyData("With new"));
    //shared_ptr<MyData> myDataobj1 = make_shared<MyData> (MyData("With make uniq"));
}

void ExamplesOfAssignments()
{
    //auto myDataobj = shared_ptr<MyData> (new MyData("With new"));
    auto myDataUniequeObj = make_unique<MyData>(MyData("With make uniq"));
    auto myDataSharedObj = make_shared<MyData>(MyData("With make shared"));

    //Example3: Demonstrates assignment and moving of a unique pointer
    //auto myDataobj3 = myDataUniequeObj;		//Error: USe of deleted function
    auto myDataobj4 = myDataSharedObj;

    assert(myDataSharedObj != nullptr);

    //Example4: Demonstrates assignment and moving of a unique pointer
    auto myDataobj5 = std::move(myDataUniequeObj);		//Ok
    auto myDataobj6 = std::move(myDataSharedObj);

    assert(myDataobj5 != nullptr);
    assert(myDataUniequeObj == nullptr);

    assert(myDataobj6 != nullptr);
    assert(myDataSharedObj == nullptr);

    //Example5. Creating WeakPointer
    myDataSharedObj = make_shared<MyData>(MyData("With make shared"));
    auto anothershared = myDataSharedObj;

    cout << "Usage ref count is = " << myDataSharedObj.use_count() << " and from another shared = " << anothershared.use_count() << endl;
    anothershared.reset();

    cout << "(After reset)Usage ref count is = " << myDataSharedObj.use_count() << " and from another shared = " << anothershared.use_count() << endl;

    weak_ptr<MyData> weakptr(myDataSharedObj);
    cout << "(After weak ptr creation)Usage ref count is = " << myDataSharedObj.use_count() << " and from another shared = " << anothershared.use_count() << endl;

    auto anotherptr = weakptr.lock();

    assert(anotherptr != nullptr);

    cout << "(After weak ptr lock)Usage ref count is = " << myDataSharedObj.use_count() << " and from another shared = " << anothershared.use_count() << " and weak count = " << weakptr.use_count() << endl;
    myDataSharedObj.reset();
    anotherptr.reset();
    cout << "(After shared ptr reset)Usage ref count is = " << anotherptr.use_count() << " and from another shared = " << anothershared.use_count() << " and weak count = " << weakptr.use_count() << endl;

    anotherptr = weakptr.lock();
    assert(anotherptr == nullptr);

}


void ExamplesOfPassAndReturnFromFuncs()
{
    auto myDataUniequeObj = make_unique<MyData>(MyData("With make uniq"));
    auto myDataSharedObj = make_shared<MyData>(MyData("With make shared"));
    weak_ptr<MyData> weakptr(myDataSharedObj);

    //unique_ptr<MyData> & reftounique = myDataUniequeObj;
    shared_ptr<MyData> & reftoshared = myDataSharedObj;
    weak_ptr<MyData> & reftoweak = weakptr;

    cout << "PassBy Ref Usage ref count is = " << myDataSharedObj.use_count() << " and weak count = " << weakptr.use_count() << endl;
    cout << "PassBy Ref Usage ref count is = " << reftoshared.use_count() << " and weak count = " << reftoweak.use_count() << endl;

    //unique_ptr<MyData>  reftounique1 = myDataUniequeObj;
    shared_ptr<MyData>  reftoshared1 = myDataSharedObj;
    weak_ptr<MyData>  reftoweak1 = weakptr;
    cout << "PassByValue  Usage ref count is = " << myDataSharedObj.use_count() << " and weak count = " << weakptr.use_count() << endl;
    cout << "PassByValue  Usage ref count is = " << reftoshared1.use_count() << " and weak count = " << reftoweak1.use_count() << endl;

}

void myDataExamples()
{
    //Example1: Demonstrate why a smart pointer should be used
    //#if 0
    try
    {
        CalculateTrajectory();
    }
    catch (const exception &e)
    {
        cout << e.what();
    }
    catch (...)
    {
        cout << "caught exception" << endl;
    }

    ExamplesOfCreation();
    //#endif
    ExamplesOfAssignments();

    //ExamplesOfPassAndReturnFromFuncs();
}
