#pragma once

#include <boost/regex/pending/unicode_iterator.hpp>
#include <boost/spirit/include/qi.hpp>

namespace boost_utilities
{
    void iterateOnMultiByteCharacters(const std::string& word, std::function<void(uint32_t&)>& funIterate);
    std::string codepoint_to_utf8(char32_t codepoint);
} // namespace boost_utilities
