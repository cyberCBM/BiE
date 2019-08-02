#pragma once

#include "stdafx.h"
// Provide an interface for creating families of related or dependent objects without specifying their concrete classes.
class Button
{
public:
    virtual void paint()=0;
};

class windowButton : public Button
{
    public:
    void paint()
    {
        cout << "Window Button" << endl;
    }
};

class MacButton : public Button
{
public:
    void paint()
    {
        cout << "Mac Button" << endl;
    }
};

class ScrollBar
{
public:
    virtual void paint() = 0;
};

class windowScrollBar : public ScrollBar
{
public:
    void paint()
    {
        cout << "Window ScrollBar" << endl;
    }
};

class MacScrollBar : public ScrollBar
{
public:
    void paint()
    {
        cout << "Mac ScrollBar" << endl;
    }
};

class GUIFactory
{
    public:
    virtual Button* createButton()=0;
    virtual ScrollBar* createScrollbar() = 0;
};

class WindowFactory : public GUIFactory
{
public:
    Button* createButton()
    {
        return new windowButton;
    }
    ScrollBar* createScrollbar()
    {
        return new windowScrollBar;
    }
};

class MacFactory : public GUIFactory
{
public:
    Button* createButton()
    {
        return new MacButton;
    }
    ScrollBar* createScrollbar()
    {
        return new MacScrollBar;
    }
};

void abstractFactoryPattern_Example()
{
    GUIFactory* guiFactory;
    Button *btn;
    ScrollBar *sb;

    guiFactory = new MacFactory;
    btn = guiFactory->createButton();
    btn->paint();
    sb = guiFactory->createScrollbar();
    sb->paint();

    guiFactory = new WindowFactory();
    btn = guiFactory->createButton();
    btn->paint();
    sb = guiFactory->createScrollbar();
    sb->paint();
}
