#ifndef PROF_BASE_H
#define PROF_BASE_H

#include "../src/core/collection.hpp"
#include "../src/core/pqlog.hpp"

class QueryBase {
 public:
     static Collection coll_full;
     static PQLog qac_log;
};

/* Declaring static members */
Collection QueryBase::coll_full;
PQLog QueryBase::qac_log;

#endif /* ifndef PROF_BASE_H */
