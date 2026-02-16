#ifndef GAME_H
#define GAME_H

#include "chess.h"
#include <string>
#include <map>
#include <SFML/Graphics.hpp>

class Game {
private:
    chessboard board;
    bool whiteTurn;
    bool isRunning;

    std::map<char, sf::Texture> textureMap;
    void loadAllTextures();
    bool parseInput(const std::string& input, int& row, int& col);

public:
    Game();
    void start();
    void handleInput();
    void runGUI();
    void drawBoard(sf::RenderWindow& window);
};

#endif