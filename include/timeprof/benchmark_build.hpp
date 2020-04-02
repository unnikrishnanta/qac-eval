#include <cstdint>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/join.hpp>
#include "../src/core/collection.hpp"
#include "../src/core/pqlog.hpp"
#include "../src/qac_impl/htrie_wrapper.hpp"
#include "../src/qac_impl/marisa_wrapper.hpp"
#include "../src/qac_impl/dawgtrie_wrapper.hpp"
#include "../src/qac_impl/IncNgTrie_wrapper.hpp"
#include <benchmark/benchmark.h>
#include <memory>

#define WIKI 'w'
#define CWEB 'c'

#define COUNTERS 

class BuildFixture : public ::benchmark::Fixture {
 public:
     static Collection coll;
};

/* Add a set of counters derived from coll  to the State object.
 * The counters will be included in the benchmark reporting */
void add_build_counters(const Collection& coll, benchmark::State& state) {
    double nbytes = 0;
    for (const auto s : coll.get_strings()) {
        nbytes += s.length();
    }
    state.counters["NBytes"] = benchmark::Counter(nbytes);
    // We process with the rate of state.range(0) bytes every iteration:
    state.counters["BytesProcessed"] = benchmark::Counter(nbytes,
                                benchmark::Counter::kIsIterationInvariantRate,
                                benchmark::Counter::OneK::kIs1024);
}


BENCHMARK_DEFINE_F(BuildFixture, BuildHTrie)(benchmark::State& state) {
    auto nrows = static_cast<size_t>(state.range(0));
    assert(coll.size());  // Make sure that coll is not empty
    coll.uniform_sample(nrows);
    for (auto _ : state) {
        HTrieCompleter ht_comp;
        ht_comp.build_index(coll.get_strings(), coll.get_scores());
    }
    add_build_counters(coll, state);
}

BENCHMARK_DEFINE_F(BuildFixture, BuildMarisa)(benchmark::State& state) {
    auto nrows = static_cast<size_t>(state.range(0));
    assert(coll.size());  // Make sure that coll is not empty
    coll.uniform_sample(nrows);
    for (auto _ : state) {
        MarisaCompleter mtc;
        mtc.build_index(coll.get_strings(), coll.get_scores());
    }
    add_build_counters(coll, state);
}

BENCHMARK_DEFINE_F(BuildFixture, BuildDAWG)(benchmark::State& state) {
    auto nrows = static_cast<size_t>(state.range(0));
    assert(coll.size());  // Make sure that coll is not empty
    coll.uniform_sample(nrows);
    for (auto _ : state) {
        DAWGTrie dtrie;
        dtrie.build_index(coll.get_strings(), coll.get_scores());
    }
    add_build_counters(coll, state);
}

BENCHMARK_DEFINE_F(BuildFixture, BuildIncNgT)(benchmark::State& state) {
    auto nrows = static_cast<size_t>(state.range(0));
    int tau = static_cast<int>(state.range(1));
    assert(coll.size());  // Make sure that coll is not empty
    coll.uniform_sample(nrows);
    MarisaCompleter mtc;
    for (auto _ : state) {
        IncNgTrieCompleter inc(tau);
        inc.build_index(coll.get_strings(), coll.get_scores());
    }
    add_build_counters(coll, state);
}

