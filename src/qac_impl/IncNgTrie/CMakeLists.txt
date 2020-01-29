cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g\
    -Wno-unused-command-line-argument \
    -I/usr/local/opt/openssl/include \
    -L/usr/local/opt/openssl/lib -lssl -lcrypto ")
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
file(GLOB QAC_SOURCES "*.cc" ".h")
add_library(IncNgTrie  SHARED ${QAC_SOURCES})
