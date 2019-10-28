# only for cmake --version >= 3.5.1
cmake_minimum_required(VERSION 3.5.1)
SET(CMAKE_C_COMPILER gcc) 
SET(CMAKE_CXX_COMPILER g++)
# project name
project(QAC_Eval)

# creates the variable EXEC and sets it to hello
set(EXEC qac_eval)

# set the C++17 standard
set(CMAKE_CXX_STANDARD 17)



# Boost libraries
find_package( Boost REQUIRED COMPONENTS program_options iostreams)
include_directories( ${Boost_INCLUDE_DIRS} )


add_subdirectory(src/core)
add_subdirectory(src/qac_impl)

# I../includes
include_directories(include)
include_directories(external/benchmark/include)

# puts all .cpp files inside src to the SOURCES variable
file(GLOB SOURCES "src/*.cpp")
# compiles the files defined by SOURCES to generante the executable defined by EXEC
add_executable(${EXEC} ${SOURCES})

# Boost
target_link_libraries( ${EXEC} ${Boost_LIBRARIES})
# google benchmark
target_link_libraries(${EXEC} "-L ../external/benchmark/build/src" benchmark)
target_link_libraries(${EXEC} pthread)
target_link_libraries( ${EXEC} core)
target_link_libraries( ${EXEC} qac_impl)

