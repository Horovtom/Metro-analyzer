//
// Created by lactosis on 22.4.20.
//

#include <iostream>
#include "Game.h"
#include "Solver.h"


namespace Game {
#define SAFE_PRINT(arg) {std::lock_guard<std::mutex> printGuard(printMut); std::cout << arg << std::endl;}


    int Solver::bestLength = 0;
    int Solver::bestGrid[8][8]{};
    long long int Solver::cycle = 0;

    Solver::Solver(int x, int y, int direction, int firstTile) {
        tiles = getTileCounts();
        resetBoard(grid);
        setTileAt(x, y, firstTile, grid);
        tiles[firstTile]--;
        this->firstTile = firstTile;
        this->sX = x;
        this->sY = y;
        this->sDirection = direction;
    }

    bool Solver::isValid() {
        return isValidTileOnPos(sX, sY, firstTile);
    }

    void Solver::start() {
        std::tuple<int, int, int, int> position = getLineLastPosition(sX, sY, sDirection, grid);
        SAFE_PRINT("===== Starting solver: " << firstTile << " =====")

        csp(std::get<0>(position), std::get<1>(position), std::get<2>(position), std::get<3>(position), 1);
        SAFE_PRINT("===== Finishing solver: " << firstTile << " =====")
    }

    void Solver::showBoard() {
        showCurrentBoard(grid);
    }

    void Solver::operator()() {
        start();
    }

    void Solver::csp(int x, int y, int direction, int lenSoFar, int depth) {
        // TODO: Just for testing...
        if (depth >= 10) {
            return;
        }

        // Is it the end of the line?
        if (!isPlaceablePosition(x, y)) {
            // It is the actual end of line, lets not do anything.
            std::cerr << "End of line at the start?!" << std::endl;
            return;
        }

        for (auto &tile : tiles) {
            if (tile.second <= 0) continue;
            if (!isValidTileOnPos(x, y, tile.first)) continue;

            tile.second--;
            setTileAt(x, y, tile.first, grid);

            std::tuple<int, int, int, int> finishPosition = getLineLastPosition(x, y, direction, grid);
            int tarX = std::get<0>(finishPosition);
            int tarY = std::get<1>(finishPosition);
            int tarDirection = std::get<2>(finishPosition);
            int length = std::get<3>(finishPosition) + lenSoFar;
            if (!isPlaceablePosition(tarX, tarY)) {
                // We actually ended the line...
                // Check if it is longer than the current max found.
                if (++cycle % 1000000 == 0) SAFE_PRINT("Explored " << cycle / 1000000 << "M possibilities")

                if (bestLength < length) {
                    std::lock_guard<std::mutex> guard(mut);
                    if (bestLength < length) {
                        // Overwrite
                        for (int i = 0; i < 8; ++i) {
                            for (int j = 0; j < 8; ++j) {
                                bestGrid[i][j] = grid[i][j];
                            }
                        }
                        bestLength = length;
                        SAFE_PRINT("\tNew length: " << length)
                        if (length > 105)
                            showCurrentBoard(bestGrid);
                    }
                }
            } else {
                csp(tarX, tarY, tarDirection, length, depth + 1);
            }

            deleteTileAt(x, y, grid);
            tile.second++;
        }
    }
}