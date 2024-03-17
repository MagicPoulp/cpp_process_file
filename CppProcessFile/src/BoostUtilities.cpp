#pragma once

#include "BoostUtilities.h"

#include <codecvt>
#include <iostream>
#include <string>
#include <vector>

namespace boost_utilities
{
    // "ü" takes 2 bytes
    // so we need to iterate over multi-byte characters in UTF8
    // https://stackoverflow.com/questions/13679669/how-to-use-boostspirit-to-parse-utf-8
    // it was tested manually that trema u has code 252
    void iterateOnMultiByteCharacters(const std::string& word, std::function<void(uint32_t&)>& funIterate)
    {
        using namespace boost;
        using namespace spirit::qi;
        using utf8_iter = boost::u8_to_u32_iterator<const char*>;
        auto tbegin     = utf8_iter{ word.c_str() };
        auto tend       = utf8_iter{ word.c_str() + word.size() };
        std::vector<uint32_t> wordInFourBytes;
        parse(tbegin, tend, *standard_wide::char_, wordInFourBytes);
        for (auto&& codepoint : wordInFourBytes) {
            funIterate(codepoint);
        }
    }

    // the apostrophe, U2019 has code 8217
    // https://unicode.scarfboy.com/?s=U%2b2019
    // the blank has code 32 (20 in hex)
    bool replaceMultiByteApostropheWithBlank(const std::string& word, std::string& trimmedWord)
    {
        using namespace boost;
        using namespace spirit::qi;
        using utf8_iter = boost::u8_to_u32_iterator<const char*>;
        auto tbegin     = utf8_iter{ word.c_str() };
        auto tend       = utf8_iter{ word.c_str() + word.size() };
        std::vector<uint32_t> wordInFourBytes;
        parse(tbegin, tend, *standard_wide::char_, wordInFourBytes);
        bool replaced = false;
        for (auto& codepoint : wordInFourBytes) {
            if (codepoint == 8217) {
                codepoint = 32;
                replaced  = true;
            }
        }
        if (replaced) {
            trimmedWord = "";
            for (const auto& codepoint : wordInFourBytes) {
                std::string codeAsAstring = codepoint_to_utf8(codepoint);
                trimmedWord += codeAsAstring;
            }
        }
        return replaced;
    }

    // https://stackoverflow.com/questions/56341221/how-to-convert-a-codepoint-to-utf-8
    std::string codepoint_to_utf8(char32_t codepoint)
    {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
        return convert.to_bytes(&codepoint, &codepoint + 1);
    }
} // namespace boost_utilities
