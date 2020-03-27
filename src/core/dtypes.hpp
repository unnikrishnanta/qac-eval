#ifndef DTYPES_H
#define DTYPES_H

#include <iostream>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include <cassert>

using namespace std;

using StrVec = vector<string>;
using ScoreType = size_t;
using ScoreVec = vector<ScoreType>;
using ScoredStr = pair<string, ScoreType>;
using StringDict = vector<ScoredStr>;
using QueryLog = map<string, vector<string>>;

/* Type declaration for a single completion */

template <typename StrType>
class CandidateSet {

    public:
        typedef typename vector<pair<StrType, ScoreType>>::iterator iterator;
        typedef typename vector<pair<StrType, ScoreType>>::const_iterator const_iterator;
        
        // using const_iterator = array_type::const_iterator;
        CandidateSet(int k=8): k_(k) {
            n_comp_ = 0;
            c_.resize(k_);
        }

        void set_k(const int& k) {
            if(k > k_) 
                c_.resize(k);
            k_ = k;
        }

        void push_back(const string& comp_str, const ScoreType& score) {
            c_[n_comp_++] = std::make_pair(comp_str, score);
        }

        /* The above function overloaded for char* instead of string */
        void push_back(const char* comp_str, const int& slen, const ScoreType& score) {
            c_[n_comp_].first.assign(comp_str, slen);
            c_[n_comp_].second = score;
            ++n_comp_;
        }

        void reset() {
            n_comp_ = 0;
        }

        iterator begin() noexcept { return c_.begin(); }
        const_iterator cbegin() const noexcept { return c_.cbegin(); }
        iterator end() noexcept { return c_.begin() + n_comp_; }
        const_iterator cend() const noexcept { return c_.cbegin() + n_comp_; }

        int size() const { return std::as_const(n_comp_);}

    protected:
        vector<pair<StrType, ScoreType>> c_;
        int k_;
        int n_comp_; // #active completions. Needed here for iterators
};


/* Reference https://stackoverflow.com/a/33181173/937153
 * StrType = std::string or std::string_view to avoid copying
*/
template <typename StrType>
class CompHandler: public CandidateSet<StrType> {
    public: 


        /* Mantain top k elements in a min-heap. If there are k elements and a
         * new element comes in, add it to the heap if it is greater than the
         * heap top (lowest value)
         */
        void insert(const string_view comp_str, const ScoreType& score){
            assert(this->n_comp_ <= this->k_);  // Invariant to maintain top-k
            if (this->n_comp_ < this->k_){ // Add to the heap straight away
                this->c_[this->n_comp_++] = make_pair(comp_str, score);
                if (this->n_comp_ == this->k_) // c_begin() + k_ is c_end()
                    make_heap(this->c_.begin(), this->c_.end(), std::greater<>{});
            }
            else if (score > this->c_[0].second) {
                // moves the smallest to the end
                std::pop_heap(this->c_.begin(), this->c_.end(), std::greater<>{});
                this->c_.back() = make_pair(comp_str, score);
                /* Insert the element at the position last-1
                   into the max heap defined by the range [first, last-1) */
                std::push_heap(this->c_.begin(), this->c_.end(), std::greater<>{});
            }
        }

        void insert(const string& comp_str, const ScoreType& score){
            assert(this->n_comp_ <= this->k_);  // Invariant to maintain top-k
            if (this->n_comp_ < this->k_){ // Add to the heap straight away
                this->c_[this->n_comp_].first.assign(comp_str);
                this->c_[this->n_comp_].second  = score;
                ++(this->n_comp_); 
                //cout << "inserted " << c_[n_comp_ -1].first << "\n";
                if (this->n_comp_ == this->k_) // c_begin() + k_ is c_end()
                    make_heap(this->c_.begin(), this->c_.end(), std::greater<>{});
            }
            else if (score > this->c_[0].second) {
                // moves the smallest to the end
                std::pop_heap(this->c_.begin(), this->c_.end(), std::greater<>{});
                this->c_.back().first.assign(comp_str);
                this->c_.back().second = score;
                /* Insert the element at the position last-1
                into the max heap defined by the range [first, last-1) */
                std::push_heap(this->c_.begin(), this->c_.end(), std::greater<>{});
            }
        }
        /* Traverse the heap and fill in the elements in reverse order to
         * obtain completions in descendng order of the score 
         */
        CandidateSet<StrType> topk_completions() {
            if (this->n_comp_ < this->k_)
                std::sort(this->c_.begin(), this->c_.begin() + this->n_comp_,
                        [](auto &left, auto &right) {
                            return left.second > right.second;
                        });
            else 
                std::sort_heap(this->c_.begin(), this->c_.end(),
                        [](auto &left, auto &right) {
                            return left.second > right.second;
                        });
            return *this;
        }

        /* Both these functions return the same value */
        int n_comp() const { return std::as_const(this->n_comp_);}

};

#endif
