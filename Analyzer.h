
//
// Created by lactosis on 21.9.19.
//

#ifndef METRO_GAME_H
#define METRO_GAME_H

#include <vector>
#include "Tile.h"
#include <cmake_variables.h>


class Analyzer {
public:
    std::vector<Tile> tileRepository;


    explicit Analyzer(const std::vector<Tile> &tiles);

    /**
     * Fills the board of the analyzer with tile configuration with the longest route that is possible with current tileset starting in the starting tile.
     * @param x x-coordinate of the starting tile
     * @param y y-coordinate of the starting tile.
     * @param direction Direction of the train coming into the starting tile.
     * @return Length of the route.
     */
    int fillWithLongestPossibleRoute(int x, int y, int direction);

    bool isBoardValid();

    Tile *getTileAt(int x, int y);

    void setTileAt(int x, int y, int tileIndex);

    void resetTileAt(int x, int y);

    int getLineLength(int x, int y, int direction);

    std::pair<int, int> getCoordsAtDirection(std::pair<int, int> pos, int direction);

    void showCurrentBoard();

    bool isValidPosition(int x, int y);

    bool isValidTileOnPos(int x, int y, int tileIndex);

    void outputBoard(const std::string &path);

    bool isValidTileOnPositionByID(int x, int y, int tileID);

    bool getNextPermutation(std::vector<int> *input, int badIndex);

private:

    /**
     * Values are indices of tileRepository + 1
     * for example for position 0,0 we can get the actual tile as: tileRepository.at(board[0][0] - 1)
     */
    int board[8][8]{};
    int currToChange = -1;


    bool genNextValidPermutation(std::vector<int> *tiles);


    bool populateGrid(std::vector<int> *tiles, int *posToCheck);

    bool checkLast20Same(std::vector<int> *tiles, const int last20[20]);

    bool nextPermTileAt(std::vector<int> *tiles, int posToChange);

    int getLastOfLine(int x, int y, int direction);

    int coordToIndex(int x, int y);

    void outputWriteBoard(std::vector<int> *tiles, const std::string &path);
};


#endif //METRO_GAME_H
