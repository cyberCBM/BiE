#pragma once

#include "stdafx.h"

void reverseSentance(const string sentance)
{
    std::stringstream stream(sentance);
    std::string word;
    std::vector<std::string> words;
    while (stream >> word)
    {
        words.push_back(word);
    }
    std::reverse(words.begin(), words.end());
    for (size_t i(0); i < words.size(); ++i)
    {
        auto rev = words[i];
        std::reverse(rev.begin(), rev.end());
        std::cout << rev << " ";
    }
}


