//
// Created by lactosis on 21.9.19.
//

#ifndef METRO_TILE_H
#define METRO_TILE_H

#include <map>
#include "Directions.h"

class Tile {
public:
    unsigned char connections[4];

    Tile(int north, int east, int south, int west);

    Tile(unsigned char id);

    std::string toString();

    /**
     * Get hash for tile with these connections
     */
    unsigned char getIdentifier();

    bool loops(const unsigned char direction);

    bool equals(Tile &pTile);
};


#endif //METRO_TILE_H
