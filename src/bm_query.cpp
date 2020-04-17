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

/* BENCHMARK_REGISTER_F(QueryFixture, QueryHTrie) */
/*     ->RangeMultiplier(8) */
/*     /1* ->Ranges({{1<<5, 1<<5}, *1/ */
/*     ->Ranges({{NROWS, NROWS}, */
/*              /1* {1<<4, PQLOG_NCONV}, *1/ */
/*              {1<<4, 1<<12}, */
/*              {SLOG, LRLOG}}) */
/*     ->Unit(benchmark::kMillisecond); */

/* BENCHMARK_REGISTER_F(QueryFixture, QueryMarisa) */
/*     ->RangeMultiplier(8) */
/*     /1* ->Ranges({{1<<5, 1<<5}, *1/ */
/*     ->Ranges({{NROWS, NROWS}, */
/*              /1* {1<<4, PQLOG_NCONV}, *1/ */
/*              {1<<4, 1<<12}, */
/*              {SLOG, LRLOG}}) */
/*     ->Unit(benchmark::kMillisecond); */

/* BENCHMARK_REGISTER_F(QueryFixture, QueryDAWG) */
/*     ->RangeMultiplier(8) */
/*     /1* ->Ranges({{1<<5, 1<<5}, *1/ */
/*     ->Ranges({{NROWS, NROWS}, */
/*              /1* {1<<4, PQLOG_NCONV}, *1/ */
/*              {1<<4, 1<<12}, */
/*              {SLOG, LRLOG}}) */
/*     ->Unit(benchmark::kMillisecond); */

/* BENCHMARK_REGISTER_F(QueryFixture, QueryDAWG) */
/*     ->RangeMultiplier(8) */
/*     /1* ->Ranges({{1<<5, 1<<5}, *1/ */
/*     ->Ranges({{NROWS, NROWS}, */
/*              /1* {1<<4, PQLOG_NCONV}, *1/ */
/*              {1<<4, 1<<12}, */
/*              {SLOG, LRLOG}}) */
/*     ->Unit(benchmark::kMillisecond); */

/* BENCHMARK_REGISTER_F(QueryFixture, QueryIncNgTrie) */
/*     ->RangeMultiplier(8) */
/*     /1* ->Ranges({{1<<5, 1<<5}, *1/ */
/*     ->Ranges({{NROWS, NROWS}, */
/*              /1* {1<<4, PQLOG_NCONV}, *1/ */
/*              {1<<4, 1<<12}, */
/*              {SLOG, LRLOG}}) */
/*     ->Unit(benchmark::kMillisecond); */

/* BENCHMARK_REGISTER_F(QueryFixture, QueryIncNgTrie) */
/*     ->RangeMultiplier(8) */
/*     ->Ranges({{NROWS, NROWS}, */
/*              /1* {1<<4, PQLOG_NCONV}, *1/ */
/*              {1<<4, 1<<12}, */
/*              {LRLOG, LRLOG}}) */
/*     ->Unit(benchmark::kMillisecond); */

/* BENCHMARK_REGISTER_F(QueryFixture, QueryHTrie) */
/*     ->RangeMultiplier(8) */
/*     ->Ranges({{1<<5, 1<<5}, */
/*     /1* ->Ranges({{NROWS, NROWS}, *1/ */
/*              /1* {1<<4, PQLOG_NCONV}, *1/ */
/*              {1<<4, 1<<12}, */
/*              {SLOG, LRLOG}}) */
/*     ->Unit(benchmark::kMillisecond); */

BENCHMARK_REGISTER_F(QueryFixturePlen, QueryHTriePlen)
    ->RangeMultiplier(4)
    /* ->Ranges({{1<<5, 1<<5}, */
    ->Ranges({
                {NROWS, NROWS}, // collection size
                /* {1<<4, PQLOG_NCONV}, */
                {1<<8, 1<<12},  // # conversations from the partial qeury log. 
                {SLOG, LRLOG},
                {1,28}, {4,32}
             }
            ) // Log type. SLOG: Synth or Bing. 
    ->Unit(benchmark::kMillisecond);


int main(int argc, char *argv[])
{
    std::cout << "Loading collections\n";
    QueryBase::coll.read_collection(COLLECTION, NROWS, true);
    QueryBase::qac_loq.load_qaclog(SYNTHLOG, SIZE_MAX);
    std::cout << "\n<Benchmark>/*/"<< (int) SLOG << ": SynthLog benchmrks\n";
    std::cout << "<Benchmark>/*/"<< (int) LRLOG << ": LRLog benchmrks\n\n";
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    return 0;
}
