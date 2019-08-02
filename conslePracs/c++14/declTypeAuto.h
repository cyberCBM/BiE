#pragma once

#include "stdafx.h"

namespace CPP11_14F
{
    auto func3()
    {
        static std::string test{"Ohhhh yes!!!"};
        return test;
    }

    void func3Test() {
        static_assert(
            std::is_same<
            decltype(func3()), std::string
            >()
            , ""
            );
    }

    decltype(auto) func4()
    {
        static std::string test{ "Ohhhh yes Byeee!!!" };
        auto& result(test);
        return result;
    }

    void func4Test() {
        static_assert(
            std::is_same<
            decltype(func4()), std::string&
            >()
            , ""
            );
    }

    decltype(auto) func5()
    {
        std::string test{"Buuuh Byee"};
        return std::move(test);
    }

    void func5Test() {
        static_assert(
            std::is_same<
            decltype(func5()), std::string&&
            >()
            , ""
            );
    } 

    void testDeclTypeAuto()
    {
        func3Test();
        func4Test();
        func5Test();
    }

}
