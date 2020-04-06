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

template<class T>
void mem_bm_build(std::unique_ptr<Collection> coll) {
    T data_strct;
    data_strct.build_index(coll->get_strings(), coll->get_scores());
    coll.reset(); // Release the memory and use the mem usage by the index
    assert(coll.get() == nullptr);

    double vm_used = get_virtual_mem()/((double)ONE_K);
    double rss_used = get_physical_mem()/((double)ONE_K);
    
    std::cout << "Virtual memory " << vm_used << "\n";
    std::cout << "RSS memory " << rss_used << "\n";
}

#endif /* ifndef MPROF_CORE_H */
