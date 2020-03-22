#include <cassert>
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
#define WIKI_NROWS 5077651
#define CWEB_NROWS 195285642

#define COLLECTION WIKI

#if COLLECTION==WIKI 
#define NROWS WIKI_NROWS
#define SYNTHLOG "../../synth_log/data/wiki-synthlog.tsv"
#else
#define NROWS CWEB_NROWS
#define SYNTHLOG "../../synth_log/data/cweb-synthlog.tsv"
#endif

class QueryBase {
 public:
     static Collection coll;
     static PQLog pqlog;
};


class QueryFixture: public QueryBase, public benchmark::Fixture {
    public:
        std::unique_ptr<HTrieCompleter> ht_comp;
        PQLog pqlog_sampled;

        void SetUp(const benchmark::State& state) {
            auto coll_nrows = static_cast<size_t>(state.range(0));
            auto n_conv = static_cast<size_t>(state.range(1));
            assert(coll.size());
            coll.uniform_sample(coll_nrows);
            ht_comp = std::make_unique<HTrieCompleter>();
            ht_comp->build_index(coll.get_strings(), coll.get_scores());
            pqlog_sampled = pqlog.uniform_sample(n_conv);
            assert(pqlog_sampled.size());
        }

        void TearDown(const benchmark::State& state) {
            assert(ht_comp.get() != nullptr);
            ht_comp.reset();
        }
};

/* Benchmark HAT-Trie on SynthLog */
BENCHMARK_DEFINE_F(QueryFixture, HTrieSynth)(benchmark::State& state) {
    double num_completions=0, plen_sum=0, num_pq=0;
    for (auto _ : state) {
        for (const auto& [qid, pvec]: pqlog_sampled) {
            for(const auto& p: pvec){
                ++num_pq;
                auto completions = ht_comp->complete(p, true);
                num_completions += completions.size();
                plen_sum += p.length();
            }
        }
    }
}
