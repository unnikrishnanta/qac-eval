#ifndef DTYPES_H
#define DTYPES_H

#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <queue>

using namespace std;

using StrVec = vector<string>;
using ScoreType = size_t;
using ScoreVec = vector<ScoreType>;
using ScoredStr = pair<string, ScoreType>;
using StringDict = vector<ScoredStr>;

/* Type declaration for a single completion */
template <typename StrType>
struct comp_t {
    StrType comp_strview;
    ScoreType score;

    comp_t () {}
    comp_t (const string_view cs, const ScoreType& val): comp_strview(cs), score(val) {}
    comp_t (const string& cs, const ScoreType& val): comp_strview(cs), score(val) {}
};

template <typename StrType>
struct CompareComps {
    bool operator()(comp_t<StrType> const & lhs, comp_t<StrType> const & rhs) {
        // Maintains a min-heap
        return lhs.score > rhs.score;
    }
};

template <typename StrType>
using CandidateSet  = vector<comp_t<StrType>>;

/* Reference https://stackoverflow.com/a/33181173/937153
 * StrType = std::string or std::string_view to avoid copying
*/
template <typename StrType>
class CompHandler {
    public: 
        using comp_heap_t = priority_queue<comp_t<StrType>,
                            vector<comp_t<StrType>>, CompareComps<StrType>>;
        CompHandler(int k): k_(k) {}
        CompHandler(): k_(10) {}  // default #completions = 10

        /* Mantain top k elements in a min-heap. If there are k elements and a
         * new element comes in, add it to the heap if it is greater than the
         * heap top (lowest value)
         */
        void insert(const string_view comp_strview, const ScoreType& score){
            if (q_.size() < k_){ // Add to the heap straight away
                q_.emplace(comp_strview, score);
            }
            else if (score > q_.top().score) {
                q_.pop();
                q_.emplace(comp_strview, score);
            }
        }

        /* Traverse the heap and fill in the elements in reverse order to
         * obtain completions in descendng order of the score 
         */
        CandidateSet<StrType> topk_completions() {
            CandidateSet<StrType> result(q_.size());
            while (q_.size()) {
                result[q_.size() - 1] = q_.top();
                q_.pop();
            }
            return result;
        }
        uint8_t n_comp() const { return std::as_const(q_.size());}

    private: 
        comp_heap_t q_;
        uint8_t k_;
};

#endif
