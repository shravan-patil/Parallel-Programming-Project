#include <iostream>
#include <vector>
#include <iomanip>
#include <exception>
#include <algorithm>
#include <chrono>
#include <omp.h>

#include "CVRP.h"
#include "Island.h"
#include "Migration.h"

using namespace std;


int main() {
    auto startTime = chrono::high_resolution_clock::now();
    try {

        // Step 1: Load CVRP dataset
        CVRP problem;

        if (!problem.loadFromFile("data/medium.txt")) {

            cerr << "Failed to load dataset.\n";

            return 1;
        }

        problem.displayProblem();


        // Step 2: Configure Island Model

        const int ISLAND_COUNT = 4;

        const int POPULATION_SIZE = 40;

        const int GENERATIONS = 200;

        const int MIGRATION_INTERVAL = 20;

        const int MIGRANTS_PER_ISLAND = 2;

        const double BASE_MUTATION_RATE = 0.15;

        const int TOURNAMENT_SIZE = 3;

        const int STAGNATION_LIMIT = 20;


        // Step 3: Create independent islands

        vector<Island> islands;

        for (int i = 0; i < ISLAND_COUNT; i++) {

            unsigned int seed =
                1000 + static_cast<unsigned int>(i * 12345);

            islands.emplace_back(
                i + 1,
                POPULATION_SIZE,
                BASE_MUTATION_RATE,
                TOURNAMENT_SIZE,
                seed,
                problem
            );
        }


        // Step 4: Find initial global best

        Solution globalBest;

        for (const Island& island : islands) {

            Solution candidate = island.getBest();

            if (
                !globalBest.feasible ||
                candidate.totalDistance < globalBest.totalDistance
            ) {

                globalBest = candidate;
            }
        }


        cout << "\n========== PARALLEL ISLAND MODEL ==========\n";

        cout << "Number of islands: "
             << ISLAND_COUNT << "\n";

        cout << "Population per island: "
             << POPULATION_SIZE << "\n";

        cout << "Total population: "
             << ISLAND_COUNT * POPULATION_SIZE << "\n";

        cout << "Migration interval: "
             << MIGRATION_INTERVAL << " generations\n";

        cout << "Migrants per island: "
             << MIGRANTS_PER_ISLAND << "\n";


        double startTime = omp_get_wtime();


        // Step 5: Evolution loop

        int stagnationCounter = 0;

        double currentMutationRate = BASE_MUTATION_RATE;

        for (
            int generation = 1;
            generation <= GENERATIONS;
            generation++
        ) {

            // Each island evolves independently in parallel
            #pragma omp parallel for schedule(static)
            for (int i = 0; i < ISLAND_COUNT; i++) {

                islands[i].evolveOneGeneration(problem);
            }


            // Step 6: Check global best

            bool improved = false;

            for (const Island& island : islands) {

                Solution candidate = island.getBest();

                if (
                    candidate.feasible &&
                    candidate.totalDistance < globalBest.totalDistance
                ) {

                    globalBest = candidate;
                    improved = true;
                }
            }


            // Step 7: Adaptive mutation rate

            if (improved) {

                stagnationCounter = 0;

                currentMutationRate = BASE_MUTATION_RATE;

            } else {

                stagnationCounter++;
            }

            if (stagnationCounter >= STAGNATION_LIMIT) {

                currentMutationRate =
                    min(0.45, currentMutationRate + 0.05);

                stagnationCounter = 0;

                cout << "\nAdaptive mutation increased to: "
                     << currentMutationRate << "\n";
            }

            // Apply the mutation rate to all islands
            for (Island& island : islands) {

                island.setMutationRate(currentMutationRate);
            }


            // Step 8: Migration

            if (
                generation % MIGRATION_INTERVAL == 0 &&
                generation < GENERATIONS
            ) {

                Migration::migrateRing(
                    islands,
                    MIGRANTS_PER_ISLAND
                );

                // Migration may improve the global best
                for (const Island& island : islands) {

                    Solution candidate = island.getBest();

                    if (
                        candidate.feasible &&
                        candidate.totalDistance < globalBest.totalDistance
                    ) {

                        globalBest = candidate;
                    }
                }

                cout << "Migration completed at generation "
                     << generation << "\n";
            }


            // Step 9: Display progress

            if (generation % 10 == 0 || generation == 1) {

                cout << "Generation "
                     << generation
                     << " | Best Distance: "
                     << fixed << setprecision(2)
                     << globalBest.totalDistance
                     << " | Mutation Rate: "
                     << currentMutationRate
                     << "\n";
            }
        }


        double endTime = omp_get_wtime();


        // Step 10: Display final result

        cout << "\n========== FINAL RESULTS ==========\n";

        cout << fixed << setprecision(4);

        cout << "Execution time: "
             << endTime - startTime
             << " seconds\n";

        cout << "Islands: "
             << ISLAND_COUNT << "\n";

        cout << "Generations: "
             << GENERATIONS << "\n";

        cout << "Total individuals: "
             << ISLAND_COUNT * POPULATION_SIZE << "\n";

        globalBest.display(problem);


    }
    catch (const exception& e) {

        cerr << "\nError: "
             << e.what()
             << "\n";

        return 1;
    }
    auto endTime = chrono::high_resolution_clock::now();

double executionTime =
    chrono::duration<double, milli>(endTime - startTime).count();

cout << "\nAlgorithm Execution Time: "
     << executionTime << " ms" << endl;
    return 0;
}