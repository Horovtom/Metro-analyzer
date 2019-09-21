#include <random>

//
// Created by lactosis on 21.9.19.
//

#include "Analyzer.h"
#include <algorithm>

bool Analyzer::isValidPosition(int x, int y) {
    if (x < 0 || x > 7 || y < 0 || y > 7) return false;

    if (x == 4 || x == 5)
        if (y == 4 || y == 5)
            return false;

    return true;
}

int Analyzer::fillWithLongestPossibleRoute(int x, int y, int direction) {
    // We need to generate all the possible combinations of tiles. These are maximum of 60! which is too much...
    // We need to prune the state-space as much as possible...

    // TODO: Temp
    std::vector<int> tiles;
    tiles.reserve(60);
    for (int i = 0; i < 60; ++i) tiles.push_back(i);

    std::shuffle(tiles.begin(), tiles.end(), std::mt19937(std::random_device()()));

    int currIn = 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (isValidPosition(i, j))
                board[i][j] = tiles.at(currIn++) + 1;
        }
    }

    if (!isBoardValid()) {
        std::cerr << "Board is not valid!" << std::endl;
    }

    return getLineLength(x, y, direction);
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



