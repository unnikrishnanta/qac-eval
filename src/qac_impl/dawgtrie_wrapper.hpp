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

#include <vector>
#include <string>

using namespace std;
using namespace dawgdic;

using sdict_t = vector<pair<string, size_t>>;
namespace {
    class Comparer {
        public:
            explicit Comparer(const vector<size_t> &values) : values_(&values) {}

            bool operator()(const ValueType &lhs, const ValueType &rhs) const {
                /* return (*values_)[lhs] < (*values_)[rhs]; */
                return true; // For now because ordering is not important
            }

        private:
            const vector<size_t> *values_;
    };

}  // namespace
class DAWGTrie{
    
    public: 
        bool build_status; // If the trie was successfully built
        /* Return ncomp completions for prefix */
         sdict_t complete(const string& prefix, const size_t& ncomp);
         DAWGTrie(const vector<string>& keys, const vector<size_t>& vals);

    private:

        Dictionary dic;
        RankedGuide guide;
        std::unique_ptr< RankedCompleterBase<Comparer> > completer;
        const vector<size_t> values;
        /* Build trie from seperate key and value vectors */
        bool build(const vector<string>& keys, const vector<size_t>& vals);
};
