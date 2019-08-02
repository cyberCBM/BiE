#pragma once

#include "stdafx.h"

template<typename T> T pi{static_cast<int>(3.14159265359)};

void test0()
{
    auto piInt(pi<int>);
    auto piFloat(pi<float>);
    auto piDouble(pi<double>);
}

template<typename T> int typeID;
template< >constexpr int typeID<int>{0};
template< >constexpr int typeID<float>{1};
template< >constexpr int typeID<double>{2};

void test1()
{
	/*static_assert(typeID<int> == 0, "");
	static_assert(typeID<float> == 0, "");
	static_assert(typeID<double> == 0, "");*/

}
