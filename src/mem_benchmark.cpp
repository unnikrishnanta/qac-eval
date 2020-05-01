#include <cstdint>
#include<iostream>
#include "core/collection.hpp"
#include "core/pqlog.hpp"
#include <qac_impl.hpp>
#include <thread>
#include <chrono>
#include <memprof/mprof_core.hpp>
#include "core/heap_mem.h"

/* using namespace std; */
/* using namespace CGE; */

int main(int argc, char *argv[])
{
    //todo: take collection as an argument
    size_t n_rows = SIZE_MAX;

    Collection coll_wiki;
    cout << "Reading collection\n";
    coll_wiki.read_collection('w', n_rows, true);
    HTrieCompleter ht_comp;
    ht_comp.build_index(coll_wiki);
    /* MemProfiler<HTrieCompleter> htrie_mprof(COLLECTION); */
    /* htrie_mprof.set_nrows(n_rows); */
    /* htrie_mprof.mem_bm_build(); */

    /* MemProfiler<MarisaCompleter> mtrie_mprof(COLLECTION); */
    /* mtrie_mprof.set_nrows(n_rows); */
    /* mtrie_mprof.mem_bm_build(); */

    /* MemProfiler<DAWGTrie> dtrie_mprof(COLLECTION); */
    /* dtrie_mprof.set_nrows(n_rows); */
    /* dtrie_mprof.mem_bm_build(); */

    /* MemProfiler<IncNgTrieCompleter> inct(COLLECTION); */
    /* inct.set_nrows(n_rows); */
    /* inct.mem_bm_build(); */
    return 0;
}
