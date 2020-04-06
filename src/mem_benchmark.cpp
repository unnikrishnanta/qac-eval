#include<iostream>
#include "core/collection.hpp"
#include "core/pqlog.hpp"
#include "qac_impl/htrie_wrapper.hpp"
#include <thread>
#include <chrono>

using namespace std;

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
    size_t n_rows = SIZE_MAX;

    Collection *coll_wiki = new Collection();;
    cout << "Reading wiki clickstream collection\n";
    coll_wiki->read_collection('w', n_rows, true);
    cout << "Done\n";

    std::this_thread::sleep_for(std::chrono::seconds(40));
    cout << "Building index\n";

    HTrieCompleter ht_comp;
    ht_comp.build_index(*coll_wiki);

    delete(coll_wiki);
    std::this_thread::sleep_for(std::chrono::seconds(40));

    
    return 0;
}
