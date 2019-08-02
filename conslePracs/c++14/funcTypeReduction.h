#pragma once

#include "stdafx.h"

namespace CPP11_14F
{
    auto func0()
    {
        return 0;
    }

    void func0Test() {
        static_assert(
            std::is_same<
            decltype(func0()), int
            >()
            , ""
            );
    }
    
    auto func1()->std::string
    {
        std::string ret{"return"};
        return ret;
    }

    void func1Test() {
        static_assert(
            std::is_same<
            decltype(func1()), std::string
            >()
            , ""
            );
    }

    const auto& func2()
    {
        static std::string ret{"return"};
        return ret;
    }

    void func2Test() {
        static_assert(
            std::is_same<
            decltype(func2()), const std::string&
            >()
            , ""
            );
    }

    template<typename T>
    struct someContainerWrapper
    {
        std::vector<const T*> vec;
        // so complex
        typename std::vector<const T*>::iterator beginContWrapIterator() const
        {
            return std::begin(vec);
        }
        // so simple to write
        auto beginContWrapItr11() const -> decltype(std::begin(vec))
        {
            return std::begin(vec);
        }
        // even easier 
        auto beginContWrapItr14() const /*-> decltype(std::begin(vec))*/
        {
            return std::begin(vec);
        }
    };

    template <typename T1, typename T2, typename T3>
    auto comlpicatedFunc11(T1 x, T2 y, T3 z) -> decltype((x*y) - (y*z))
    {
        return ((x*y) - (y*z))
    }
    
    template <typename T1, typename T2, typename T3>
    auto comlpicatedFunc14(T1 x, T2 y, T3 z) 
    {
        return ((x*y) - (y*z))
    }

    void testFuncTypeDeduction()
    {
        func0Test();
        func1Test();
        func2Test();
        comlpicatedFunc11(5, 2.3, (float)7);
        comlpicatedFunc14(10.6, 8, (float)3);
    }
}