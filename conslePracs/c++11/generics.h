#pragma once

#include "stdafx.h"

template<class InputIterator, class T>
InputIterator Mfind(InputIterator first, InputIterator last, const T& val)
{
    while (first != last) {
        if (*first == val) return first;
        ++first;
    }
    return last;
}

using namespace std;

void generic_example()
{
    vector<string> vs{ "This","is","team","Meghnaad" };
    auto p = find(begin(vs), end(vs), "team"); //  p is vector<string>::iterator
    if (p != vs.end())
        cout << "found string -" << *p << endl;

    vector<int> vi{ 11,22,32,42,52 };
    auto q = find(begin(vi), end(vi), 42);       // q is vector<int>::iterator    
    if (q != vi.end())
        cout << "found int -" << *q << endl;

    list<double> ld{ 10.0,20.4,30,40.0,50 };
    auto r = find(begin(ld), end(ld), 10.0);      // r is list<double>::iterator    
    if (r != ld.end())
        cout << "found double -" << *r << endl;

    int ar[10] = { 1,2,3,4,5 };
    auto s = find(begin(ar), end(ar), 4);       //         s is int *
    if (s != ar + 5)
        cout << "found array element -" << *s << endl;


    auto p2 = Mfind(begin(vs), end(vs), "team"); //  p is vector<string>::iterator
    if (p != vs.end())
        cout << "found string with Mfind -" << *p << endl;
}


template<typename T>
T sum1(vector<T>& v, T s)
{
    for (auto x : v) s += x;
    return s;
}


template<typename T>
T sum2(list<T>& v, T s)
{
    for (auto x : v) s = s + x;
    return s;
}



template<typename Cont, typename T>
T sum(const Cont& v, T s)
{
    for (auto x : v) s += x;
    return s;
}

int generic_sum_example()
{
    vector<int> a{ 1,2,3,4,5,6,7 };
    list<int> la{ 1,2,3,4,5,6,7 };
    int b = 20;

    auto result_inc = sum1(a, b);
    cout << result_inc << endl;

    auto result_add = sum2(la, b);
    cout << result_add << endl;

    auto result_generic = sum(a, b);
    cout << result_generic << endl;
    return 0;
}






