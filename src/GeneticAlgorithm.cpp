#include "GeneticAlgorithm.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <stdexcept>

using namespace std;


GeneticAlgorithm::GeneticAlgorithm(
    int populationSize,
    int generations,
    double mutationRate,
    int tournamentSize
) {

    this->populationSize = populationSize;
    this->generations = generations;
    this->mutationRate = mutationRate;
    this->tournamentSize = tournamentSize;

    rng.seed(random_device{}());
}


// Create initial population
void GeneticAlgorithm::initializePopulation(
    const CVRP& problem
) {

    population.clear();

    int N = problem.getNumCustomers();

    vector<int> customerIDs(N);

    iota(customerIDs.begin(), customerIDs.end(), 1);

    int attempts = 0;
    int maximumAttempts = populationSize * 1000;

    while (
        static_cast<int>(population.size()) < populationSize &&
        attempts < maximumAttempts
    ) {

        Solution candidate;

        candidate.chromosome = customerIDs;

        shuffle(
            candidate.chromosome.begin(),
            candidate.chromosome.end(),
            rng
        );

        attempts++;

        if (candidate.decode(problem)) {
            population.push_back(candidate);
        }
    }

    if (static_cast<int>(population.size()) < populationSize) {

        throw runtime_error(
            "Could not generate enough feasible initial solutions. "
            "Try changing the dataset or initialization strategy."
        );
    }
}


// Tournament Selection
Solution GeneticAlgorithm::tournamentSelection() {

    uniform_int_distribution<int> distribution(
        0,
        static_cast<int>(population.size()) - 1
    );

    int bestIndex = distribution(rng);

    for (int i = 1; i < tournamentSize; i++) {

        int candidateIndex = distribution(rng);

        if (
            population[candidateIndex].totalDistance <
            population[bestIndex].totalDistance
        ) {

            bestIndex = candidateIndex;
        }
    }

    return population[bestIndex];
}


// Order Crossover (OX)
Solution GeneticAlgorithm::crossover(
    const Solution& parent1,
    const Solution& parent2,
    const CVRP& problem
) {

    int N = static_cast<int>(parent1.chromosome.size());

    Solution child;

    child.chromosome.assign(N, -1);

    if (N < 2) {

        child.chromosome = parent1.chromosome;
        child.decode(problem);

        return child;
    }

    uniform_int_distribution<int> positionDistribution(0, N - 1);

    int left = positionDistribution(rng);
    int right = positionDistribution(rng);

    if (left > right) {
        swap(left, right);
    }

    // Copy a section from parent 1
    for (int i = left; i <= right; i++) {

        child.chromosome[i] = parent1.chromosome[i];
    }

    // Fill remaining positions using parent 2
    int currentPosition = (right + 1) % N;

    for (int i = 0; i < N; i++) {

        int customer =
            parent2.chromosome[(right + 1 + i) % N];

        // Check whether customer is already present
        bool exists = false;

        for (int value : child.chromosome) {

            if (value == customer) {
                exists = true;
                break;
            }
        }

        if (!exists) {

            child.chromosome[currentPosition] = customer;

            currentPosition = (currentPosition + 1) % N;
        }
    }

    child.decode(problem);

    return child;
}


// Mutation
void GeneticAlgorithm::mutate(Solution& solution) {

    int N = static_cast<int>(solution.chromosome.size());

    if (N < 2) {
        return;
    }

    uniform_real_distribution<double> probability(0.0, 1.0);

    if (probability(rng) > mutationRate) {
        return;
    }

    uniform_int_distribution<int> positionDistribution(0, N - 1);

    int i = positionDistribution(rng);
    int j = positionDistribution(rng);

    while (i == j) {
        j = positionDistribution(rng);
    }

    if (i > j) {
        swap(i, j);
    }

    // Randomly choose swap or inversion mutation
    if (probability(rng) < 0.5) {

        // Swap mutation
        swap(
            solution.chromosome[i],
            solution.chromosome[j]
        );

    } else {

        // Inversion mutation
        reverse(
            solution.chromosome.begin() + i,
            solution.chromosome.begin() + j + 1
        );
    }
}


// Main Genetic Algorithm
Solution GeneticAlgorithm::run(const CVRP& problem) {

    initializePopulation(problem);

    LocalSearch localSearch;

    // Find initial best solution
    Solution globalBest = population[0];

    for (const Solution& solution : population) {

        if (solution.totalDistance < globalBest.totalDistance) {
            globalBest = solution;
        }
    }

    cout << "\nStarting Genetic Algorithm...\n";

    for (int generation = 1;
         generation <= generations;
         generation++) {

        vector<Solution> newPopulation;

        // Elitism: preserve the best solution
        newPopulation.push_back(globalBest);

        int attempts = 0;
        int maximumAttempts = populationSize * 100;

        while (
            static_cast<int>(newPopulation.size()) < populationSize &&
            attempts < maximumAttempts
        ) {

            attempts++;

            // 1. Select parents
            Solution parent1 = tournamentSelection();
            Solution parent2 = tournamentSelection();

            // 2. Crossover
            Solution child = crossover(
                parent1,
                parent2,
                problem
            );

            // 3. Mutation
            mutate(child);

            // 4. Decode mutated chromosome
            if (!child.decode(problem)) {
                continue;
            }

            // 5. Apply local search occasionally
            uniform_real_distribution<double> probability(0.0, 1.0);

            if (probability(rng) < 0.15) {

                localSearch.improve(
                    child,
                    problem,
                    30
                );
            }

            // Add only feasible children
            if (child.feasible) {
                newPopulation.push_back(child);
            }
        }

        // If not enough children were generated,
        // fill remaining positions with existing solutions.
        while (
            static_cast<int>(newPopulation.size()) < populationSize
        ) {

            newPopulation.push_back(globalBest);
        }

        population = newPopulation;

        // Update global best
        for (const Solution& solution : population) {

            if (solution.totalDistance < globalBest.totalDistance) {

                globalBest = solution;
            }
        }

        if (generation % 10 == 0 || generation == 1) {

            cout << "Generation "
                 << generation
                 << " | Best Distance: "
                 << globalBest.totalDistance
                 << endl;
        }
    }

    cout << "\nGenetic Algorithm completed.\n";

    return globalBest;
}