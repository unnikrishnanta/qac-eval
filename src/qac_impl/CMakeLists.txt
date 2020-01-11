# only for cmake --version >= 3.5.1
cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -lmarisa")
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
file(GLOB QAC_SOURCES "*.cpp" ".hpp")
include_directories(hat-trie)
include_directories(marisa/include)
include_directories("dawgdic/include")

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/dawgdic/bin)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/marisa/lib)
add_library(qac_impl SHARED ${QAC_SOURCES})
