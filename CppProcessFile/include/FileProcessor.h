#pragma once

#include <fstream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

class FileProcessor
{
public:
    void process(const std::string& inputPath, const std::string& outputPath) const;
    void processWordWithoutDuplicates(const std::string& word,
        std::unordered_set<std::string_view>& hashSetProcessedWords,
        std::vector<std::pair<std::string, int>>& pairingUniqueWordsToPoints) const;
    void validateEncoding(const std::string& word) const;
    void processWordForPairingToPoints(
        const std::string& word, std::vector<std::pair<std::string, int>>& pairingUniqueWordsToPoints) const;
    std::vector<std::pair<std::string, int>> createPairingUniqueWordsToPoints(const std::string& outputPath) const;
    void createSortedOutputFile(
        const std::string& outputPath, std::vector<std::pair<std::string, int>>& pairingUniqueWordsToPoints) const;
    bool splitWordOnMultiByteApostrophe(const std::string& word, std::vector<std::string>& splitWords) const;
    int countPoints(const std::string& word) const;
};
