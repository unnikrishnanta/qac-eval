#ifndef DTYPES_H
#define DTYPES_H

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <queue>

using namespace std;

using strvec_t = vector<string>;
using scores_t = vector<size_t>;
using sdict_t = vector<pair<string, size_t>>;

struct comp_t {
    std::string_view comp_str;
    size_t score;

    comp_t () {}
    comp_t (const string& cs, const size_t& val): comp_str(cs), score(val) {}
};

struct CompareComps {
    bool operator()(comp_t const & lhs, comp_t const & rhs) {
        // Maintains a min-heap
        return lhs.score > rhs.score;
    }
};

/* Reference https://stackoverflow.com/a/33181173/937153 */
class Completions {
    public: 
        using comp_heap_t = priority_queue<comp_t, vector<comp_t>, CompareComps>;
        Completions(int k): k_(k) {}
        Completions(): k_(10) {}  // default #completions = 10

        /* Mantain top k elements in a min-heap. If there are k elements and a
         * new element comes in, add it to the heap if it is greater than the
         * heap top (lowest value)
         */
        void insert(const string& comp_str, const size_t& score){
            if (q_.size() < k_){ // Add to the heap straight away
                q_.emplace(comp_str, score);
            }
            else if (score > q_.top().score) {
                q_.pop();
                q_.emplace(comp_str, score);
            }
        }

        /* Traverse the heap and fill in the elements in reverse order to
         * obtain completions in descendng order of the score 
         */
        std::vector<comp_t> tok_completions() {
            std::vector<comp_t> result(q_.size());
            while (q_.size()) {
                result[q_.size() - 1] = q_.top();
                q_.pop();
            }
            return result;
        }

        const uint8_t n_comp() const { return q_.size();}

    private: 
        comp_heap_t q_;
        uint8_t k_;
};

#endif
