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


BENCHMARK_REGISTER_F(QueryFixture, SynthQuery)->Args({1000, 100});

Collection QueryFixture::coll;
PQLog QueryFixture::pqlog;

int main(int argc, char *argv[])
{
    benchmark::Initialize(&argc, argv);
    QueryFixture::coll.read_collection(COLLECTION, NROWS, true);
    QueryFixture::pqlog.load_pqlog(SYNTHLOG, SIZE_MAX);
    benchmark::RunSpecifiedBenchmarks();
    return 0;
}
