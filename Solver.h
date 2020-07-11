//
// Created by lactosis on 22.4.20.
//

#ifndef METRO_SOLVER_H
#define METRO_SOLVER_H

#include <map>
namespace Game {
    class Solver {
    public:
        static int bestGrid[8][8];
        static int bestLength;
        static long long int cycle;

        int grid[8][8]{};
        std::map<int, int> tiles;
        int sX, sY, sDirection;
        int firstTile;

        Solver(int x, int y, int direction, int firstTile);

        bool isValid();

        void showBoard();

        void operator()();

        void start();

        void csp(int x, int y, int direction, int lenSoFar, int depth);
    };
}


#endif //METRO_SOLVER_H
