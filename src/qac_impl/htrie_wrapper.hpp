#ifndef HTRIE_WRAPPER_H
#define HTRIE_WRAPPER_H

#include "dtypes.hpp"
#include "hat-trie/tsl/htrie_map.h"

#include <vector>
#include <string>
#include <utility>

using namespace std;

class HTrieCompleter {
    public:
        void build_index(const sdict_t& str_dict);
        void update_index(const scored_str_t& sc);
        sdict_t complete(const string& prefix, const size_t& n_comp,
                        const bool& topk=true);
        void print_index_meta();
    private:
        tsl::htrie_map<char, size_t> ht_map;
};

#endif

