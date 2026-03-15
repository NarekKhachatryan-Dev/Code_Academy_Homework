#ifndef GAME_H
#define GAME_H

#include "chess.h"
#include <string>
#include <map>
#include <vector>
#include <SFML/Graphics.hpp>

class Game {
private:
    chessboard board;
    sf::Font font;
    bool fontLoaded = false;
    bool whiteToMove = true;

    
    std::map<char, sf::Texture> textureMap;

    
    void loadAllTextures();
    bool loadFont();
    void drawBoard(sf::RenderWindow& window);
    void highlightSquare(sf::RenderWindow& window, position pos, sf::Color color);

    
    std::string evaluatePosition(int searchDepth = 4);
    std::string movesToString(const std::vector<Move>& seq, bool startWhite) const;

public:
    Game();
    
    
    void runGUI();
};

#endif