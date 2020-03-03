#include <iostream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/join.hpp>
#include "core/collection.hpp"
#include "core/pqlog.hpp"
#include "qac_impl/htrie_wrapper.hpp"
#include "qac_impl/dawgtrie_wrapper.hpp"
#include "qac_impl/marisa_wrapper.hpp"
#include "qac_impl/IncNgTrie_wrapper.hpp"
#include "config.hpp"
#include <numeric>

/* #define TEST_HTRIE 1 */
#define TEST_MARISA 1
#define TEST_DAWG 1
/* #define TEST_INCGT 1 */
#define NCOMP 10

using namespace std;
using namespace boost::program_options;

int main(int argc, char *argv[])
{
    string ofile;
    bool verbose = false;
    size_t n_rows = SIZE_MAX;
    string prefix = "eagle";
    try {
        cout << "Program options\n";
        cout << "==========================================================\n";
        options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "print usage message")
            ("output,o", value(&ofile), "pathname for output")
            ("testsize,t", value<size_t>(&n_rows),
                            "Run on test mode using t lines")
            ("prefix,p", value<string>(&prefix),
                            "Prefix to query")
            ("verbose,v", "Run on verbose")
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

    Collection coll_wiki('c');
    cout << "Reading wiki clickstream collection\n";
    coll_wiki.read_collection(n_rows, true);

#ifdef TEST_HTRIE
    cout << "\nBuilding HAT Trie\n";
    HTrieCompleter ht_comp;
    ht_comp.build_index(coll_wiki);
    cout << "HAT Trie Completions\n"<< string(30, '-') << endl;
    auto completions = ht_comp.complete(prefix, 10);
    for (const auto& c : completions) {
        cout << c.first << "\t" << c.second << "\n";
    }
#endif

#ifdef TEST_DAWG
    cout << "Building DAWG Trie\n";
    DAWGTrie dtrie (coll_wiki);
    if(!dtrie.build_status){
        cout << "\nDAWG Trie construction failed\n";
        return 1;
    }
    cout << "\nDAWG Trie Completions\n" << string(30, '-') << endl;
    auto completions = dtrie.complete(prefix, 10);
    for (const auto& c : completions) {
        cout << c.comp_strview << "\t" << c.score << "\n";
    }

#endif

#ifdef TEST_MARISA
    cout << "\nBuilding marisa trie";
    MarisaCompleter mtc;
    mtc.build_index(coll_wiki);
    cout << "\nMarisa Trie Completions\n" << string(30, '-') << endl;
    auto mt_completions = mtc.complete(prefix, 10);
    for (const auto& c : mt_completions) {
       cout << c.comp_strview << "\t" << c.score << "\n";
    }
#endif

#ifdef TEST_INCGT
    cout << "\nBuilding IncNgTrie\n";
    IncNgTrieCompleter inc(1); // Allow edit distance of 1
    inc.build_index(coll_wiki);
    cout << "IncNgTrieCompletions\n" << string(30, '-') << endl;
    auto inc_completions = inc.complete(prefix, 10);
    for (const auto& c : inc_completions) {
        cout << c.comp_strview << "\t" << c.score << "\n";

#endif
    /* PQLog plog; */
    /* cout << "Loading partial query log\n"; */
    /* plog.load_pqlog("../../synth_log/data/wiki-synthlog.tsv", n_rows); */
    /* cout << "Done\n"; */
    /* cout << "Testing on synth log\n"; */
    /* for (const auto& kv: plog) { */
    /*     for(const auto& p: kv.second){ */
    /*         cout << "PQ: " << p << "\n"; */
    /*         auto completions = dtrie.complete(p, 10); */
    /*         for(const auto& c: completions) */
    /*             cout << c.comp_strview << "\t" << c.score << "\n"; */
    /*         cout << "\n\n"; */
    /*     } */
    /*     cout << endl; */
    /* } */

    /* LRLog lrlog; */
    /* lrlog.generate_lr_log(plog); */
    /* for(const auto& [k, v]: lrlog) */
    /*     cout << k << "\t" << boost::join(v, ",") << "\n"; */

    return 0;
}
