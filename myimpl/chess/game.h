#ifndef GAME_H
#define GAME_H

#include "chess.h"
#include <string>

class Game {
private:
    chessboard board;
    bool whiteTurn;
    bool isRunning;

    bool parseInput(const std::string& input, int& row, int& col);

public:
    Game();
    void start();
    void handleInput();
};

#endif