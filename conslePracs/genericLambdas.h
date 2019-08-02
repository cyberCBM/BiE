#pragma once

#include "stdafx.h"

namespace CPP11_14F
{
    auto l0([](auto x) {return x * 2; });

    static_assert(std::is_same<decltype(l0(1)), int>(), "");
    static_assert(std::is_same<decltype(l0(1.f)), float>(), "");
    static_assert(std::is_same<decltype(l0(1.0)), double > (), "");

    // variadic arguments packs
    auto l1([](auto... xs) {return sizeof...(xs);  });

    // This get internally converted to 
    auto l2([](auto x, auto y) {return x*y;  });

    // l2 lambda
    struct compGeneratedl2
    {
        template<typename T1, typename T2>
        auto operator() (T1 x, T2 y)
        {
            return x*y;
        }
    };

    void GenericlambdaExample()
    {

    }

}