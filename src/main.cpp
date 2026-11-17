#include <iostream>
#include <exception>

#include "CVRP.h"
#include "GeneticAlgorithm.h"

using namespace std;

int main() {

    try {

        // Step 1: Load CVRP dataset
        CVRP problem;

        if (!problem.loadFromFile("data/medium.txt")) {

            cout << "Failed to load CVRP dataset."
                 << endl;

            return 1;
        }

        problem.displayProblem();

        // Step 2: Configure Genetic Algorithm
        int populationSize = 40;
        int generations = 200;

        double mutationRate = 0.15;
        int tournamentSize = 3;

        GeneticAlgorithm ga(
            populationSize,
            generations,
            mutationRate,
            tournamentSize
        );

        // Step 3: Run Genetic Algorithm
        Solution bestSolution = ga.run(problem);

        // Step 4: Display best solution
        bestSolution.display(problem);

    }
    catch (const exception& e) {

        cerr << "\nError: "
             << e.what()
             << endl;

        return 1;
    }

    return 0;
}