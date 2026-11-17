#include <iostream>
#include <vector>
#include <iomanip>
#include <exception>
#include <chrono>
#include <omp.h>

#include "CVRP.h"
#include "GeneticAlgorithm.h"

using namespace std;

int main() {
    auto startTime = chrono::high_resolution_clock::now();
    try {

        CVRP problem;

        if (!problem.loadFromFile("data/medium.txt")) {
            cerr << "Failed to load dataset.\n";
            return 1;
        }

        problem.displayProblem();

        const int RUNS = 4;

        int populationSize = 40;
        int generations = 200;
        double mutationRate = 0.15;
        int tournamentSize = 3;

        vector<Solution> results(RUNS);

        cout << "\n========== PARALLEL EXECUTION ==========\n";

        cout << "OpenMP maximum threads: "
             << omp_get_max_threads() << "\n";

        double startTime = omp_get_wtime();

        #pragma omp parallel for schedule(dynamic)
        for (int run = 0; run < RUNS; run++) {

            GeneticAlgorithm ga(
                populationSize,
                generations,
                mutationRate,
                tournamentSize
            );

            Solution result = ga.run(problem);

            results[run] = result;
        }

        double endTime = omp_get_wtime();

        // Find the best solution after all runs finish
        Solution bestSolution;

        for (const Solution& result : results) {

            if (result.feasible &&
                (!bestSolution.feasible ||
                 result.totalDistance < bestSolution.totalDistance)) {

                bestSolution = result;
            }
        }

        cout << "\n========== PARALLEL RESULTS ==========\n";

        cout << fixed << setprecision(4);

        cout << "Number of independent runs: "
             << RUNS << "\n";

        cout << "Execution time: "
             << endTime - startTime
             << " seconds\n";

        if (bestSolution.feasible) {
            bestSolution.display(problem);
        } else {
            cout << "No feasible solution found.\n";
        }

    }
    catch (const exception& e) {

        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    auto endTime = chrono::high_resolution_clock::now();

    double executionTime =
    chrono::duration<double, milli>(endTime - startTime).count();

    cout << "\nAlgorithm Execution Time: "
     << executionTime << " ms" << endl;
    return 0;
}