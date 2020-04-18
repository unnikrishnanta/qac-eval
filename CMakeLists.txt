# only for cmake --version >= 3.5.1
cmake_minimum_required(VERSION 3.5.1)
if(APPLE)
    SET(CMAKE_C_COMPILER clang)
    SET(CMAKE_CXX_COMPILER clang++)
else()
    SET(CMAKE_C_COMPILER gcc)
    SET(CMAKE_CXX_COMPILER g++)
endif()
# set the C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

# project name
project(QAC_Eval)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
add_compile_options(
  "-Wall" "-Wpedantic" "-Wextra" "-fexceptions"
  "$<$<CONFIG:DEBUG>:-O0;-g3;-ggdb>"
)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
  message("Debug build.")
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
  message("Release build.")
ELSE()
  message("Some other build type.")
ENDIF()

# creates the variable MEM_PROF and sets it to hello
set(BUILD_TPROF bm_build)
set(QUERY_TPROF bm_query)
set(MEM_PROF mem_prof)
set(TEST_WRAPPERS test_wrappers)

# Boost libraries
# find_package( Boost REQUIRED COMPONENTS program_options iostreams)
# include_directories( ${Boost_INCLUDE_DIRS} )


add_subdirectory(src/core)
add_subdirectory(src/qac_impl)


# I../includes
include_directories(include)
include_directories(external/benchmark/include)

# puts all .cpp files inside src to the SOURCES variable
# file(GLOB SOURCES "src/*.cpp")
# compiles the files defined by SOURCES to generante the executable defined by MEM_PROF
add_executable(${MEM_PROF} src/mem_benchmark.cpp)
add_executable(${BUILD_TPROF} src/bm_build.cpp)
add_executable(${QUERY_TPROF} src/bm_query.cpp)
add_executable(${TEST_WRAPPERS} src/test_wrappers.cpp)

# Boost
# target_link_libraries( ${MEM_PROF} ${Boost_LIBRARIES})
# target_link_libraries( ${BUILD_TPROF} ${Boost_LIBRARIES})
# target_link_libraries( ${TEST_WRAPPERS} ${Boost_LIBRARIES})
# google benchmark
target_link_libraries(${BUILD_TPROF} "-L ../external/benchmark/build/src" benchmark)
target_link_libraries(${QUERY_TPROF} "-L ../external/benchmark/build/src" benchmark)
target_link_libraries(${BUILD_TPROF} pthread)
target_link_libraries(${QUERY_TPROF} pthread)
target_link_libraries(${TEST_WRAPPERS} pthread)

# QAC Evaluation Code libraries
target_link_libraries( ${MEM_PROF} core)
target_link_libraries( ${MEM_PROF} qac_impl)
# target_link_libraries( ${MEM_PROF} "-L ../src/qac_impl/marisa/lib" marisa)

target_link_libraries( ${BUILD_TPROF} core)
target_link_libraries( ${BUILD_TPROF} qac_impl)
# target_link_libraries( ${BUILD_TPROF} "-L ../src/qac_impl/marisa/lib" marisa)

target_link_libraries( ${QUERY_TPROF} core)
target_link_libraries( ${QUERY_TPROF} qac_impl)
# target_link_libraries( ${QUERY_TPROF} "-L ../src/qac_impl/marisa/lib" marisa)

target_link_libraries( ${TEST_WRAPPERS} core)
target_link_libraries( ${TEST_WRAPPERS} qac_impl)
# target_link_libraries( ${TEST_WRAPPERS} "-L ../src/qac_impl/marisa/lib" marisa)
# target_link_libraries( ${MEM_PROF} benchmark_main)

execute_process(COMMAND date "+%Y-%m-%d_%H-%M"
       OUTPUT_VARIABLE BUILD_TIME)
   string(STRIP ${BUILD_TIME} BUILD_TIME)

set(EXPORT_DIR  "${CMAKE_SOURCE_DIR}/export/data/")

add_custom_target(build-time
    COMMAND ${BUILD_TPROF}
            --benchmark_counters_tabular=true 
            --benchmark_out_format=csv
            --benchmark_out="${EXPORT_DIR}build-times.csv"
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

add_custom_target(query-time
    COMMAND ${QUERY_TPROF}
            --benchmark_counters_tabular=true 
            --benchmark_out_format=csv
            --benchmark_out="${EXPORT_DIR}query-times.csv"
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)
# Set up rsync
SET(path "*")
SET(HOST "raksasa.eng.unimelb.edu.au")
SET(USER "uthoombayil")
SET(COPY_PATH "~/CodeBase/QAC-Eval/")
SET(RSYNC_EXCLUDE "{Makefile,build/*,debug/*,export/*,*.swp,*.DS_Store,\
                    src/qac_impl/marisa/lib/*,external/*}")

add_custom_target(sync
    COMMAND rsync -av -e ssh * --exclude=${RSYNC_EXCLUDE} ${USER}@${HOST}:${COPY_PATH}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
