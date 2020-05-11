#include <cstdint>
#include <iostream>
#include <vector>
#include "../heap_count/mymem_alloc.hpp"
#include "core/collection.hpp"
#include "core/config.hpp"
#include "core/pqlog.hpp"
#include <qac_impl.hpp>
#include <stdexcept>
#include <string>
#include <thread>
#include <chrono>
#include <memprof/mprof_core.hpp>

/* using namespace std; */
#define RANGE_MULTIPLIER 4
#define NROWS_RANGE_BEGIN 512


/* std::string generate_export_filename(const string& bm, const char& coll_type) { */
/*     auto time_now = std::time(0); */
/*     string coll_name; */ 
/*     switch(coll_type) { */
/*         case WIKI: coll_name = "wiki"; break; */
/*         case CWEB: coll_name = "cweb"; break; */
/*         case BING: coll_name = "bing"; break; */
/*     } */
/*     auto out_file = "../export/data/mem-bm/" */
/*                         + coll_name + "-" */
/*                         + std::to_string(time_now) + "-" */
/*                         + "-" + bm + "-mem.csv"; */
/*     std::ofstream new_file(out_file);  // overwrite existing file */
/*     if (! new_file.is_open()) */
/*         throw std::runtime_error( "Unable to open output log file" ); */
/*     new_file.close(); */
/*     return out_file; */
/* } */

/* size_t update_loop_counter(const size_t& i, const size_t& n_rows) { */
/*     if( i*RANGE_MULTIPLIER <= n_rows ) */
/*         return i * RANGE_MULTIPLIER; */
/*     else if ( i != n_rows) */
/*         return n_rows; */
/*     else */
/*         return n_rows + 1;   // End loop with this update */
/* } */

/* int main(int argc, char *argv[]) */
/* { */
/*     std::cerr << "Platform detected: " << PLATFORM << "\n"; */
/*     //todo: take collection as an argument */
/*     size_t n_rows = 0; */
/*     if ((argc > 1) && (std::string(argv[1]) == "-t")) { */
/*         std::cout << "Test mode. Running on " << n_rows << " rows\n"; */
/*         n_rows = 10000; */
/*     } */
/*     Collection coll; */ 
/*     for (const auto& coll_type : {WIKI, CWEB, BING}) { */
/*         std::cout << "Collection type: " << coll_type << "\n"; */
/*         if(!n_rows) {  // Not test mode */
/*             switch(coll_type) { */
/*                 case WIKI: n_rows = WIKI_NROWS; break; */
/*                 case CWEB: n_rows = CWEB_NROWS; break; */
/*                 case BING: n_rows = BING_NROWS; break; */
/*             } */
/*         } */
/*         auto outfile = generate_export_filename("build", coll_type); */
/*         coll.read_collection(coll_type, n_rows, true); */
/*         coll.uniform_sample(coll.size(), false);  // Do a full shuffle. Don't sort */
/*         { */ 
/*                 MemProfiler<HTrieCompleter> htrie_mprof(outfile); */
/*                 for (size_t i = NROWS_RANGE_BEGIN; i <= n_rows; ) { */
/*                    auto str_set = coll.get_strings(i); // [0:i) slice */
/*                    auto scores = coll.get_scores(i); */
/*                    htrie_mprof.mem_bm_build(str_set, scores); */
/*                    i = update_loop_counter(i, n_rows); */
/*                 } */
/*                 std::cout << "\n\n"; */
/*         } */

/*         { */
/*             MemProfiler<MarisaCompleter> marisa_mprof(outfile); */
/*             for (size_t i = NROWS_RANGE_BEGIN; i <= n_rows;) { */
/*                 auto str_set = coll.get_strings(i); */
/*                 auto scores = coll.get_scores(i); */
/*                 marisa_mprof.mem_bm_build(str_set, scores); */
/*                 i = update_loop_counter(i, n_rows); */
/*             } */
/*             std::cout << "\n\n"; */
/*         } */

/*         { */
/*             MemProfiler<DAWGTrie> dawg_mprof(outfile); */
/*             for (size_t i = NROWS_RANGE_BEGIN; i <= n_rows;) { */
/*                 auto str_set = coll.get_strings(i); */
/*                 auto scores = coll.get_scores(i); */
/*                 dawg_mprof.mem_bm_build(str_set, scores); */
/*                 i = update_loop_counter(i, n_rows); */
/*             } */
/*             std::cout << "\n\n"; */
            
/*         } */

/*         { */
/*             MemProfiler<IncNgTrieCompleter> incngt_mprof(outfile); */
/*             for (size_t i = NROWS_RANGE_BEGIN; i <= n_rows;) { */
/*                 auto str_set = coll.get_strings(i); */
/*                 auto scores = coll.get_scores(i); */
/*                 incngt_mprof.mem_bm_build(str_set, scores); */
/*                 i = update_loop_counter(i, n_rows); */
/*             } */
/*             std::cout << "\n\n"; */
/*         } */
        
/*     } */

/*     CGE::GeneralHeap.dump(); */
/*     return 0; */
/* } */


int main(int argc, char *argv[])
{
    std::vector<int> v(1000);
#ifdef MALLOC_OVERRIDE
    /* CGE::GeneralHeap.dump(); */
#endif
    return 0;
}
