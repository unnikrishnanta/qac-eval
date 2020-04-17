#ifndef PROF_BASE_H
#define PROF_BASE_H

#include "../src/core/collection.hpp"
#include "../src/core/pqlog.hpp"

class QueryBase {
 public:
     static Collection coll;
     static PQLog qac_loq;
};

/* Declaring static members */
Collection QueryBase::coll;
PQLog QueryBase::qac_loq;

#endif /* ifndef PROF_BASE_H */
