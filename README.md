
# How to build and run the project

We generate and then build the CMake project in a cross platform way.

Open a command prompt MSDOS, and go where the CMakeLists.txt file lies.

MSDOS```
cd CppProcessFile
mkdir build
cd build
cmake build ..
cd ..
cmake --build build
.\build\Debug\cpp_process_file.exe %cd%\assets\texte.txt
```

# Run from Visual Studio

Add Debbugging/Command line arguments to the project, and put the fullpath the the asset file
"C:\Users\ThierryVilmartEXT\repos\temp\cpp_process_file\CppProcessFile\assets\texte.txt"
Select the project as Startup Project
Run in Visual Studio

# how the boost include sample was made

Boost is too big but one can generate a small subset using the bcp command:
bcp  boost/regex/pending/unicode_iterator.hpp boost/spirit/include/qi.hpp output

