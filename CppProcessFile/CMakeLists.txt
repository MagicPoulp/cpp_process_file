cmake_minimum_required(VERSION 3.15...3.25)

project(CppProcessFile
  VERSION
    1.0
  DESCRIPTION
    "Processing a file of data"
  LANGUAGES
    CXX
)

file(GLOB cpp_process_file_SRC
     "src/*.cpp"
)

add_executable(cpp_process_file ${cpp_process_file_SRC})
target_include_directories(cpp_process_file PRIVATE include)
