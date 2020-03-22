/* Benchmark the querying-time of various QAC implementations using Google
 * benchmark library. 
 *
 * Written by: Unni Krishnan. On : 19/03/2020.
 */
#include "core/pqlog.hpp"
#include "qac_impl/htrie_wrapper.hpp"
#include <cstdint>
#include <iostream>
#include <benchmark/benchmark.h>
#include <timeprof/benchmark_query.hpp>

BENCHMARK_REGISTER_F(QueryFixture, QueryHTrie)
    ->RangeMultiplier(8)
    ->Ranges({{1000, 1000},
    /* ->Ranges({{NROWS, NROWS}, */
             {1<<4, WIKISYNTH_SIZE},
             {SYNTHLOG, SYNTHLOG}})
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(QueryFixture, QueryMarisa)
    ->RangeMultiplier(8)
    ->Ranges({{1000, 1000},
    /* ->Ranges({{NROWS, NROWS}, */
             {1<<4, WIKISYNTH_SIZE},
             {SYNTHLOG, SYNTHLOG}})
    ->Unit(benchmark::kMillisecond);


/* Declaring static members */
Collection QueryBase::coll;
PQLog QueryBase::synth_log;

int main(int argc, char *argv[])
{
    benchmark::Initialize(&argc, argv);
    QueryBase::coll.read_collection(COLLECTION, NROWS, true);
    QueryBase::synth_log.load_synthlog(WIKISYNTH, SIZE_MAX);
    std::cout << "\n<Benchmark>/*/"<< (int) SYNTHLOG << ": SynthLog benchmrks\n";
    std::cout << "<Benchmark>/*/"<< (int) LRLOG << ": LRLog benchmrks\n\n";
    benchmark::RunSpecifiedBenchmarks();
    return 0;
}
