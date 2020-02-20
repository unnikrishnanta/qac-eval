#ifndef HTRIE_WRAPPER_H
#define HTRIE_WRAPPER_H

#include "../core/dtypes.hpp"
#include "../core/collection.hpp"
#include "hat-trie/tsl/htrie_map.h"

#include <vector>
#include <string>
#include <utility>

using namespace std;

class HTrieCompleter {
    public:
        void build_index(const StringDict& str_dict);
        void build_index(const Collection& coll);
        void update_index(const ScoredStr& sc);
        StringDict complete(const string& prefix, const size_t& n_comp,
                        const bool& topk=true);
        void print_index_meta();
    private:
        tsl::htrie_map<char, ScoreType> ht_map;
};

#endif

