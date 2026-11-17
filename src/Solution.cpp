#include "Solution.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <limits>

using namespace std;

Solution::Solution() {

    totalDistance = 0.0;
    feasible = false;
}


// Calculate distance of one route:
// Depot -> customers -> Depot
double Solution::calculateRouteDistance(
    const vector<int>& route,
    const CVRP& problem
) const {

    if (route.empty()) {
        return 0.0;
    }

    double distance = 0.0;

    // Depot to first customer
    distance += problem.getDistance(0, route[0]);

    // Between consecutive customers
    for (int i = 1; i < static_cast<int>(route.size()); i++) {

        distance += problem.getDistance(
            route[i - 1],
            route[i]
        );
    }

    // Last customer to depot
    distance += problem.getDistance(
        route.back(),
        0
    );

    return distance;
}


// Dynamic Programming Split
bool Solution::decode(const CVRP& problem) {

    routes.clear();
    totalDistance = 0.0;
    feasible = false;

    int N = problem.getNumCustomers();
    int K = problem.getNumVehicles();
    int Q = problem.getVehicleCapacity();

    // Check chromosome size
    if (static_cast<int>(chromosome.size()) != N) {
        return false;
    }

    // Check that every customer appears exactly once
    vector<bool> visited(N + 1, false);

    for (int customer : chromosome) {

        if (customer < 1 || customer > N) {
            return false;
        }

        if (visited[customer]) {
            return false;
        }

        visited[customer] = true;
    }

    // dp[r][j] = minimum distance to serve the first
    // j customers using exactly r non-empty routes.

    const double INF = numeric_limits<double>::infinity();

    vector<vector<double>> dp(
        K + 1,
        vector<double>(N + 1, INF)
    );

    // parent[r][j] stores the starting position
    // of the final route.
    vector<vector<int>> parent(
        K + 1,
        vector<int>(N + 1, -1)
    );

    dp[0][0] = 0.0;

    // Consider the number of routes
    for (int r = 1; r <= K; r++) {

        // Consider the number of customers served
        for (int j = r; j <= N; j++) {

            int segmentDemand = 0;
            double segmentCost = 0.0;

            // Try every possible starting position
            // of the final route, moving backwards.
            for (int i = j; i >= r; i--) {

                int customer = chromosome[i - 1];

                segmentDemand +=
                    problem.getCustomers()[customer].demand;

                // Since demands are positive, earlier
                // starting positions will also exceed Q.
                if (segmentDemand > Q) {
                    break;
                }

                if (i == j) {

                    // Route contains only one customer
                    segmentCost =
                        problem.getDistance(0, customer) +
                        problem.getDistance(customer, 0);

                } else {

                    // Extend the segment one customer
                    // towards the beginning.
                    int nextCustomer = chromosome[i];

                    segmentCost =
                        segmentCost
                        - problem.getDistance(0, nextCustomer)
                        + problem.getDistance(0, customer)
                        + problem.getDistance(customer, nextCustomer);
                }

                if (dp[r - 1][i - 1] == INF) {
                    continue;
                }

                double newCost =
                    dp[r - 1][i - 1] + segmentCost;

                if (newCost < dp[r][j]) {

                    dp[r][j] = newCost;
                    parent[r][j] = i;
                }
            }
        }
    }

    // Select the best solution using at most K routes.
    // This allows some vehicles to remain unused.
    double bestCost = INF;
    int bestRouteCount = -1;

    for (int r = 1; r <= K; r++) {

        if (dp[r][N] < bestCost) {

            bestCost = dp[r][N];
            bestRouteCount = r;
        }
    }

    // No feasible contiguous partition was found
    if (bestRouteCount == -1 || bestCost == INF) {
        return false;
    }

    // Reconstruct routes using the parent table
    int j = N;
    int r = bestRouteCount;

    while (r > 0) {

        int start = parent[r][j];

        if (start == -1) {
            routes.clear();
            return false;
        }

        vector<int> route;

        for (int pos = start; pos <= j; pos++) {

            route.push_back(chromosome[pos - 1]);
        }

        routes.push_back(route);

        j = start - 1;
        r--;
    }

    // Routes were reconstructed backwards
    reverse(routes.begin(), routes.end());

    totalDistance = bestCost;
    feasible = true;

    return true;
}


// Display solution
void Solution::display(const CVRP& problem) const {

    if (!feasible) {

        cout << "\nSolution is infeasible.\n";
        return;
    }

    cout << "\n========== BEST SOLUTION ==========\n";

    cout << "Giant Tour: ";

    for (int customer : chromosome) {
        cout << customer << " ";
    }

    cout << "\n\nVehicle Routes:\n";

    for (int i = 0; i < static_cast<int>(routes.size()); i++) {

        cout << "Vehicle " << i + 1 << ": 0 ";

        for (int customer : routes[i]) {
            cout << "-> " << customer << " ";
        }

        cout << "-> 0";

        cout << " | Distance: "
             << fixed << setprecision(2)
             << calculateRouteDistance(routes[i], problem);

        cout << "\n";
    }

    cout << "\nVehicles Used: " << routes.size();

    cout << "\nTotal Distance: "
         << fixed << setprecision(2)
         << totalDistance;

    cout << "\n===================================\n";
}