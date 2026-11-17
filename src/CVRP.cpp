
#include "CVRP.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <stdexcept>
#include <iomanip>

using namespace std;

// Constructor
CVRP::CVRP() {

    numCustomers = 0;
    numVehicles = 0;
    vehicleCapacity = 0;

}

// Read dataset from file
bool CVRP::loadFromFile(const string& filename) {

    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Cannot open dataset file: "
             << filename << endl;

        return false;
    }

    // Read problem parameters
    file >> numCustomers
         >> numVehicles
         >> vehicleCapacity;

    if (!file ||
        numCustomers <= 0 ||
        numVehicles <= 0 ||
        vehicleCapacity <= 0) {

        cerr << "Error: Invalid problem parameters."
             << endl;

        return false;
    }

    customers.clear();

    // Read depot and customer information
    for (int i = 0; i <= numCustomers; i++) {

        Customer c;

        file >> c.id
             >> c.x
             >> c.y
             >> c.demand;

        if (!file) {
            cerr << "Error: Invalid customer data."
                 << endl;

            return false;
        }

        // IDs must be 0, 1, 2, ... N
        if (c.id != i) {
            cerr << "Error: Customer IDs must be "
                 << "sequential starting from 0."
                 << endl;

            return false;
        }

        // Depot must have zero demand
        if (i == 0 && c.demand != 0) {
            cerr << "Error: Depot demand must be zero."
                 << endl;

            return false;
        }

        // Customer demand must be positive
        if (i > 0 && c.demand <= 0) {
            cerr << "Error: Customer demand must "
                 << "be positive." << endl;

            return false;
        }

        // Individual customer demand cannot exceed
        // vehicle capacity
        if (i > 0 && c.demand > vehicleCapacity) {
            cerr << "Error: Customer " << i
                 << " exceeds vehicle capacity."
                 << endl;

            return false;
        }

        customers.push_back(c);
    }

    file.close();

    // Check basic capacity feasibility
    if (!isCapacityFeasible()) {

        cerr << "Error: Total demand exceeds "
             << "total fleet capacity." << endl;

        return false;
    }

    // Calculate all pairwise distances
    calculateDistanceMatrix();

    return true;
}


// Calculate Euclidean distance
void CVRP::calculateDistanceMatrix() {

    int totalLocations = numCustomers + 1;

    distanceMatrix.assign(
        totalLocations,
        vector<double>(totalLocations, 0.0)
    );

    for (int i = 0; i < totalLocations; i++) {

        for (int j = i + 1;
             j < totalLocations;
             j++) {

            double dx = customers[i].x
                      - customers[j].x;

            double dy = customers[i].y
                      - customers[j].y;

            double distance = sqrt(
                dx * dx + dy * dy
            );

            // Matrix is symmetric
            distanceMatrix[i][j] = distance;
            distanceMatrix[j][i] = distance;
        }
    }
}


// Display problem information
void CVRP::displayProblem() const {

    cout << "\n========== CVRP PROBLEM =========="
         << endl;

    cout << "Number of Customers: "
         << numCustomers << endl;

    cout << "Number of Vehicles: "
         << numVehicles << endl;

    cout << "Vehicle Capacity: "
         << vehicleCapacity << endl;

    cout << "\nLocation Information:\n";

    cout << left
         << setw(10) << "ID"
         << setw(15) << "X"
         << setw(15) << "Y"
         << setw(15) << "Demand"
         << endl;

    for (const Customer& c : customers) {

        cout << left
             << setw(10) << c.id
             << setw(15) << c.x
             << setw(15) << c.y
             << setw(15) << c.demand
             << endl;
    }

    cout << "=================================="
         << endl;
}


// Display distance matrix
void CVRP::displayDistanceMatrix() const {

    cout << "\n========== DISTANCE MATRIX =========="
         << endl;

    cout << fixed << setprecision(2);

    for (const auto& row : distanceMatrix) {

        for (double distance : row) {

            cout << setw(9) << distance;
        }

        cout << endl;
    }

    cout << "====================================="
         << endl;
}


// Getters
int CVRP::getNumCustomers() const {
    return numCustomers;
}

int CVRP::getNumVehicles() const {
    return numVehicles;
}

int CVRP::getVehicleCapacity() const {
    return vehicleCapacity;
}

const vector<Customer>& CVRP::getCustomers() const {
    return customers;
}


// Get distance between two locations
double CVRP::getDistance(int i, int j) const {

    if (i < 0 || j < 0 ||
        i >= static_cast<int>(distanceMatrix.size()) ||
        j >= static_cast<int>(distanceMatrix.size())) {

        throw out_of_range("Invalid location index.");
    }

    return distanceMatrix[i][j];
}


// Check total fleet capacity
bool CVRP::isCapacityFeasible() const {

    long long totalDemand = 0;

    for (int i = 1; i <= numCustomers; i++) {

        totalDemand += customers[i].demand;
    }

    long long totalCapacity =
        1LL * numVehicles * vehicleCapacity;

    return totalDemand <= totalCapacity;
}