cmake_minimum_required(VERSION 3.15...3.25)
set(CMAKE_CXX_STANDARD 20)

project(CppProcessFile
  VERSION
    1.0
  DESCRIPTION
    "Processing a file of data"
  LANGUAGES
    CXX
)

file(GLOB cpp_process_file_SOURCES "src/*.cpp")
file(GLOB cpp_process_file_HEADERS "include/*.h")

source_group("Headers" FILES ${cpp_process_file_HEADERS})

add_executable(cpp_process_file ${cpp_process_file_SOURCES} ${cpp_process_file_HEADERS})
target_include_directories(cpp_process_file PRIVATE include)
