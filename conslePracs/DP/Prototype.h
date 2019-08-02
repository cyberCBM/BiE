#pragma once

#include "stdafx.h"

const int N=4;
//Prototype allows us to hide the complexity of making new instances from the client. 
// In this sample, we used prototype manager to set the registry of concrete prototypes.
class Document
{
    public:
virtual Document* clone() const=0;
virtual void store() const=0;
virtual ~Document(){}
};

class xmlDoc : public Document
{
    public:
    Document* clone() const {return new xmlDoc;}
    void store() const {cout<< "XML document" << endl;}
};

class plainDoc : public Document
{
public:
    Document* clone() const { return new plainDoc; }
    void store() const { cout << "Plain document" << endl; }
};

class spreadsheetDoc : public Document
{
public:
    Document* clone() const { return new spreadsheetDoc; }
    void store() const { cout << "Spread shit[:P] document" << endl; }
};


class DocumentManager
{
public:
    static Document* makeDocument(int choice);
    ~DocumentManager(){}
private:
    static Document* mDocTypes[N];
};
Document* DocumentManager::mDocTypes[] =
{
    0, new xmlDoc, new plainDoc, new spreadsheetDoc
};

Document* DocumentManager::makeDocument(int choice)
{
    return mDocTypes[choice]->clone();
}

// functor for for each for delete
struct Destruct
{
    void operator()(Document* d)const 
    {
        delete d;
    }
};

void prototypePattern_Example()
{
    vector<Document*> docs(N);
    int choice;
    cout << "quit(0), xml(1), plain(2), spreadsheet(3): \n";
    while (true) {
        cout << "Type in your choice (0-3)\n";
        cin >> choice;
        if (choice <= 0 || choice >= N)
            break;
        docs[choice] = DocumentManager::makeDocument(choice);
    }

    for (size_t i = 1; i < docs.size(); ++i)
        if (docs[i]) docs[i]->store();

    Destruct d;
    // this calls Destruct::operator()
    for_each(docs.begin(), docs.end(), d);
}
