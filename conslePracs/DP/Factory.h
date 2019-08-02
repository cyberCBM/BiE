#pragma once

#include "stdafx.h"
// Define an interface for creating an object, but let subclasses decide which class to instantiate.
class FButton
{
public:
    virtual void paint()=0;
};

class WindowFButton : public FButton
{
public:
    void paint()
    {
        cout << "Window Button" << endl;
    }
};

class MacFButton : public FButton
{
public:
    void paint()
    {
        cout << "Window Button" << endl;
    }
};

class FGUIFactory
{
    public:
    virtual FButton *createButton(const char*)=0;
};

class Factory: public FGUIFactory
{
    FButton *createButton(const char* type)
    {
        FButton * fbutton= nullptr;
        if (strcmp(type, "Windows") == 0) {
            fbutton = new WindowFButton;
        }
        else if (strcmp(type, "MAC") == 0) {
            fbutton = new MacFButton;
        }
        return fbutton;
    }

};

void FactoryPattern_Example()
{
    FGUIFactory* guiFactory;
    FButton *btn;

    guiFactory = new Factory;

    btn = guiFactory->createButton("MAC");
    btn->paint();
    btn = guiFactory->createButton("Windows");
    btn->paint();
}
