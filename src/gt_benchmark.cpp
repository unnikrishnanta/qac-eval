/*
 * Fixtures over simple benchmarks. With simple benchmarks, the file will be
 * reloaded for each iteration -> too slow
 */
#include "core/collection.hpp"
#include <iostream>
#include <benchmark/benchmark.h>
#include <timeprof/benchmark-build.hpp>


#define WIKI 'w'
#define CWEB 'c'
#define WIKI_NROWS 5077651
#define CWEB_NROWS 195285642

#if COLLECTION==WIKI 
#define NROWS WIKI_NROWS
#else
#define NROWS CWEB_NROWS
#endif


 BENCHMARK_REGISTER_F(BuildFixture, BuildHTrie)
     ->RangeMultiplier(2)->Ranges({{1<<9, NROWS}});
 /* BENCHMARK_REGISTER_F(BuildFixture, BuildMarisa) */
 /*     ->RangeMultiplier(2)->Ranges({{1<<9, NROWS}}); */
 /* BENCHMARK_REGISTER_F(BuildFixture, BuildDAWG) */
 /*     ->RangeMultiplier(2)->Ranges({{1<<9, NROWS}}); */
 /* BENCHMARK_REGISTER_F(BuildFixture, BuildIncNgT) */
 /*     ->RangeMultiplier(2)->Ranges({{1<<9, NROWS}, {1,2}}); */
 /* BENCHMARK_REGISTER_F(HTrieFixture, SynthQuery)->Args({WIKI, 10000, 1000}); */

/* BENCHMARK_REGISTER_F(BuildFixture, BuildHTrie) */
/*     ->Arg(1<<9)->Arg(NROWS); */
/* BENCHMARK_REGISTER_F(BuildFixture, BuildMarisa) */
/*     ->Arg(1<<9)->Arg(NROWS); */
/* BENCHMARK_REGISTER_F(BuildFixture, BuildDAWG) */
/*     ->Arg(1<<9)->Arg(NROWS); */
/* BENCHMARK_REGISTER_F(BuildFixture, BuildIncNgT) */
/*     ->Arg(1<<9)->Arg(NROWS); */

int main(int argc, char** argv) {
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
}
