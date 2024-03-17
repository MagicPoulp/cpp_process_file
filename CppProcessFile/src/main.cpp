
#include "main.h"
#include "CustomExceptions.h"
#include "FileProcessor.h"

#include <fstream>
#include <iostream>
#include <regex>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
    try {
        string inputPath = getFilePathFromArgv(argc, argv);
        regex reg("(.txt)$");
        string outputPath = std::regex_replace(inputPath, reg, ".count.txt");
        cout << "Processing the file" << endl << inputPath << endl;
        FileProcessor fileProcessor;
        fileProcessor.process(inputPath, outputPath);
        cout << "Processing success. The output lies in the file" << endl << outputPath << endl;
    } catch (CustomException& ex) {
        cerr << "Error with custom exception" << endl;
        cerr << ex.what() << endl;
        return 1;
    } catch (exception& ex) {
        cerr << "Error with unknown exception" << endl;
        cerr << ex.what() << endl;
        return 1;
    }
}

std::string getFilePathFromArgv(int argc, char* argv[])
{
    if (argc != 2) {
        throw ProgramArgumentsException("Error - Only 1 command line argument is expected.");
    }
    const auto filePath = std::string(argv[1]);
    if (filePath.length() < 10) {
        throw ProgramArgumentsException("Error - The path is too small, the full path is expected.");
    }
    fstream file;
    file.open(filePath, ios::in);
    if (!file.is_open()) {
        throw ProgramArgumentsException("Error - The file in the program argument could not be opened");
    }
    file.close();
    return filePath;
}
