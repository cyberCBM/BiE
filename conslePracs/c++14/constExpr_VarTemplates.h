#pragma once

#include "stdafx.h"

namespace CPP11_14F
{
    constexpr int computeSomething(int mx)
    {
        int result{mx};
        for (int i{ 0 }; i < 10; ++i)
        {
            result+=i;
        }
        if (result>5)result+=10;

        return result;
    }

    template<int value>
    struct Test
    {

    };

    Test<computeSomething(10)> intance;

}
