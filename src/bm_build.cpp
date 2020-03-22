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
#include <cstdint>
#include <iostream>
#include <benchmark/benchmark.h>
#include <memory>
#include <timeprof/benchmark_build.hpp>


#define WIKI 'w'
#define CWEB 'c'
#define WIKI_NROWS 5077651
#define CWEB_NROWS 195285642

#define COLLECTION WIKI

#if COLLECTION==WIKI 
#define NROWS WIKI_NROWS
#else
#define NROWS CWEB_NROWS
#endif


 BENCHMARK_REGISTER_F(BuildFixture, BuildHTrie)
     ->RangeMultiplier(2)->Ranges({{1<<9, NROWS}});
 BENCHMARK_REGISTER_F(BuildFixture, BuildMarisa)
     ->RangeMultiplier(2)->Ranges({{1<<9, NROWS}});
 BENCHMARK_REGISTER_F(BuildFixture, BuildDAWG)
     ->RangeMultiplier(2)->Ranges({{1<<9, NROWS}});
 BENCHMARK_REGISTER_F(BuildFixture, BuildIncNgT)
     ->RangeMultiplier(2)->Ranges({{1<<9, NROWS}, {1,2}});

 /* BENCHMARK_REGISTER_F(HTrieFixture, SynthQuery)->Args({WIKI, 10000, 1000}); */

// Define the static variable outside all functions
// https://stackoverflow.com/a/9282425/937153
Collection BuildFixture::coll;

int main(int argc, char** argv) {
  BuildFixture::coll.read_collection(COLLECTION, NROWS, true);
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
}
