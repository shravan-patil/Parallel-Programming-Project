#include "Island.h"

#include <algorithm>
#include <numeric>
#include <stdexcept>

using namespace std;


Island::Island(
    int id,
    int populationSize,
    double mutationRate,
    int tournamentSize,
    unsigned int seed,
    const CVRP& problem
) {

    islandId = id;
    this->populationSize = populationSize;
    this->mutationRate = mutationRate;
    this->tournamentSize = tournamentSize;

    rng.seed(seed);

    if (populationSize < 2 || tournamentSize < 1) {
        throw invalid_argument("Invalid island configuration.");
    }

    initializePopulation(problem);
}


// Generate an initial feasible population
void Island::initializePopulation(const CVRP& problem) {

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
            "Island " + to_string(islandId) +
            " could not generate enough feasible solutions."
        );
    }
}


// Tournament selection
Solution Island::tournamentSelection() {

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


// Order Crossover
Solution Island::crossover(
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

    vector<bool> used(N + 1, false);

    // Copy a section from parent 1
    for (int i = left; i <= right; i++) {

        int customer = parent1.chromosome[i];

        child.chromosome[i] = customer;
        used[customer] = true;
    }

    // Fill remaining positions using parent 2
    int currentPosition = (right + 1) % N;

    for (int i = 0; i < N; i++) {

        int customer =
            parent2.chromosome[(right + 1 + i) % N];

        if (!used[customer]) {

            child.chromosome[currentPosition] = customer;

            used[customer] = true;

            currentPosition = (currentPosition + 1) % N;
        }
    }

    child.decode(problem);

    return child;
}


// Swap or inversion mutation
void Island::mutate(Solution& solution) {

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

    if (probability(rng) < 0.5) {

        swap(
            solution.chromosome[i],
            solution.chromosome[j]
        );

    } else {

        reverse(
            solution.chromosome.begin() + i,
            solution.chromosome.begin() + j + 1
        );
    }
}


// Execute one generation
void Island::evolveOneGeneration(const CVRP& problem) {

    vector<Solution> newPopulation;

    newPopulation.reserve(populationSize);

    // Elitism: preserve the current best
    Solution elite = getBest();

    newPopulation.push_back(elite);

    LocalSearch localSearch;

    int attempts = 0;
    int maximumAttempts = populationSize * 100;

    while (
        static_cast<int>(newPopulation.size()) < populationSize &&
        attempts < maximumAttempts
    ) {

        attempts++;

        Solution parent1 = tournamentSelection();
        Solution parent2 = tournamentSelection();

        Solution child = crossover(parent1, parent2, problem);

        mutate(child);

        if (!child.decode(problem)) {
            continue;
        }

        uniform_real_distribution<double> probability(0.0, 1.0);

        if (probability(rng) < 0.15) {

            localSearch.improve(
                child,
                problem,
                30
            );
        }

        if (child.feasible) {
            newPopulation.push_back(child);
        }
    }

    // Fill any remaining positions with existing individuals
    while (
        static_cast<int>(newPopulation.size()) < populationSize
    ) {

        newPopulation.push_back(elite);
    }

    population = std::move(newPopulation);
}


// Return best solution
Solution Island::getBest() const {

    if (population.empty()) {
        return Solution();
    }

    int bestIndex = 0;

    for (int i = 1; i < static_cast<int>(population.size()); i++) {

        if (
            population[i].totalDistance <
            population[bestIndex].totalDistance
        ) {
            bestIndex = i;
        }
    }

    return population[bestIndex];
}


// Return best N solutions
vector<Solution> Island::getBestIndividuals(int count) const {

    vector<Solution> sortedPopulation = population;

    sort(
        sortedPopulation.begin(),
        sortedPopulation.end(),
        [](const Solution& a, const Solution& b) {
            return a.totalDistance < b.totalDistance;
        }
    );

    if (count < 0) {
        count = 0;
    }

    if (count > static_cast<int>(sortedPopulation.size())) {
        count = static_cast<int>(sortedPopulation.size());
    }

    sortedPopulation.resize(count);

    return sortedPopulation;
}


// Insert migrants by replacing worse individuals
void Island::insertMigrants(const vector<Solution>& migrants) {

    if (population.empty()) {
        return;
    }

    vector<Solution> validMigrants;

    for (const Solution& migrant : migrants) {

        if (migrant.feasible) {
            validMigrants.push_back(migrant);
        }
    }

    sort(
        validMigrants.begin(),
        validMigrants.end(),
        [](const Solution& a, const Solution& b) {
            return a.totalDistance < b.totalDistance;
        }
    );

    for (const Solution& migrant : validMigrants) {

        int worstIndex = 0;

        for (
            int i = 1;
            i < static_cast<int>(population.size());
            i++
        ) {

            if (
                population[i].totalDistance >
                population[worstIndex].totalDistance
            ) {
                worstIndex = i;
            }
        }

        if (
            migrant.totalDistance <
            population[worstIndex].totalDistance
        ) {

            population[worstIndex] = migrant;
        }
    }
}


// Set adaptive mutation rate
void Island::setMutationRate(double rate) {

    if (rate < 0.0) {
        rate = 0.0;
    }

    if (rate > 1.0) {
        rate = 1.0;
    }

    mutationRate = rate;
}


int Island::getId() const {
    return islandId;
}