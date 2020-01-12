#include<iostream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/join.hpp>
#include "core/collection.hpp"
#include "core/pqlog.hpp"
#include "qac_impl/htrie_wrapper.hpp"
#include "qac_impl/dawgtrie_wrapper.hpp"
#include "qac_impl/marisa_wrapper.hpp"
#include "config.hpp"
#include <benchmark/benchmark.h>
#include <numeric>

using namespace std;
using namespace boost::program_options;
using namespace qac_ht;
using namespace qac_mt;

static void  BM_build_index(benchmark::State& state) {
    Collection coll_wiki;
    const string wiki_file = WIKI_ROOT + "clickstream-agg-wiki.tsv";
    coll_wiki.read_collection(wiki_file, SIZE_MAX, true);
    for (auto _ : state){
        state.PauseTiming(); // Stop timers.
        HTrieCompleter ht_comp;
        state.ResumeTiming(); // And resume timers.
        for(const auto& kv: coll_wiki)
            ht_comp.update_index(kv);
    }
}


static void  BM_build_dawg(benchmark::State& state) {
    Collection coll_wiki;
    const string wiki_file = WIKI_ROOT + "clickstream-agg-wiki.tsv";
    coll_wiki.read_collection(wiki_file, SIZE_MAX, true);
    coll_wiki.sort_keys();
    /* Current DAWGTrie wrapper needs separate key and value vectors */
    vector<string> keys;
    /* vector<size_t> values; */
    transform(coll_wiki.begin(), coll_wiki.end(), std::back_inserter(keys),
                [](const auto& p) { return p.first; });
    /* transform(coll_wiki.begin(), coll_wiki.end(), std::back_inserter(values), */
    /*             [](const auto& p) { return p.second; }); */

    std::vector<size_t> values(keys.size()) ; // vector with 100 ints.
    std::iota (std::begin(values), std::end(values), 0); // Fill with 0, 1, ..., 99.
    for (auto _ : state){
            DAWGTrie dtrie (keys, values);
    }
}


static void BM_synth_query(benchmark::State& state) {
    size_t n_comp = state.range(0);
    Collection coll_wiki;
    const string wiki_file = WIKI_ROOT + "clickstream-agg-wiki.tsv";
    coll_wiki.read_collection(wiki_file, SIZE_MAX, true);
    PQLog pqlog;
    pqlog.load_pqlog("../../synth_log/data/wiki-synthlog.tsv", 1000);
    HTrieCompleter ht_comp;
    ht_comp.build_index(coll_wiki.get_collection());
    /* ht_comp.print_index_meta(); */
    double num_completions=0, plen_sum=0, num_pq=0;
    for (auto _ : state){
        for (const auto& [qid, pvec]: pqlog) {
            for(const auto& p: pvec){
                ++num_pq;
                auto completions = ht_comp.complete(p, n_comp, false);
                num_completions += completions.size();
                plen_sum += p.length();
            }
        }
    }
    // Avg. no. of completions per partial query
    state.counters["NCompAvg"] = (double)num_completions/num_pq;
    state.counters["NPQ"] = num_pq; // number of partial queries
    // Total length of partial queries processed.
    state.counters["PQBytes"] = benchmark::Counter(plen_sum,
                                 benchmark::Counter::kIsIterationInvariantRate,
                                 benchmark::Counter::OneK::kIs1024);
    // How many partial queries are processed per second
    state.counters["PQRate"] = benchmark::Counter(num_pq, benchmark::Counter::kIsRate);
    // How many seconds it takes to process one partial query
    state.counters["PQInvRate"] = benchmark::Counter(num_pq,
                                    benchmark::Counter::kIsRate |
                                    benchmark::Counter::kInvert);

}

static void BM_synth_query_dawg(benchmark::State& state) {
    size_t n_comp = state.range(0);
    Collection coll_wiki;
    const string wiki_file = WIKI_ROOT + "clickstream-agg-wiki.tsv";
    coll_wiki.read_collection(wiki_file, SIZE_MAX, true);
    coll_wiki.sort_keys();
    vector<string> keys;
    /* vector<size_t> values; */
    transform(coll_wiki.begin(), coll_wiki.end(), std::back_inserter(keys),
                [](const auto& p) { return p.first; });
    std::vector<size_t> values(keys.size()) ; // vector with 100 ints.
    std::iota (std::begin(values), std::end(values), 0); // Fill with 0, 1, ..., 99.
    /* transform(coll_wiki.begin(), coll_wiki.end(), std::back_inserter(values), */
    /*             [](const auto& p) { return p.second; }); */
    DAWGTrie dtrie (keys, values);

    PQLog pqlog;
    pqlog.load_pqlog("../../synth_log/data/wiki-synthlog.tsv", 1000);
    /* ht_comp.print_index_meta(); */
    double num_completions=0, plen_sum=0, num_pq=0;
    for (auto _ : state){
        for (const auto& [qid, pvec]: pqlog) {
            for(const auto& p: pvec){
                ++num_pq;
                auto completions = dtrie.complete(p, n_comp);
                num_completions += completions.size();
                plen_sum += p.length();
            }
        }
    }
    // Avg. no. of completions per partial query
    state.counters["NCompAvg"] = (double)num_completions/num_pq;
    state.counters["NPQ"] = num_pq; // number of partial queries
    // Total length of partial queries processed.
    state.counters["PQBytes"] = benchmark::Counter(plen_sum,
                                 benchmark::Counter::kIsIterationInvariantRate,
                                 benchmark::Counter::OneK::kIs1024);
    // How many partial queries are processed per second
    state.counters["PQRate"] = benchmark::Counter(num_pq, benchmark::Counter::kIsRate);
    // How many seconds it takes to process one partial query
    state.counters["PQInvRate"] = benchmark::Counter(num_pq,
                                    benchmark::Counter::kIsRate |
                                    benchmark::Counter::kInvert);

}

static void BM_lr_query(benchmark::State& state) {
    size_t n_comp = state.range(0);
    Collection coll_wiki;
    const string wiki_file = WIKI_ROOT + "clickstream-agg-wiki.tsv";
    coll_wiki.read_collection(wiki_file, SIZE_MAX, true);
    PQLog plog;
    plog.load_pqlog("../../synth_log/data/wiki-synthlog.tsv", n_comp);
    LRLog lrlog;
    lrlog.generate_lr_log(plog);
    HTrieCompleter ht_comp;
    ht_comp.build_index(coll_wiki.get_collection());
    /* ht_comp.print_index_meta(); */
    double num_completions=0, plen_sum=0, num_pq=0;
    for (auto _ : state){
        for (const auto& kv: lrlog) {
            for(const auto& p: kv.second){
                ++num_pq;
                auto completions = ht_comp.complete(p, n_comp);
                num_completions += completions.size();
                plen_sum += p.length();
            }
        }
    }
    /* state.SetComplexityN(state.range(0)); */
    // Avg number of completions per partial query
    state.counters["NCompAvg"] = (double)num_completions/num_pq;
    state.counters["NPQ"] = num_pq; // Number of partial queries
    state.counters["PQBytes"] = benchmark::Counter(plen_sum,
                                 benchmark::Counter::kIsIterationInvariantRate,
                                 benchmark::Counter::OneK::kIs1024);
    // How many partial queries are processed per second
    state.counters["PQRate"] = benchmark::Counter(num_pq, benchmark::Counter::kIsRate);
    // How many seconds it takes to process one partial query
    state.counters["PQInvRate"] = benchmark::Counter(num_pq,
                                    benchmark::Counter::kIsRate |
                                    benchmark::Counter::kInvert);
}

// Register the function as a benchmark
BENCHMARK(BM_build_index)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_build_dawg)->Unit(benchmark::kMillisecond);

/* BENCHMARK(BM_synth_query)->Unit(benchmark::kMillisecond)->RangeMultiplier(2)->Range(2, 8); */
/* BENCHMARK(BM_lr_query)->Unit(benchmark::kMillisecond)->RangeMultiplier(2)->Range(2, 8); */
BENCHMARK(BM_synth_query)->Unit(benchmark::kMillisecond)->Arg(8);
BENCHMARK(BM_synth_query_dawg)->Unit(benchmark::kMillisecond)->Arg(8);
BENCHMARK(BM_lr_query)->Unit(benchmark::kMillisecond)->Arg(8);

/* int main (int argc, char ** argv) { */
/*     benchmark::Initialize (& argc, argv); */
/*     benchmark::RunSpecifiedBenchmarks (); */
/*     return 0; */
/* } */

int main(int argc, char *argv[])
{
    string ofile;
    bool verbose = false;
    size_t n_rows = SIZE_MAX;
    try {
        cout << "Program options\n";
        cout << "==========================================================\n";
        options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "print usage message")
            ("output,o", value(&ofile), "pathname for output")
            ("testsize,t", value<size_t>(&n_rows),
                            "Run on test mode using t lines")
            ("verbose,v", "Run on verbose")
        ;
        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        vm.notify();

        if (vm.count("help")) {
            cout << desc << "\n";
            return 0;
        }

        if(vm.count("verbose")){
            verbose = true;
            cout << "Verbose mode: ON\n";
        }
        if(vm.count("testsize"))
            cout << "Test mode: ON Loading " << n_rows << " lines\n";
        cout << "==========================================================\n";
    }
    catch(exception& e) {
        cerr << e.what() << "\n";
    }

    Collection coll_wiki;
    cout << "Reading wiki clickstream collection\n";
    const string wiki_file = WIKI_ROOT + "clickstream-agg-wiki.tsv";
    coll_wiki.read_collection(wiki_file, n_rows, true);


    cout << "Building HAT Trie\n";
    HTrieCompleter ht_comp;
    for(const auto& kv: coll_wiki)
        ht_comp.update_index(kv);

    /* coll_wiki.sort_keys(); */
    /* vector<string> keys; */
    /* /1* vector<size_t> values; *1/ */
    /* transform(coll_wiki.begin(), coll_wiki.end(), std::back_inserter(keys), */
    /*                                 [](const auto& p) { return p.first; }); */
    /* /1* transform(coll_wiki.begin(), coll_wiki.end(), std::back_inserter(values), *1/ */
    /* /1*                                 [](const auto& p) { return p.second; }); *1/ */
    /* std::vector<size_t> values(keys.size()) ; // vector with 100 ints. */
    /* std::iota (std::begin(values), std::end(values), 0); // Fill with 0, 1, ..., 99. */

    /* cout << "Building DAWG Trie\n"; */
    /* DAWGTrie dtrie (keys, values); */
    /* if(!dtrie.build_status) */
    /*     return 1; */

    cout << "Testing completor\n\n";
    cout << "HAT Trie Completions\n";
    string prefix = "melb";
    auto completions = ht_comp.complete(prefix, 10);
    for (const auto& c : completions) {
       cout << c.first << "\t" << c.second << "\n";
    }

    cout << "\nMarisa Trie Completions\n";
    MarisaCompleter mtc;
    mtc.build_index(coll_wiki.get_collection());
    mtc.predictive_search(prefix);
    /* cout << "\nDAWG Trie Completions\n"; */
    /* completions = dtrie.complete("miller", 20); */
    /* for (const auto& c : completions) { */
    /*    cout << c.first << "\t" << c.second << "\n"; */
    /* } */

    /* PQLog plog; */
    /* cout << "Loading partial query log\n"; */
    /* plog.load_pqlog("../../synth_log/data/wiki-synthlog.tsv", n_rows); */
    /* for (const auto& kv: plog) { */
    /*     for(const auto& p: kv.second){ */
    /*         cout << "PQ: " << p << "\n____________________________\n"; */
    /*         auto completions = ht_comp.complete(p, 10); */
    /*         for(const auto& c: completions) */
    /*             cout << c.first << "\n"; */
    /*     } */
    /* } */

    /* LRLog lrlog; */
    /* lrlog.generate_lr_log(plog); */
    /* for(const auto& [k, v]: lrlog) */
    /*     cout << k << "\t" << boost::join(v, ",") << "\n"; */

    return 0;
}
