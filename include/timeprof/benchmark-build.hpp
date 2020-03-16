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

#define COLLECTION 'w'

class BuildFixture : public ::benchmark::Fixture {
 public:
     static Collection coll;
     
     /* BuildFixture(): coll() { */
     /*    cout << "Fixture Constructor\n"; */
     /*    coll.read_collection(COLLECTION, SIZE_MAX, true); */
     /* } */

     ~BuildFixture() {
         cout << "Fixture destructor\n";
     }
};

BENCHMARK_DEFINE_F(BuildFixture, BuildHTrie)(benchmark::State& state) {
    auto nrows = static_cast<size_t>(state.range(0));
    assert(coll.size());  // Make sure that coll is not empty
    coll.uniform_sample(nrows);
    double nbytes = 0, n_iter = 0;
    for (auto _ : state) {
        HTrieCompleter ht_comp;
        ht_comp.build_index(coll.get_strings(), coll.get_scores());
        n_iter ++;
    }
    for (const auto s : coll.get_strings()) {
        nbytes += s.length();
    }
    state.counters["NBytes"] = nbytes;
    state.counters["NIter"] = n_iter; // Nor required. Kept for cross checking
    // We process with the rate of state.range(0) bytes every iteration:
    state.counters["BytesProcessed"] = benchmark::Counter(n_iter * nbytes,
                                benchmark::Counter::kIsIterationInvariantRate,
                                benchmark::Counter::OneK::kIs1024);
}

BENCHMARK_DEFINE_F(BuildFixture, BuildMarisa)(benchmark::State& state) {
    auto nrows = static_cast<size_t>(state.range(0));
    assert(coll.size());  // Make sure that coll is not empty
    coll.uniform_sample(nrows);
    for (auto _ : state) {
        MarisaCompleter mtc;
        mtc.build_index(coll.get_strings(), coll.get_scores());
    }
}

BENCHMARK_DEFINE_F(BuildFixture, BuildDAWG)(benchmark::State& state) {
    auto nrows = static_cast<size_t>(state.range(0));
    assert(coll.size());  // Make sure that coll is not empty
    coll.uniform_sample(nrows);
    for (auto _ : state) {
        DAWGTrie dtrie;
        dtrie.build_index(coll.get_strings(), coll.get_scores());
    }
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
}

/* /1* BENCHMARK_DEFINE_F(BuildFixture, SynthQuery)(benchmark::State& state) { *1/ */
/* /1*   nrows = static_cast<size_t>(state.range(1)); *1/ */
/* /1*   /2* cout << "PQ nrows = " << nrows; *2/ *1/ */
/* /1*   PQLog pqlog; *1/ */
/* /1*   pqlog.load_pqlog("../../synth_log/data/wiki-synthlog.tsv", nrows); *1/ */
/* /1*   double num_completions=0, plen_sum=0, num_pq=0; *1/ */
/* /1*   for (auto _ : state) { *1/ */
/* /1*     for (const auto& [qid, pvec]: pqlog) { *1/ */
/* /1*       for(const auto& p: pvec){ *1/ */
/* /1*           ++num_pq; *1/ */
/* /1*           auto completions = ht_comp.complete(p, true); *1/ */
/* /1*           num_completions += completions.size(); *1/ */
/* /1*           plen_sum += p.length(); *1/ */
/* /1*       } *1/ */
/* /1*     } *1/ */
/* /1*   } *1/ */
