#ifndef PQLOG_H
#define PQLOG_H

#include <map>
#include <vector>
#include <string>

using namespace std;


class PQLog {

    using log_t = map<string, vector<string>>;

    public: 
        void load_pqlog(const string& file_name, const size_t& n_rows);
        log_t::const_iterator begin() const { return pq_log.cbegin(); }
        log_t::const_iterator end() const { return pq_log.cend(); }
        /* log_t::iterator begin() { return pq_log.begin(); } */
        /* log_t::iterator end() { return pq_log.end(); } */


    private:
        log_t pq_log;
        /* log_t lr_log; /1* Log obtained by typing left to right *1/ */
        /* void generate_lr_log(); */ 
    
};

class LRLog {

    using log_t = map<string, vector<string>>;
    public: 
        void generate_lr_log(const PQLog& pqlog); 
        log_t::const_iterator begin() const { return lr_log.cbegin(); }
        log_t::const_iterator end() const { return lr_log.cend(); }

    private:
        log_t lr_log; /* Log obtained by typing left to right */
    
};

#endif /* ifndef PQLOG_H */
