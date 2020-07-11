//
// Created by lactosis on 22.4.20.
//

#ifndef METRO_GAME_H
#define METRO_GAME_H

#include <mutex>
#include <vector>
#include <tuple>
#include <thread>
#include <iostream>
#include <fstream>
#include <cmake_variables.h>
#include "Tile.h"


namespace Game {
    extern std::mutex mut, printMut;
    extern std::vector<Tile> tileRepository;

    bool isPlaceablePosition(int x, int y);

    bool isValidTileOnPos(int x, int y, int tileIndex);

    void setTileAt(int x, int y, int tileIndex, int board[8][8]);

    std::pair<int, int> getCoordsAtDirection(std::pair<int, int> pos, int direction);

    int coordToIndex(int x, int y);

    Tile *getTileAt(int x, int y, int board[8][8]);

    void outputBoard(const std::string &path, int board[8][8]);

    void showCurrentBoard(int board[8][8]);

    std::tuple<int, int, int, int> getLineLastPosition(int x, int y, int direction, int board[8][8]);

    void deleteTileAt(int i, int j, int board[8][8]);

    void resetBoard(int board[8][8]);

    void cspStart(int x, int y, int direction);

    std::map<int, int> getTileCounts();

    void setTileRepository(std::vector<Tile> tiles);
}


#endif //METRO_GAME_H
