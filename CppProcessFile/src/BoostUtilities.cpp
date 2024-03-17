#pragma once

#include "BoostUtilities.h"

#include <codecvt>
#include <iostream>
#include <string>
#include <vector>

namespace boost_utilities
{
    // it was tested manually that trema u has code 252
    void iterateOnMultiByteCharacters(const std::string& word, std::function<void(uint32_t&)>& funIterate)
    {
        using namespace boost;
        using namespace spirit::qi;
        using utf8_iter = boost::u8_to_u32_iterator<const char*>;

        const char8_t* u8word = new char8_t(word.size());
        auto tbegin           = utf8_iter{ word.c_str() };
        auto tend             = utf8_iter{ word.c_str() + word.size() };
        std::vector<uint32_t> wordInFourBytes;
        parse(tbegin, tend, *standard_wide::char_, wordInFourBytes);
        for (auto&& code_point : wordInFourBytes) {
            funIterate(code_point);
        }
    }

    // https://stackoverflow.com/questions/56341221/how-to-convert-a-codepoint-to-utf-8
    std::string codepoint_to_utf8(char32_t codepoint)
    {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
        return convert.to_bytes(&codepoint, &codepoint + 1);
    }
} // namespace boost_utilities
