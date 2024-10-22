#include <cstdint>
#include <iostream>
#include "../src/core/collection.hpp"
#include "../src/core/pqlog.hpp"
#include "../src/qac_impl/htrie_wrapper.hpp"
#include "../src/qac_impl/marisa_wrapper.hpp"
#include "../src/qac_impl/dawgtrie_wrapper.hpp"
#include "../src/qac_impl/IncNgTrie_wrapper.hpp"
#include <benchmark/benchmark.h>
#include <memory>


class BuildFixture : public ::benchmark::Fixture {
 public:
     static Collection coll_full;
};

/* Add a set of counters derived from coll  to the State object.
 * The counters will be included in the benchmark reporting
 *  NPQ: No. of partial queries processed. 
 *  PQBytes: No. of bytes from partial query strings processed. 
 *  TotalBytes: Total no. of bytes processed including strings and scores
 */
void add_build_counters(const StrVec& coll_strs,
                        benchmark::State& state) {
    auto nrows = static_cast<size_t>(state.range(0));
    double plen_sum=0, total_bytes=0;
    for (const auto s : coll_strs) {
        plen_sum += s.length();
        total_bytes = total_bytes + s.length() + sizeof(ScoreType);
    }
    state.counters["NPQ"] = nrows; // number of partial queries
    state.counters["PQBytes"] = benchmark::Counter(plen_sum);
    state.counters["TotalBytes"] = benchmark::Counter(total_bytes);
    // We process with the rate of state.range(0) bytes every iteration:
    state.counters["PQBytesRate"] = benchmark::Counter(plen_sum,
                                benchmark::Counter::kIsIterationInvariantRate,
                                benchmark::Counter::OneK::kIs1024);
    state.counters["TotalBytesRate"] = benchmark::Counter(total_bytes,
                                benchmark::Counter::kIsIterationInvariantRate,
                                benchmark::Counter::OneK::kIs1024);
    state.counters["PQByesInvRate"] = benchmark::Counter(plen_sum,
                                    benchmark::Counter::kIsRate |
                                    benchmark::Counter::kInvert, 
                                    benchmark::Counter::OneK::kIs1024);
    state.counters["TotalByesInvRate"] = benchmark::Counter(total_bytes,
                                    benchmark::Counter::kIsRate |
                                    benchmark::Counter::kInvert, 
                                    benchmark::Counter::OneK::kIs1024);
}


BENCHMARK_DEFINE_F(BuildFixture, BuildHTrie)(benchmark::State& state) {
    auto nrows = static_cast<size_t>(state.range(0));
    assert(coll_full.size());  // Make sure that coll is not empty
    auto coll_sample = coll_full.uniform_sample(nrows);
    for (auto _ : state) {
        HTrieCompleter ht_comp;
        ht_comp.build_index(coll_sample.first, coll_sample.second);
    }
    add_build_counters(coll_sample.first, state);
}

BENCHMARK_DEFINE_F(BuildFixture, BuildMarisa)(benchmark::State& state) {
    auto nrows = static_cast<size_t>(state.range(0));
    assert(coll_full.size());  // Make sure that coll is not empty
    auto coll_sample = coll_full.uniform_sample(nrows);
    for (auto _ : state) {
        MarisaCompleter mtc;
        mtc.build_index(coll_sample.first, coll_sample.second);
    }
    add_build_counters(coll_sample.first, state);
}

BENCHMARK_DEFINE_F(BuildFixture, BuildDAWG)(benchmark::State& state) {
    auto nrows = static_cast<size_t>(state.range(0));
    assert(coll_full.size());  // Make sure that coll is not empty
    auto coll_sample = coll_full.uniform_sample(nrows);
    for (auto _ : state) {
        DAWGTrie dtrie;
        dtrie.build_index(coll_sample.first, coll_sample.second);
    }
    add_build_counters(coll_sample.first, state);
}

BENCHMARK_DEFINE_F(BuildFixture, BuildIncNgT)(benchmark::State& state) {
    auto nrows = static_cast<size_t>(state.range(0));
    int tau = static_cast<int>(state.range(1));
    assert(coll_full.size());  // Make sure that coll is not empty
    auto coll_sample = coll_full.uniform_sample(nrows);
    MarisaCompleter mtc;
    for (auto _ : state) {
        IncNgTrieCompleter inc(tau);
        inc.build_index(coll_sample.first, coll_sample.second);
    }
    add_build_counters(coll_sample.first, state);
}

