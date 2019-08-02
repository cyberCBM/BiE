#pragma once
/*
Composite objects into tree structures to represent part-whole hierarchies.
Composite lets clients treat individual objects and compositions of objects uniformly.
*/
#include "stdafx.h"

class Graphics
{
public:
    virtual void draw()const = 0;
    virtual void remove(Graphics*){}
    virtual void add(Graphics*) {}
    virtual void getChild(int) {}
    virtual ~Graphics(){}
};

class Line :public Graphics
{
public:
    void draw() const
    { cout << "Line draw" << endl ;}
};

class CRectangle :public Graphics
{
public:
    void draw() const
    {
        cout << "Rect draw" << endl;
    }
};

class Text :public Graphics
{
public:
    void draw() const
    {
        cout << "Text draw" << endl;
    }
};

// create composition

class Picture : public Graphics
{
public:
    void draw() const {
        for_each(gList.begin(), gList.end(), mem_fun(&Graphics::draw));
    }

    void add(Graphics*g)
    {
        gList.push_back(g);
    }
private:
    vector<Graphics*> gList;
};

void compositePattern_example()
{
    Line line;
    line.draw();
    CRectangle rect;
    rect.draw();
    Text text;
    text.draw();

    Picture pic;
    pic.add(&line);
    pic.add(&rect);
    pic.add(&text);
    pic.add(&rect);
    pic.draw();
}
