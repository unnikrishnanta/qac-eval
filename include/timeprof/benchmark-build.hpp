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
     Collection coll;
     
     BuildFixture(): coll() {
        cout << "Fixture Constructor\n";
        coll.read_collection(COLLECTION, SIZE_MAX, true);
     }

     void SetUp(const ::benchmark::State& st) {
     }

     void TearDown(const ::benchmark::State&) {
     }

     ~BuildFixture() {
         cout << "Fixture destructor\n";
     }
};

BENCHMARK_DEFINE_F(BuildFixture, BuildHTrie)(benchmark::State& state) {
    auto nrows = static_cast<size_t>(state.range(0));
    coll.uniform_sample(nrows);
    /* std::cout << "Sampled size " << coll.size() << std::endl; */
    for (auto _ : state) {
        HTrieCompleter ht_comp;
        ht_comp.build_index(coll.get_strings(), coll.get_scores());
    }
}

BENCHMARK_DEFINE_F(BuildFixture, BuildMarisa)(benchmark::State& state) {
    auto nrows = static_cast<size_t>(state.range(0));
    coll.uniform_sample(nrows);
    for (auto _ : state) {
        MarisaCompleter mtc;
        mtc.build_index(coll.get_strings(), coll.get_scores());
    }
}

BENCHMARK_DEFINE_F(BuildFixture, BuildDAWG)(benchmark::State& state) {
    auto nrows = static_cast<size_t>(state.range(0));
    coll.uniform_sample(nrows);
    for (auto _ : state) {
        DAWGTrie dtrie;
        dtrie.build_index(coll.get_strings(), coll.get_scores());
    }
}

BENCHMARK_DEFINE_F(BuildFixture, BuildIncNgT)(benchmark::State& state) {
    auto nrows = static_cast<size_t>(state.range(0));
    int tau = static_cast<int>(state.range(1));
    /* std::cout << "Tau = " << tau << "\n"; */
    coll.uniform_sample(nrows);
    MarisaCompleter mtc;
    for (auto _ : state) {
        IncNgTrieCompleter inc(tau);
        inc.build_index(coll.get_strings(), coll.get_scores());
    }
}

/* BENCHMARK_DEFINE_F(BuildFixture, SynthQuery)(benchmark::State& state) { */
/*   nrows = static_cast<size_t>(state.range(1)); */
/*   /1* cout << "PQ nrows = " << nrows; *1/ */
/*   PQLog pqlog; */
/*   pqlog.load_pqlog("../../synth_log/data/wiki-synthlog.tsv", nrows); */
/*   double num_completions=0, plen_sum=0, num_pq=0; */
/*   for (auto _ : state) { */
/*     for (const auto& [qid, pvec]: pqlog) { */
/*       for(const auto& p: pvec){ */
/*           ++num_pq; */
/*           auto completions = ht_comp.complete(p, true); */
/*           num_completions += completions.size(); */
/*           plen_sum += p.length(); */
/*       } */
/*     } */
/*   } */
