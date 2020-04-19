#ifndef BM_QUERY_PLEN_H
#define BM_QUERY_PLEN_H
/* benchmark_query_full.hpp benchmarks the implementations for varying
 * collection sizes. This file provides benchmarks for fixed collection sizes
 * and varying length of partial qeury |P|. 
 *
 * Why do we need a seperate file: 
 *  - benchmark_query_full samples the collection and rebuilds the index for
 *  each iteration. This is not ideal for this case, especially since some of
 *  the |P| based on bins, are skipped to estimate the querying time for a given
 *  |P| bin. 
 *  - This file always operates on the full collection - the index construction
 *  can be moved from SetUp to the Fixture class constructor. 
 */
#include <qac_impl.hpp>
#include "../prof_base.hpp"
#include <benchmark/benchmark.h>
#include <memory>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <utility>


template <class T>
class QueryFixturePlen: public QueryBase, public benchmark::Fixture {
    public:
        std::unique_ptr<T> qac_impl;
        PQLog pqlog;
        
        /* It would be more efficient to construct the index in the constructor.
         * But since all the Fixture objects are created first, this will load
         * all the index structures at once, taking up huge amounts of memory. 
         */
        QueryFixturePlen() {
            curr_full_logtype = 0;
        }

        void SetUp(const benchmark::State& state) {
            assert(qac_impl.get() == nullptr);  // Entry check
            assert(qac_loq.size() != 0);
            // Keep the following arg to maintain consistent API.
            [[maybe_unused]]auto coll_nrows = static_cast<size_t>(state.range(0));
            auto n_conv = static_cast<size_t>(state.range(1));
            auto log_type = state.range(2);
            qac_impl = std::make_unique<T>();
            qac_impl->build_index(coll.get_strings(), coll.get_scores());
            if(n_conv != PQLOG_NCONV) {  // Sample query log
                pqlog = qac_loq.uniform_sample(n_conv);
                if (log_type == LRLOG) {  // Generate LR log from current sample
                    pqlog = pqlog.lr_log();
                }
            }
            else {  // Work on a full log. 
                if(log_type == SLOG){
                    pqlog = qac_loq;
                }
                else {  // Load LR log if needed
                    if(curr_full_logtype != LRLOG) {
                        pqlog = qac_loq.lr_log();
                    }
                }
                curr_full_logtype = log_type;
                pqlog.set_log_type(curr_full_logtype);
            }
            assert(pqlog.size());
        }

        void TearDown([[maybe_unused]] const benchmark::State& state) {
            assert(qac_impl.get() != nullptr);
            qac_impl.reset();
        }

    private:
        char curr_full_logtype;

    protected:
         /* Benchmark on partial queries that has a given |P|
         * Since it is hard to introduce manual timers and understand which
         * iteration lead to the resulting times, a filtering based approach is
         * followed. Each time, filter out partial queries that doesn't fall
         * into the bing of |P| and count the time taken to find completions 
         */
        void run_plenq_bechmarks(benchmark::State& state){
            /* To keep the SetUp function arguments consistent, pass the upper
             * and lower bounds of |P| bin as range(3) and range(4)
             */
            auto plen_lower = static_cast<int>(state.range(3));
            auto plen_upper = static_cast<int>(state.range(4));
            unsigned int bin_width = plen_upper - plen_lower;
            double num_completions=0, plen_sum=0, num_pq=0, comp_len_sum=0;
            for (auto _ : state) {
                for (const auto& qid_pvec: pqlog) {
                    const auto& pvec = qid_pvec.second;  // partial q vector
                    for(const auto& p: pvec){
                        state.PauseTiming();
                        /* Check if p.length() is between plen_lower and
                         * plen_upper https://stackoverflow.com/a/17095534/937153 */
                        if ((unsigned)(p.length()-plen_lower) <= (bin_width)) {
                            // Find completions, but skip timing
                            auto completions = qac_impl->complete(p, NCOMP);
                            continue;
                        }
                        state.ResumeTiming();
                        // Count this time if p.length() == plen
                        auto completions = qac_impl->complete(p, NCOMP);
                        // high overhead counters outsider timing units
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
BENCHMARK_TEMPLATE_DEFINE_F(QueryFixturePlen, QueryHTriePlen,
            HTrieCompleter)(benchmark::State& state) {
    run_plenq_bechmarks(state);
}

/* Benchmark Marisa-Trie */
BENCHMARK_TEMPLATE_DEFINE_F(QueryFixturePlen, QueryMarisaPlen,
            MarisaCompleter)(benchmark::State& state) {
    run_plenq_bechmarks(state);
}

/* Benchmark DAWG-Trie */
BENCHMARK_TEMPLATE_DEFINE_F(QueryFixturePlen, QueryDAWGPlen,
            DAWGTrie)(benchmark::State& state) {
    run_plenq_bechmarks(state);
}

/* Benchmark IncNgTrie */
BENCHMARK_TEMPLATE_DEFINE_F(QueryFixturePlen, QueryIncNgTriePlen,
            IncNgTrieCompleter)(benchmark::State& state) {
    run_plenq_bechmarks(state);
}

#endif /* ifndef BM_QUERY_PLEN_H */
