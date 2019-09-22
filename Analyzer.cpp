#include <random>

#include <random>

//
// Created by lactosis on 21.9.19.
//

#include "Analyzer.h"
#include <algorithm>
#include <fstream>
#include <zconf.h>
#include <stdlib.h>

bool Analyzer::isValidPosition(int x, int y) {
    if (x < 0 || x > 7 || y < 0 || y > 7) return false;

    if (x == 3 || x == 4)
        if (y == 3 || y == 4)
            return false;

    return true;
}

bool Analyzer::genNextValidPermutation(std::vector<int> *tiles) {
    int posToCheck = -1;
    if (currToChange != -1) {
        if (!nextPermTileAt(tiles, currToChange))
            return false;
        currToChange = -1;
        if (populateGrid(tiles, &posToCheck))
            return true;
        return genNextValidPermutation(tiles);
    }

    int last20[20];
    int s = tiles->size();
    for (int i = 0; i < 20; ++i) {
        last20[i] = tileRepository.at(tiles->at(s - i - 1)).getIdentifier();
    }

    int identical = 0;
    while (true) {
        if (!std::next_permutation(tiles->begin(), tiles->end()))
            return false;
        if (!checkLast20Same(tiles, last20)) break;
        identical++;
    }
//    std::cout << "\tSkipped " << identical << " identical permutaitons" << std::endl;


    int tries = 0;
    while (true) {
        tries++;
        if (populateGrid(tiles, &posToCheck)) {
//            std::cout << "Tries: " << tries << std::endl;
            return true;
        }

        if (posToCheck != -1) {
            if (!nextPermTileAt(tiles, posToCheck))
                return false;
            posToCheck = -1;
        }
    }
}

int Analyzer::fillWithLongestPossibleRoute(int x, int y, int direction) {
    // We need to generate all the possible combinations of tiles. These are maximum of 60! which is too much...
    // We need to prune the state-space as much as possible...

    std::vector<int> tiles;
    tiles.reserve(60);
    for (int i = 0; i < 60; ++i) tiles.push_back(i);

    int longest = 0;

    while (genNextValidPermutation(&tiles)) {

//        if (!isBoardValid())
//            std::cerr << "We have actually gotten an invalid board!" << std::endl;
        int length = getLineLength(x, y, direction);
        currToChange = getLastOfLine(x, y, direction);
        if (length > longest) {
            outputBoard(Project::getProjectRootFilePath("/output.txt"));
            std::cout << "New longest path: " << length << std::endl;
            longest = length;
        }
    }

    // We are done! We searched through everything!
    return longest;
}

bool Analyzer::getNextPermutation(std::vector<int> *input, int pivot) {
    int largerIndex = -1;
    for (int i = pivot + 1; i < input->size(); ++i) {
        if ((*input)[i] > (*input)[pivot]) {
            largerIndex = i;
            break;
        }
    }

    if (largerIndex != -1) {
        std::swap((*input)[pivot], (*input)[largerIndex]);
        // sort subarray from p+1
        std::sort(input->begin() + pivot + 1, input->end());
    } else {

        // Did not find it! We have to swap for larger
        largerIndex = pivot;
        for (int i = pivot; i < input->size(); ++i) {
            if (input->at(i) < input->at(largerIndex) && input->at(i) > input->at(pivot - 1))
                largerIndex = i;
        }
        if (input->at(largerIndex) < input->at(pivot - 1))
            return std::next_permutation(input->begin(), input->end());
        std::swap((*input)[pivot - 1], (*input)[largerIndex]);
        std::sort(input->begin() + pivot, input->end());
    }

    return true;
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
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    std::string pathOld = Project::getProjectRootFilePath("/visualiser/bin/win/visualiser.exe");
    const char *path = pathOld.c_str();
    int result = system(path);
    std::cout << "Visualiser return code: " << result << ", launched on path: " << path << std::endl;
#else
    execl(getProjectRootFilePath("/visualiser/bin/linux/visualiser"), nullptr);
#endif
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

bool Analyzer::populateGrid(std::vector<int> *tiles, int *posToCheck) {
    int index = 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (!isValidPosition(i, j)) continue;
            if (!isValidTileOnPos(i, j, tiles->at(index))) {
                *posToCheck = index;
                return false;
            }

            setTileAt(i, j, tiles->at(index++));
        }
    }
    return true;
}

bool Analyzer::isValidTileOnPositionByID(int x, int y, int tileID) {
    for (int i = 0; i < tileRepository.size(); ++i) {
        if (tileRepository.at(i).getIdentifier() == tileID)
            return isValidTileOnPos(x, y, i);
    }
    return false;
}

bool Analyzer::checkLast20Same(std::vector<int> *tiles, const int *last20) {
    int s = tiles->size();
    for (int i = 0; i < 20; ++i) {
        if (tileRepository.at(tiles->at(s - i - 1)).getIdentifier() != last20[i])
            return false;
    }
    return true;
}

bool Analyzer::nextPermTileAt(std::vector<int> *tiles, int posToChange) {
    int oldID = tileRepository.at(tiles->at(posToChange)).getIdentifier();
    while (true) {
        if (!getNextPermutation(tiles, posToChange))
            return false;
        if (tileRepository.at(tiles->at(posToChange)).getIdentifier() != oldID)
            break;

    }
    return true;
}

int Analyzer::getLastOfLine(int x, int y, int direction) {
    Tile *currTile = getTileAt(x, y);
    std::pair<int, int> currPos(x, y);
    unsigned char cameFromDir = direction;

    int largestTileIndex = coordToIndex(x, y);
    while (true) {
        if (currTile == nullptr) return largestTileIndex;
        unsigned char targetDir = currTile->connections[cameFromDir];
        currPos = getCoordsAtDirection(currPos, targetDir);
        int currIndex = coordToIndex(currPos.first, currPos.second);
        if (currIndex > largestTileIndex)
            largestTileIndex = currIndex;

        currTile = getTileAt(currPos.first, currPos.second);
        cameFromDir = Directions::flip(targetDir);
    }
}

int Analyzer::coordToIndex(int x, int y) {
    int index = y * 8 + x;
    if (y > 3)
        index -= 2;
    else if (y == 3)
        if (x > 2) index -= 2;

    if (y > 4)
        index -= 2;
    else if (y == 4)
        if (x > 2) index -= 2;

    return index;
}

void Analyzer::outputWriteBoard(std::vector<int> *tiles, const std::string &path) {
    int index = 0;
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j) {
            if (isValidPosition(i, j))
                setTileAt(i, j, tiles->at(index++));
        }

    outputBoard(path);
}



