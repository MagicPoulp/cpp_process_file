
#include "FileProcessor.h"
#include "CustomExceptions.h"
#include "StringUtilities.h"

#include <fstream>
#include <iostream>
#include <ranges>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <boost/range.hpp>
#include <boost/regex/pending/unicode_iterator.hpp>
#include <boost/spirit/include/qi.hpp>

using namespace std;

namespace
{
    constexpr int GIGANTIC_LINE_SIZE = 10000;

    // we store bytes in UTF-8
    const unordered_map<string_view, int> mapLetterToPoints = { { "A", 32 },
        { "B", 36 },
        { "C", 33 },
        { "D", 40 },
        { "E", 41 },
        { "F", 47 },
        { "G", 31 },
        { "H", 27 },
        { "I", 49 },
        { "J", 28 },
        { "K", 30 },
        { "L", 42 },
        { "M", 29 },
        { "N", 38 },
        { "O", 51 },
        { "P", 43 },
        { "Q", 45 },
        { "R", 39 },
        { "S", 35 },
        { "T", 52 },
        { "U", 37 },
        { "V", 46 },
        { "W", 34 },
        { "X", 48 },
        { "Y", 44 },
        { "Z", 50 },
        { "é", 60 },
        { "è", 61 },
        { "ê", 62 },
        { "à", 63 },
        { "â", 64 },
        { "ë", 65 },
        { "û", 66 },
        { "ù", 67 },
        { "î", 68 },
        { "ç", 69 },
        { "ô", 70 },
        { "ö", 71 },
        { "ü", 72 },
        { "a", 1 },
        { "b", 4 },
        { "c", 5 },
        { "d", 8 },
        { "e", 10 },
        { "f", 11 },
        { "g", 13 },
        { "h", 16 },
        { "i", 18 },
        { "j", 19 },
        { "k", 21 },
        { "l", 21 },
        { "m", 23 },
        { "n", 2 },
        { "o", 3 },
        { "p", 6 },
        { "q", 7 },
        { "r", 9 },
        { "s", 12 },
        { "t", 14 },
        { "u", 15 },
        { "v", 17 },
        { "w", 20 },
        { "x", 24 },
        { "y", 25 },
        { "z", 26 } };
} // namespace

void FileProcessor::process(const string& inputPath, const string& outputPath) const
{
    vector<pair<string, int>> pairingUniqueWordsToPoints = createPairingUniqueWordsToPoints(inputPath);
    createSortedOutputFile(outputPath, pairingUniqueWordsToPoints);
}

vector<pair<string, int>> FileProcessor::createPairingUniqueWordsToPoints(const string& inputPath) const
{
    fstream inputFile;
    try {
        string lineBuffer;
        // reserving avoids heap allocation and speeds up
        // if a line is bigger, we still support it with re-allocation
        lineBuffer.reserve(GIGANTIC_LINE_SIZE);

        inputFile.open(inputPath, ios::in);
        if (!inputFile.is_open()) {
            throw FileOpenException("Error - Impossible to open the input file.");
        }
        // failbit is set for end of file so we ignore it
        inputFile.exceptions(std::ifstream::badbit);

        try {
            vector<pair<string, int>> pairingUniqueWordsToPoints;
            std::unordered_set<string_view> hashSetProcessedWords;
            while (getline(inputFile, lineBuffer)) {
                regex reg("[',’]");
                while (regex_search(lineBuffer, reg)) {
                    lineBuffer = std::regex_replace(lineBuffer, reg, "");
                }
                stringstream ss(lineBuffer);
                string word;
                while (ss >> word) {
                    // there must be no duplicates
                    if (hashSetProcessedWords.contains(word)) {
                        continue;
                    }
                    processWordForPairingToPoints(word, pairingUniqueWordsToPoints);
                    hashSetProcessedWords.insert(word);
                }
            }
            return pairingUniqueWordsToPoints;
        } catch (std::ifstream::failure& e) {
            std::cerr << "Exception happened: " << e.what() << "\n"
                      << "Error bits are: "
                      << "\nfailbit: " << inputFile.fail() << "\neofbit: " << inputFile.eof()
                      << "\nbadbit: " << inputFile.bad() << std::endl;
            throw FileReadException();
        }
    } catch (CustomException& ex) {
        inputFile.close();
        throw ex;
    } catch (exception& ex) {
        inputFile.close();
        throw ex;
    }
}

void FileProcessor::validateEncoding(const string& word) const
{
    // for now, we only support UTF8 input, we discard latin1
    if (auto pos = string_utilities::find_first_not_utf8(word); pos < word.length()) {
        throw NonUtf8CharactersFoundException();
    }

    // temporary
    // for now, we only support one-bye characters or we would need a cross-platform iteration on utf-8 strings
    // but the exercise requires no external library
    // https://stackoverflow.com/questions/4579215/cross-platform-iteration-of-unicode-string-counting-graphemes-using-icu
    if (auto onlyASCII = string_utilities::containsOnlyExtendedASCII(word); !onlyASCII) {
        throw NonExtendedASCIICharactersFoundException();
    }
}

void FileProcessor::createSortedOutputFile(
    const string& outputPath, vector<pair<string, int>>& pairingUniqueWordsToPoints) const
{
    fstream outputFile;
    try {

        outputFile.open(outputPath, ios::trunc | ios::out);
        if (!outputFile.is_open()) {
            throw FileOpenException("Error - Impossible to open the output file.");
        }
        ranges::sort(
            pairingUniqueWordsToPoints, [](const std::pair<string, int>& left, const std::pair<string, int>& right) {
                return left.second < right.second;
            });
        std::ranges::for_each(pairingUniqueWordsToPoints, [&outputFile](const std::pair<string, int>& element) {
            if (element.first == "mûr")
                throw CustomException();
            outputFile << element.first << ", " << element.second << endl;
        });
    } catch (CustomException& ex) {
        outputFile.close();
        throw ex;
    } catch (exception& ex) {
        outputFile.close();
        throw ex;
    }
}

void FileProcessor::processWordForPairingToPoints(
    const string& word, vector<pair<std::string, int>>& pairingUniqueWordsToPoints) const
{
    validateEncoding(word);
    int points = countPoints(word);
    pairingUniqueWordsToPoints.emplace_back(pair(word, points));
}

// "ü" takes 2 bytes
// so we need to iterate over multi-byte characters in UTF8
// https://stackoverflow.com/questions/13679669/how-to-use-boostspirit-to-parse-utf-8
int FileProcessor::countPoints(const std::string& word) const
{
    using namespace boost;
    using namespace spirit::qi;
    using namespace std;

    const char8_t* u8word = new char8_t(word.size());
    try {
        auto&& utf8_text = u8word;
        u8_to_u32_iterator<const char*> tbegin(begin(utf8_text)), tend(end(utf8_text));

        vector<uint32_t> result;
        parse(tbegin, tend, *standard_wide::char_, result);
        for (auto&& code_point : result)
            cout << "&#" << code_point << ";";
        cout << endl;

        // TOODO: we need a cross platform way to iterate over a utf-8 string
        int total = 0;
        int size  = 0;
        for (const char& c : word) {
            string key{ c };
            try {
                int points = mapLetterToPoints.at(key);
                total += points;
            } catch (out_of_range&) {
                // any other character counts as 0
                std::ignore = 0;
            }
        }
        return total;
    } catch (exception& ex) {
        delete[] (u8word);
        throw ex;
    }
}
