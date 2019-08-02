#pragma once

#include "stdafx.h"

void f(const int& i) { std::cout << "lvalue ref: " << i << "\n"; }
void f(int&& i) { std::cout << "rvalue ref: " << i << "\n"; }

int rlvalue_example()
{
    int i = 77;

    f(i);    // lvalue ref called
    f(99);   // rvalue ref called
    f(std::move(i));  // rvalue ref called

    int x = 10;
    f(x);
    int &y = x;
    f(y);
    int &&z = 10;
    f(z + 5);
    const int &r = 10;
    f(r);

    return 0;

}

void print_reference(int& value)
{
    cout << "lvalue: value = " << value << endl;
}

void print_reference(int&& value)
{
    cout << "rvalue: value = " << value << endl;
}

int getValue()
{
    int temp_ii = 99;
    return temp_ii;
}

auto getDirectValue()
{
    return 999;
}

int rlvalue_example2()
{
    int ii = 11;
    print_reference(ii);
    print_reference(getValue());  //  printReference(99);
    print_reference(getDirectValue());  //  printReference(99);
    return 0;
}


