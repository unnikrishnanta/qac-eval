set(CMAKE_CXX_STANDARD 17)
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
file(GLOB HEAPC_SOURCES "*.cpp" ".h")
add_library(heap_count ${HEAPC_SOURCES})

