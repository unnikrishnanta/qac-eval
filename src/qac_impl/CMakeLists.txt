# only for cmake --version >= 3.5.1
cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-command-line-argument -g")
add_compile_options(
  "-Wall" "-Wpedantic" "-Wextra" "-fexceptions"
  "$<$<CONFIG:DEBUG>:-O0;-g3;-ggdb>"
)
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/hat-trie)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/marisa/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/IncNgTrie)
file(GLOB QAC_SOURCES "*.cpp" ".hpp")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/IncNgTrie)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/marisa/lib)

add_library(qac_impl SHARED ${QAC_SOURCES})
target_link_libraries(qac_impl IncNgTrie)
target_link_libraries(qac_impl "-L ${CMAKE_SOURCE_DIR}/external/marisa-trie/install/lib" marisa)
