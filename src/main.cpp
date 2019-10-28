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

static void BM_StringCreation(benchmark::State& state) {
  for (auto _ : state)
    std::string empty_string;
}
// Register the function as a benchmark
BENCHMARK(BM_StringCreation);

// Define another benchmark
static void BM_StringCopy(benchmark::State& state) {
  std::string x = "hello";
  for (auto _ : state)
    std::string copy(x);
}
BENCHMARK(BM_StringCopy);

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
    
/*     /1* Collection coll_wiki; *1/ */
/*     /1* cout << "Reading wiki clickstream collection\n"; *1/ */
/*     /1* const string wiki_file = WIKI_ROOT + "clickstream-agg-wiki.tsv"; *1/ */
/*     /1* coll_wiki.read_collection(wiki_file, n_rows, true); *1/ */

/*     /1* cout << "Building index\n"; *1/ */
/*     /1* HTrieCompleter ht_comp; *1/ */
/*     /1* for(const auto& kv: coll_wiki) *1/ */
/*     /1*     ht_comp.update_index(kv); *1/ */

/*     /1* cout << "Testing completor\n\n"; *1/ */
/*     /1* auto completions = ht_comp.complete("pre", 5); *1/ */
/*     /1* for (const auto& c : completions) { *1/ */
/*     /1*    cout << c.first << "\t" << c.second << "\n"; *1/ */ 
/*     /1* } *1/ */

/*     /1* PQLog plog; *1/ */
/*     /1* plog.load_pqlog("../../synth_log/data/wiki-synthlog.tsv", n_rows); *1/ */
/*     /1* for (const auto& kv: plog) { *1/ */
/*     /1*    cout << kv.first << "\t" << boost::join(kv.second, ",") << "\n"; *1/ */ 
/*     /1* } *1/ */
     
/*     return 0; */
/* } */
