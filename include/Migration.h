#ifndef MIGRATION_H
#define MIGRATION_H

#include "Island.h"

#include <vector>

class Migration {

public:

    // Ring topology migration
    static void migrateRing(
        std::vector<Island>& islands,
        int migrantsPerIsland
    );
};

#endif