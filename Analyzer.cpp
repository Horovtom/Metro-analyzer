#include <random>

//
// Created by lactosis on 21.9.19.
//

#include "Analyzer.h"
#include <algorithm>
#include <fstream>
#include <zconf.h>
#include <a.out.h>

bool Analyzer::isValidPosition(int x, int y) {
    if (x < 0 || x > 7 || y < 0 || y > 7) return false;

    if (x == 3 || x == 4)
        if (y == 3 || y == 4)
            return false;

    return true;
}

bool Analyzer::genNextValidPermutation(std::vector<int> *tiles) {
    int posToCheck = -1;
    int valToCheck = -1;

    do {
        if (posToCheck != -1) {
            if (tiles->at(posToCheck) == valToCheck)
                continue;

            posToCheck = -1;
            valToCheck = -1;
        }

        if (populateGrid(tiles, &posToCheck, &valToCheck))
            return true;
        // TODO: We need some better permutation generator, which can skip large sections of regular permutations
    } while (std::next_permutation(tiles->begin(), tiles->end()));

    std::cout << "We are actually out of permutations!" << std::endl;
    return false;
}

int Analyzer::fillWithLongestPossibleRoute(int x, int y, int direction) {
    // We need to generate all the possible combinations of tiles. These are maximum of 60! which is too much...
    // We need to prune the state-space as much as possible...

    std::vector<int> tiles;
    tiles.reserve(60);
    for (int i = 0; i < 60; ++i) tiles.push_back(i);

    int longest = 0;

    while (genNextValidPermutation(&tiles)) {
        if (!isBoardValid())
            std::cerr << "We have actually gotten an invalid board!" << std::endl;
        int length = getLineLength(x, y, direction);
        if (length > longest) {
            std::cout << "New longest path: " << length << std::endl;
            longest = length;
        }
    }

    // We are done! We searched through everything!
    return longest;


//    int tried = 0;
//    do {
//        std::shuffle(tiles.begin(), tiles.end(), std::mt19937(std::random_device()()));
//
//        int currIn = 0;
//        for (int i = 0; i < 8; ++i) {
//            for (int j = 0; j < 8; ++j) {
//                if (isValidPosition(i, j))
//                    setTileAt(i, j, tiles.at(currIn++));
//            }
//        }
//        tried++;
//        if (tried % 100000 == 0) std::cout << "Evaluated: " << tried / 1000 << "k positions" << std::endl;
//    } while (!isBoardValid());
//
//    outputBoard("/home/lactosis/Documents/Programming/C++/Metro/output.txt");
//    std::cout << "Found valid board after: " << tried << " tries." << std::endl;
//    return getLineLength(x, y, direction);
}

bool Analyzer::isBoardValid() {
    Tile *t;
    // Check edges
    for (int i = 0; i < 8; ++i) {
        t = getTileAt(i, 0);
        if (t != nullptr && t->loops(Directions::N)) return false;
        t = getTileAt(i, 7);
        if (t != nullptr && t->loops(Directions::S)) return false;
        t = getTileAt(0, i);
        if (t != nullptr && t->loops(Directions::W)) return false;
        t = getTileAt(7, i);
        if (t != nullptr && t->loops(Directions::E)) return false;
    }

    // Check corners
    t = getTileAt(0, 0);
    if (t != nullptr &&
        (t->connections[Directions::W] == Directions::N || t->connections[Directions::N] == Directions::W))
        return false;
    t = getTileAt(7, 0);
    if (t != nullptr &&
        (t->connections[Directions::E] == Directions::N || t->connections[Directions::N] == Directions::E))
        return false;
    t = getTileAt(0, 7);
    if (t != nullptr &&
        (t->connections[Directions::W] == Directions::S || t->connections[Directions::S] == Directions::W))
        return false;
    t = getTileAt(7, 7);
    if (t != nullptr &&
        (t->connections[Directions::E] == Directions::S || t->connections[Directions::S] == Directions::E))
        return false;

    // Check middle
    t = getTileAt(2, 3);
    if (t != nullptr && t->loops(Directions::E)) return false;
    t = getTileAt(2, 4);
    if (t != nullptr && t->loops(Directions::E)) return false;

    t = getTileAt(3, 2);
    if (t != nullptr && t->loops(Directions::S)) return false;
    t = getTileAt(4, 2);
    if (t != nullptr && t->loops(Directions::S)) return false;

    t = getTileAt(5, 3);
    if (t != nullptr && t->loops(Directions::W)) return false;
    t = getTileAt(5, 4);
    if (t != nullptr && t->loops(Directions::W)) return false;

    t = getTileAt(3, 5);
    if (t != nullptr && (t->loops(Directions::N))) return false;
    t = getTileAt(4, 5);
    return !(t != nullptr && t->loops(Directions::N));
}


Analyzer::Analyzer(const std::vector<Tile> &tiles) {
    if (tiles.size() != 60) {
        std::cerr << "Incorrect number of tiles specified in the input.txt file: " << tiles.size() << std::endl;
        exit(9);
    }

    tileRepository = tiles;
}

Tile *Analyzer::getTileAt(int x, int y) {
    if (!isValidPosition(x, y)) return nullptr;
    if (board[y][x] == 0) return nullptr;
    return &tileRepository.at(board[y][x] - 1);
}

void Analyzer::setTileAt(int x, int y, int tileIndex) {
    board[y][x] = tileIndex + 1;
}

void Analyzer::resetTileAt(int x, int y) {
    board[y][x] = 0;
}

int Analyzer::getLineLength(int x, int y, int direction) {

    Tile *currTile = getTileAt(x, y);
    std::pair<int, int> currPos(x, y);
    unsigned char cameFromDir = direction;

    int length = 0;
    while (true) {
        if (currTile == nullptr) return length;
        unsigned char targetDir = currTile->connections[cameFromDir];
        currPos = getCoordsAtDirection(currPos, targetDir);
        currTile = getTileAt(currPos.first, currPos.second);
        cameFromDir = Directions::flip(targetDir);
        length++;
    }

    return length;
}

std::pair<int, int> Analyzer::getCoordsAtDirection(std::pair<int, int> pos, int direction) {
    switch (direction) { // NOLINT(hicpp-multiway-paths-covered)
        case Directions::N:
            return std::pair<int, int>(pos.first, pos.second - 1);
        case Directions::E:
            return std::pair<int, int>(pos.first + 1, pos.second);
        case Directions::S:
            return std::pair<int, int>(pos.first, pos.second + 1);
        case Directions::W:
            return std::pair<int, int>(pos.first - 1, pos.second);
    }
    return std::pair<int, int>();
}

void Analyzer::outputBoard(const std::string &path) {
    std::ofstream oss(path);
    if (oss.is_open()) {
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                Tile *t = getTileAt(i, j);
                if (t != nullptr)
                    oss << (int) getTileAt(i, j)->getIdentifier() << " ";
                else
                    oss << "- ";
            }
            oss << std::endl;
        }

        oss.close();
    } else {
        std::cerr << "Could not open output file at: " << path << std::endl;
        exit(100);
    }
}

void Analyzer::showCurrentBoard() {
    execl("/home/lactosis/Documents/Programming/C++/Metro/visualiser/bin/visualiser", nullptr);
}

bool Analyzer::isValidTileOnPos(int x, int y, int tileIndex) {
    Tile *t;
    t = &tileRepository.at(tileIndex);

    // Left edge
    if (x == 0 && t->loops(Directions::W)) return false;
    // Top edge
    if (y == 0 && t->loops(Directions::N)) return false;
    // Right edge
    if (x == 7 && t->loops(Directions::E)) return false;
    // Bottom edge
    if (y == 7 && t->loops(Directions::S)) return false;

    if (x == 2 && (y == 3 || y == 4) && t->loops(Directions::E)) return false;
    if (x == 5 && (y == 3 || y == 4) && t->loops(Directions::W)) return false;
    if (y == 2 && (x == 3 || x == 4) && t->loops(Directions::S)) return false;
    if (y == 5 && (x == 3 || x == 4) && t->loops(Directions::N)) return false;

    if (x == 0 && y == 0 &&
        (t->connections[Directions::N] == Directions::W || t->connections[Directions::W] == Directions::N))
        return false;
    if (x == 7 && y == 0 &&
        (t->connections[Directions::N] == Directions::E || t->connections[Directions::E] == Directions::N))
        return false;
    if (x == 0 && y == 7 &&
        (t->connections[Directions::S] == Directions::W || t->connections[Directions::W] == Directions::S))
        return false;
    if (x == 7 && y == 7 &&
        (t->connections[Directions::S] == Directions::E || t->connections[Directions::E] == Directions::S))
        return false;

    return true;
}

bool Analyzer::populateGrid(std::vector<int> *tiles, int *posToCheck, int *valToCheck) {
    int index = 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (!isValidTileOnPos(i, j, tiles->at(index))) {
                *posToCheck = index;
                *valToCheck = tiles->at(index);
                return false;
            }

            setTileAt(i, j, tiles->at(index++));
        }
    }
    return true;
}



