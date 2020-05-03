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
#include "qac_impl/IncNgTrie_wrapper.hpp"
#include "qac_impl/marisa_wrapper.hpp"

/* using namespace std; */
/* using namespace CGE; */


int main(int argc, char *argv[])
{
    std::cerr << "Platform detected: " << PLATFORM << "\n";
    //todo: take collection as an argument
    size_t n_rows = 10;
    Collection coll; 
    coll.read_collection(WIKI, n_rows, true);

    { 
        MemProfiler<HTrieCompleter> htrie_mprof;
        htrie_mprof.mem_bm_build(coll);
    }
    
    {
        MemProfiler<MarisaCompleter> marisa_mprof;
        marisa_mprof.mem_bm_build(coll);
    }

    {
        MemProfiler<DAWGTrie> dawg_mprof;
        dawg_mprof.mem_bm_build(coll);
    }

    {
        MemProfiler<IncNgTrieCompleter> incngt_mprof;
        incngt_mprof.mem_bm_build(coll);
    }

    return 0;
}
