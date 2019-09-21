#include <random>

#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include "Tile.h"
#include "Analyzer.h"
#include <algorithm>

std::vector<Tile> loadTilesList(const std::string &input);

int main(int argc, char **argv) {
    std::vector<Tile> tilesList = loadTilesList("/home/lactosis/Documents/Programming/C++/Metro/input.txt");
    Analyzer a(tilesList);
    int length = a.fillWithLongestPossibleRoute(0, 0, Directions::W);
    std::cout << length << std::endl;
    a.showCurrentBoard();
    return 0;
}

/**
 * Loads tiles from input file
 * @return
 */
std::vector<Tile> loadTilesList(const std::string &input) {
    std::vector<Tile> result;

    std::string line;
    std::ifstream inputFile(input);
    if (inputFile.is_open()) {
        while (getline(inputFile, line)) {
            std::istringstream iss(line);
            if (line[0] == '#') continue;

            int howMany;
            iss >> howMany;
            int connections[4];
            for (int i = 0; i < 4; ++i) {
                char ch1, ch2;
                iss >> ch1;
                iss >> ch2;
                connections[Directions::fromChar(ch1)] = Directions::fromChar(ch2);
            }

            for (int i = 0; i < howMany; ++i) {
                result.emplace_back(
                        Tile(connections[Directions::N], connections[Directions::E], connections[Directions::S],
                             connections[Directions::W]));
            }
        }
        inputFile.close();
    } else {
        std::cerr << "Unable to open input file!" << std::endl;
        exit(3);
    }

    return result;
}
