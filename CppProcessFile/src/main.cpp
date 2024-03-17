
#include "main.h"
#include "FileProcessor.h"
#include "CustomExceptions.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
    string filePath = getFilePathFromArgv(argc, argv);
    FileProcessor fileProcessor;
    fileProcessor.process(filePath);
    cout << "Processing the file" << endl << filePath << endl;
}

std::string getFilePathFromArgv(int argc, char* argv[]) {
    if (argc != 2) {
        throw ProgramArgumentsException("Error - Only 1 command line argument is expected.");
    }
    const auto filePath = std::string(argv[1]);
    if (filePath.length() <10) {
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
