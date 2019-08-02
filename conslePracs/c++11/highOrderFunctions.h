#pragma once

#include "stdafx.h"

void hof_example()
{

    std::vector<int> v{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    std::cout << "before transformation :";

    //classical
    for (size_t i = 0; i < v.size(); i++)
        std::cout << v[i] << "\t";

    //Lambda
    auto printV = [](auto v)->void {
        for (auto x : v)
            std::cout << x << "\t";
        std::cout << std::endl;
    };


    std::transform(v.begin(), v.end(), v.begin(),
        [](int n) {return n + (n * 2); }
    );

    std::cout << std::endl << "after transformation  :";

    //using STL iterator with lambda
    std::for_each(v.begin(), v.end(),
        [](const int&x) {std::cout << x << "\t"; });

    std::cout << std::endl;


    std::remove_if(v.begin(), v.end(),
        [](int n) {return n % 2 != 0; }
    );
    std::cout << "after remove_if  :";
    printV(v);

    auto sum = std::accumulate(begin(v), end(v), 0);
    std::cout << "after accumulate  :" << sum << std::endl;

    auto fi = std::find_if(v.begin(), v.end(), [](const int &x){return x>25;} );
    std::cout << "found element in Vec is :" << *fi << std::endl;

    auto minv = std::min_element(v.begin(), v.end());
    std::cout << "min element in Vec is :" << *minv << std::endl;

    auto maxv= std::max_element(v.begin(), v.end());
    std::cout << "max element in Vec is :" << *maxv << std::endl;

}

