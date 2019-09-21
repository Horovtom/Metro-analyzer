//
// Created by lactosis on 21.9.19.
//

#ifndef METRO_TILE_H
#define METRO_TILE_H

#include <map>
#include "Directions.h"

class Tile {
public:
    int connections[4];

    Tile(int north, int east, int south, int west);

    std::string toString();
};


#endif //METRO_TILE_H
