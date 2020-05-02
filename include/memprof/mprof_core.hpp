/* 
 * TODO: May be go for a benchmark class instead and read the
         collection in set up 
 */
#ifndef MPROF_CORE_H
#define MPROF_CORE_H
#include <cassert>
#include <cstddef>
#include <iostream>
#include <cstdint>
#include <memory>
#include <qac_impl.hpp>
#include "../src/core/collection.hpp"
#include "mem_usage.hpp"

#define ONE_K 1024

template <class T>
class MemProfiler {
    public: 
        MemProfiler(){
            setup();
        }

        ~MemProfiler(){
            teardown();
        }

        /* Any benchmark function should follow the same sequence: 
         * 1. Set up
         * 2. Build the index 
         * 3. Release the memory used to hold the collection 
         * 4. Update the counters used to hold memory usages 
         * 5. Tear down. 
         *
         * desc: A description for this benchmark
         */
        void mem_bm_build(Collection& coll) {
            /* std::cout << "BM_BUILD" << "\n"; */
            assert(coll.size() != 0);
            T data_strct;
            std::cout << "Building index\n";
            data_strct.build_index(coll.get_strings(), coll.get_scores());
            update_couters();
            print_counters();
        }
        

        void print_counters() {
            std::cout << "Virtual memory: " << vm_used << " MB\n";
            std::cout << "RSS memory: " << rss_used << " MB\n";
        }

        void set_nrows(const size_t& n) { n_rows_ = n; }


    private: 
        double vm_used;
        double rss_used;
        size_t n_rows_;

        void setup() {
            std::cout << std::string(40, '=') << std::endl;
            std::cout <<  typeid(T).name() << "\n";
            std::cout << std::string(40, '=') << std::endl;
            std::cerr << "Set up\n";
            FILE* file = fopen("/proc/self/status", "r");
            if (!file)
                std::cerr << "/proc/self/status info not available \n";
        }

        void teardown() {
            std::cerr << "Tear down\n";
        }

        void update_couters() {
            vm_used = get_virtual_mem()/((double)ONE_K);
            rss_used = get_physical_mem()/((double)ONE_K);
        }
};

#endif /* ifndef MPROF_CORE_H */
