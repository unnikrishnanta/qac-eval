/* Data type definitions used in various QAC implementation wrappers.
 * Written by Unni on 27/01/2020
 */

#include <vector>
#include <string>
#include <utility>


using namespace std;

using scored_str_t = pair<string, size_t>;
using sdict_t  = vector<scored_str_t>;
