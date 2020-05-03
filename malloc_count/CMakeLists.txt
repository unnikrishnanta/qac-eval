set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
file(GLOB MC_SOURCES "*.c" ".h")
add_library(malloc_count ${MC_SOURCES})
