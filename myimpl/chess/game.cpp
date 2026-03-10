#include "game.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <algorithm>

Game::Game() {
    board.clear();
}

void Game::loadAllTextures() {
    std::string pieces = "pnbrqkPNBRQK";
    for (char c : pieces) {
        sf::Texture tex;
        std::string filename = "images/";
        filename += c;
        filename += ".png";
        if (!tex.loadFromFile(filename)) {
            std::cerr << "Error loading texture: " << filename << std::endl;
        } else {
            textureMap[c] = std::move(tex);
        }
    }
}

void Game::runGUI() {
    sf::RenderWindow window(sf::VideoMode(800, 950), "Position Analyzer Pro");
    loadAllTextures();
    
    
    whiteToMove = true; 

    bool fontOK = false;
    if (font.loadFromFile("arial.ttf") || 
        font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") ||
        font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeSans.ttf")) {
        fontOK = true;
    }

    sf::Text evalText;
    if (fontOK) evalText.setFont(font);
    evalText.setCharacterSize(16);
    evalText.setFillColor(sf::Color::Cyan);
    evalText.setPosition(10.f, 820.f);

    
    sf::Text turnText;
    if (fontOK) turnText.setFont(font);
    turnText.setCharacterSize(18);
    turnText.setPosition(10.f, 895.f);

    
    sf::RectangleShape clearBtn(sf::Vector2f(120.f, 40.f));
    clearBtn.setPosition(660.f, 890.f);
    clearBtn.setFillColor(sf::Color(150, 50, 50));

    const std::vector<char> pieceOrder = {'.','P','N','B','R','Q','K','p','n','b','r','q','k'};

    
    bool needEvaluation = true;
    std::string currentEvalString = "Evaluating...";

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            
            
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    whiteToMove = !whiteToMove; 
                    needEvaluation = true; 
                }
            }

            
            if (event.type == sf::Event::MouseButtonPressed) {
                int x = event.mouseButton.x;
                int y = event.mouseButton.y;
                
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (clearBtn.getGlobalBounds().contains(x, y)) {
                        board.clear();
                        needEvaluation = true; 
                    } else if (y < 800) {
                        int c = x / 100;
                        int r = y / 100;
                        char cur = board.getPieceSymbol(r, c);
                        auto it = std::find(pieceOrder.begin(), pieceOrder.end(), cur);
                        int idx = (it == pieceOrder.end() ? 0 : (it - pieceOrder.begin() + 1) % pieceOrder.size());
                        board.placePiece(pieceOrder[idx], r, c);
                        needEvaluation = true; 
                    }
                }
            }
        }

        
        if (needEvaluation) {
            currentEvalString = evaluatePosition(4);
            evalText.setString(currentEvalString);
            needEvaluation = false; 
        }

        window.clear(sf::Color(30, 30, 30));
        drawBoard(window);

        
        if (board.isCheck(true)) highlightSquare(window, board.getWhiteKingPos(), sf::Color(255, 0, 0, 150));
        if (board.isCheck(false)) highlightSquare(window, board.getBlackKingPos(), sf::Color(255, 0, 0, 150));

        
        window.draw(clearBtn);
        if (fontOK) {
            sf::Text btnText("Clear Board", font, 16);
            btnText.setPosition(670.f, 900.f);
            window.draw(btnText);

            
            turnText.setString(whiteToMove ? "Turn: WHITE (Press Space to flip)" : "Turn: BLACK (Press Space to flip)");
            turnText.setFillColor(whiteToMove ? sf::Color::White : sf::Color(180, 180, 180));
            window.draw(turnText);
        }

        
        window.draw(evalText);

        window.display();
    }
}

void Game::highlightSquare(sf::RenderWindow& window, position pos, sf::Color color) {
    if (pos.row < 0 || pos.col < 0) return;
    sf::RectangleShape highlight(sf::Vector2f(100.f, 100.f));
    highlight.setPosition(pos.col * 100.f, pos.row * 100.f);
    highlight.setFillColor(color);
    window.draw(highlight);
}

void Game::drawBoard(sf::RenderWindow& window) {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            sf::RectangleShape rect(sf::Vector2f(100.f, 100.f));
            rect.setPosition(c * 100.f, r * 100.f);
            rect.setFillColor((r + c) % 2 == 0 ? sf::Color(238, 238, 210) : sf::Color(118, 150, 86));
            window.draw(rect);
            
            char symbol = board.getPieceSymbol(r, c);
            if (symbol != '.') {
                sf::Sprite sprite;
                sprite.setTexture(textureMap[symbol]);
                sprite.setPosition(c * 100.f, r * 100.f);
                window.draw(sprite);
            }
        }
    }

    for (int i = 0; i < 8; i++) {
        sf::Text label;
        label.setFont(font);
        label.setCharacterSize(20);
        
        
        label.setFillColor(sf::Color::Black); 

        
        label.setString(std::to_string(8 - i));
        label.setPosition(5.f, i * 100 + 40.f);
        window.draw(label);

        
        label.setString(std::string(1, 'A' + i));
        label.setPosition(i * 100 + 45.f, 775.f);
        window.draw(label);
    }
}

std::string Game::evaluatePosition(int searchDepth) {
    std::ostringstream out;
    std::vector<Move> seq;

    
    if (board.isCheckmate(whiteToMove)) {
        return whiteToMove ? "RESULT: Black Wins by Checkmate!" : "RESULT: White Wins by Checkmate!";
    }
    if (board.isStalemate(whiteToMove)) {
        return "RESULT: Draw by Stalemate (PAT)!";
    }

    
    int mateIn = board.findMate(searchDepth, whiteToMove, seq);
    if (mateIn > 0 && !seq.empty()) {
        const auto& m = seq[0];
        char pieceChar = std::toupper(board.getPieceSymbol(m.fromRow, m.fromCol));
        
        
        std::string from = std::string(1, 'a' + m.fromCol) + std::to_string(8 - m.fromRow);
        std::string to = std::string(1, 'a' + m.toCol) + std::to_string(8 - m.toRow);
        
        out << "MATE IN " << mateIn << " (" << pieceChar << from << "-" << to << ") | ";
        out << (whiteToMove ? "WHITE wins" : "BLACK wins"); 
        return out.str();
    }

    
    int score = board.evaluate();
    out << "Eval: " << (score / 100.0) << (whiteToMove ? " | White's turn" : " | Black's turn");
    return out.str();
}

std::string Game::movesToString(const std::vector<Move>& seq, bool startWhite) const {
    chessboard temp = board;
    std::ostringstream oss;
    for (const auto& m : seq) {
        char f = 'a' + m.fromCol;
        int r = 8 - m.fromRow;
        char tf = 'a' + m.toCol;
        int tr = 8 - m.toRow;
        
        oss << f << r << (m.capture ? "x" : "-") << tf << tr << " ";
        temp.makeMove(m.fromRow, m.fromCol, m.toRow, m.toCol, m.promotion);
    }
    return oss.str();
}