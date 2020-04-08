#include <cstdint>
#include <iostream>
#include <string_view>
#include "core/collection.hpp"
#include "core/pqlog.hpp"
#include "qac_impl/htrie_wrapper.hpp"
#include "qac_impl/dawgtrie_wrapper.hpp"
#include "qac_impl/marisa_wrapper.hpp"
#include "qac_impl/IncNgTrie_wrapper.hpp"
#include <numeric>
#include <myprog_opts.hpp>

/* #define TEST_HTRIE 1 */
/* #define TEST_MARISA 1 */
/* #define TEST_DAWG 1 */
#define TEST_INCGT 1
#define NCOMP 10
#define COLLECTION 'w'

using namespace std;

int main(int argc, char *argv[])
{
    string ofile;
    if (cmdOptionExists(argv, argv + argc, "-h")) {
        cout << "Program options\n";
        cout << "==========================================================\n";
        cout << "-h Print usage.";
        cout << "-t testsize: Run on testsize number of lines\n";
        cout << "-s size for synth log\n";
        cout << "-p : Partial query to search\n";
        return 0;
    }

    bool verbose = false;
    if (cmdOptionExists(argv, argv + argc, "-v"))
        verbose = true;

    size_t n_rows = SIZE_MAX;
    char *test_size = getCmdOption(argv, argv + argc, "-t");
    if(test_size){
        n_rows = atoi(test_size);
        cout  << "Test mode. Loading " << n_rows << " lines from file\n";
    }

    size_t sln_rows = SIZE_MAX;
    char *sl_size = getCmdOption(argv, argv + argc, "-s");
    if(sl_size){
        sln_rows = atoi(sl_size);
        cout  << "Loading " << n_rows << " lines from synth log\n";
    }

    string prefix = "eagle";
    char *pf = getCmdOption(argv, argv + argc, "-p");
    if(pf){
        prefix = string(pf);
    }

    Collection coll_wiki;
    cout << "Reading collection\n";
    coll_wiki.read_collection(COLLECTION, n_rows, true);

#ifdef TEST_HTRIE
    cout << "\nBuilding HAT Trie\n";
    HTrieCompleter ht_comp;
    ht_comp.build_index(coll_wiki);
    cout << "HAT Trie Completions\n"<< string(30, '-') << endl;
    auto ht_completions = ht_comp.complete(prefix, 10);
    for (const auto& c : ht_completions) {
        cout << c.first << "\t" << c.second << "\n";
    }
    cout << "Total " << ht_completions.size() << " completions \n";
#endif

#ifdef TEST_DAWG
    cout << "Building DAWG Trie\n";

    for (size_t i = 126367744; i < 195285642; i = i + (1<<5)) {
        cout << "Sample size " << i << "\n";
        DAWGTrie dtrie;
        coll_wiki.uniform_sample(i);
        dtrie.build_index(coll_wiki);
        if(!dtrie.build_status){
            cout << "\nDAWG Trie construction failed\n";
            return 1;
        }
    }

    /* DAWGTrie dtrie; */
    /* dtrie.build_index(coll_wiki); */
    /* cout << "\nDAWG Trie Completions\n" << string(30, '-') << endl; */
    /* auto dt_completions = dtrie.complete(prefix, 10); */
    /* for (const auto& c : dt_completions) { */
    /*     cout << c.first << "\t" << c.second << "\n"; */
    /* } */

#endif

#ifdef TEST_MARISA
    cout << "\nBuilding marisa trie";
    MarisaCompleter mtc;
    mtc.build_index(coll_wiki);
    cout << "\nMarisa Trie Completions\n" << string(30, '-') << endl;
    auto mt_completions = mtc.complete(prefix, 10);
    for (const auto& c : mt_completions) {
       cout << c.first << "\t" << c.second << "\n";
    }
#endif

#ifdef TEST_INCGT
    cout << "\nBuilding IncNgTrie\n";
    IncNgTrieCompleter inc(1); // Allow edit distance of 1
    inc.build_index(coll_wiki);
    cout << "IncNgTrieCompletions\n" << string(30, '-') << endl;
    auto inc_completions = inc.complete(prefix, 5);
    for (const auto& c : inc_completions) {
        cout << c.first << "\t" << c.second << "\n";
    }

#endif

    /* PQLog plog; */
    /* cout << "Loading partial query log\n"; */
    /* plog.load_synthlog("../../synth_log/data/wiki-synthlog.tsv", sln_rows); */
    /* cout << "Done\n"; */
    /* cout << "Testing on synth log\n"; */
    /* for (const auto& kv: plog) { */
    /*     for(const auto& p: kv.second){ */
    /*         cout << "Synth PQ: " << p << "\n"; */
    /*         auto completions = ht_comp.complete(p, 10); */
    /*         for (const auto& c: completions) */ 
    /*             cout << c.first << "\t" << c.second << "\n"; */
    /*         cout << "\n\n"; */
    /*     } */
    /*     cout << endl; */
    /* } */

    /* auto lrlog = plog.lr_log(); */
    /* for(const auto& [k, v]: lrlog){ */
    /*     /1* cout << k << "\t" << boost::join(v, ",") << "\n"; *1/ */
    /*     for(const auto& p: v){ */
    /*         cout << "LR PQ: " << p << "\n"; */
    /*         auto completions = ht_comp.complete(p, 10); */
    /*         for (const auto& c: completions) { */
    /*             cout << c.first << "\t" << c.second << "\n"; */
    /*         } */
    /*         cout << "\n\n"; */
    /*     } */
    /* } */

    return 0;
}
