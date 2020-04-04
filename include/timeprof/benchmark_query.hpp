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

#define WIKISYNTH_SIZE 1206653
#define SYNTHLOG 's'
#define LRLOG 'l'

#define COLLECTION CWEB

#if COLLECTION==WIKI 
#define NROWS WIKI_NROWS
#define WIKISYNTH "../../synth_log/data/wiki-synthlog.tsv"
#else
#define NROWS CWEB_NROWS
#define WIKISYNTH "../../synth_log/data/cweb-synthlog.tsv"
#endif

#define NCOMP 8  // Number of completions 

class QueryBase {
 public:
     static Collection coll;
     static PQLog synth_log;
};

template <class T>
class QueryFixture: public QueryBase, public benchmark::Fixture {
    public:
        std::unique_ptr<T> qac_impl;
        PQLog pqlog;

        void SetUp(const benchmark::State& state) {
            auto coll_nrows = static_cast<size_t>(state.range(0));
            auto n_conv = static_cast<size_t>(state.range(1));
            auto log_type = state.range(2);
            assert(coll.size());
            coll.uniform_sample(coll_nrows);
            qac_impl = std::make_unique<T>();
            qac_impl->build_index(coll.get_strings(), coll.get_scores());
            pqlog = synth_log.uniform_sample(n_conv);
            if (log_type == LRLOG) {
                pqlog = pqlog.lr_log();
            }
            assert(pqlog.size());
        }

        void TearDown(const benchmark::State& state) {
            assert(qac_impl.get() != nullptr);
            qac_impl.reset();
        }

    protected:
        void run_benchmarks(benchmark::State& state){
            double num_completions=0, plen_sum=0, num_pq=0;
            for (auto _ : state) {
                for (const auto& [qid, pvec]: pqlog) {
                    for(const auto& p: pvec){
                        ++num_pq;
                        auto completions = qac_impl->complete(p, NCOMP);
                        num_completions += completions.size();
                        plen_sum += p.length();
                    }
                }
            }
            state.counters["NComp"] = num_completions;
            state.counters["NPQ"] = num_pq; // number of partial queries
            // Total length of partial queries processed.
            state.counters["PQBytes"] = plen_sum;
            // How many partial queries are processed per second
            state.counters["NPQRate"] = benchmark::Counter(num_pq,
                                            benchmark::Counter::kIsRate);
            state.counters["PQBytesRate"] = benchmark::Counter(plen_sum,
                                            benchmark::Counter::kIsRate,
                                            benchmark::Counter::OneK::kIs1024);
            // How many seconds it takes to process one partial query
            state.counters["NPQInvRate"] = benchmark::Counter(num_pq,
                                           benchmark::Counter::kIsRate |
                                           benchmark::Counter::kInvert);
            state.counters["PQByesInvRate"] = benchmark::Counter(plen_sum,
                                            benchmark::Counter::kIsRate |
                                            benchmark::Counter::kInvert, 
                                            benchmark::Counter::OneK::kIs1024);
        }

};

/* Benchmark HAT-Trie */
BENCHMARK_TEMPLATE_DEFINE_F(QueryFixture, QueryHTrie,
            HTrieCompleter)(benchmark::State& state) {
    run_benchmarks(state);
}

/* Benchmark Marisa-Trie */
BENCHMARK_TEMPLATE_DEFINE_F(QueryFixture, QueryMarisa,
            MarisaCompleter)(benchmark::State& state) {
    run_benchmarks(state);
}

/* Benchmark DAWG-Trie */
BENCHMARK_TEMPLATE_DEFINE_F(QueryFixture, QueryDAWG,
            DAWGTrie)(benchmark::State& state) {
    run_benchmarks(state);
}

/* Benchmark IncNgTrie */
BENCHMARK_TEMPLATE_DEFINE_F(QueryFixture, QueryIncNgTrie,
            IncNgTrieCompleter)(benchmark::State& state) {
    run_benchmarks(state);
}
