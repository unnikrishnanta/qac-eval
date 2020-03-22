#ifndef PQLOG_H
#define PQLOG_H

#include <cstddef>
#include <map>
#include <vector>
#include <string>
#include "../core/dtypes.hpp"

using namespace std;


class PQLog {

    public: 
        void load_pqlog(const string& file_name, const size_t& n_rows);
        QueryLog::const_iterator begin() const { return pq_log_.cbegin(); }
        QueryLog::const_iterator end() const  { return pq_log_.cend(); }
        PQLog uniform_sample(const size_t& sample_size);
        bool insert(const string& qid, const vector<string>& pvec);
        size_t size() { return pq_log_.size(); }
        vector<string>& operator[] (const string& qid) { return pq_log_[qid]; }
        QueryLog lr_log(); // Generaet LR log from the finalPs in pq_log_

    private:
        QueryLog pq_log_;
};

#endif /* ifndef PQLOG_H */
