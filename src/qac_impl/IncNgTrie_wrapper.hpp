#ifndef INCTRIE_WRAPPER_H
#define INCTRIE_WRAPPER_H

#include "dtypes.hpp"
#include "IncNgTrie/searcher_base.h"
#include "IncNgTrie/fastss_searcher.h"

#include <vector>
#include <string>
#include <utility>

using namespace std;
using namespace dbwsim;

class IncNgTrieCompleter {
    public:
        IncNgTrieCompleter();
        void build_index(const sdict_t& str_dict);
        void update_index(const scored_str_t& sc);
        sdict_t complete(const string& prefix, const size_t& n_comp,
                        const bool& topk=true);
        void print_index_meta();
    private:
        int max_tau; // Number of mismatches.
        FastssIndex *fastss_index = NULL;
        SearcherBase *searcher = NULL;
        ActiveNodePool *active_pool = NULL;
        TrieBase* indextrie;
};

#endif

