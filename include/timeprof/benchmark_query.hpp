#include <cassert>
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
            assert(qac_impl.get() != nullptr);  // Entry check
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

        void TearDown([[maybe_unused]] const benchmark::State& state) {
            assert(qac_impl.get() != nullptr);
            qac_impl.reset();
        }

    protected:
        void run_benchmarks(benchmark::State& state){
            double num_completions=0, plen_sum=0, num_pq=0, comp_len_sum=0;
            for (auto _ : state) {
                for (const auto& qid_pvec: pqlog) {
                    const auto& pvec = qid_pvec.second;  // partial q vector
                    for(const auto& p: pvec){
                        auto completions = qac_impl->complete(p, NCOMP);
                        state.PauseTiming(); //Stop timers.Warning:high overhead
                        ++num_pq;
                        num_completions += completions.size();
                        plen_sum += p.length();
                        for (const auto& c : completions) {
                            comp_len_sum += c.first.length();  // len(comp str)
                        }
                        state.ResumeTiming();
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
            state.counters["CompBytesRate"] = benchmark::Counter(comp_len_sum,
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
            state.counters["CompByesInvRate"] = benchmark::Counter(comp_len_sum,
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
