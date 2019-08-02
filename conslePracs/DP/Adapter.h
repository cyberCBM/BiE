#pragma once
#include "stdafx.h"
/*
Adapter pattern relies on object composition.
Client calls operation on Adapter object.
Adapter calls Adaptee to carry out the operation.
In STL, stack adapted from vector:
*/

class Rectangle
{
public:
    virtual void draw()=0;
};

class LegacyRectangle
{
public:
    LegacyRectangle(int x1, int y1, int x2, int y2) {
        x1_ = x1;
        y1_ = y1;
        x2_ = x2;
        y2_ = y2;
        std::cout << "LegacyRectangle(x1,y1,x2,y2)\n";
    }
    void oldDraw() {
        std::cout << "LegacyRectangle:  oldDraw(). \n";
    }
private:
    int x1_;
    int y1_;
    int x2_;
    int y2_;
};

// Adapter wrapper
class RectangleAdapter : public Rectangle, private LegacyRectangle
{
public:
    RectangleAdapter(int x, int y, int w, int h) :
        LegacyRectangle(x, y, x + w, y + h) {
        std::cout << "RectangleAdapter(x,y,x+w,x+h)\n";
    }

    void draw() {
        std::cout << "RectangleAdapter: draw().\n";
        oldDraw(); // draw old details also
    }
};

void adapterPattern_Example()
{
    int x = 20, y = 50, w = 300, h = 200;
    Rectangle *r = new RectangleAdapter(x, y, w, h);
    r->draw();
}
