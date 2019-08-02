#pragma once

#include "stdafx.h"

vector<int> findMatches(const string & str, const string & matchStr)
{
    vector<int> matchingVec;
    auto index = str.find(matchStr,0);
    while (index != string::npos)
    {
        matchingVec.push_back(index);
        index = str.find(matchStr, index +1);
    }
    return matchingVec;
}

void trimWhiteSpaces(string& str)
{
    //cout << str <<"\n" ;
    const string whiteSpace = { " \t\r\f\n" };
    str.erase(str.find_last_not_of(whiteSpace) + 1);
    str.erase(0, str.find_first_not_of(whiteSpace) );
}

void smallFuncs()
{
    cout << "Abs: " << abs(-10)<< "\n";
    cout << "Max: " << max(5,10)<< "\n";
    cout << "min: " << min(5, 10) << "\n";
    cout << "fmax: " << fmax(1.4, 5.4) << "\n";
    cout << "fmin: " << fmin(1.4, 5.4) << "\n";
    cout << "e^x: " << exp(2)<< "\n";
    cout << "2^x: " << exp2(2) << "\n";
    cout << "log(20.60): " << log(20.60)<< "\n";
    cout << "log10(1000): " << log10(1000)<< "\n";
    cout << "log2(8): " << log2(8) << "\n";
    cout << "pow(2,3)->2^3: " << pow(2,3)<< "\n";
    cout << "sqrt(100): " << sqrt(100) << "\n";
    cout << "cbrt(1000): " << cbrt(1000)<< "\n";
    cout << "hypot(2,3): " << hypot(2, 3) << "\n";
    cout << "ceil(3.45): " << ceil(3.45) << "\n";
    cout << "floor(3.45): " << floor(3.45) << "\n";
    cout << "round(3.45): " << round(3.45) << "\n";
}

void testStr(string& str)
{
    // getting string in uppercase
    // Replace each car to corresponding Ascii
    // a-z = 97-121
    // A-Z = 65-90
    // Space = 32
    // 0-9 = 48-57
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    string secret = "";
    for (auto ichar : str)
    {
        int iInt = ichar;
        secret += to_string(iInt);
    }
    cout << "Secret:" << secret << endl;
    auto count = 0;
    string digit;
    string message;
    for (auto ichar : secret)
    {
        digit += ichar;
        count++;
        if (count == 2)
        {
            char chChar = static_cast<char>(atoi(digit.c_str()));
            message += chChar;
            digit = "";
            count = 0;
        }
    }
    cout << "Original:" << message << endl;
}

void stringSpecific()
{
    vector<string> stringVec(10);

    string str = { "I am a string, but I am not literal." };
    cout << "str length "<< str.length() << endl;

    // creating lambda function but not calling it
    auto lmdStr = [](string& str, vector<string> & stringVec) {
        stringVec[0] = str;

        string str1(str);
        stringVec[1] = str1;

        string str2(str, 4);
        stringVec[2] = str2;

        string str3(5, 'x');
        stringVec[3] = str3;

        string str4 = str + (" and not other literal.");
        stringVec[4] = str4;

        string str5 = str.append(str4, 34, 36);
        stringVec[5] = str5;

        stringVec[6] = str;
        for (auto str : stringVec)
        {
            cout << "string value: " << str << endl;
        }
    };

    auto index = str.find("string");
    if (index != string::npos)
    {
        cout << "string found at :" << index << "\n";
    }

    cout << "substring of str :" << str.substr(0, 5) << "----" << str.substr(10, 15) << "\n";
    reverse(str.begin(), str.end());
    cout << "reversed" << str << "\n";

    transform(str.begin(), str.end(), str.begin(), ::toupper);
    cout << "Upper str" << str << "\n";

    transform(str.begin(), str.end(), str.begin(), ::tolower);
    cout << "lower str" << str << "\n";

    // Asscii value of string
    char aChar = 'A';
    int aInt = aChar;
    cout << "A code" << aInt << "\n";
    aChar = 'z';
    aInt = aChar;
    cout << "z code" << aInt << "\n";

    // int to string
    cout << to_string(5 + 6) << "\n";

}

void variousString(const string str)
{
    cout << str.front() << " -> " << str.back() << endl;
    cout << str.length() << endl;
    printContainer(str);
}

vector<string> strinToVec(const string& str, const char &delimiter)
{
    vector<string> stringVec;
    stringstream ss(str);
    string tempStr = "";
    while (getline(ss, tempStr, delimiter))
    {
        stringVec.push_back(tempStr);
    }
    return stringVec;
}

void stringDelimeterExample()
{
    string sentance = "break into words";
    string sentance1 = "12:11:09";
    char delim = { ' ' };
    char delim1 = { ':' };

    vector<string> wordsVec;
    vector<int> timeVec;

    stringstream ss(sentance);
    stringstream ss1(sentance1);
    
    string tempStr;
    while (getline(ss, tempStr, delim))
    {
        wordsVec.push_back(tempStr);
    }

    printContainer<vector<string>>(wordsVec);

    while (getline(ss1, tempStr, delim1))
    {
        timeVec.push_back(atoi(tempStr.c_str()));
    }
    printContainer<vector<int>>(timeVec);
}

void vecFill()
{
    vector<float> intVec(10);
    fillContainer<vector<float>, float>(intVec, static_cast<float>(-1.3));
    printContainer(intVec);
}

void randomInts()
{
    vector<int> randVec;
    std::srand(static_cast<unsigned int>(std::time(0)));
    for (auto i = 0; i < 20; i++)
        randVec.push_back(std::rand() % 6);
    printContainer(randVec);
}