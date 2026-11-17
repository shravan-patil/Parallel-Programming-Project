#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

#include "Solution.h"
#include "LocalSearch.h"

#include <vector>
#include <random>

using namespace std;

class GeneticAlgorithm {

private:

    int populationSize;
    int generations;
    double mutationRate;
    int tournamentSize;

    vector<Solution> population;

    mt19937 rng;

    // Create initial feasible population
    void initializePopulation(const CVRP& problem);

    // Select a parent using tournament selection
    Solution tournamentSelection();

    // Order Crossover
    Solution crossover(
        const Solution& parent1,
        const Solution& parent2,
        const CVRP& problem
    );

    // Mutate chromosome
    void mutate(Solution& solution);

public:

    GeneticAlgorithm(
        int populationSize = 40,
        int generations = 200,
        double mutationRate = 0.15,
        int tournamentSize = 3
    );

    // Run the genetic algorithm
    Solution run(const CVRP& problem);
};

#endif