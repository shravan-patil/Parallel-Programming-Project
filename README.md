# Parallel Vehicle Routing Problem (CVRP)

## 1. Project Overview

This project implements the Capacitated Vehicle Routing Problem (CVRP)
using a Hybrid Genetic Algorithm.

The objective is to minimize the total distance travelled by a fleet
of vehicles while satisfying customer demands and vehicle capacity
constraints.

## 2. Algorithms Used

- Genetic Algorithm
- Dynamic Programming Split
- Tournament Selection
- Order Crossover (OX)
- Swap Mutation
- Inversion Mutation
- Local Search
- 2-opt Improvement
- Elitism

## 3. Parallelization

The project uses OpenMP for parallel execution.

The current parallel experiment runs multiple independent Genetic
Algorithm executions concurrently.

A future extension will implement the Island Model, where independent
populations exchange good solutions periodically.

## 4. Technologies

- C++
- OpenMP
- STL
- Dynamic Programming
- Genetic Algorithms

## 5. Project Structure

Parallel_CVRP_Project/

    data/
        small.txt
        medium.txt
        large.txt

    include/
        CVRP.h
        Solution.h
        GeneticAlgorithm.h
        LocalSearch.h

    src/
        main.cpp
        CVRP.cpp
        Solution.cpp
        GeneticAlgorithm.cpp
        LocalSearch.cpp

    build/
    results/

    sequential.cpp
    parallel.cpp
    Makefile
    README.md

## 6. Dataset Format

The first line contains:

    NumberOfCustomers NumberOfVehicles VehicleCapacity

The following lines contain:

    CustomerID XCoordinate YCoordinate Demand

Customer ID 0 represents the depot.

## 7. Compilation

Compile all current versions:

    make all

Compile only the sequential version:

    make sequential

Compile only the parallel version:

    make parallel

## 8. Execution

Run the main GA:

    ./build/cvrp.exe

Run sequential experiments:

    ./build/sequential.exe

Run parallel experiments:

    ./build/parallel.exe

## 9. Clean Build

    make clean