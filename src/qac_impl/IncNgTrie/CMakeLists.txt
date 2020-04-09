cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")
add_compile_options(
  "-Wall" "-fexceptions"
  "$<$<CONFIG:DEBUG>:-O0;-g3;-ggdb>"
)
# include_directories("/usr/local/opt/openssl/include")
include_directories("${CMAKE_SOURCE_DIR}/external/openssl/install/include")
message("${CMAKE_PROJECT_DIR}")
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
file(GLOB QAC_SOURCES "*.cc" ".h")
add_library(IncNgTrie  SHARED ${QAC_SOURCES})
# target_link_libraries(IncNgTrie "-L/usr/local/opt/openssl/lib" ssl)
# target_link_libraries(IncNgTrie crypto)
target_link_libraries(IncNgTrie "-L${CMAKE_SOURCE_DIR}/external/openssl/install/lib" ssl)
target_link_libraries(IncNgTrie "-L${CMAKE_SOURCE_DIR}/external/openssl/install/lib" crypto)
