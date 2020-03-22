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

#define WIKISYNTH_SIZE 1206653


BENCHMARK_REGISTER_F(QueryFixture, HTrieSynth)
    ->RangeMultiplier(8)
    ->Ranges({{NROWS, NROWS},
             {1<<4, WIKISYNTH_SIZE},
             {COLLECTION, COLLECTION}})
    ->Unit(benchmark::kMillisecond);
    /* ->RangeMultiplier(2)->Ranges({{1<<9, 100}}); */
    /* ->Args({1000, 100}); */

Collection QueryBase::coll;
PQLog QueryBase::pqlog;

int main(int argc, char *argv[])
{
    benchmark::Initialize(&argc, argv);
    QueryBase::coll.read_collection(COLLECTION, NROWS, true);
    QueryBase::pqlog.load_pqlog(SYNTHLOG, SIZE_MAX);
    benchmark::RunSpecifiedBenchmarks();
    return 0;
}
