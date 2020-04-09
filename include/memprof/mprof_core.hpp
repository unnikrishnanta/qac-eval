/* 
 * TODO: May be go for a benchmark class instead and read the collection in set
 * up 
 */
#ifndef MPROF_CORE_H
#define MPROF_CORE_H
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
        MemProfiler(const char& c):coll_type(c) {
            n_rows = SIZE_MAX; // By default
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
        void mem_bm_build() {
            std::cout << std::string(40, '=') << std::endl;
            std::cout << "BM_BUILD\t" << typeid(T).name() << "\n";
            std::cout << std::string(40, '=') << std::endl;
            setup();
            std::cout << "Building index\n";
            data_strct->build_index(coll->get_strings(), coll->get_scores());
            reset_collection();
            update_couters();
            print_counters();
            teardown();
        }
        
        void set_nrows(const size_t& nr) { n_rows = nr;}

        void print_counters() {
            std::cout << "Virtual memory: " << vm_used << " MB\n";
            std::cout << "RSS memory: " << rss_used << " MB\n";
        }


    private: 
        double vm_used;
        double rss_used;
        const char coll_type;  // Collection type
        std::unique_ptr<Collection> coll;
        std::unique_ptr<T> data_strct;
        size_t n_rows;

        void setup() {
            std::cout << "Set up\n";
            assert(coll.get() == nullptr);
            assert(data_strct.get() == nullptr);
            coll = make_unique<Collection>();
            data_strct = make_unique<T>();
            coll->read_collection(coll_type, n_rows, true);
        }

        void teardown() {
            std::cout << "Tear down\n";
            assert(coll.get() == nullptr);  // A benchmark should reset coll
            assert(data_strct.get() != nullptr);
            data_strct.reset();
        }

        void reset_collection() {
            assert(coll.get() != nullptr);
            coll.reset();
        }

        void update_couters() {
            vm_used = get_virtual_mem()/((double)ONE_K);
            rss_used = get_physical_mem()/((double)ONE_K);
        }
};

#endif /* ifndef MPROF_CORE_H */
