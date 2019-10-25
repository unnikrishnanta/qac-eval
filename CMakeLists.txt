# only for cmake --version >= 3.5.1
cmake_minimum_required(VERSION 3.5.1)

# project name
project(qac_eval)

# creates the variable EXEC and sets it to hello
set(EXEC qac_eval)

# set the C++17 standard
set(CMAKE_CXX_STANDARD 17)

# I../includes
include_directories(include)

# Boost libraries
find_package( Boost REQUIRED COMPONENTS program_options iostreams)
include_directories( ${Boost_INCLUDE_DIRS} )

# puts all .cpp files inside src to the SOURCES variable
file(GLOB SOURCES "src/*.cpp")
add_subdirectory(src/core)
add_subdirectory(src/qac_impl)

# compiles the files defined by SOURCES to generante the executable defined by EXEC
add_executable(${EXEC} ${SOURCES})
target_link_libraries( ${EXEC} ${Boost_LIBRARIES})
target_link_libraries( ${EXEC} ${core})
target_link_libraries( ${EXEC} ${qac_impl})

