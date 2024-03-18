
#include "FileProcessor.h"
#include "BoostUtilities.h"
#include "CustomExceptions.h"
#include "StringUtilities.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ranges>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

namespace
{
    constexpr int GIGANTIC_LINE_SIZE = 10000;

    // we store bytes in UTF-8
    const unordered_map<u8string_view, int> mapLetterToPoints = { { u8"A", 32 },
        { u8"B", 36 },
        { u8"C", 33 },
        { u8"D", 40 },
        { u8"E", 41 },
        { u8"F", 47 },
        { u8"G", 31 },
        { u8"H", 27 },
        { u8"I", 49 },
        { u8"J", 28 },
        { u8"K", 30 },
        { u8"L", 42 },
        { u8"M", 29 },
        { u8"N", 38 },
        { u8"O", 51 },
        { u8"P", 43 },
        { u8"Q", 45 },
        { u8"R", 39 },
        { u8"S", 35 },
        { u8"T", 52 },
        { u8"U", 37 },
        { u8"V", 46 },
        { u8"W", 34 },
        { u8"X", 48 },
        { u8"Y", 44 },
        { u8"Z", 50 },
        { u8"é", 60 },
        { u8"è", 61 },
        { u8"ê", 62 },
        { u8"à", 63 },
        { u8"â", 64 },
        { u8"ë", 65 },
        { u8"û", 66 },
        { u8"ù", 67 },
        { u8"î", 68 },
        { u8"ç", 69 },
        { u8"ô", 70 },
        { u8"ö", 71 },
        { u8"ü", 72 },
        { u8"a", 1 },
        { u8"b", 4 },
        { u8"c", 5 },
        { u8"d", 8 },
        { u8"e", 10 },
        { u8"f", 11 },
        { u8"g", 13 },
        { u8"h", 16 },
        { u8"i", 18 },
        { u8"j", 19 },
        { u8"k", 21 },
        { u8"l", 21 },
        { u8"m", 23 },
        { u8"n", 2 },
        { u8"o", 3 },
        { u8"p", 6 },
        { u8"q", 7 },
        { u8"r", 9 },
        { u8"s", 12 },
        { u8"t", 14 },
        { u8"u", 15 },
        { u8"v", 17 },
        { u8"w", 20 },
        { u8"x", 24 },
        { u8"y", 25 },
        { u8"z", 26 } };
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
                regex reg("[',]");
                // here we cannot replace the multi byte apostrophe, but we do it in splitWordOnMultiByteApostrophe
                while (regex_search(lineBuffer, reg)) {
                    lineBuffer = std::regex_replace(lineBuffer, reg, "");
                }
                stringstream ss(lineBuffer);
                string word;
                while (ss >> word) {
                    std::vector<std::string> splitWords;
                    // this is to split the word into multiple words on the multi-byte apostrophe
                    if (splitWordOnMultiByteApostrophe(word, splitWords)) {
                        std::ranges::for_each(splitWords,
                            [this, &hashSetProcessedWords, &pairingUniqueWordsToPoints](std::string& newWord) {
                                processWordWithoutDuplicates(
                                    newWord, hashSetProcessedWords, pairingUniqueWordsToPoints);
                            });
                    } else {
                        processWordWithoutDuplicates(word, hashSetProcessedWords, pairingUniqueWordsToPoints);
                    }
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

void FileProcessor::processWordWithoutDuplicates(const string& word,
    std::unordered_set<string_view>& hashSetProcessedWords,
    vector<pair<string, int>>& pairingUniqueWordsToPoints) const
{
    // there must be no duplicates
    if (hashSetProcessedWords.contains(word)) {
        return;
    }
    processWordForPairingToPoints(word, pairingUniqueWordsToPoints);
    hashSetProcessedWords.insert(word);
}

void FileProcessor::validateEncoding(const string& word) const
{
    // for now, we only support UTF8 input, we discard latin1
    if (auto pos = string_utilities::find_first_not_utf8(word); pos < word.length()) {
        throw NonUtf8CharactersFoundException();
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

// the apostrophe, U2019 has code 8217
// https://unicode.scarfboy.com/?s=U%2b2019
bool FileProcessor::splitWordOnMultiByteApostrophe(const std::string& word, vector<std::string>& splitWords) const
{
    if (string trimmedWord; boost_utilities::replaceMultiByteApostropheWithBlank(word, trimmedWord)) {
        stringstream check1(trimmedWord);
        string buffer1;
        // Tokenizing with regard to space ' '
        while (getline(check1, buffer1, ' ')) {
            splitWords.push_back(buffer1);
        }
        return true;
    }
    return false;
}

// "ü" takes 2 bytes in UTF-8, so we need to iterate on multi-byte characters
int FileProcessor::countPoints(const std::string& word) const
{
    int total                                 = 0;
    std::function<void(uint32_t&)> funIterate = [&total](uint32_t& codePoint) {
        // tested to work on trema u code 252
        // char32_t codePoint2 = (char32_t)252;
        std::u8string key = reinterpret_cast<const char8_t*>(boost_utilities::codepoint_to_utf8(codePoint).c_str());
        try {
            int points = mapLetterToPoints.at(key);
            total += points;
        } catch (out_of_range&) {
            // any other character counts as 0
            std::ignore = 0;
        }
    };
    boost_utilities::iterateOnMultiByteCharacters(word, funIterate);
    return total;
}
