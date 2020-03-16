#ifndef DAWG_WRAPPER_H
#define DAWG_WRAPPER_H
/* TODO: 
 *  3. Get completions with prefix added. 
 *  */
#include "dawgdic/dawg-builder.h"
#include "dawgdic/dictionary-builder.h"
#include "dawgdic/guide-builder.h"
#include "dawgdic/ranked-completer.h"
#include "dawgdic/ranked-guide-builder.h"
#include "../core/dtypes.hpp"
#include "../core/collection.hpp"
#include <vector>
#include <string>
#include <memory>

using namespace std;
using namespace dawgdic;

class DAWGTrie{
    
    public: 
         bool build_status; // If the trie was successfully built
         void build_index(const Collection& coll);
         bool build_index(const StrVec& str_set, const ScoreVec& scores);
         void build_index(const StrVec& keys);
         CandidateSet<std::string>
             complete(const string& prefix, const uint8_t& ncomp);

    private:
        Dictionary dic;
        RankedGuide guide;
        std::unique_ptr< RankedCompleter > completer;
        /* Build trie from seperate key and value vectors */
        bool build(const StrVec& keys);
};
#endif /* ifndef DAWG_WRAPPER_H */
