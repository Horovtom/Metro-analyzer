
//
// Created by lactosis on 21.9.19.
//

#ifndef METRO_GAME_H
#define METRO_GAME_H

#include <vector>
#include "Tile.h"

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

private:
    bool isValidPosition(int x, int y);

    /**
     * Values are indices of tileRepository + 1
     * for example for position 0,0 we can get the actual tile as: tileRepository.at(board[0][0] - 1)
     */
    int board[8][8]{};

    void outputBoard(const std::string &path);
};


#endif //METRO_GAME_H
