#include <cstdint>
#include<iostream>
#include "core/collection.hpp"
#include "core/pqlog.hpp"
#include "qac_impl/IncNgTrie_wrapper.hpp"
#include "qac_impl/dawgtrie_wrapper.hpp"
#include "qac_impl/htrie_wrapper.hpp"
#include "qac_impl/marisa_wrapper.hpp"
#include <thread>
#include <chrono>
#include <memprof/mprof_core.hpp>

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
    std::cout << "Args count by " << argv[0] << ": " << argc << "\n";
    //todo: take collection as an argument
    size_t n_rows = SIZE_MAX;
    const char COLLECTION = 'w';

    MemProfiler<HTrieCompleter> htrie_mprof(COLLECTION);
    htrie_mprof.set_nrows(n_rows);
    htrie_mprof.mem_bm_build();

    MemProfiler<MarisaCompleter> mtrie_mprof(COLLECTION);
    mtrie_mprof.set_nrows(n_rows);
    mtrie_mprof.mem_bm_build();

    MemProfiler<DAWGTrie> dtrie_mprof(COLLECTION);
    dtrie_mprof.set_nrows(n_rows);
    dtrie_mprof.mem_bm_build();

    MemProfiler<IncNgTrieCompleter> inct(COLLECTION);
    inct.set_nrows(n_rows);
    inct.mem_bm_build();

    return 0;
}
