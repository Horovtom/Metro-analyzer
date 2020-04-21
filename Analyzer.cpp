#include <random>

#include <random>

//
// Created by lactosis on 21.9.19.
//

#include "Analyzer.h"
#include <algorithm>
#include <fstream>
#include <thread>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <zconf.h>
#else

#include <unistd.h>

#endif

#include <stdlib.h>

bool Analyzer::isPlaceablePosition(int x, int y) {
    if (x < 0 || x > 7 || y < 0 || y > 7) return false;

    if (x == 3 || x == 4)
        if (y == 3 || y == 4)
            return false;

    return true;
}


void Analyzer::csp(int x, int y, int direction, std::vector<std::pair<int, int>> *tiles, int lenSoFar, int depth) {
    // Is it the end of the line?
    if (!isPlaceablePosition(x, y)) {
        // It is the actual end of line, lets not do anything.
        std::cerr << "End of line at the start?!" << std::endl;
        return;
    }


    for (auto &tile : *tiles) {
        if (tile.second <= 0) continue;
        if (!isValidTileOnPos(x, y, tile.first)) continue;

        tile.second--;
        setTileAt(x, y, tile.first);

        std::tuple<int, int, int, int> finishPosition = getLineLastPosition(x, y, direction);
        int tarX = std::get<0>(finishPosition);
        int tarY = std::get<1>(finishPosition);
        int tarDirection = std::get<2>(finishPosition);
        int length = std::get<3>(finishPosition) + lenSoFar;
        if (!isPlaceablePosition(tarX, tarY)) {
            // We actually ended the line...
            // Check if it is longer than the current max found.
            if (++cycle % 1000000000 == 0)
                std::cout << "Explored " << cycle / 1000000 << "M possibilities" << std::endl;
            if (longestPathLength < length) {
                std::lock_guard<std::mutex> guard(mut);
                if (longestPathLength < length) {
                    // Overwrite
                    for (int i = 0; i < 8; ++i) {
                        for (int j = 0; j < 8; ++j) {
                            longestBoard[i][j] = board[i][j];
                        }
                    }
                    longestPathLength = length;
                    std::cout << "New length: " << length << std::endl;
                    if (length > 105)
                        showCurrentBoard();
                }
            }
        } else {
            csp(tarX, tarY, tarDirection, tiles, length, depth + 1);
        }

        deleteTileAt(x, y);
        tile.second++;
    }
}


void Analyzer::cspStart(int x, int y, int direction) {

//    std::vector<int[8][8]> grids;
//    std::vector<std::thread> threads;
//    std::vector<std::pair<int, int>> freeToUse = getTileCounts();
//
//    grids.resize(freeToUse.size() + 1);
//    int longest = 0;
//
//    for (int i = 0 ; i < freeToUse.size(); ++i ) {
//        std::vector<std::pair<int, int>> ftu = freeToUse;
//        resetBoard(grids.at(i));
//        std::thread th(cspStatic, x, y, direction, ftu, 0, 1, grids.at(i), &grids.at(grids.size() -1 ), &longest);
//    }

};

std::vector<std::pair<int, int>> Analyzer::getTileCounts() {
    std::vector<std::pair<int, int>> res;

    for (int tileIndex = 0; tileIndex < tileRepository.size(); ++tileIndex) {
        bool found = false;
        for (auto &re : res) {
            if (tileRepository.at(tileIndex).equals(tileRepository.at(re.first))) {
                re.second++;
                found = true;
                break;
            }
        }

        if (!found)
            res.emplace_back(tileIndex, 1);
    }

    return res;
}


int Analyzer::fillWithLongestPossibleRoute(int x, int y, int direction) {
    // Ok, permutations are not going to cut it...
    // Let's do some form of CSP

    resetBoard();

    std::vector<std::pair<int, int>> freeToUse = getTileCounts();

//    cspStart(x,y,direction);

    csp(x, y, direction, &freeToUse, 0, 1);

    return longestPathLength;
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
    if (!isPlaceablePosition(x, y)) return nullptr;
    if (board[y][x] == 0) return nullptr;
    return &tileRepository.at(board[y][x] - 1);
}

void Analyzer::setTileAt(int x, int y, int tileIndex) {
    board[y][x] = tileIndex + 1;
}

void Analyzer::setTileAt(int x, int y, int tileIndex, int board[8][8]) {
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
    oss << "0 0 3" << std::endl;
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
    outputBoard(Project::getProjectRootFilePath("/output.txt"));

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    std::string pathOld = Project::getProjectRootFilePath("/visualiser/bin/win/visualiser.exe");
    const char *path = pathOld.c_str();
    int result = system(path);
    std::cout << "Visualiser return code: " << result << ", launched on path: " << path << std::endl;
#else
    system(Project::getProjectRootFilePath("/visualiser/bin/linux/visualiser").c_str());
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
            if (!isPlaceablePosition(i, j)) continue;
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

void Analyzer::overwriteBoard(std::vector<int> *tiles) {
    int index = 0;
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j) {
            if (isPlaceablePosition(i, j))
                setTileAt(i, j, tiles->at(index++));
        }
}

void Analyzer::resetBoard() {
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            deleteTileAt(i, j);
}

void Analyzer::resetBoard(int board[8][8]) {
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            deleteTileAt(i, j, board);
}

void Analyzer::deleteTileAt(int i, int j) {
    setTileAt(i, j, -1);
}

void Analyzer::deleteTileAt(int i, int j, int board[8][8]) {
    setTileAt(i, j, -1, board);
}

std::tuple<int, int, int, int> Analyzer::getLineLastPosition(int x, int y, int direction) {
    Tile *currTile = getTileAt(x, y);
    std::pair<int, int> currPos(x, y);
    unsigned char cameFromDir = direction;
    int length = 0;
    int largestTileIndex = coordToIndex(x, y);
    while (true) {
        if (currTile == nullptr) {
            // We went out of the known map (maybe there are no tiles placed there, or even we came out to some station)
            return std::tuple(currPos.first, currPos.second, cameFromDir, length);
        }
        unsigned char targetDir = currTile->connections[cameFromDir];
        currPos = getCoordsAtDirection(currPos, targetDir);
        int currIndex = coordToIndex(currPos.first, currPos.second);
        if (currIndex > largestTileIndex)
            largestTileIndex = currIndex;

        currTile = getTileAt(currPos.first, currPos.second);
        cameFromDir = Directions::flip(targetDir);
        if (currPos.first == x && currPos.second == y && cameFromDir == direction) {
            std::cerr << "We encountered a cycle!" << std::endl;
            return std::tuple(x, y, direction, -1);
        }
        length++;
    }
}





