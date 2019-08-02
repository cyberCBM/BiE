#pragma once

#include "stdafx.h"

/*
Attach additional responsibilities to an object dynamically.
Decorators provide flexible alternatives to subclassing for extending functionality.
*/

class Window
{
    public:
    virtual void draw()=0;
    virtual string getDescription()=0;
    virtual ~Window(){}
};

class SimpleWindow : public Window
{
public:
    void draw() {
        cout<< "simple window draw" << endl;
    }
    string getDescription() {
        return "simple window\n";
    }
};

class WindowCreator :public Window 
{
protected:
    Window* m_decoratedWindow;
public:
    WindowCreator(Window* w)
    {
        m_decoratedWindow = w;
    }
};

class VerticalScrollBarDecorator : public WindowCreator
{
public:
    VerticalScrollBarDecorator(Window *decoratedWindow) :
        WindowCreator(decoratedWindow) {}

    void draw() {
        drawVerticalScrollBar();
        m_decoratedWindow->draw();
    }

    string getDescription() {
        return m_decoratedWindow->getDescription() + "with vertical scrollbars\n";
    }

private:
    void drawVerticalScrollBar() {
        // draw the vertical scrollbar
    }
};

class HorizontalScrollBarDecorator : public WindowCreator
{
public:
    HorizontalScrollBarDecorator(Window *decoratedWindow) :
        WindowCreator(decoratedWindow) {}

    void draw() {
        drawHorizontalScrollBar();
        m_decoratedWindow->draw();
    }

    string getDescription() {
        return m_decoratedWindow->getDescription() + "with horizontal scrollbars\n";
    }
private:
    void drawHorizontalScrollBar() {
        // draw the horizontal scrollbar
    }
};

void decoratorPattern_Example()
{
    Window *simple = new SimpleWindow();
    cout << simple->getDescription() << endl;

    Window *horiz = new HorizontalScrollBarDecorator(new SimpleWindow());
    cout << horiz->getDescription() << endl;

    Window *vert = new VerticalScrollBarDecorator(new SimpleWindow());
    cout << vert->getDescription() << endl;

    Window *decoratedWindow = new HorizontalScrollBarDecorator(
        new VerticalScrollBarDecorator(new SimpleWindow()));
    cout << decoratedWindow->getDescription() << endl;
}