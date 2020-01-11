#ifndef MARISA_WRAPPER_H
#define MARISA_WRAPPER_H

#include "marisa/include/marisa.h"
/* #include "marisa/include/marisa/keyset.h" */
#include <vector>
#include <string>
#include <utility>

using namespace std;

namespace qac_mt {

    using scored_str_t = pair<string, size_t>;
    using sdict_t  = vector<scored_str_t>;

    class MarisaCompleter {
        public:
            void build_index(const sdict_t& str_dict);
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
    };

}

#endif

