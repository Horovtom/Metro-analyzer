//
// Created by lactosis on 21.9.19.
//

#ifndef METRO_DIRECTIONS_H
#define METRO_DIRECTIONS_H

#include <iostream>

namespace Directions {
    const int N = 0;
    const int E = 1;
    const int S = 2;
    const int W = 3;

    static char toChar(int dir) {
        switch (dir) {
            case N:
                return 'N';
            case E:
                return 'E';
            case S:
                return 'S';
            case W:
                return 'W';
            default:
                std::cerr << "Error parsing direction: " << dir << std::endl;
                exit(5);
        }
    }

    static int fromChar(char ch) {
        switch (ch) {
            case 'N':
                return N;
            case 'E':
                return E;
            case 'S':
                return S;
            case 'W':
                return W;
            default:
                std::cerr << "Error parsing direction char: " << ch << std::endl;
                exit(6);
        }
    }

    static int flip(int dir) {
        switch (dir) {
            case Directions::N:
                return Directions::S;
            case Directions::S:
                return Directions::N;
            case Directions::E:
                return Directions::W;
            case Directions::W:
                return Directions::E;
            default:
                std::cerr << "Error parsing direction: " << dir << std::endl;
                exit(18);
        }
    }
}


#endif //METRO_DIRECTIONS_H
