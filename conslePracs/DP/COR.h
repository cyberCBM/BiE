#pragma once

#include "stdafx.h"

// Chain of responsibility

class Photo
{
public:
    Photo(const string & s) : mTitle(s)
    {
        cout << "Processing: " << mTitle << endl;
    }
private:
    string mTitle;
};

class PhotoProcessor
{
public:
    PhotoProcessor() : mNextProcessor(0) { }

    void process(Photo &p) {
        processImplementation(p);
        if (mNextProcessor != 0)
            mNextProcessor->process(p);
    }

    virtual ~PhotoProcessor() { }

    void setNextProcessor(PhotoProcessor *p) {
        mNextProcessor = p;
    }
protected:
    virtual void processImplementation(Photo& p)=0;
private:
    PhotoProcessor *mNextProcessor;;
};

class Scale : public PhotoProcessor
{
public:
    enum SCALE { S50, S100, S200, S300, S500 };
    Scale(SCALE s) : mSCALE(s) { }

private:
    void processImplementation(Photo &) {
        cout << "Scaling photo\n";
    }

    SCALE mSCALE;
};

class RedEye : public PhotoProcessor
{
private:
    void processImplementation(Photo &) {
        cout << "Removing red eye\n";
    }
};

class Filter : public PhotoProcessor
{
private:
    void processImplementation(Photo &) {
        cout << "Applying filters\n";
    }
};

class ColorMatch : public PhotoProcessor
{
private:
    void processImplementation(Photo &)
    {
        cout << "Matching colors\n";
    }
};

void processPhoto(Photo &photo)
{
    ColorMatch match;
    RedEye eye;
    Filter filter;
    Scale scale(Scale::S200);
    scale.setNextProcessor(&eye);
    eye.setNextProcessor(&match);
    match.setNextProcessor(&filter);
    scale.process(photo);
}

void corPattern_Example()
{
    Photo *p = new Photo("Not My Photo");
    processPhoto(*p);
}
