#include "../src/core/collection.hpp"
#include "../src/core/pqlog.hpp"
#include "../src/qac_impl/htrie_wrapper.hpp"
#include "../src/qac_impl/marisa_wrapper.hpp"
#include "../src/qac_impl/dawgtrie_wrapper.hpp"
#include "../src/qac_impl/IncNgTrie_wrapper.hpp"
#include <benchmark/benchmark.h>
#include <memory>
#include <mytimers.hpp>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <utility>

class QueryBase {
 public:
     static Collection coll;
     static PQLog synth_log;
};

/* A logger class for keeping track of the querying time for various lengths of
 * the partial query (|P|)
 */
class QTimePlen {
    public: 
        void log_plen_time(const int& plen) {
            auto wall_time = get_wall_time();
            auto cpu_time = get_cpu_time();
            plen_time_map_[plen].push_back(std::make_pair(cpu_time, wall_time));
        }
    private: 
        unordered_map<int, vector<pair<double, double>>> plen_time_map_;
};
        

template <class T>
class QueryFixture: public QueryBase, public benchmark::Fixture {
    public:
        std::unique_ptr<T> qac_impl;
        PQLog pqlog;

        void SetUp(const benchmark::State& state) {
            assert(qac_impl.get() == nullptr);  // Entry check
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

        /* Benchmark on partial queries that has a given |P|
         * Since it is hard to introduce manual timers and understand which
         * iteration lead to the resulting times, a filtering based approach is
         * followed. Each time, filter out partial queries that doesn't fall
         * into the bing of |P| and count the time taken to find completions 
         */
        void run_plenq_bechmarks(benchmark::State& state){
            auto plen_lower = static_cast<int>(state.range(0));
            auto plen_upper = static_cast<int>(state.range(1));
            double num_completions=0, plen_sum=0, num_pq=0, comp_len_sum=0;
            for (auto _ : state) {
                for (const auto& qid_pvec: pqlog) {
                    const auto& pvec = qid_pvec.second;  // partial q vector
                    for(const auto& p: pvec){
                        state.PauseTiming();
                        /* Check if p.length() is between plen_lower and
                         * plen_upper https://stackoverflow.com/a/17095534/937153 */
                        if ((unsigned)(p.length()-plen_lower)
                                    <= (plen_upper-plen_lower)) {
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
                double plen_sum, double comp_len_sum, benchmark::State state){
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
