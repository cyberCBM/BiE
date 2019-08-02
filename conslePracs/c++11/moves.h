#pragma once

#include "stdafx.h"


struct A
{
    std::string s;
    int k;

    A() : s("test"), k(-1) { }

    A(const A& o) : s(o.s), k(o.k) { std::cout << "move failed!\n"; }

    A(A&& o) noexcept :
        s(std::move(o.s)),       // explicit move of a member of class type
        k(std::exchange(o.k, 0)) // explicit move of a member of non-class type
    { }
};

A f(A a)
{
    return a;
}

struct B : A
{
    std::string s2;
    int n;
    // implicit move constructor B::(B&&)
    // calls A's move constructor
    // calls s2's move constructor
    // and makes a bitwise copy of n
};

struct C : B
{
    ~C() { } // destructor prevents implicit move constructor C::(C&&)
};

struct D : B
{
    D() { }
    ~D() { }          // destructor would prevent implicit move constructor D::(D&&)
    D(D&&) = default; // forces a move constructor anyway
};

int move_example()
{
    std::cout << "Trying to move A\n";

    A a1 = f(A()); // return by value move-constructs the target from the function parameter

    std::cout << "Before move, a1.s = " << std::quoted(a1.s) << " a1.k = " << a1.k << '\n';
    A a2 = std::move(a1); // move-constructs from xvalue
    std::cout << "After move, a1.s = " << std::quoted(a1.s) << " a1.k = " << a1.k << '\n';

    std::cout << "Trying to move B\n";

    B b1;

    std::cout << "Before move, b1.s = " << std::quoted(b1.s) << "\n";
    B b2 = std::move(b1); // calls implicit move constructor
    std::cout << "After move, b1.s = " << std::quoted(b1.s) << "\n";

    std::cout << "Trying to move C\n";

    C c1;
    C c2 = std::move(c1); // calls copy constructor
    std::cout << "After move, c1.s = " << std::quoted(c1.s) << "\n";

    std::cout << "Trying to move D\n";
    D d1;
    D d2 = std::move(d1);
    std::cout << "After move, d1.s = " << std::quoted(d1.s) << "\n";

    return 0;
}


// Driver code 
int move_vector_example()
{
    std::vector <int> vec1{ 1, 2, 3, 4, 5 };
    std::vector <int> vec2{ 7, 7, 7, 7, 7 };

    // Print elements 
    std::cout << "Vector1 contains :";
    for (size_t i = 0; i < vec1.size(); i++)
        std::cout << " " << vec1[i];
    std::cout << "\n";

    // Print elements 
    std::cout << "Vector2 contains :";
    for (unsigned int i = 0; i < vec2.size(); i++)
        std::cout << " " << vec2[i];
    std::cout << "\n\n";

    // std :: move function 
    // move first 4 element from vec1 to starting position of vec2 
    std::move(vec1.begin(), vec1.begin() + 4, vec2.begin() + 1);

    // Print elements 
    std::cout << "Vector2 contains after std::move function:";
    for (unsigned int i = 0; i < vec2.size(); i++)
        std::cout << "\t" << vec2[i];
    std::cout << "\n";

    std::cout << "Vector1 contains after std::move function:";
    for (auto x : vec1)
        std::cout << x << "\t";
    std::cout << "\n";

    std::vector<int> vec3;
    std::cout << "Vector1 contains after std::move function:" << std::endl;
    vec3 = std::move(vec1);
    for (auto x : vec1)
        std::cout << x << "\t";
    std::cout << "\n";

    return 0;
}
