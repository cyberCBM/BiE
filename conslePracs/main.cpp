#include "stdafx.h"

#include "Basics/strVec.h"

int main(int /*argc*/, char* /*argv*/)
{
    auto str = "I 'm Getting It => there.";
    cout << str << endl;

    try
    {
        auto vec = findMatches("to be or not to be, so be it.", "be");
        printContainer(vec);
        cout << str << "\n";
    }
    catch (exception &e)
    {
        cout << e.what() << endl;
    }
    /*
        stringDelimeterExample();
        vecFill();
        randomInts();
        variousString(str);
        stringSpecific();
        testStr(str);
        smallFuncs();
        trimWhiteSpaces("           " + string(str) + "       ");
        */
    

    cout << "Press any key to return !" << endl;
    getchar();
    return 0;
}