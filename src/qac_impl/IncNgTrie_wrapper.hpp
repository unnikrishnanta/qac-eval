#ifndef INCTRIE_WRAPPER_H
#define INCTRIE_WRAPPER_H

#include "IncNgTrie/searcher_base.h"
#include "IncNgTrie/fastss_searcher.h"
#include "../core/dtypes.hpp"
#include "../core/collection.hpp"

#include <vector>
#include <string>
#include <utility>

using namespace std;
using namespace dbwsim;

class IncNgTrieCompleter {
    public:
        IncNgTrieCompleter();
        IncNgTrieCompleter(int mt);
        void build_index(const StrVec& str_set, const ScoreVec& scores);
        void build_index(const Collection& coll);
        CandidateSet<std::string_view>
            complete(const string& prefix, const size_t& n_comp);
        void print_index_meta();
    private:
        int max_tau; // Number of mismatches.
        FastssIndex *fastss_index = NULL;
        SearcherBase *searcher = NULL;
        ActiveNodePool *active_pool = NULL;
        TrieBase* indextrie;
};

#endif

