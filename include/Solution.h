#ifndef SOLUTION_H
#define SOLUTION_H

#include <vector>
#include "CVRP.h"

using namespace std;

class Solution {

public:

    // Giant-tour chromosome: customer IDs in visiting order
    vector<int> chromosome;

    // Decoded vehicle routes
    vector<vector<int>> routes;

    double totalDistance;
    bool feasible;

    Solution();

    // Decode chromosome into routes using DP Split
    bool decode(const CVRP& problem);

    // Display chromosome and vehicle routes
    void display(const CVRP& problem) const;

private:

    // Calculate the distance of one route
    double calculateRouteDistance(
        const vector<int>& route,
        const CVRP& problem
    ) const;
};

#endif