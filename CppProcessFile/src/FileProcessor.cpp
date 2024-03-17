
#include "FileProcessor.h"
#include "CustomExceptions.h"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace
{
    constexpr int GIGANTIC_LINE_SIZE = 10000;
}

FileProcessor::FileProcessor(std::string& inputPath, std::string& outputPath)
{
    m_inputFile.open(outputPath, ios::in);
    if (!m_inputFile.is_open()) {
        throw FileOpenException("Error - Impossible to open the input file.");
    }

    m_outputFile.open(outputPath, ios::trunc | ios::out);
    if (!m_outputFile.is_open()) {
        throw FileOpenException("Error - Impossible to open the output file.");
    }
}

// the RAII ensures tu free resources in case of exceptions
FileProcessor::~FileProcessor()
{
    try {
        m_inputFile.close();
    } catch (std::exception ex) {
        cerr << "Error while closing the input file" << endl;
    }
    try {
        m_outputFile.close();
    } catch (std::exception ex) {
        cerr << "Error while closing the output file" << endl;
    }
}

// It is much simpler to just save the data in memory and process it and save results.
// I chose to do only streaming to manage larger sets of data.
void FileProcessor::process()
{
    string tp;
    // reserving avoids heap allocation and speeds up
    tp.reserve(GIGANTIC_LINE_SIZE);
    while (getline(m_inputFile, tp)) {
        stringstream ss(tp);
        string word;
        while (ss >> word) {
            // processWord(word, resultPath);
        }
    }
}
