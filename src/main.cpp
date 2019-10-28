#include<iostream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/join.hpp>
#include "core/collection.hpp"
#include "core/pqlog.hpp"
#include "qac_impl/htrie_wrapper.hpp"
#include "config.hpp"
#include <benchmark/benchmark.h>

using namespace std;
using namespace boost::program_options;
using namespace qac_ht;

static void BM_synth_query(benchmark::State& state) {
    const size_t NCOMP = 10;
    Collection coll_wiki;
    const string wiki_file = WIKI_ROOT + "clickstream-agg-wiki.tsv";
    coll_wiki.read_collection(wiki_file, SIZE_MAX, true);
    PQLog plog;
    plog.load_pqlog("../../synth_log/data/wiki-synthlog.tsv", SIZE_MAX);
    HTrieCompleter ht_comp;
    for (auto _ : state){
        for (const auto& kv: plog) {
            for(const auto& p: kv.second)
                auto completions = ht_comp.complete(p, NCOMP);
        }
    }
}

static void  BM_build_index(benchmark::State& state) {
    Collection coll_wiki;
    const string wiki_file = WIKI_ROOT + "clickstream-agg-wiki.tsv";
    coll_wiki.read_collection(wiki_file, SIZE_MAX, true);
    HTrieCompleter ht_comp;
    for (auto _ : state){
        for(const auto& kv: coll_wiki)
            ht_comp.update_index(kv);
    }
}

// Register the function as a benchmark
BENCHMARK(BM_build_index);
BENCHMARK(BM_synth_query);

BENCHMARK_MAIN();

/* int main(int argc, char *argv[]) */
/* { */
/*     string ofile; */ 
/*     bool verbose = false; */
/*     size_t n_rows = SIZE_MAX; */
/*     try { */
/*         cout << "Program options\n"; */
/*         cout << "==========================================================\n"; */
/*         options_description desc("Allowed options"); */
/*         desc.add_options() */
/*             ("help,h", "print usage message") */
/*             ("output,o", value(&ofile), "pathname for output") */
/*             ("testsize,t", value<size_t>(&n_rows), */ 
/*                             "Run on test mode using t lines") */
/*             ("verbose,v", "Run on verbose") */
/*         ; */
/*         variables_map vm; */
/*         store(parse_command_line(argc, argv, desc), vm); */
/*         vm.notify(); */

/*         if (vm.count("help")) { */  
/*             cout << desc << "\n"; */
/*             return 0; */
/*         } */

/*         if(vm.count("verbose")){ */
/*             verbose = true; */
/*             cout << "Verbose mode: ON\n"; */
/*         } */
/*         if(vm.count("testsize")) */
/*             cout << "Test mode: ON Loading " << n_rows << " lines\n"; */
/*         cout << "==========================================================\n"; */
/*     } */
/*     catch(exception& e) { */
/*         cerr << e.what() << "\n"; */
/*     } */
    
/*     Collection coll_wiki; */
/*     cout << "Reading wiki clickstream collection\n"; */
/*     const string wiki_file = WIKI_ROOT + "clickstream-agg-wiki.tsv"; */
/*     coll_wiki.read_collection(wiki_file, n_rows, true); */

/*     cout << "Building index\n"; */
/*     HTrieCompleter ht_comp; */
/*     for(const auto& kv: coll_wiki) */
/*         ht_comp.update_index(kv); */

/*     cout << "Testing completor\n\n"; */
/*     auto completions = ht_comp.complete("pre", 5); */
/*     for (const auto& c : completions) { */
/*        cout << c.first << "\t" << c.second << "\n"; */ 
/*     } */

/*     PQLog plog; */
/*     cout << "Loading partial query log\n"; */
/*     plog.load_pqlog("../../synth_log/data/wiki-synthlog.tsv", n_rows); */
/*     for (const auto& kv: plog) { */
/*         for(const auto& p: kv.second){ */
/*             cout << "PQ: " << p << "\n____________________________\n"; */
/*             auto completions = ht_comp.complete(p, 10); */
/*             for(const auto& c: completions) */
/*                 cout << c.first << "\n"; */
/*         } */
/*     } */
     
/*     return 0; */
/* } */
