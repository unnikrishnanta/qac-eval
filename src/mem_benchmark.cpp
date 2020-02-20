#include<iostream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/join.hpp>
#include "core/collection.hpp"
#include "core/pqlog.hpp"
#include "qac_impl/htrie_wrapper.hpp"
#include "config.hpp"
#include <thread>
#include <chrono>

using namespace std;
using namespace boost::program_options;

tsl::htrie_map<char, size_t> build_index(const StringDict& str_dict){
    tsl::htrie_map<char, size_t> ht_map;
    for(const auto& sv:str_dict){
        ht_map[sv.first] = sv.second;
    }
    return ht_map;
}



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

    Collection *coll_wiki = new Collection();;
    cout << "Reading wiki clickstream collection\n";
    const string wiki_file = WIKI_ROOT + "clickstream-agg-wiki.tsv";
    coll_wiki->read_collection(wiki_file, n_rows, true);
    cout << "Done\n";

    std::this_thread::sleep_for(std::chrono::seconds(40));
    cout << "Building index\n";

    HTrieCompleter ht_comp;
    ht_comp.build_index(*coll_wiki);

    delete(coll_wiki);
    std::this_thread::sleep_for(std::chrono::seconds(40));

    /* cout << "Testing completor\n\n"; */
    /* auto completions = ht_comp.complete("pre", 5); */
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
