#ifndef HTRIE_WRAPPER_H
#define HTRIE_WRAPPER_H

#include "../core/dtypes.hpp"
#include "../core/collection.hpp"
#include "hat-trie/tsl/htrie_map.h"

#include <cstdint>
#include <vector>
#include <string>
#include <utility>

using namespace std;

class HTrieCompleter {
    public:
        void build_index(const StringDict& str_dict);
        void build_index(const Collection& coll);
        void update_index(const ScoredStr& sc);
        CandidateSet<std::string_view>
            complete(const string& prefix, const uint8_t& n_comp);
        void print_index_meta();
    private:
        tsl::htrie_map<char, ScoreType> ht_map;
};

#endif
