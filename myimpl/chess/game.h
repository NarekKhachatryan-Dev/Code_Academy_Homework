#ifndef GAME_H
#define GAME_H

#include "chess.h"
#include <string>
#include <map>
#include <SFML/Graphics.hpp>

class Game {
private:
    chessboard board;
    sf::Font font;             // font used for coordinate labels and evaluation text

    std::map<char, sf::Texture> textureMap;
    void loadAllTextures();
    std::string evaluatePosition(int depth = 8);
    std::string movesToString(const std::vector<Move>& seq, bool startWhite) const;

public:
    Game();
    void runGUI();
    void drawBoard(sf::RenderWindow& window);
};

#endif