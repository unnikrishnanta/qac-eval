# only for cmake --version >= 3.5.1
cmake_minimum_required(VERSION 3.5.1)

file(GLOB QAC_SOURCES "*.cpp" ".hpp")
add_library(qac_impl ${QAC_SOURCES})
