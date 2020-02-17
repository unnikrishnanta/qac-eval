#ifndef MARISA_WRAPPER_H
#define MARISA_WRAPPER_H

#include "marisa/include/marisa.h"
#include "dtypes.hpp"
#include "../core/collection.hpp"
/* #include "marisa/include/marisa/keyset.h" */
#include <vector>
#include <string>
#include <utility>

using namespace std;


using scored_str_t = pair<string, size_t>;
using sdict_t  = vector<scored_str_t>;

/* In the class, it is ok to maintain the weights as a seperate vector since the
 * other implementaions store the weights along with the strings in the trie
 * structure.
 */
class MarisaCompleter {
    public:
        void build_index(const sdict_t& str_dict);
        void build_index(const Collection& coll);
        /* void update_index(const scored_str_t& sc); */
        sdict_t complete(const string& prefix, const size_t& n_comp,
                        const bool& topk=true);
        /* void print_index_meta(); */
        void predictive_search(const string& str,
                        const size_t& max_num_results=10);
    private:
        marisa::Trie trie;
        marisa::Keyset keyset;
        marisa::Agent agent;
        vector<size_t> weights; // node ID to weights mapping.
};

#endif

