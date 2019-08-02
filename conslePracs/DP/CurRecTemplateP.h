#pragma once

#include "stdafx.h"

/*  Usage of VPtr and VTable can be avoided altogether through Curiously Recurring Template Pattern (CRTP). 
    CRTP is a design pattern in C++ in which a class X derives from a class template 
    instantiation using X itself as template argument. 
    More generally it is known as F-bound polymorphisms.
*/

using Clock = chrono::high_resolution_clock;

class Dimension
{
private:
    int mX;
    int mY;

public:
    Dimension(int _x, int _y)
    {
        mX = _x;
        mY = _y;
    }
};

template <typename T>
class Image
{
protected:
    int dimensionX;
    int dimensionY;

public:
    void Draw()
    {
        static_cast<T*>(this)->Draw();
    }

    Dimension getDimentionInPixel()
    {
        static_cast<t*> (this)->getDimentionInPixel()
    }
};

class TiffImage : public Image<TiffImage>
{
public:
    void Draw()
    {
        cout << "TiffImage::Draw() called" << endl; 
    }

    Dimension getDimentionInPixel()
    {
        return Dimension(dimensionX, dimensionY);
    }
};

// Driver code 
void CRTP_Example()
{
    // An Image type pointer pointing to Tiffimage 
    Image<TiffImage>* pImage = new TiffImage;

    // Store time before virtual function calls 
    auto then = Clock::now();

    // Call Draw 1000 times to make sure performance 
    // is visible 
    for (int i = 0; i < 1000; ++i)
        pImage->Draw();

    // Store time after virtual function calls 
    auto now = Clock::now();

    cout << "Time taken: "
        << std::chrono::duration_cast
        <std::chrono::nanoseconds>(now - then).count()
        << " nanoseconds" << endl;

}

/*
Virtual method vs CRTP benchmark
The time taken while using virtual method was 2613 nanoseconds. 
This (small) performance gain from CRTP is because the use of a VTable dispatch has been circumvented. 
Please note that the performance depends on a lot of factors like compiler used,operations performed by virtual methods. 
Performance numbers might differ in different runs, but (small) performance gain is expected from CRTP.
Note: If we print size of class in CRTP, it can bee seen that VPtr no longer reserves 4 bytes of memory.
*/