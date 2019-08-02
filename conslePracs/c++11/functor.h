#pragma once

#include "stdafx.h"

int sum(int x, int y) {
    return x + y;
}
class Add {
public:
    int operator() (int x, int y) const {
        return x + y;
    }
};

int functor_example() {
    Add add;
    std::cout << "Functor object " << add(5, 7) << std::endl;

    std::function<int(int, int)> func;
    func = sum;
    std::cout << "std::function " << func(6, 7) << std::endl;

    auto sum = [](double A, double B) { return A + B; };
    auto add_ = sum;

    std::cout << "Lambda " << add_(7.5, 7.5) << std::endl;
    std::cout << "Lambda sum " << sum(7.5, 7.5) << std::endl;


    return 0;
}