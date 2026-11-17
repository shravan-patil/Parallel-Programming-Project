#include <iostream>
#include <vector>
#include <iomanip>
#include <exception>
#include <chrono>
#include "CVRP.h"
#include "GeneticAlgorithm.h"

using namespace std;

int main() {
    auto startTime = chrono::high_resolution_clock::now();

// Execute your algorithm
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

        Solution bestSolution;
        double totalTimePlaceholder = 0.0;

        cout << "\n========== SEQUENTIAL EXECUTION ==========\n";

        for (int run = 1; run <= RUNS; run++) {

            cout << "\nSequential Run " << run << "\n";

            GeneticAlgorithm ga(
                populationSize,
                generations,
                mutationRate,
                tournamentSize
            );

            Solution result = ga.run(problem);

            if (result.feasible &&
                (!bestSolution.feasible ||
                 result.totalDistance < bestSolution.totalDistance)) {

                bestSolution = result;
            }
        }

        cout << "\n========== SEQUENTIAL RESULTS ==========\n";

        cout << "Total independent runs: " << RUNS << "\n";

        if (bestSolution.feasible) {
            bestSolution.display(problem);
        } else {
            cout << "No feasible solution found.\n";
        }

        (void)totalTimePlaceholder;

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