#ifndef MARISA_WRAPPER_H
#define MARISA_WRAPPER_H

#include "marisa/include/marisa.h"
#include "../core/collection.hpp"
#include "../core/dtypes.hpp"
#include <vector>
#include <string>
#include <utility>

using namespace std;


/* In the class, it is ok to maintain the weights as a seperate vector since the
 * other implementaions store the weights along with the strings in the trie
 * structure.
 */
class MarisaCompleter {
    public:
        void build_index(const StringDict& str_dict);
        void build_index(const Collection& coll);
        /* void update_index(const scored_str_t& sc); */
        StringDict complete(const string& prefix, const size_t& n_comp,
                        const bool& topk=true);
        /* void print_index_meta(); */
        void predictive_search(const string& str,
                        const size_t& max_num_results=10);
    private:
        marisa::Trie trie;
        marisa::Keyset keyset;
        marisa::Agent agent;
        ScoreVec weights; // node ID to weights mapping.
};

#endif

