
//
// Created by lactosis on 21.9.19.
//

#ifndef METRO_GAME_H
#define METRO_GAME_H

#include <vector>
#include "Tile.h"
#include <mutex>
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

    void setTileAt(int x, int y, int tileIndex, int board[8][8]);

    void resetTileAt(int x, int y);

    int getLineLength(int x, int y, int direction);

    std::pair<int, int> getCoordsAtDirection(std::pair<int, int> pos, int direction);

    void showCurrentBoard();

    bool isPlaceablePosition(int x, int y);

    bool isValidTileOnPos(int x, int y, int tileIndex);

    void outputBoard(const std::string &path);

    bool isValidTileOnPositionByID(int x, int y, int tileID);

private:

    /**
     * Values are indices of tileRepository + 1
     * for example for position 0,0 we can get the actual tile as: tileRepository.at(board[0][0] - 1)
     */
    int board[8][8]{};
    int currToChange = -1;

    std::mutex mut;

    long long int cycle = 0;

    int longestPathLength = 0;
    int longestBoard[8][8]{};

    bool populateGrid(std::vector<int> *tiles, int *posToCheck);

    bool checkLast20Same(std::vector<int> *tiles, const int last20[20]);

    int getLastOfLine(int x, int y, int direction);

    int coordToIndex(int x, int y);

    void overwriteBoard(std::vector<int> *tiles);

    void csp(int x, int y, int direction, std::vector<std::pair<int, int>> *tiles, int lengthSoFar, int depth);

//    static void cspStatic(int x, int y, int direction, std::vector<std::pair<int, int>> *tiles, int lengthSoFar, int depth, int grid[8][8], int *longestBoard[8][8], int *longestPathLength);

    void resetBoard();

    void resetBoard(int board[8][8]);

    std::vector<std::pair<int, int>> getTileCounts();

    void deleteTileAt(int i, int j);

    void deleteTileAt(int i, int j, int board[8][8]);

    /**
     * @param direction Direction from which the lien comes to the x,y tile.
     * @return  tuple (x, y, direction, length)
     *          finishing position of a line. x,y are the finish coordinates (the tile the line comes out to, not the
     *          last tile of the line). Direction is  the direction, the line comes from and length is the number of
     *          tiles along this line (if the line is a cycle, the length will be -1).
     */
    std::tuple<int, int, int, int> getLineLastPosition(int x, int y, int direction);

    void cspStart(int x, int y, int direction);
};


#endif //METRO_GAME_H
