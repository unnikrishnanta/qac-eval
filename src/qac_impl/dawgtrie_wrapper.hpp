/* TODO: 
 *  3. Get completions with prefix added. 
 *  */
#include "dawgdic/include/dawgdic/dawg-builder.h"
#include "dawgdic/include/dawgdic/dictionary-builder.h"
#include "dawgdic/include/dawgdic/guide-builder.h"
#include "dawgdic/include/dawgdic/ranked-completer.h"
#include "dawgdic/include/dawgdic/ranked-guide-builder.h"
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
        /* Return ncomp completions for prefix */
         DAWGTrie(const vector<string>& keys, const vector<size_t>& vals);
         DAWGTrie(const Collection& coll);
         // Constructor will fill in values with 0 to keys.size() - 1
         DAWGTrie(const vector<string>& keys);
         CandidateSet<std::string>
             complete(const string& prefix, const uint8_t& ncomp);

    private:

        Dictionary dic;
        RankedGuide guide;
        std::unique_ptr< RankedCompleter > completer;
        /* Build trie from seperate key and value vectors */
        bool build(const StrVec& keys);
        bool build(const StrVec& keys, const ScoreVec& values);
};
