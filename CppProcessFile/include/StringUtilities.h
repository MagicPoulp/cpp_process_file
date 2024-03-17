#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace string_utilities
{

    // https://stackoverflow.com/questions/75070990/c17-check-if-a-file-is-encoded-in-utf-8
    size_t find_first_not_utf8(std::string_view s)
    {
        // ----------------------------------------------------
        // returns true if fn(c) returns true for all n first charac-ters c of
        // string src. the sring_voew is updated to exclude the first n characters
        // if a match is found, left untouched otherwise.
        const auto match_n = [](std::string_view& src, size_t n, auto&& fn) noexcept {
            if (src.length() < n)
                return false;

            if (!std::all_of(src.begin(), src.begin() + n, fn))
                return false;

            src.remove_prefix(n);
            return true;
        };

        // ----------------------------------------------------
        // returns true if fn(c) returns true for the first character c of
        // string src. the sring_view is updated to exclude the first character
        // if a match is found, left untouched otherwise.
        const auto match_1 = [](std::string_view& src, auto&& fn) noexcept {
            if (src.empty())
                return false;

            if (!fn(src.front()))
                return false;

            src.remove_prefix(1);
            return true;
        };

        // ----------------------------------------------------
        // returns true if the first chatacter sequence of src is a valid non-ascii
        // utf8 sequece.
        // the sring_view is updated to exclude the first utf-8 sequence if non-ascii
        // sequence is found, left untouched otherwise.

        const auto utf8_non_ascii = [&](std::string_view& src) noexcept {
            const auto SRC = src;

            auto UTF8_CONT = [](uint8_t c) noexcept { return 0x80 <= c && c <= 0xBF; };

            if (match_1(src, [](uint8_t c) { return 0xC0 <= c && c <= 0xDF; }) && match_1(src, UTF8_CONT)) {
                return true;
            }
            src = SRC;
            if (match_1(src, [](uint8_t c) { return 0xE0 <= c && c <= 0xEF; }) && match_n(src, 2, UTF8_CONT)) {
                return true;
            }
            src = SRC;
            if (match_1(src, [](uint8_t c) { return 0xF0 <= c && c <= 0xF7; }) && match_n(src, 3, UTF8_CONT)) {
                return true;
            }
            src = SRC;
            if (match_1(src, [](uint8_t c) { return 0xF8 <= c && c <= 0xFB; }) && match_n(src, 4, UTF8_CONT)) {
                return true;
            }
            src = SRC;
            if (match_1(src, [](uint8_t c) { return 0xFC <= c && c <= 0xFD; }) && match_n(src, 5, UTF8_CONT)) {
                return true;
            }
            src = SRC;
            return false;
        };

        // ----------------------------------------------------
        // returns true if the first symbol of st(ring src is a valid UTF8 character
        // not-including control characters, nor space.
        // the sring_view is updated to exclude the first utf-8 sequence
        // if a valid symbol sequence is found, left untouched otherwise.

        const auto utf8_char = [&](std::string_view& src) noexcept {
            auto rule = [](uint8_t c) noexcept -> bool { return (0x21 <= c && c <= 0x7E) || std::isspace(c); };

            const auto SRC = src;

            if (match_1(src, rule))
                return true;
            s = SRC;
            return utf8_non_ascii(src);
        };

        // ----------------------------------------------------

        const auto S = s;
        while (!s.empty() && utf8_char(s)) {
        }

        if (s.empty())
            return std::string_view::npos;

        return size_t(s.data() - S.data());
    }

    // https://stackoverflow.com/questions/48212992/how-to-find-out-if-there-is-any-non-ascii-character-in-a-string-with-a-file-path
    bool containsOnlyExtendedASCII(const std::string& filePath)
    {
        for (auto c : filePath) {
            if (static_cast<unsigned char>(c) > 255) {
                return false;
            }
        }
        return true;
    }

    // https://gist.github.com/GenesisFR/cceaf433d5b42dcdddecdddee0657292
    static std::string replaceAll(std::string str, const std::string& from, const std::string& to)
    {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
        }
        return str;
    }
} // namespace string_utilities
