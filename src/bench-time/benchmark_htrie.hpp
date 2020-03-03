#include <cstdint>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/join.hpp>
#include "../core/collection.hpp"
#include "../core/pqlog.hpp"
#include "../qac_impl/htrie_wrapper.hpp"
#include <benchmark/benchmark.h>
#include <memory>


class HTrieFixture : public ::benchmark::Fixture {
 public:
     char coll_type = 'w'; // Default collection type is Wiki
     size_t nrows = SIZE_MAX;
     HTrieCompleter ht_comp;
     std::unique_ptr<Collection> coll;

     void SetUp(const ::benchmark::State& st) {
        coll_type = static_cast<char>(st.range(0));
        nrows = static_cast<size_t>(st.range(1));
        coll = std::make_unique<Collection>(Collection(coll_type));
        coll->read_collection(nrows, true);
     }

     void TearDown(const ::benchmark::State&) {
         coll.reset();
     }

};

BENCHMARK_DEFINE_F(HTrieFixture, Build)(benchmark::State& state) {
  for (auto _ : state) {
      ht_comp.build_index(coll->get_strings(), coll->get_scores());
  }
}

BENCHMARK_DEFINE_F(HTrieFixture, SynthQuery)(benchmark::State& state) {
  nrows = static_cast<size_t>(state.range(0));
  PQLog pqlog;
  pqlog.load_pqlog("../../synth_log/data/wiki-synthlog.tsv", nrows);
  double num_completions=0, plen_sum=0, num_pq=0;
  for (auto _ : state) {
    for (const auto& [qid, pvec]: pqlog) {
      for(const auto& p: pvec){
          ++num_pq;
          auto completions = ht_comp.complete(p, true);
          num_completions += completions.size();
          plen_sum += p.length();
      }
    }
  }
}
