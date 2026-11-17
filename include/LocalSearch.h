#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include "Solution.h"

class LocalSearch {

public:

    // Try improving a solution using swap and 2-opt.
    // maxChecks limits the number of candidate moves tested.
    void improve(
        Solution& solution,
        const CVRP& problem,
        int maxChecks
    );
};

#endif