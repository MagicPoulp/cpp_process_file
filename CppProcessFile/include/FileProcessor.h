#pragma once

#include <fstream>
#include <string>
#include <utility>
#include <vector>

class FileProcessor
{
public:
    void process(const std::string& inputPath, const std::string& outputPath) const;
    void validateEncoding(const std::string& word) const;
    void processWordForPairingToPoints(
        const std::string& word, std::vector<std::pair<std::string, int>>& pairingUniqueWordsToPoints) const;
    std::vector<std::pair<std::string, int>> createPairingUniqueWordsToPoints(const std::string& outputPath) const;
    void createSortedOutputFile(
        const std::string& outputPath, std::vector<std::pair<std::string, int>>& pairingUniqueWordsToPoints) const;
    int countPoints(const std::string& word) const;
};
