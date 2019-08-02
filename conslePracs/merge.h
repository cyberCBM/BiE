#include "stdafx.h"


void swapLetters(string &word)
{
    string last;
    if (word.length() % 2 != 0) // if odd then take out last
    {
        last = word[word.length() - 1];
        word.erase(word.length() - 1);
    }
    char c = 0;
    for (auto i = 0; i < word.length(); i += 2)
    {
        c = word[i];
        word[i] = word[i + 1];
        word[i + 1] = c;
    }
    if (last.length())
    {
        word = word + last;
    }
}

void findTandReverse() {
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */
    string input = "get it right first time";
    /*cin >> input;
    cout << input;
    */string tStr = "t";
    std::stringstream stream(input);
    std::string word;
    std::vector<std::string> words;
    while (stream >> word) {
        words.push_back(word);
    }

    auto containsT = false;
    for (auto &word : words)
    {
        size_t found = word.find(tStr);
        if (found != string::npos)
        {
            swapLetters(word);
            cout << word << "\n";
            containsT = true;
        }
    }
    if (containsT == false)
    {
        cout << "None";
    }
   
}



