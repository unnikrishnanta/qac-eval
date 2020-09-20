/* Benchmark the build-time of various QAC implementations using Google
 * benchmark library. 
 *
 * Release notes: 
 * =====================
 * Fixtures over simple benchmarks. With simple benchmarks, the file will be
 * reloaded for each iteration -> too slow
 *
 * Written by: Unni Krishnan. Last major modification: 18/03/2020.
 */
#include "core/collection.hpp"
#include "core/config.hpp"
#include <cstdint>
#include <iostream>
#include <benchmark/benchmark.h>
#include <memory>
#include <timeprof/benchmark_build.hpp>
#include <thread>

/* #define TEST_RUN 1 */
#ifdef TEST_RUN 
#pragma message "TEST MODE ON. undef TEST_RUN from above"
#ifdef NROWS 
    #undef NROWS 
    #define NROWS 320000
#endif
#ifdef PQLOG_NCONV 
    #undef PQLOG_NCONV
    #define PQLOG_NCONV 400
#endif
#endif

BENCHMARK_REGISTER_F(BuildFixture, BuildHTrie)
     ->RangeMultiplier(2)->Ranges({{1<<9, NROWS}});
BENCHMARK_REGISTER_F(BuildFixture, BuildMarisa)
     ->RangeMultiplier(2)->Ranges({{1<<9, NROWS}});
BENCHMARK_REGISTER_F(BuildFixture, BuildDAWG)
     ->RangeMultiplier(2)->Ranges({{1<<9, NROWS}});
BENCHMARK_REGISTER_F(BuildFixture, BuildIncNgT)
     ->RangeMultiplier(2)->Ranges({{1<<9, NROWS}, {1,1}});

// Define the static variable outside all functions
// https://stackoverflow.com/a/9282425/937153
Collection BuildFixture::coll_full;

int main(int argc, char** argv) {
  BuildFixture::coll_full.read_collection(COLLECTION, NROWS, true);
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
}
