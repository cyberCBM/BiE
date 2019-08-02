#pragma once

#include "stdafx.h"

/*
The strategy pattern is intended to provide a means to define a family of algorithms, 
encapsulate each one as an object, 
and make them interchangeable. 
The strategy pattern lets the algorithms vary independently from clients that use them.
*/

class SortStrategy
{
    public:
    virtual void sort() const=0;
};

class Merge : public SortStrategy
{
public:
    virtual void sort() const {
        cout << "Merge sort()\n";
    }
};

class Quick : public SortStrategy {
public:
    virtual void sort() const {
        cout << "Quick sort()\n";
    }
};

class SearchStrategy
{
public:
    virtual void search() const = 0;
};

class Sequential : public SearchStrategy
{
public:
    virtual void search() const {
        cout << "Sequential search()\n";
    }
};

class BinaryTree : public SearchStrategy
{
public:
    virtual void search() const {
        cout << "BinaryTree search()\n";
    }
};

// Context is composition
class Collection
{
private:
    SortStrategy* m_sort;
    SearchStrategy* m_search;
public:
    Collection() {}
    void set_sort(SortStrategy* s) {
        m_sort = s;
    }
    void set_search(SearchStrategy* s) {
        m_search = s;
    }
    void sort() const {
        m_sort->sort();
    }
    void search() const {
        m_search->search();
    }
};

void strategyPattern_Example()
{
    Merge merge;
    Quick quick;

    Sequential sequential;
    BinaryTree binaryTree;

    Collection colA;
    colA.set_sort(&merge);
    colA.sort();

    Collection colB;
    colB.set_search(&binaryTree);
    colB.search();
}