#ifndef CVRP_H
#define CVRP_H

#include <vector>
#include <string>

using namespace std;

// Represents one location: depot or customer
struct Customer {
    int id;
    double x;
    double y;
    int demand;
};

class CVRP {

private:

    int numCustomers;
    int numVehicles;
    int vehicleCapacity;

    vector<Customer> customers;

    // Distance matrix between all locations
    vector<vector<double>> distanceMatrix;

public:

    // Constructor
    CVRP();

    // Read CVRP dataset
    bool loadFromFile(const string& filename);

    // Calculate distances between all locations
    void calculateDistanceMatrix();

    // Display problem information
    void displayProblem() const;

    // Display distance matrix
    void displayDistanceMatrix() const;

    // Getters
    int getNumCustomers() const;
    int getNumVehicles() const;
    int getVehicleCapacity() const;

    const vector<Customer>& getCustomers() const;

    double getDistance(int i, int j) const;

    // Check basic capacity feasibility
    bool isCapacityFeasible() const;
};

#endif
