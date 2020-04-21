#include <random>

#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include "Tile.h"
#include "Analyzer.h"
#include "SpecPermutations.h"
#include <algorithm>
#include <unistd.h>
//#include <cmake_variables.h>

std::vector<Tile> loadTilesList(const std::string &input);

void calculateIt();

long double getForPositions(int *positionsLeft, int groupSize, int numOfGroups);

void getValidForTiles();


int main(int argc, char **argv) {
//    getValidForTiles();

//    calculateIt();
//
    std::vector<Tile> tilesList = loadTilesList(Project::getProjectRootFilePath("/input.txt"));
    Analyzer a(tilesList);
    int length = a.fillWithLongestPossibleRoute(0, 0, Directions::W);
    std::cout << length << std::endl;
    a.showCurrentBoard();
    return 0;
}


void getValidForTiles() {
    std::vector<int> validTileNumbers{2, 3, 4, 5, 6, 7, 8, 9,
                                      12, 13, 14, 15, 16, 17, 18, 19,
                                      22, 23, 24, 25, 26, 27, 28, 29};
    std::vector<Tile> tilesList = loadTilesList(Project::getProjectRootFilePath("/input.txt"));
    Analyzer a(tilesList);
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (a.isPlaceablePosition(i, j)) {
//                std::cout << "Position: " << i << "," << j << " - ";
                int howMany = 0;
                for (int c: validTileNumbers) {
                    if (a.isValidTileOnPositionByID(i, j, c)) {
                        std::cout << c << ",";
                        howMany++;
                    }
                }
                std::cout
//                << " - " << howMany
                        << std::endl;
            }
        }
    }
}

void calculateIt() {
    long double result = 1;
    int positionsLeft = 60;

    result *= getForPositions(&positionsLeft, 4, 5);
    result *= getForPositions(&positionsLeft, 2, 17);
    result *= getForPositions(&positionsLeft, 3, 2);

    std::cout << result << std::endl;
}

long double getForPositions(int *positionsLeft, int groupSize, int numOfGroups) {
    long double result = 1;
    for (int i = 0; i < numOfGroups; ++i) {
        result *= SpecPermutations::getSpec(*positionsLeft, groupSize);
        *positionsLeft -= groupSize;
    }
    return result;
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
