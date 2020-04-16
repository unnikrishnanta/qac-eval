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
#include <timeprof/benchmark_query.hpp>

/* BENCHMARK_REGISTER_F(QueryFixture, QueryHTrie) */
/*     ->RangeMultiplier(8) */
/*     /1* ->Ranges({{1<<5, 1<<5}, *1/ */
/*     ->Ranges({{NROWS, NROWS}, */
/*              /1* {1<<4, WIKI_NCONV}, *1/ */
/*              {1<<4, 1<<12}, */
/*              {SLOG, LRLOG}}) */
/*     ->Unit(benchmark::kMillisecond); */

/* BENCHMARK_REGISTER_F(QueryFixture, QueryMarisa) */
/*     ->RangeMultiplier(8) */
/*     /1* ->Ranges({{1<<5, 1<<5}, *1/ */
/*     ->Ranges({{NROWS, NROWS}, */
/*              /1* {1<<4, WIKI_NCONV}, *1/ */
/*              {1<<4, 1<<12}, */
/*              {SLOG, LRLOG}}) */
/*     ->Unit(benchmark::kMillisecond); */

/* BENCHMARK_REGISTER_F(QueryFixture, QueryDAWG) */
/*     ->RangeMultiplier(8) */
/*     /1* ->Ranges({{1<<5, 1<<5}, *1/ */
/*     ->Ranges({{NROWS, NROWS}, */
/*              /1* {1<<4, WIKI_NCONV}, *1/ */
/*              {1<<4, 1<<12}, */
/*              {SLOG, LRLOG}}) */
/*     ->Unit(benchmark::kMillisecond); */

/* BENCHMARK_REGISTER_F(QueryFixture, QueryDAWG) */
/*     ->RangeMultiplier(8) */
/*     /1* ->Ranges({{1<<5, 1<<5}, *1/ */
/*     ->Ranges({{NROWS, NROWS}, */
/*              /1* {1<<4, WIKI_NCONV}, *1/ */
/*              {1<<4, 1<<12}, */
/*              {SLOG, LRLOG}}) */
/*     ->Unit(benchmark::kMillisecond); */

/* BENCHMARK_REGISTER_F(QueryFixture, QueryIncNgTrie) */
/*     ->RangeMultiplier(8) */
/*     /1* ->Ranges({{1<<5, 1<<5}, *1/ */
/*     ->Ranges({{NROWS, NROWS}, */
/*              /1* {1<<4, WIKI_NCONV}, *1/ */
/*              {1<<4, 1<<12}, */
/*              {SLOG, LRLOG}}) */
/*     ->Unit(benchmark::kMillisecond); */

BENCHMARK_REGISTER_F(QueryFixture, QueryIncNgTrie)
    ->RangeMultiplier(8)
    ->Ranges({{NROWS, NROWS},
             /* {1<<4, WIKI_NCONV}, */
             {1<<4, 1<<12},
             {LRLOG, LRLOG}})
    ->Unit(benchmark::kMillisecond);

string get_current_datetime() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");
    auto str = oss.str();
    return str;
}

/* Declaring static members */
Collection QueryBase::coll;
PQLog QueryBase::synth_log;

int main(int argc, char *argv[])
{
    benchmark::Initialize(&argc, argv);
    QueryBase::coll.read_collection(COLLECTION, NROWS, true);
    QueryBase::synth_log.load_synthlog(SYNTHLOG, SIZE_MAX);
    std::cout << "\n<Benchmark>/*/"<< (int) SLOG << ": SynthLog benchmrks\n";
    std::cout << "<Benchmark>/*/"<< (int) LRLOG << ": LRLog benchmrks\n\n";
    benchmark::RunSpecifiedBenchmarks();
    return 0;
}
