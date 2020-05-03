/* 
 * TODO: May be go for a benchmark class instead and read the
         collection in set up 
 */
#ifndef MPROF_CORE_H
#define MPROF_CORE_H
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <memory>
#include <chrono>
#include <ctime>
#include <vector>
#include <string>
#include <qac_impl.hpp>
#include <csvfile.h>
#include "../src/core/collection.hpp"
#include "mem_usage.hpp"
#include "../malloc_count/malloc_count.h"
#include "../malloc_count/stack_count.h"


#define MBYTES "MB"
#define BYTES "B"
#define MEM_UNIT BYTES

#define ONE_K 1024

struct MemCounters {
    double vm_used;
    double rss_used;
    double heap_used; 
    double heap_peak;
    double heap_total;
    double stack_used;

    /* All counts maintained in MB */
    void reset_counters() {
        vm_used    = 0;
        rss_used   = 0;
        heap_used  = 0; 
        heap_peak  = 0;
        heap_total = 0;
        stack_used = 0;
    }

    double Bytes_to_MB(const size_t& Bytes){
        return (double)Bytes/ONE_K/ONE_K;
    }

    void update_counters(void* stack_base, const string unit=MEM_UNIT) {
        heap_used = malloc_count_current();
        heap_peak = malloc_count_peak();
        heap_total = malloc_count_total();
        vm_used = get_virtual_mem() * ONE_K; // KB to B
        rss_used = get_physical_mem() * ONE_K;
        stack_used = stack_count_usage(stack_base);
        if(unit == MBYTES){  // Convert all counters in Bytes to MB
            heap_used = Bytes_to_MB(heap_used);
            heap_peak = Bytes_to_MB(heap_peak);
            heap_total = Bytes_to_MB(heap_total);
            vm_used = Bytes_to_MB(vm_used); // KB to MB
            rss_used = Bytes_to_MB(rss_used);
            stack_used = Bytes_to_MB(stack_used);
        }
    }

    std::vector<double> get_counters() {
        vector<double> count_vec;
        count_vec.push_back(vm_used);
        count_vec.push_back(rss_used);
        count_vec.push_back(heap_used);
        count_vec.push_back(heap_peak);
        count_vec.push_back(heap_total);
        count_vec.push_back(stack_used);
        return count_vec;
    }

    std::vector<std::string> counter_labels(){
        std::vector<std::string> label_vec;
        label_vec.push_back("vm_used");
        label_vec.push_back("rss_used");
        label_vec.push_back("heap_used");
        label_vec.push_back("heap_peak");
        label_vec.push_back("heap_total");
        label_vec.push_back("stack_used");
        return label_vec;
    }

    void print_counters() {
        std::cout << "Memory counters\n";
        std::cout << std::string(40, '=') << std::endl;
        std::cout << "vm_used\t\t:" << vm_used << " " << MEM_UNIT << "\n";
        std::cout << "rss_used\t:"  << rss_used << " " << MEM_UNIT << "\n";
        std::cout << "heap_used\t:" << heap_used << " " << MEM_UNIT << "\n";
        std::cout << "heap_peak\t:"<< heap_peak << " " << MEM_UNIT << "\n";
        std::cout << "heap_total\t:"  << heap_total << " " << MEM_UNIT << "\n";
        std::cout << "stack_used\t:" << stack_used << " " << MEM_UNIT << "\n\n";
    }
};



template <class T>
class MemProfiler {
    public: 
        MemProfiler(){
            auto out_file = generate_export_filename();
            std::cerr << "Outfile set to: " << out_file << "\n";
            csv_out_.open(out_file);
        }

        ~MemProfiler(){
        }

        /* Any benchmark function should follow the same sequence: 
         * 1. Set up
         * 2. Build the index 
         * 3. Release the memory used to hold the collection 
         * 4. Update the counters used to hold memory usages 
         * 5. Tear down. 
         */
        void mem_bm_build(Collection& coll) {
            setup(coll);
            /* assert(coll.size() != 0); */
            /* T data_strct; */
            /* std::cout << "Building index\n"; */
            base_counts_.print_counters();
            /* data_strct.build_index(coll.get_strings(), coll.get_scores()); */
            std::vector<int> v(10);
            set_counters();
            curr_counts_.print_counters();
            teardown();
        }
        
    private: 
        MemCounters base_counts_;
        MemCounters curr_counts_;
        csvfile csv_out_;
        void* stack_base_;


        void setup(const Collection& coll) {
            std::cout << std::string(40, '=') << std::endl;
            std::cout <<  typeid(T).name() << "\n";
            std::cout << std::string(40, '=') << std::endl;
            std::cerr << "Set up\n";
            FILE* file = fopen("/proc/self/status", "r");
            if (!file)
                std::cerr << "/proc/self/status info not available \n";
            else 
                fclose(file);
            // Print log file header
            for (const auto& l : base_counts_.counter_labels()) {
               csv_out_ << l; 
            }
            csv_out_ << endrow;
            set_base_counters();
        }

        /* Set the counter values to current usage
         * 1. Clear stack and heap static counters from malloc_count
         * 2. Set base_counts_ 
         * 3. Reset curr_counts_
        */
        void set_base_counters() {
            malloc_count_reset_peak();  // Resets global malloc_peak
            stack_base_ = stack_count_clear();  // Resets stack count on stack_base_
            base_counts_.reset_counters();
            base_counts_.update_counters(stack_base_);
            curr_counts_.reset_counters();
        }


        void set_counters() {
            curr_counts_.update_counters(stack_base_);
            curr_counts_.rss_used = curr_counts_.rss_used - base_counts_.rss_used;
            curr_counts_.vm_used = curr_counts_.vm_used - base_counts_.vm_used;
            curr_counts_.heap_used = curr_counts_.heap_used - base_counts_.heap_used;
            curr_counts_.heap_peak = curr_counts_.heap_peak - base_counts_.heap_peak;
            curr_counts_.heap_total = curr_counts_.heap_total - base_counts_.heap_total;
            curr_counts_.stack_used = curr_counts_.stack_used - base_counts_.stack_used;
        }

        void write_counters() {
            for (const auto cnt : curr_counts_.get_counters()) {
               csv_out_ << cnt; 
            }
            csv_out_ << endrow;
        }

        std::string generate_export_filename() {
            auto end = std::chrono::system_clock::now();
            std::time_t time_now = std::chrono::system_clock::to_time_t(end);
            string time_str = std::ctime(&time_now);
            std::replace( time_str.begin(), time_str.end(), ' ', '-'); 
            time_str.pop_back();
            auto qac_impl = typeid(T).name();
            auto out_file = "../export/data/mem-bm/" + string(qac_impl)
                            + "-" + time_str + ".csv";
            return out_file;
        }

        void teardown() {
            std::cerr << "Tear down\n";
            write_counters();
        }
};

#endif /* ifndef MPROF_CORE_H */
