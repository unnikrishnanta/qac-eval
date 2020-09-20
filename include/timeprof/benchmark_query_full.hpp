#ifndef BM_QUERY_FULL_H
#define BM_QUERY_FULL_H

#include <qac_impl.hpp>
#include "../prof_base.hpp"
#include <benchmark/benchmark.h>
#include <exception>
#include <memory>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <utility>

template <class T>
class QueryFixture: public QueryBase, public benchmark::Fixture {
    public:
        std::unique_ptr<T> qac_impl;
        PQLog pqlog;
        
        QueryFixture() {
            curr_full_logtype = 0;
        }
        /* Each iteration rebuilds the index and samples the partial query log.
         * This should eventually give a stable reading. 
         * Building the collection in a constructor prevents resampling the
         * collection and rebuilding the index in each iteration. 
         */
        void SetUp(const benchmark::State& state) {
            assert(qac_impl.get() == nullptr);  // Entry check
            auto coll_nrows = static_cast<size_t>(state.range(0));
            auto n_conv = static_cast<size_t>(state.range(1));
            auto log_type = state.range(2);
            assert(coll_full.size());
            auto coll_sample = coll_full.uniform_sample(coll_nrows);
            qac_impl = std::make_unique<T>();
            qac_impl->build_index(coll_sample.first, coll_sample.second);
            if(n_conv != PQLOG_NCONV) {  // Sample query log
                pqlog = qac_log.uniform_sample(n_conv);
                if (log_type == LRLOG) {  // Generate LR log from current sample
                    pqlog = pqlog.lr_log();
                }
            }
            else {  // Work on a full log. 
                if(log_type == SLOG){
                    pqlog = qac_log;
                }
                else {  // Load LR log if needed
                    if(curr_full_logtype != LRLOG) {
                        pqlog = qac_log.lr_log();
                    }
                }
                curr_full_logtype = log_type;
                pqlog.set_log_type(curr_full_logtype);  // For debugging later. 
            }
            assert(pqlog.size());
        }

        void TearDown([[maybe_unused]] const benchmark::State& state) {
            assert(qac_impl.get() != nullptr);
            qac_impl.reset();
        }

    private: 
        char curr_full_logtype;  // If a full log is loaded, is it SYNTH or LR

    protected:
        // Benchmark on all partial queries from the logs
        void run_completeq_bechmarks(benchmark::State& state){
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
            add_counters(num_pq, num_completions, plen_sum, comp_len_sum, state);
         }

    private:

        void add_counters(double num_pq, double num_completions,
                double plen_sum, double comp_len_sum, benchmark::State& state){
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
    run_completeq_bechmarks(state);
}
/* Benchmark Marisa-Trie */
BENCHMARK_TEMPLATE_DEFINE_F(QueryFixture, QueryMarisa,
            MarisaCompleter)(benchmark::State& state) {
    run_completeq_bechmarks(state);
}
/* Benchmark DAWG-Trie */
BENCHMARK_TEMPLATE_DEFINE_F(QueryFixture, QueryDAWG,
            DAWGTrie)(benchmark::State& state) {
    run_completeq_bechmarks(state);
}
/* Benchmark IncNgTrie */
BENCHMARK_TEMPLATE_DEFINE_F(QueryFixture, QueryIncNgTrie,
            IncNgTrieCompleter)(benchmark::State& state) {
    run_completeq_bechmarks(state);
}

#endif /* ifndef BM_QUERY_FULL_H */
