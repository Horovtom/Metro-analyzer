//
// Created by lactosis on 21.9.19.
//

#include "Tile.h"
#include <iostream>

Tile::Tile(int north, int east, int south, int west) {
    unsigned char res = 1 << north | 1 << east | 1 << south | 1 << west;
    if (res != 0b1111) {
        std::cerr << "Invalid tile definition! Not all directions were defined!" << std::endl;
        exit(3);
    }

    connections[Directions::N] = north;
    connections[Directions::S] = south;
    connections[Directions::W] = west;
    connections[Directions::E] = east;
}

std::string Tile::toString() {
    std::string result = "N E S W";
    result.insert(7, 1, Directions::toChar(connections[Directions::W]));
    result.insert(5, 1, Directions::toChar(connections[Directions::S]));
    result.insert(3, 1, Directions::toChar(connections[Directions::E]));
    result.insert(1, 1, Directions::toChar(connections[Directions::N]));
    return result;
}
