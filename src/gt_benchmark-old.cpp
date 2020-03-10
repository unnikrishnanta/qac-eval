
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/join.hpp>
#include "core/collection.hpp"
#include "core/pqlog.hpp"
#include "qac_impl/htrie_wrapper.hpp"
#include "qac_impl/dawgtrie_wrapper.hpp"
#include "qac_impl/marisa_wrapper.hpp"
#include "qac_impl/IncNgTrie_wrapper.hpp"
#include <benchmark/benchmark.h>
#include <numeric>

#define TEST_HTRIE 1
#define TEST_MARISA 1
/* #define TEST_DAWG 1 */
#define TEST_INCGT 1


using namespace std;
using namespace boost::program_options;

static void  BM_build_htrie(benchmark::State& state) {
    Collection coll_wiki('w');
    coll_wiki.read_collection(SIZE_MAX, true);
    for (auto _ : state){
        state.PauseTiming(); // Stop timers.
        HTrieCompleter ht_comp;
        state.ResumeTiming(); // And resume timers.
        ht_comp.build_index(coll_wiki);
    }
}

static void  BM_build_marisa(benchmark::State& state) {
    Collection coll_wiki('w');
    coll_wiki.read_collection(SIZE_MAX, true);
    MarisaCompleter mtc;
    for (auto _ : state){
        mtc.build_index(coll_wiki);
    }
}

static void  BM_build_dawg(benchmark::State& state) {
    Collection coll_wiki('w');
    const string wiki_file = WIKI_ROOT + "clickstream-agg-wiki-64B.tsv";
    coll_wiki.read_collection(SIZE_MAX, true);

    for (auto _ : state){
        DAWGTrie dtrie (coll_wiki);
    }
}

static void  BM_build_IncNgT(benchmark::State& state) {
    Collection coll_wiki('w');
    const string wiki_file = WIKI_ROOT + "clickstream-agg-wiki-64B.tsv";
    coll_wiki.read_collection(SIZE_MAX, true);

    for (auto _ : state){
        IncNgTrieCompleter inc(1);
        inc.build_index(coll_wiki);
    }
}

static void BM_synth_query_htrie(benchmark::State& state) {
    size_t n_comp = state.range(0);
    Collection coll_wiki('w');
    const string wiki_file = WIKI_ROOT + "clickstream-agg-wiki-64B.tsv";
    coll_wiki.read_collection(SIZE_MAX, true);
    PQLog pqlog;
    pqlog.load_pqlog("../../synth_log/data/wiki-synthlog.tsv", 1000);
    HTrieCompleter ht_comp;
    ht_comp.build_index(coll_wiki);
    /* ht_comp.print_index_meta(); */
    double num_completions=0, plen_sum=0, num_pq=0;
    for (auto _ : state){
        for (const auto& [qid, pvec]: pqlog) {
            for(const auto& p: pvec){
                ++num_pq;
                auto completions = ht_comp.complete(p, n_comp);
                num_completions += completions.size();
                plen_sum += p.length();
            }
        }
    }
    // Avg. no. of completions per partial query
    state.counters["NCompAvg"] = (double)num_completions/num_pq;
    state.counters["NPQ"] = num_pq; // number of partial queries
    // Total length of partial queries processed.
    state.counters["PQBytes"] = benchmark::Counter(plen_sum,
                                 benchmark::Counter::kIsIterationInvariantRate,
                                 benchmark::Counter::OneK::kIs1024);
    // How many partial queries are processed per second
    state.counters["PQRate"] = benchmark::Counter(num_pq, benchmark::Counter::kIsRate);
    // How many seconds it takes to process one partial query
    state.counters["PQInvRate"] = benchmark::Counter(num_pq,
                                    benchmark::Counter::kIsRate |
                                    benchmark::Counter::kInvert);

}

static void BM_synth_query_marisa(benchmark::State& state) {
    size_t n_comp = state.range(0);
    Collection coll_wiki('w');
    const string wiki_file = WIKI_ROOT + "clickstream-agg-wiki-64B.tsv";
    coll_wiki.read_collection(SIZE_MAX, true);
    PQLog pqlog;
    pqlog.load_pqlog("../../synth_log/data/wiki-synthlog.tsv", 1000);
    MarisaCompleter mtc;
    mtc.build_index(coll_wiki);
    double num_completions=0, plen_sum=0, num_pq=0;
    for (auto _ : state){
        for (const auto& [qid, pvec]: pqlog) {
            for(const auto& p: pvec){
                ++num_pq;
                auto completions = mtc.complete(p, n_comp);
                num_completions += completions.size();
                plen_sum += p.length();
            }
        }
    }
    // Avg. no. of completions per partial query
    state.counters["NCompAvg"] = (double)num_completions/num_pq;
    state.counters["NPQ"] = num_pq; // number of partial queries
    // Total length of partial queries processed.
    state.counters["PQBytes"] = benchmark::Counter(plen_sum,
                                 benchmark::Counter::kIsIterationInvariantRate,
                                 benchmark::Counter::OneK::kIs1024);
    // How many partial queries are processed per second
    state.counters["PQRate"] = benchmark::Counter(num_pq, benchmark::Counter::kIsRate);
    // How many seconds it takes to process one partial query
    state.counters["PQInvRate"] = benchmark::Counter(num_pq,
                                    benchmark::Counter::kIsRate |
                                    benchmark::Counter::kInvert);

}

static void BM_synth_query_IncNgT(benchmark::State& state) {
    size_t n_comp = state.range(0);
    Collection coll_wiki('w');
    const string wiki_file = WIKI_ROOT + "clickstream-agg-wiki-64B.tsv";
    coll_wiki.read_collection(SIZE_MAX, true);
    PQLog pqlog;
    pqlog.load_pqlog("../../synth_log/data/wiki-synthlog.tsv", 1000);
    IncNgTrieCompleter inc(1);
    inc.build_index(coll_wiki);
    double num_completions=0, plen_sum=0, num_pq=0;
    for (auto _ : state){
        for (const auto& [qid, pvec]: pqlog) {
            for(const auto& p: pvec){
                ++num_pq;
                auto completions = inc.complete(p, n_comp);
                num_completions += completions.size();
                plen_sum += p.length();
            }
        }
    }
    // Avg. no. of completions per partial query
    state.counters["NCompAvg"] = (double)num_completions/num_pq;
    state.counters["NPQ"] = num_pq; // number of partial queries
    // Total length of partial queries processed.
    state.counters["PQBytes"] = benchmark::Counter(plen_sum,
                                 benchmark::Counter::kIsIterationInvariantRate,
                                 benchmark::Counter::OneK::kIs1024);
    // How many partial queries are processed per second
    state.counters["PQRate"] = benchmark::Counter(num_pq, benchmark::Counter::kIsRate);
    // How many seconds it takes to process one partial query
    state.counters["PQInvRate"] = benchmark::Counter(num_pq,
                                    benchmark::Counter::kIsRate |
                                    benchmark::Counter::kInvert);

}
static void BM_synth_query_dawg(benchmark::State& state) {
    size_t n_comp = state.range(0);
    Collection coll_wiki('w');
    const string wiki_file = WIKI_ROOT + "clickstream-agg-wiki-64B.tsv";
    coll_wiki.read_collection(SIZE_MAX, true);
    DAWGTrie dtrie (coll_wiki);

    PQLog pqlog;
    pqlog.load_pqlog("../../synth_log/data/wiki-synthlog.tsv", 1000);
    /* ht_comp.print_index_meta(); */
    double num_completions=0, plen_sum=0, num_pq=0;
    for (auto _ : state){
        for (const auto& [qid, pvec]: pqlog) {
            for(const auto& p: pvec){
                ++num_pq;
                auto completions = dtrie.complete(p, n_comp);
                num_completions += completions.size();
                plen_sum += p.length();
            }
        }
    }
    // Avg. no. of completions per partial query
    state.counters["NCompAvg"] = (double)num_completions/num_pq;
    state.counters["NPQ"] = num_pq; // number of partial queries
    // Total length of partial queries processed.
    state.counters["PQBytes"] = benchmark::Counter(plen_sum,
                                 benchmark::Counter::kIsIterationInvariantRate,
                                 benchmark::Counter::OneK::kIs1024);
    // How many partial queries are processed per second
    state.counters["PQRate"] = benchmark::Counter(num_pq, benchmark::Counter::kIsRate);
    // How many seconds it takes to process one partial query
    state.counters["PQInvRate"] = benchmark::Counter(num_pq,
                                    benchmark::Counter::kIsRate |
                                    benchmark::Counter::kInvert);

}

static void BM_lr_query(benchmark::State& state) {
    size_t n_comp = state.range(0);
    Collection coll_wiki('w');
    const string wiki_file = WIKI_ROOT + "clickstream-agg-wiki-64B.tsv";
    coll_wiki.read_collection(SIZE_MAX, true);
    PQLog plog;
    plog.load_pqlog("../../synth_log/data/wiki-synthlog.tsv", n_comp);
    LRLog lrlog;
    lrlog.generate_lr_log(plog);
    HTrieCompleter ht_comp;
    ht_comp.build_index(coll_wiki);
    /* ht_comp.print_index_meta(); */
    double num_completions=0, plen_sum=0, num_pq=0;
    for (auto _ : state){
        for (const auto& kv: lrlog) {
            for(const auto& p: kv.second){
                ++num_pq;
                auto completions = ht_comp.complete(p, n_comp);
                num_completions += completions.size();
                plen_sum += p.length();
            }
        }
    }
    /* state.SetComplexityN(state.range(0)); */
    // Avg number of completions per partial query
    state.counters["NCompAvg"] = (double)num_completions/num_pq;
    state.counters["NPQ"] = num_pq; // Number of partial queries
    state.counters["PQBytes"] = benchmark::Counter(plen_sum,
                                 benchmark::Counter::kIsIterationInvariantRate,
                                 benchmark::Counter::OneK::kIs1024);
    // How many partial queries are processed per second
    state.counters["PQRate"] = benchmark::Counter(num_pq, benchmark::Counter::kIsRate);
    // How many seconds it takes to process one partial query
    state.counters["PQInvRate"] = benchmark::Counter(num_pq,
                                    benchmark::Counter::kIsRate |
                                    benchmark::Counter::kInvert);
}

// Register the function as a benchmark
BENCHMARK(BM_build_htrie)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_build_marisa)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_build_dawg)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_build_IncNgT)->Unit(benchmark::kMillisecond);

BENCHMARK(BM_synth_query_htrie)->Unit(benchmark::kMillisecond)->Arg(8);
BENCHMARK(BM_synth_query_marisa)->Unit(benchmark::kMillisecond)->Arg(8);
BENCHMARK(BM_synth_query_dawg)->Unit(benchmark::kMillisecond)->Arg(8);
BENCHMARK(BM_synth_query_IncNgT)->Unit(benchmark::kMillisecond)->Arg(8);

/* BENCHMARK(BM_synth_query_htrie)->Unit(benchmark::kMillisecond)->RangeMultiplier(2)->Range(2, 8); */

int main (int argc, char ** argv) {
    benchmark::Initialize (& argc, argv);
    benchmark::RunSpecifiedBenchmarks ();
    return 0;
}

