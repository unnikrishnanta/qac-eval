#ifndef HTRIE_WRAPPER_H
#define HTRIE_WRAPPER_H

#include "hat-trie/tsl/htrie_map.h"
#include <vector>
#include <string>
#include <utility>

using namespace std;

namespace qac_ht {

    using scored_str_t = pair<string, size_t>;
    using sdict_t  = vector<scored_str_t>;

    class HTrieCompleter {
        public:
            void build_index(const sdict_t& str_dict);
            sdict_t complete(const string& prefix);
        private:
            tsl::htrie_map<char, int> ht_map;
    };
    
}

#endif 

