#include <utility>

//
// Created by lactosis on 21.4.20.
//

#include "Game.h"
#include "Solver.h"


 std::mutex Game::mut;
std::mutex Game::printMut;
 std::vector<Tile> Game::tileRepository;

bool Game::isPlaceablePosition(int x, int y) {
    if (x < 0 || x > 7 || y < 0 || y > 7) return false;

    if (x == 3 || x == 4)
        if (y == 3 || y == 4)
            return false;

    return true;
}

bool Game::isValidTileOnPos(int x, int y, int tileIndex) {
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
    return !(x == 7 && y == 7 &&
             (t->connections[Directions::S] == Directions::E || t->connections[Directions::E] == Directions::S));

}

void Game::setTileAt(int x, int y, int tileIndex, int board[8][8]) {
    board[y][x] = tileIndex + 1;
}

std::map<int, int> Game::getTileCounts() {
    std::map<int, int> res;

    for (int tileIndex = 0; tileIndex < tileRepository.size(); ++tileIndex) {
        bool found = false;
        for (auto &re : res) {
            if (tileRepository.at(re.first).equals(tileRepository.at(tileIndex))) {
                re.second++;
                found = true;
                break;
            }
        }
        if (!found)
            res[tileIndex] = 1;
    }

    return res;
}

std::pair<int, int> Game::getCoordsAtDirection(std::pair<int, int> pos, int direction) {
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


int Game::coordToIndex(int x, int y) {
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


Tile *Game::getTileAt(int x, int y, int board[8][8]) {
    if (!isPlaceablePosition(x, y)) return nullptr;
    if (board[y][x] == 0) return nullptr;
    return &tileRepository.at(board[y][x] - 1);
}


void Game::outputBoard(const std::string &path, int board[8][8]) {
    std::ofstream oss(path);
    oss << "0 0 3" << std::endl;
    if (oss.is_open()) {
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                Tile *t = getTileAt(i, j, board);
                if (t != nullptr)
                    oss << (int) getTileAt(i, j, board)->getIdentifier() << " ";
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

void Game::showCurrentBoard(int board[8][8]) {
    outputBoard(Project::getProjectRootFilePath("/output.txt"), board);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    std::string pathOld = Project::getProjectRootFilePath("/visualiser/bin/win/visualiser.exe");
const char *path = pathOld.c_str();
int result = system(path);
std::cout << "Visualiser return code: " << result << ", launched on path: " << path << std::endl;
#else
    system(Project::getProjectRootFilePath("/visualiser/bin/linux/visualiser").c_str());
#endif
}

std::tuple<int, int, int, int> Game::getLineLastPosition(int x, int y, int direction, int board[8][8]) {
    Tile *currTile = getTileAt(x, y, board);
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

        currTile = getTileAt(currPos.first, currPos.second, board);
        cameFromDir = Directions::flip(targetDir);
        if (currPos.first == x && currPos.second == y && cameFromDir == direction) {
            std::cerr << "We encountered a cycle!" << std::endl;
            return std::tuple(x, y, direction, -1);
        }
        length++;
    }
}

void Game::deleteTileAt(int i, int j, int board[8][8]) {
    setTileAt(i, j, -1, board);
}


void Game::resetBoard(int board[8][8]) {
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            deleteTileAt(i, j, board);
}


void Game::cspStart(int x, int y, int direction) {
    Solver::bestLength = 0;
    Solver::cycle = 0;
    std::vector<std::thread> threads;
    std::vector<Solver*> solvers;

    std::map<int, int> tiles = getTileCounts();

    for (auto &tile : tiles) {
        auto * s= new Solver(x, y, direction, tile.first);
        if (!s->isValid()) {
            continue;
        }
        solvers.emplace_back(s);
        threads.emplace_back(std::ref(*s));
    }

//    // TODO: Testing
//    for (auto &tile: tiles) {
//        Solver s(x,y,direction, tile.first) ;
//        if (s.isValid()) {
//            solvers.emplace_back(s);
////            s.start();
//            threads.emplace_back(&Solver::start, &s);
//            break;
//        }
//    }
//
//    Solver s(x,y,direction, 48);
//    s.start();
//    Solver l(x,y,direction,34);
//    l.start();
//    Solver k(x,y,direction,40);
//    k.start();
//    Solver m(x,y,direction,56);
//    m.start();

    std::cout << "Running all: " << threads.size() << " threads" << std::endl;

    for (auto &thread : threads) {
        thread.join();
        std::cout << "Joined thread..." << std::endl;
    }

   for (int i=0;i < solvers.size();++i)
       delete solvers[i];

    std::cout << "Final board: " << std::endl;
    showCurrentBoard(Solver::bestGrid);
}

void Game::setTileRepository(std::vector<Tile> tiles) {
    tileRepository = std::move(tiles);
}

