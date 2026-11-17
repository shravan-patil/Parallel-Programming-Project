#ifndef ISLAND_H
#define ISLAND_H

#include "Solution.h"
#include "LocalSearch.h"

#include <vector>
#include <random>

class Island {

private:

    int islandId;
    int populationSize;
    int tournamentSize;

    double mutationRate;

    std::vector<Solution> population;

    std::mt19937_64 rng;

    void initializePopulation(const CVRP& problem);

    Solution tournamentSelection();

    Solution crossover(
        const Solution& parent1,
        const Solution& parent2,
        const CVRP& problem
    );

    void mutate(Solution& solution);

public:

    Island(
        int id,
        int populationSize,
        double mutationRate,
        int tournamentSize,
        unsigned int seed,
        const CVRP& problem
    );

    // Evolve the island by one generation
    void evolveOneGeneration(const CVRP& problem);

    // Return the best individual
    Solution getBest() const;

    // Return the best N individuals
    std::vector<Solution> getBestIndividuals(int count) const;

    // Accept incoming migrants
    void insertMigrants(const std::vector<Solution>& migrants);

    // Change mutation rate for adaptive experiment
    void setMutationRate(double rate);

    int getId() const;
};

#endif