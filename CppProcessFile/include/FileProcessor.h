#pragma once

#include <fstream>
#include <string>

class FileProcessor
{
public:
    explicit FileProcessor(std::string& inputPath, std::string& outputPath);
    ~FileProcessor();

    void process();

private:
    std::fstream m_inputFile;
    std::fstream m_outputFile;
};
