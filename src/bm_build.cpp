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

/* BENCHMARK_REGISTER_F(BuildFixture, BuildHTrie) */
/*      ->RangeMultiplier(2)->Ranges({{1<<9, NROWS}}); */
/* BENCHMARK_REGISTER_F(BuildFixture, BuildMarisa) */
/*      ->RangeMultiplier(2)->Ranges({{1<<9, NROWS}}); */
/* BENCHMARK_REGISTER_F(BuildFixture, BuildDAWG) */
/*      ->RangeMultiplier(2)->Ranges({{1<<9, NROWS}}); */
/* BENCHMARK_REGISTER_F(BuildFixture, BuildIncNgT) */
/*      ->RangeMultiplier(2)->Ranges({{1<<9, NROWS}, {1,1}}); */

static void BM_ManualTiming(benchmark::State& state) {
  int microseconds = state.range(0);
  std::chrono::duration<double, std::micro> sleep_duration {
    static_cast<double>(microseconds)
  };

  for (auto _ : state) {
    auto start = std::chrono::high_resolution_clock::now();
    // Simulate some useful workload with a sleep
    std::this_thread::sleep_for(sleep_duration);
    auto end = std::chrono::high_resolution_clock::now();

    auto elapsed_seconds =
      std::chrono::duration_cast<std::chrono::duration<double>>(
        end - start);

    state.SetIterationTime(elapsed_seconds.count());
  }
}
BENCHMARK(BM_ManualTiming)->Range(1, 1<<17)->UseManualTime();

// Define the static variable outside all functions
// https://stackoverflow.com/a/9282425/937153
Collection BuildFixture::coll;

int main(int argc, char** argv) {
  BuildFixture::coll.read_collection(COLLECTION, NROWS, true);
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
}
