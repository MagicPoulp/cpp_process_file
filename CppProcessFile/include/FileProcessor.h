#pragma once

#include <fstream>
#include <string>

class FileProcessor
{
public:
    void process(const std::string& inputPath, const std::string& outputPath);
    void processWord(const std::string& word, std::fstream& outputFile) const;
    int countPoints(const std::string& word) const;
};
