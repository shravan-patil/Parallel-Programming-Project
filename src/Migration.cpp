#include "Migration.h"

using namespace std;


// Perform ring migration
void Migration::migrateRing(
    vector<Island>& islands,
    int migrantsPerIsland
) {

    int islandCount = static_cast<int>(islands.size());

    if (islandCount < 2 || migrantsPerIsland <= 0) {
        return;
    }

    // First collect migrants from every island.
    // This prevents earlier insertions from affecting
    // the migrants selected by later islands.
    vector<vector<Solution>> migrants(islandCount);

    for (int i = 0; i < islandCount; i++) {

        migrants[i] =
            islands[i].getBestIndividuals(migrantsPerIsland);
    }

    // Send each island's migrants to the next island
    for (int i = 0; i < islandCount; i++) {

        int destination = (i + 1) % islandCount;

        islands[destination].insertMigrants(migrants[i]);
    }
}