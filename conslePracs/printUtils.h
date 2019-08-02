#pragma once

#include "stdafx.h"

template<typename T>
void printContainer(const T& t)
{
    for (auto& val: t)
    {
        cout << val << endl;
    }
}

template <typename T, typename U>
void fillContainer(T& t, U startValue)
{
    iota(std::begin(t), std::end(t), startValue);
}

template <typename T>
int cItemSize(const T& t)
{
    return sizeof(t);
}