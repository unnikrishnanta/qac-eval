#include<iostream>
#include <boost/program_options.hpp>
#include "core/collection.hpp"
#include "config.hpp"

using namespace std;
using namespace boost::program_options;

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
    for(const auto sd: coll_wiki)
        cout << sd.first << "\t" << sd.second << "\n"; 
    return 0;
}
