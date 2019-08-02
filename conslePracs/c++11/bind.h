#pragma once

#include "stdafx.h"

int func(int x, int y)
{
    std::cout << "Func called with : " << x << "\t" << y << std::endl;
    return 0;
}


int func_ref(int x, string& y)
{
    y += " Saha";
    std::cout << "Func_Ref : " << x << "\t" << y << std::endl;
    return 0;
}

void bind_example()
{
    using namespace std::placeholders;

    int x = 10;
    auto f1 = std::bind(&func, _1, x);
    f1(42);

    //f1(42,x+1);

    auto f2 = std::bind(&func, x, _1);
    f2(42);

    string name = "Meghnad";
    auto f3 = std::bind(&func_ref, _1, std::ref(name));
    f3(42);
}

