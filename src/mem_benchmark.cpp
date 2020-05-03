#include <cstdint>
#include <iostream>
#include "core/collection.hpp"
#include "core/config.hpp"
#include "core/pqlog.hpp"
#include <qac_impl.hpp>
#include <stdexcept>
#include <string>
#include <thread>
#include <chrono>
#include <memprof/mprof_core.hpp>
#include "core/heap_mem.h"
#include "qac_impl/IncNgTrie_wrapper.hpp"
#include "qac_impl/marisa_wrapper.hpp"

/* using namespace std; */
/* using namespace CGE; */
#define RANGE_MULTIPLIER 4
#define NROWS_RANGE_BEGIN 512


std::string generate_export_filename(const string& bm, const char& coll_type) {
    auto time_now = std::time(0);
    string coll_name; 
    switch(coll_type) {
        case WIKI: coll_name = "wiki"; break;
        case CWEB: coll_name = "cweb"; break;
        case BING: coll_name = "bing"; break;
    }
    auto out_file = "../export/data/mem-bm/"
                        + coll_name + "-"
                        + std::to_string(time_now) + "-"
                        + "-" + bm + "-mem.csv";
    std::ofstream new_file(out_file);  // overwrite existing file
    if (! new_file.is_open())
        throw std::runtime_error( "Unable to open output log file" );
    new_file.close();
    return out_file;
}


int main(int argc, char *argv[])
{
    std::cerr << "Platform detected: " << PLATFORM << "\n";
    //todo: take collection as an argument
    size_t n_rows = 10000;
    Collection coll; 
    for (const auto& coll_type : {WIKI, CWEB, BING}) {
        std::cout << "Collection type: " << coll_type << "\n";
        auto outfile = generate_export_filename("build", coll_type);
        coll.read_collection(coll_type, n_rows, true);
        { 
                MemProfiler<HTrieCompleter> htrie_mprof(outfile);
                for (size_t i = NROWS_RANGE_BEGIN; i < n_rows; i*=RANGE_MULTIPLIER) {
                   coll.uniform_sample(i); 
                   htrie_mprof.mem_bm_build(coll);
                }
        }

        {
            MemProfiler<MarisaCompleter> marisa_mprof(outfile);
            for (size_t i = NROWS_RANGE_BEGIN; i < n_rows; i*=RANGE_MULTIPLIER) {
               coll.uniform_sample(i); 
               marisa_mprof.mem_bm_build(coll);
            }
        }

        {
            MemProfiler<DAWGTrie> dawg_mprof(outfile);
            for (size_t i = NROWS_RANGE_BEGIN; i < n_rows; i*=RANGE_MULTIPLIER) {
                coll.uniform_sample(i); 
                dawg_mprof.mem_bm_build(coll);
            }
        }

        {
            MemProfiler<IncNgTrieCompleter> incngt_mprof(outfile);
            for (size_t i = NROWS_RANGE_BEGIN; i < n_rows; i*=RANGE_MULTIPLIER) {
                coll.uniform_sample(i); 
                incngt_mprof.mem_bm_build(coll);
            }
        }
        
    }

    return 0;
}
