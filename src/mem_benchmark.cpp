#include <cstdint>
#include <iostream>
#include "core/collection.hpp"
#include "core/config.hpp"
#include "core/pqlog.hpp"
#include <qac_impl.hpp>
#include <thread>
#include <chrono>
#include <memprof/mprof_core.hpp>
#include "core/heap_mem.h"
#include "qac_impl/marisa_wrapper.hpp"

/* using namespace std; */
/* using namespace CGE; */


int main(int argc, char *argv[])
{
    std::cerr << "Platform detected: " << PLATFORM << "\n";
    //todo: take collection as an argument
    size_t n_rows = 10;
    Collection coll; 
    coll.read_collection(COLLECTION, n_rows, true);

    {
        MemProfiler<HTrieCompleter> htrie_mprof;
        htrie_mprof.mem_bm_build(coll);
    }

    {
        MemProfiler<MarisaCompleter> mtrie_mprof;
        mtrie_mprof.mem_bm_build(coll);
    }

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
