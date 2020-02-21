/* TODO: 
 *  1. Add include directories 
 *  2. Write a proper comparator
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

using StringDict = vector<pair<string, size_t>>;
namespace {
    class Comparer {
        public:
            explicit Comparer(const vector<size_t> &values) : values_(&values) {}

            bool operator()(const ValueType &lhs, const ValueType &rhs) const {
                /* if (lhs < values_->size() && rhs < values_->size()) */
                    return values_->at(lhs) < values_->at(rhs);
                /* else */ 
                /*     cout << "Out of bounds. size() = " << values_->size() */ 
                /*          << " LHS " << lhs << " RHS " << rhs << "\n"; */
                /* return false; */
            }

        private:
            const vector<size_t> *values_;
    };

}  // namespace
class DAWGTrie{
    
    public: 
        bool build_status; // If the trie was successfully built
        /* Return ncomp completions for prefix */
         StringDict complete(const string& prefix, const size_t& ncomp);
         DAWGTrie(const vector<string>& keys, const vector<size_t>& vals);
         DAWGTrie(const Collection& coll);
         // Constructor will fill in values with 0 to keys.size() - 1
         DAWGTrie(const vector<string>& keys);

    private:

        Dictionary dic;
        RankedGuide guide;
        std::unique_ptr< RankedCompleter > completer;
        const vector<size_t> values;
        /* Build trie from seperate key and value vectors */
        bool build(const StrVec& keys);
        bool build(const StrVec& keys, const ScoreVec& values);
};
