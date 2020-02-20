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
file(GLOB QAC_SOURCES "*.cpp" ".hpp")
include_directories(hat-trie)
include_directories(marisa/include)
# include_directories("dawgdic/include")
include_directories(IncNgTrie)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/IncNgTrie)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/dawgdic/bin)
# link_directories(${CMAKE_CURRENT_SOURCE_DIR}/marisa/lib)
# link_directories(${CMAKE_CURRENT_SOURCE_DIR}/IncNgTrie)

add_library(qac_impl SHARED ${QAC_SOURCES})
target_link_libraries(qac_impl IncNgTrie)
target_link_libraries(qac_impl "-L ${CMAKE_CURRENT_SOURCE_DIR}/marisa/lib" marisa)
