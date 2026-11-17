#include "LocalSearch.h"

#include <algorithm>
#include <utility>

using namespace std;


void LocalSearch::improve(
    Solution& solution,
    const CVRP& problem,
    int maxChecks
) {

    if (!solution.feasible) {
        return;
    }

    Solution best = solution;

    int N = static_cast<int>(best.chromosome.size());
    int checks = 0;

    // Try swap and 2-opt moves
    for (int i = 0; i < N - 1; i++) {

        for (int j = i + 1; j < N; j++) {

            if (checks >= maxChecks) {
                solution = best;
                return;
            }

            // -------------------------
            // Move 1: Swap two customers
            // -------------------------

            Solution candidate = best;

            swap(
                candidate.chromosome[i],
                candidate.chromosome[j]
            );

            checks++;

            if (candidate.decode(problem) &&
                candidate.totalDistance < best.totalDistance) {

                best = candidate;
            }

            if (checks >= maxChecks) {
                solution = best;
                return;
            }

            // -------------------------
            // Move 2: 2-opt reversal
            // Reverse a chromosome segment
            // -------------------------

            candidate = best;

            reverse(
                candidate.chromosome.begin() + i,
                candidate.chromosome.begin() + j + 1
            );

            checks++;

            if (candidate.decode(problem) &&
                candidate.totalDistance < best.totalDistance) {

                best = candidate;
            }
        }
    }

    solution = best;
}