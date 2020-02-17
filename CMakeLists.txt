# only for cmake --version >= 3.5.1
cmake_minimum_required(VERSION 3.5.1)
SET(CMAKE_C_COMPILER clang)
SET(CMAKE_CXX_COMPILER clang++)
# set the C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

# project name
project(QAC_Eval)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")

# if(APPLE)
#     set(CMAKE_CXX_FLAGS "-I/Library/Developer/CommandLineTools/usr/include/c++/v1/")
#     set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
#     file(COPY ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
#         DESTINATION ${CMAKE_SOURCE_DIR})
# endif()

# creates the variable MEM_PROF and sets it to hello
set(TIME_PROF time_prof)
set(MEM_PROF mem_prof)
set(TEST_WRAPPERS test_wrappers)

# Boost libraries
find_package( Boost REQUIRED COMPONENTS program_options iostreams)
include_directories( ${Boost_INCLUDE_DIRS} )


add_subdirectory(src/core)
add_subdirectory(src/qac_impl)

# I../includes
include_directories(include)
include_directories(external/benchmark/include)

# puts all .cpp files inside src to the SOURCES variable
# file(GLOB SOURCES "src/*.cpp")
# compiles the files defined by SOURCES to generante the executable defined by MEM_PROF
add_executable(${MEM_PROF} src/mem_benchmark.cpp)
add_executable(${TIME_PROF} src/gt_benchmark.cpp)
add_executable(${TEST_WRAPPERS} src/test_wrappers.cpp)

# Boost
target_link_libraries( ${MEM_PROF} ${Boost_LIBRARIES})
target_link_libraries( ${TIME_PROF} ${Boost_LIBRARIES})
target_link_libraries( ${TEST_WRAPPERS} ${Boost_LIBRARIES})
# google benchmark
target_link_libraries(${TIME_PROF} "-L ../external/benchmark/build/src" benchmark)
target_link_libraries(${TIME_PROF} pthread)
target_link_libraries(${TEST_WRAPPERS} pthread)

# QAC Evaluation Code libraries
target_link_libraries( ${MEM_PROF} core)
target_link_libraries( ${MEM_PROF} qac_impl)
target_link_libraries( ${TIME_PROF} core)
target_link_libraries( ${TIME_PROF} qac_impl)
target_link_libraries( ${TIME_PROF} marisa)
target_link_libraries( ${TEST_WRAPPERS} core)
target_link_libraries( ${TEST_WRAPPERS} qac_impl)
target_link_libraries( ${TEST_WRAPPERS} marisa)
# target_link_libraries( ${MEM_PROF} benchmark_main)

add_custom_target(run
    COMMAND ${TIME_PROF} --benchmark_repetitions=1 --benchmark_counters_tabular=true
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

# Set up rsync
SET(path "*")
SET(HOST "raksasa.eng.unimelb.edu.au")
SET(USER "uthoombayil")
SET(COPY_PATH "~/CodeBase/QAC-Eval/")
SET(RSYNC_EXCLUDE "{Makefile,build/*,*.swp,*.DS_Store,src/qac_impl/dawgdic/*,\
                    src/qac_impl/marisa/lib/*,external/*}")

add_custom_target(sync
    COMMAND rsync -av -e ssh * --exclude=${RSYNC_EXCLUDE} ${USER}@${HOST}:${COPY_PATH}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
