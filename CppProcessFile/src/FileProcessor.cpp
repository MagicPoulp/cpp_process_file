
#include "FileProcessor.h"
#include "CustomExceptions.h"
#include "StringUtilities.h"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

using namespace std;

namespace
{
    constexpr int GIGANTIC_LINE_SIZE = 10000;

    // we store bytes in UTF-8
    const map<string_view, int> mapLetterToPoints = { { "A", 32 },
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
        string tp;
        // reserving avoids heap allocation and speeds up
        // if a line is bigger, we still support it with re-allocation
        tp.reserve(GIGANTIC_LINE_SIZE);

        inputFile.open(inputPath, ios::in);
        if (!inputFile.is_open()) {
            throw FileOpenException("Error - Impossible to open the input file.");
        }
        // failbit is set for end of file so we ignore it
        inputFile.exceptions(std::ifstream::badbit);

        try {
            vector<pair<string, int>> pairingUniqueWordsToPoints;
            std::unordered_set<string_view> hashSetProcessedWords;
            while (getline(inputFile, tp)) {
                stringstream ss(tp);
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

void FileProcessor::createSortedOutputFile(
    const string& outputPath, const vector<pair<string, int>>& pairingUniqueWordsToPoints) const
{
    fstream outputFile;
    try {

        outputFile.open(outputPath, ios::trunc | ios::out);
        if (!outputFile.is_open()) {
            throw FileOpenException("Error - Impossible to open the output file.");
        }
        //     //outputFile << word << ", " << points << endl;
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
    // for now, we only support UTF8 input, we discard latin1
    if (auto pos = string_utilities::find_first_not_utf8(word); pos < word.length()) {
        throw NonUtf8CharactersFoundException();
    }

    // temporary
    // for now, we only support one-bye characters or we would need a cross-platform iteration on utf-8 strings
    // but the exercise requires no external library
    // https://stackoverflow.com/questions/4579215/cross-platform-iteration-of-unicode-string-counting-graphemes-using-icu
    auto onlyASCII = string_utilities::containsOnlyExtendedASCII(word);
    if (!onlyASCII) {
        throw NonExtendedASCIICharactersFoundException();
    }
    int points = countPoints(word);
    pairingUniqueWordsToPoints.emplace_back(pair(word, points));
}

int FileProcessor::countPoints(const std::string& word) const
{

    // TOODO: we need a cross platform way to iterate over a utf-8 string
    // but the exercise requires no external library
    int total = 0;
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
}
