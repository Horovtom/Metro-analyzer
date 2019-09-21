//
// Created by lactosis on 21.9.19.
//

#include "Tile.h"
#include <iostream>
#include <vector>
#include <set>

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

unsigned char Tile::getIdentifier() {
    unsigned char result = 0;
    result = connections[Directions::N] << 3;
    result += connections[Directions::E] << 1;
    result += (connections[Directions::S] > connections[Directions::W]);

    return result;
}

Tile::Tile(unsigned char id) {
    connections[Directions::N] = id >> 3;
    // Get rid of the highest bits
    id = id xor connections[Directions::N] << 3;

    connections[Directions::E] = id >> 1;
    unsigned char start, p;
    if ((id & 1) == 0) {
        start = 0;
        p = 1;
    } else {
        start = 3;
        p = -1;
    }
    unsigned char toFind = Directions::S;

    for (unsigned char i = start; i >= 0 && i < 4; i += p) {
        if (connections[Directions::N] != i && connections[Directions::E] != i) {
            connections[toFind] = i;
            toFind = Directions::W;
        }
    }

    unsigned char res =
            1 << connections[Directions::N] | 1 << connections[Directions::E] | 1 << connections[Directions::S] |
            1 << connections[Directions::W];
    if (res != 0b1111) {
        std::cerr << "Invalid tile definition! Not all directions were defined!" << std::endl;
        exit(12);
    }

}

bool Tile::loops(const unsigned char direction) {
    return connections[direction] == direction;
}
