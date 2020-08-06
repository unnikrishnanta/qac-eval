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

#ifdef TEST_RUN 
#pragma message "TEST MODE ON. undef TEST_RUN from above"
#ifdef NROWS 
    #undef NROWS 
    #define NROWS 4<<20
#endif
#ifdef PQLOG_NCONV 
    #undef PQLOG_NCONV
    #define PQLOG_NCONV 512
#endif
#endif

// Querying time for varying collection size
// ===========================================================
/* BENCHMARK_REGISTER_F(QueryFixture, QueryHTrie) */
/*     ->RangeMultiplier(8) */
/*     ->Ranges({ */
/*                 {1<<9, NROWS}, */
/*                 {PQLOG_NCONV, PQLOG_NCONV}, */
/*                 {SLOG, LRLOG} */
/*              }) */
/*     ->Unit(benchmark::kMillisecond); */

/* BENCHMARK_REGISTER_F(QueryFixture, QueryMarisa) */
/*     ->RangeMultiplier(8) */
/*     ->Ranges({ */
/*                 {1<<9, NROWS}, */
/*                 {PQLOG_NCONV, PQLOG_NCONV}, */
/*                 {SLOG, LRLOG} */
/*              }) */
/*     ->Unit(benchmark::kMillisecond); */

/* BENCHMARK_REGISTER_F(QueryFixture, QueryDAWG) */
/*     ->RangeMultiplier(8) */
/*     ->Ranges({ */
/*                 {1<<9, NROWS}, */
/*                 {PQLOG_NCONV, PQLOG_NCONV}, */
/*                 {SLOG, LRLOG} */
/*              }) */
/*     ->Unit(benchmark::kMillisecond); */

/* BENCHMARK_REGISTER_F(QueryFixture, QueryIncNgTrie) */
/*     ->RangeMultiplier(8) */
/*     ->Ranges({ */
/*                 {1<<9, NROWS}, */
/*                 {PQLOG_NCONV, PQLOG_NCONV}, */
/*                 {SLOG, LRLOG} */
/*              }) */
/*     ->Unit(benchmark::kMillisecond); */

// Querying time for various bins of the legth of partial query
// ===========================================================

static void CustomArguments(benchmark::internal::Benchmark* b) {
    for (const char logtype: {SLOG, LRLOG}){
            b->Args(
                    {
                        NROWS, // collection size. 
                        PQLOG_NCONV,  // #Conversations from the PQLOLOG
                        logtype,
                    }
                   );
    }
}

BENCHMARK_REGISTER_F(QueryFixturePlen, QueryHTriePlen)
        ->Apply(CustomArguments)
        ->Unit(benchmark::kMillisecond)
        ->UseManualTime();

/* BENCHMARK_REGISTER_F(QueryFixturePlen, QueryMarisaPlen) */
/*         ->Apply(CustomArguments) */
/*         ->Unit(benchmark::kMillisecond) */
/*         ->UseManualTime(); */

BENCHMARK_REGISTER_F(QueryFixturePlen, QueryDAWGPlen)
        ->Apply(CustomArguments)
        ->Unit(benchmark::kMillisecond)
        ->UseManualTime();

/* BENCHMARK_REGISTER_F(QueryFixturePlen, QueryIncNgTriePlen) */
/*         ->Apply(CustomArguments) */
/*         ->Unit(benchmark::kMillisecond) */
/*         ->UseManualTime(); */

int main(int argc, char *argv[])
{
    // Empty contents from export file
    auto out_file = TimeLogger::generate_export_fname();
    std::ofstream new_file(out_file);  // overwrite existing file
    if (! new_file.is_open())
        throw std::runtime_error( "Unable to open output log file" );
    new_file.close();

    std::cout << "Loading collections\n";
    QueryBase::coll.read_collection(COLLECTION, NROWS, true);  // +1 for header
    QueryBase::qac_log.load_qaclog(SYNTHLOG, SIZE_MAX);
    std::cout << "\n<Benchmark>/*/"<< (int) SLOG << ": SynthLog benchmrks\n";
    std::cout << "<Benchmark>/*/"<< (int) LRLOG << ": LRLog benchmrks\n\n";
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    return 0;
}
