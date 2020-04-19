/* Benchmark the querying-time of various QAC implementations using Google
 * benchmark library. 
 *
 * Written by: Unni Krishnan. On : 19/03/2020.
 */
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include "core/pqlog.hpp"
#include "core/config.hpp"
#include "qac_impl/htrie_wrapper.hpp"
#include <benchmark/benchmark.h>
#include <timeprof/benchmark_query_full.hpp>
#include <timeprof/benchmark_query_plen.hpp>

/* #define TEST_RUN 1 */
#ifdef TEST_RUN 
    #pragma message "TEST MODE ON. undef TEST_RUN from above"
    #ifdef NROWS 
        #undef NROWS 
        #define NROWS 10000
    #endif
    #ifdef PQLOG_NCONV 
        #undef PQLOG_NCONV
        #define PQLOG_NCONV 100
    #endif
#endif

// Querying time for varying collection size
// ===========================================================
BENCHMARK_REGISTER_F(QueryFixture, QueryHTrie)
    ->RangeMultiplier(8)
    ->Ranges({
                {1<<9, NROWS},
                {PQLOG_NCONV, PQLOG_NCONV},
                {SLOG, LRLOG}
             })
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(QueryFixture, QueryMarisa)
    ->RangeMultiplier(8)
    ->Ranges({
                {1<<9, NROWS},
                {PQLOG_NCONV, PQLOG_NCONV},
                {SLOG, LRLOG}
             })
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(QueryFixture, QueryDAWG)
    ->RangeMultiplier(8)
    ->Ranges({
                {1<<9, NROWS},
                {PQLOG_NCONV, PQLOG_NCONV},
                {SLOG, LRLOG}
             })
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(QueryFixture, QueryIncNgTrie)
    ->RangeMultiplier(8)
    ->Ranges({
                {1<<9, NROWS},
                {PQLOG_NCONV, PQLOG_NCONV},
                {SLOG, LRLOG}
             })
    ->Unit(benchmark::kMillisecond);

// Querying time for various bins of the legth of partial query
// ===========================================================

static void CustomArguments(benchmark::internal::Benchmark* b) {
    const int bin_width = 4;
    for (const char logtype: {SLOG, LRLOG}){
        for (int j = 0; j <= MAX_STRLEN - bin_width; j += bin_width)
            b->Args(
                    {
                        NROWS, // collection size. 
                        PQLOG_NCONV,  // #Conversations from the PQLOLOG
                        logtype,
                        j,  // |P| lower bound
                        j + bin_width + 1,  // |P| upper bound
                    }
                   );
    }
}

BENCHMARK_REGISTER_F(QueryFixturePlen, QueryHTriePlen)
        ->Apply(CustomArguments)
        ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(QueryFixturePlen, QueryMarisaPlen)
        ->Apply(CustomArguments)
        ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(QueryFixturePlen, QueryDAWGPlen)
        ->Apply(CustomArguments)
        ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(QueryFixturePlen, QueryIncNgTriePlen)
        ->Apply(CustomArguments)
        ->Unit(benchmark::kMillisecond);

int main(int argc, char *argv[])
{
    std::cout << "Loading collections\n";
    QueryBase::coll.read_collection(COLLECTION, NROWS+1, true);  // +1 for header
    QueryBase::qac_loq.load_qaclog(SYNTHLOG, SIZE_MAX);
    std::cout << "\n<Benchmark>/*/"<< (int) SLOG << ": SynthLog benchmrks\n";
    std::cout << "<Benchmark>/*/"<< (int) LRLOG << ": LRLog benchmrks\n\n";
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    return 0;
}
