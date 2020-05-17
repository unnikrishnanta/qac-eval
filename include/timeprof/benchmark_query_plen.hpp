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
#include <ios>
#include <qac_impl.hpp>
#include "../prof_base.hpp"
#include <benchmark/benchmark.h>
#include <memory>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <chrono>
#include <vector>
#include "csvfile.h"

#define USE_LOGGING 1

struct TimeLogger {
    double qtime;
    int plen;
    string fixture;
    string collection;

    static std::string generate_export_fname(){
        string coll_name; 
        switch(COLLECTION) {
            case WIKI: coll_name = "wiki"; break;
            case CWEB: coll_name = "cweb"; break;
            case BING: coll_name = "bing"; break;
        }
        string outfile = "../export/data/time-bm/"
                            + coll_name + 
                            "-plen-qtime.csv";
        return outfile;
    }
};


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
            qac_impl.reset();
            ++instance_count_;
            switch(COLLECTION) {
                case WIKI: coll_name_ = "wiki"; break;
                case CWEB: coll_name_ = "cweb"; break;
                case BING: coll_name_ = "bing"; break;
            }
            fixture_name_ = typeid(T).name();
        }

        void SetUp(const benchmark::State& state) {
            assert(qac_loq.size() != 0);
            auto coll_nrows = static_cast<size_t>(state.range(0));
            auto n_conv = static_cast<size_t>(state.range(1));
            auto log_type = state.range(2);
            log_type_ = log_type; // Set the private member to export the logs
            if(coll_nrows != NROWS)
                coll.uniform_sample(coll_nrows);
            // Rebuild the index only if collection is sampled or a full 
            // collection index is not already built
            if((qac_impl.get() == nullptr) || (coll_nrows != NROWS)){
                qac_impl = std::make_unique<T>();
                qac_impl->build_index(coll.get_strings(), coll.get_scores());
            }
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
            auto coll_nrows = static_cast<size_t>(state.range(0));
            assert(qac_impl.get() != nullptr);
            if(coll_nrows != NROWS) // Otherwise retain the previous value
                qac_impl.reset();
        }

        ~QueryFixturePlen(){
            if(qac_impl.get() == nullptr)
                qac_impl.reset();
            --instance_count_;
            export_qlentmap();
        }

    private:
        char curr_full_logtype;
        std::vector<TimeLogger> plen_qtime;
        int log_type_;
        std::string fixture_name_;
        static inline size_t instance_count_;
        std::string coll_name_;


        void export_qlentmap() {
            csvfile csv_out_;
            string outfile = TimeLogger::generate_export_fname();
            std::cout << "Exporting plen_qtime with size: " << plen_qtime.size()
                      << " to: " << outfile << "\n";
            csv_out_.open(outfile);
            for (const auto tl : plen_qtime) {
                csv_out_ << tl.fixture << tl.plen
                         << tl.qtime << tl.collection ;
                csv_out_ << endrow ;
            }
        }

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
            assert(qac_impl.get() != nullptr);
            double num_completions=0, plen_sum=0, num_pq=0, comp_len_sum=0;
            for (auto _ : state) {
                auto iter_start = std::chrono::high_resolution_clock::now();
                for (const auto& qid_pvec: pqlog) {
                    const auto& pvec = qid_pvec.second;  // partial q vector
                    for(const auto& p: pvec){
                        auto compt_start = std::chrono::high_resolution_clock::now();
                        auto completions = qac_impl->complete(p, NCOMP);
                        auto compt_end = std::chrono::high_resolution_clock::now();
#ifdef USE_LOGGING
                        // Timer paused
                        state.PauseTiming();
                        auto qtime_elapsed = std::chrono::duration_cast<
                            std::chrono::duration<double>>(compt_end - compt_start);
                        TimeLogger tl;
                        tl.qtime = qtime_elapsed.count();
                        tl.collection = coll_name_; 
                        tl.plen = p.length();
                        tl.fixture = fixture_name_;
                        plen_qtime.push_back(tl);
                        ++num_pq;
                        num_completions += completions.size();
                        plen_sum += p.length();
                        for (const auto& c : completions) {
                            comp_len_sum += c.first.length();  // len(comp str)
                        }
                        // Timer resumes
                        state.ResumeTiming();
#endif
                    }
                }
                auto iter_end = std::chrono::high_resolution_clock::now();
                auto elapsed_seconds = std::chrono::duration_cast<
                    std::chrono::duration<double>>(iter_end - iter_start);
                state.SetIterationTime(elapsed_seconds.count());
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
